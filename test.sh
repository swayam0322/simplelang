#!/bin/sh
# Compile with the documented command and check the assembly examples.
set -eu
cd "$(dirname "$0")"

g++ -std=c++20 -o simple compiler.cpp

tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
fail=0

check() {
    name=$1
    src=$2
    expected=$3
    ./simple "$src"
    if diff -u "$expected" generated.asm; then
        echo "ok $name"
    else
        echo "FAIL $name" >&2
        fail=1
    fi
}

cat > "$tmp/straight.spl" << 'EOF'
var a;
var b;
var c;

a : 10;
b : 20;
c : a + b;
EOF
cat > "$tmp/straight.asm" << 'EOF'
.text

_start:
ldi A 10
sta %a
ldi A 20
sta %b
lda %b
mov B A
lda %a
add
sta %c

END:
hlt

.data
a=0
b=0
c=0
EOF
check straight "$tmp/straight.spl" "$tmp/straight.asm"

cat > "$tmp/input.asm" << 'EOF'
.text

_start:
ldi A 10
sta %a
ldi A 20
sta %b
lda %b
mov B A
lda %a
add
sta %c
lda %c
mov B A
lda %a
add
sta %d
lda %d
mov B A
lda %c
add
sta %e
ldi A 30
mov B A
lda %c
cmp
jnz %ENDBLOCK0
BLOCK0:
ldi A 1
mov B A
lda %c
add
sta %c
ENDBLOCK0:

END:
hlt

.data
a=0
b=0
c=0
d=0
e=0
EOF
check input.spl input.spl "$tmp/input.asm"

cat > "$tmp/nested.spl" << 'EOF'
var a;
var b;
var c;

a : 10;
b : 20;
c : a + b;

if (c = 30) {
  c : c + 1;
  a : a - 2;
  if (c ! 20) {
    a : a + 10;
  }
}
EOF
cat > "$tmp/nested.asm" << 'EOF'
.text

_start:
ldi A 10
sta %a
ldi A 20
sta %b
lda %b
mov B A
lda %a
add
sta %c
ldi A 30
mov B A
lda %c
cmp
jnz %ENDBLOCK0
BLOCK0:
ldi A 1
mov B A
lda %c
add
sta %c
ldi A 2
mov B A
lda %a
sub
sta %a
ldi A 20
mov B A
lda %c
cmp
jz %ENDBLOCK1
BLOCK1:
ldi A 10
mov B A
lda %a
add
sta %a
ENDBLOCK1:
ENDBLOCK0:

END:
hlt

.data
a=0
b=0
c=0
EOF
check nested "$tmp/nested.spl" "$tmp/nested.asm"

cat > "$tmp/after.spl" << 'EOF'
var a;
var b;
a : 1;
if (a = 1) {
  a : 2;
  if (a ! 0) {
    a : 4;
  }
  b : 5;
}
b : 6;
EOF
cat > "$tmp/after.asm" << 'EOF'
.text

_start:
ldi A 1
sta %a
ldi A 1
mov B A
lda %a
cmp
jnz %ENDBLOCK0
BLOCK0:
ldi A 2
sta %a
ldi A 0
mov B A
lda %a
cmp
jz %ENDBLOCK1
BLOCK1:
ldi A 4
sta %a
ENDBLOCK1:
ldi A 5
sta %b
ENDBLOCK0:
ldi A 6
sta %b

END:
hlt

.data
a=0
b=0
EOF
check after-if "$tmp/after.spl" "$tmp/after.asm"

# A token that starts no statement stops the top-level parse.
# The compiler must return, not hang, and keep the statements before it.
printf 'var a;\na : 1;\n@\nb : 2;\n' > "$tmp/bad.spl"
if timeout 2 ./simple "$tmp/bad.spl"; then
    if grep -q 'sta %a' generated.asm && ! grep -q 'sta %b' generated.asm; then
        echo "ok unknown-token"
    else
        echo "FAIL unknown-token output" >&2
        fail=1
    fi
else
    echo "FAIL unknown-token (hung or crashed)" >&2
    fail=1
fi

# A file that ends on an identifier must not walk off the input.
printf 'var abc' > "$tmp/end.spl"
if timeout 2 ./simple "$tmp/end.spl" >/dev/null 2>&1; then
    echo "ok eof-identifier"
else
    echo "FAIL eof-identifier" >&2
    fail=1
fi

if ./simple >/dev/null 2>&1; then
    echo "FAIL missing-args" >&2
    fail=1
else
    echo "ok missing-args"
fi

if ./simple "$tmp/no-such-file.spl" >/dev/null 2>&1; then
    echo "FAIL missing-file" >&2
    fail=1
else
    echo "ok missing-file"
fi

# generated.asm is written in the current directory, not next to the source.
root=$(pwd)
mkdir -p "$tmp/other"
(
    cd "$tmp/other"
    "$root/simple" "$tmp/straight.spl"
)
if diff -q "$tmp/straight.asm" "$tmp/other/generated.asm" >/dev/null; then
    echo "ok writes-cwd"
else
    echo "FAIL writes-cwd" >&2
    fail=1
fi

exit "$fail"
