#include <bits/stdc++.h>
using namespace std;

typedef enum
{
    TOK_INTEGER,       // 0
    TOK_IDENTIFIER,    // 1
    TOK_NUMBER,        // 2
    TOK_EQUAL,         // 3
    TOK_PLUS,          // 4
    TOK_MINUS,         // 5
    TOK_STAR,          // 6
    TOK_SLASH,         // 7
    TOK_IF,            // 8
    TOK_ELSE,          // 9
    TOK_EQUAL_EQUAL,   // 10
    TOK_BANG,          // 11
    TOK_BANG_EQUAL,    // 12
    TOK_GREATER,       // 13
    TOK_GREATER_EQUAL, // 14
    TOK_LESS,          // 15
    TOK_LESS_EQUAL,    // 16
    TOK_LEFT_PAREN,    // 17
    TOK_RIGHT_PAREN,   // 18
    TOK_LEFT_BRACE,    // 19
    TOK_RIGHT_BRACE,   // 20
    TOK_SEMICOLON,     // 21
    TOK_UNKNOWN,       // 22
    TOK_EOF            // 23
} TokenType;

typedef struct
{
    TokenType type;
    string val;
} Token;

vector<Token> tokens;

void addToken(TokenType t, string s)
{
    tokens.push_back({.type = t, .val = s});
}

int main()
{
    ifstream file("input.txt");
    if (!file)
    {
        perror("Failed to open file");
        return 1;
    }

    for (auto c = istreambuf_iterator<char>(file); c != istreambuf_iterator<char>();)
    {
        switch (*c)
        {
        case '(':
            addToken(TOK_LEFT_PAREN, "(");
            c++;
            break;
        case ')':
            addToken(TOK_RIGHT_PAREN, ")");
            c++;
            break;
        case '{':
            addToken(TOK_LEFT_BRACE, "{");
            c++;
            break;
        case '}':
            addToken(TOK_RIGHT_BRACE, "}");
            c++;
            break;
        case '-':
            addToken(TOK_MINUS, "-");
            c++;
            break;
        case '+':
            addToken(TOK_PLUS, "+");
            c++;
            break;
        case ';':
            addToken(TOK_SEMICOLON, ";");
            c++;
            break;
        case '*':
            addToken(TOK_STAR, "*");
            c++;
            break;
        case '/':
            addToken(TOK_SLASH, "/");
            c++;
            break;
        case '!':
            if (*next(c, 1) == '=')
                addToken(TOK_BANG_EQUAL, "!=");
            else
                addToken(TOK_BANG, "!");
            c++;
            break;
        case '=':
            c++;
            if (*c == '=')
                addToken(TOK_EQUAL_EQUAL, "==");
            else
                addToken(TOK_EQUAL, "=");
            break;
        case '<':
            if (*next(c, 1) == '=')
                addToken(TOK_LESS_EQUAL, "<=");
            else
                addToken(TOK_LESS, "<");
            break;
        case '>':
            if (*next(c, 1) == '=')
                addToken(TOK_GREATER_EQUAL, ">=");
            else
                addToken(TOK_GREATER, "<");
            break;
        case ' ':
        case '\r':
        case '\t':
        case '\n':
            // Ignore whitespace.
            c++;
            break;
        default:
            string buffer;
            if (isalpha(*c))
            {
                while (isalnum(*c))
                {
                    buffer.push_back(*c);
                    c++;
                }
                if (buffer == "int")
                    addToken(TOK_INTEGER, buffer);
                else if (buffer == "if")
                    addToken(TOK_IF, buffer);
                else if (buffer == "else")
                    addToken(TOK_ELSE, buffer);
                else
                    addToken(TOK_IDENTIFIER, buffer);
            }
            else if (isdigit(*c))
            {
                while (isdigit(*c))
                {
                    buffer.push_back(*c);
                    c++;
                }
                addToken(TOK_NUMBER, buffer);
            }
            else
                addToken(TOK_UNKNOWN, " ");
        }
    }
    addToken(TOK_EOF, "\0");
    for (auto i : tokens)
    {
        cout << i.type << ' ' << i.val << '\n';
    }
}