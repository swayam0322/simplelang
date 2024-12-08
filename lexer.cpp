#include <bits/stdc++.h>
using namespace std;

typedef enum
{
    TOK_INTEGER,
    TOK_IDENTIFIER,
    TOK_NUMBER,
    TOK_EQUAL,
    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_IF,
    TOK_ELSE,
    TOK_EQUAL_EQUAL,
    TOK_BANG,
    TOK_BANG_EQUAL,
    TOK_GREATER,
    TOK_GREATER_EQUAL,
    TOK_LESS,
    TOK_LESS_EQUAL,
    TOK_LEFT_PAREN,
    TOK_RIGHT_PAREN,
    TOK_LEFT_BRACE,
    TOK_RIGHT_BRACE,
    TOK_SEMICOLON,
    TOK_UNKNOWN,
    TOK_EOF
} TokenType;

typedef struct
{
    TokenType type;
    optional<string> val;
} Token;

unordered_map<string, TokenType> keywords = {
    {"int", TOK_INTEGER},
    {"if", TOK_IF},
    {"else", TOK_ELSE},
};

vector<Token> tokens;

void addToken(TokenType t)
{
    tokens.push_back({.type = t});
}

int main()
{
    ifstream file("input.txt");
    if (!file)
    {
        perror("Failed to open file");
        return 1;
    }

    for (auto c = istreambuf_iterator<char>(file); c != istreambuf_iterator<char>(); c++)
    {
        switch (*c)
        {
        case '(':
            addToken(TOK_LEFT_PAREN);
            break;
        case ')':
            addToken(TOK_RIGHT_PAREN);
            break;
        case '{':
            addToken(TOK_LEFT_BRACE);
            break;
        case '}':
            addToken(TOK_RIGHT_BRACE);
            break;
        case '-':
            addToken(TOK_MINUS);
            break;
        case '+':
            addToken(TOK_PLUS);
            break;
        case ';':
            addToken(TOK_SEMICOLON);
            break;
        case '*':
            addToken(TOK_STAR);
            break;
        case '/':
            addToken(TOK_SLASH);
            break;
        case '!':
            addToken((*next(c, 1) == '=') ? TOK_BANG_EQUAL : TOK_BANG);
            break;
        case '=':
            addToken((*next(c, 1) == '=') ? TOK_EQUAL_EQUAL : TOK_EQUAL);
            break;
        case '<':
            addToken((*next(c, 1) == '=') ? TOK_LESS_EQUAL : TOK_LESS);
            break;
        case '>':
            addToken((*next(c, 1) == '=') ? TOK_GREATER_EQUAL : TOK_GREATER);
            break;
        case ' ':
        case '\r':
        case '\t':
        case '\n':
            // Ignore whitespace.
            break;
        default:
            if (isalpha(*c))
            {
                addToken(TOK_IDENTIFIER);
            }
            if (isdigit(*c))
                addToken(TOK_NUMBER);
            else
                addToken(TOK_UNKNOWN);
        }
    }
    addToken(TOK_EOF);
    for (auto i : tokens)
    {
        cout << i.type << '\n';
    }
}