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
jz %BLOCK0
END:
hlt

.data
a=0
b=0
c=0
d=0
e=0
