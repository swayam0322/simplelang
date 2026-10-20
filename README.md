# simplelang

A small compiler written in C++ (about 470 lines) to learn compiler basics. It reads a source file in a tiny language, tokenizes it, builds an abstract syntax tree with a hand-written recursive-descent parser, and emits assembly for a simple two-register (A/B) machine.

## The language

- Integer variables, declared with `var`
- Assignment with `:` (not `=`)
- Expressions: a single term, or `term + term` / `term - term`
- `if (term = term) { ... }` and `if (term ! term) { ... }` (`!` means "not equal"), which can be nested

```
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
```

`input.spl` is a separate sample: it declares `a` through `e`, assigns `c : a + b`, `d : a + c`, and `e : c + d`, then increments `c` inside `if (c = 30)`.

### Grammar

```
Program     := Statement*
Statement   := Declaration | Assignment | Conditional
Declaration := "var" IDENTIFIER ";"
Assignment  := IDENTIFIER ":" Expression ";"
Expression  := Term ("+" | "-") Term | Term
Conditional := "if" "(" Condition ")" Block
Condition   := Term ("=" | "!") Term
Term        := IDENTIFIER | NUMBER
Block       := "{" Statement* "}"
```

## Build and run

```bash
g++ -std=c++20 -o simple compiler.cpp
./simple input.spl          # writes generated.asm in the current directory
cat generated.asm
```

`make` runs that same compile. `make test` runs `test.sh`, which rebuilds `simple` and checks the examples. `make clean` deletes `simple` and `generated.asm`.

Tokens are created with C++20 designated initializers (`{.type = t, .val = s}`). The source includes `<bits/stdc++.h>`, a GCC header, so build with g++. GCC also accepts those initializers as an extension in older modes and warns under `-pedantic`; the command above uses `-std=c++20`.

## Architecture

```
source file ──► tokenize() ──► vector<Token> ──► parse() ──► AST (RootNode) ──► generate() ──► generated.asm
```

**1. Tokenizer (`tokenize`)**
Walks the file with an `istreambuf_iterator`. Single-character tokens are `( ) { } + - ; ! = :`. A run of letters and digits that starts with a letter becomes an identifier, or the keyword `var` or `if` (those spellings only). A run of digits becomes a number. Space, tab, CR, and LF are skipped. Any other character becomes `TOK_UNKNOWN` with the text `$`. A final `TOK_EOF` is appended.

**2. Parser (`parse`, `parseDeclaration`, `parseAssignment`, `parseExpression`, `parseConditional`, `parseBlock`)**
A recursive-descent parser over a token iterator. `parseDeclaration`, `parseAssignment`, and `parseConditional` either match a statement, add a node to the current block, and return true, or return false. `parseExpression` returns a `LiteralNode`, `VariableNode`, or `BinExprNode`; it does not add that node itself. `parseBlock` reads statements until `}` and calls `parseConditional` for a nested `if`. A successful statement parser leaves the iterator on the terminating `;` or `}`, and the caller steps past it.

**3. AST**
A small class hierarchy on `ASTNode`, held in `shared_ptr`: `RootNode` and `BlockNode` (lists of statements; `BlockNode` extends `RootNode` and uses its list), `DeclarationNode`, `AssignmentNode`, `BinExprNode`, `LiteralNode`, `VariableNode`, `ConditionNode`, and `ConditionalNode`.

**4. Code generator (`generate`)**
Walks the AST and uses `dynamic_pointer_cast` to dispatch on node type. The output file is opened once; a nested block is written into that same stream.

- Declarations are collected into a symbol table and, after the code, written as a `.data` section of `name=0` lines. A declaration inside an `if` is included.
- `x : a + b` loads `b` into A (`ldi` for a literal, `lda %var` for a variable), copies it with `mov B A`, loads `a`, emits `add` or `sub`, then `sta %x`. `x : 10` is `ldi A 10` / `sta %x`. `x : a` is `lda %a` / `sta %x`.
- A condition loads both sides the same way, then emits `cmp`. `=` emits `jnz %ENDBLOCKn` and `!` emits `jz %ENDBLOCKn`, so a failed comparison skips the block. The block is labeled `BLOCKn` and ends at `ENDBLOCKn`. `n` starts at 0 and increases for every `if`, including nested ones.
- The program ends with `END:` / `hlt`.

Output for `input.spl`:

```
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
```

## Known limitations / next steps

- Error reporting is minimal. At the top level, the first token that starts no statement ends the parse with no message, and later statements are dropped. Inside a block, that token is skipped. If a failed match has already read through to the end of the file, the compiler prints `Unexpected token ` with no token text and no newline.
- Expressions allow only one `+` or `-`. There is no precedence, no parentheses, and no unary minus.
- Names are not checked against the symbol table. An undeclared name is still loaded with `lda`, and only `var` declarations are written to `.data`.
- No `else` and no `while`.
- Planned: error messages with line numbers, `while`, and richer expressions.

## References

- [Crafting Interpreters: Scanning](https://craftinginterpreters.com/scanning.html)
- [LLVM Kaleidoscope tutorial, ch. 2](https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl02.html)
- [AST in C++ (Lesley Lai)](https://lesleylai.info/en/ast-in-cpp-part-1-variant/)
- [Abstract syntax tree (Wikipedia)](https://en.wikipedia.org/wiki/Abstract_syntax_tree)
