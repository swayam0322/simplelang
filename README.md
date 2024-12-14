# simplelang
Simple compiler 

## Grammar:
```
Program := Statement*

Statement := Declaration
           | Assignment
           | Conditional

Declaration := TOK_INT TOK_IDENTIFIER TOK_SEMICOLON

Assignment := TOK_IDENTIFIER TOK_ASSIGN Expression TOK_SEMICOLON

Expression := Term (+|-) Term | Term

Conditional := TOK_IF TOK_LEFT_PAREN Condition TOK_RIGHT_PAREN Block

Condition := Term (=|>|<|!) Term

Term := TOK_IDENTIIFIER | TERM_NUMBER

Block := TOK_LEFT_BRACE Statement* TOK_RIGHT_BRACE
```

## Sample program:
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
 if(c ! 20) {
   a : a + 10;
 }
}
```
