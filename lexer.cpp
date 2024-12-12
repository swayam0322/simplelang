#include <bits/stdc++.h>
// #include"./parser.hpp"
#include <memory>

using namespace std;

typedef enum
{
    TOK_INTEGER,       // 0
    TOK_IDENTIFIER,    // 1
    TOK_NUMBER,        // 2
    TOK_EQUAL,         // 3
    TOK_PLUS,          // 4
    TOK_MINUS,         // 5
    TOK_IF,            // 6
    TOK_EQUAL_EQUAL,   // 7
    TOK_BANG,          // 8
    TOK_BANG_EQUAL,    // 9
    TOK_GREATER,       // 10
    TOK_GREATER_EQUAL, // 11
    TOK_LESS,          // 12
    TOK_LESS_EQUAL,    // 13
    TOK_LEFT_PAREN,    // 14
    TOK_RIGHT_PAREN,   // 15
    TOK_LEFT_BRACE,    // 16
    TOK_RIGHT_BRACE,   // 17
    TOK_SEMICOLON,     // 18
    TOK_UNKNOWN,       // 19
    TOK_EOF,           // 20
} TokenType;

typedef struct
{
    TokenType type;
    string val;
} Token;

class ASTNode
{
public:
    virtual ~ASTNode() = default;
};

class RootNode : public ASTNode
{
public:
    vector<shared_ptr<ASTNode>> nodes;

    void addNode(shared_ptr<ASTNode> node)
    {
        nodes.push_back(node);
    }
};

class DeclarationNode : public ASTNode
{
public:
    string name;
    DeclarationNode(string &name) : name(name) {}
};

class LiteralNode : public ASTNode
{
public:
    int value;
    LiteralNode(const int &val) : value(val) {}
};

class VariableNode : public ASTNode
{
public:
    string name;
    VariableNode(string &name) : name(name) {}
};
class BinExprNode : public ASTNode
{
public:
    string op;
    shared_ptr<ASTNode> LHS, RHS;

    BinExprNode(string Op, shared_ptr<ASTNode> LHS,
                shared_ptr<ASTNode> RHS)
        : op(Op), LHS(move(LHS)), RHS(move(RHS)) {}
};

class AssignmentNode : public ASTNode
{
public:
    string name;
    shared_ptr<ASTNode> exp;

    AssignmentNode(string variable, shared_ptr<ASTNode> expression)
        : name(variable), exp(expression) {}
};

class IfNode : public ASTNode
{
public:
    shared_ptr<ASTNode> Condition;
};
static vector<Token> tokens;

void addToken(TokenType t, string s)
{
    tokens.push_back({.type = t, .val = s});
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Invalid Syntax.....\nSyntax: <executable> <file>\n";
        return 1;
    }
    ifstream file(argv[1]);
    if (!file)
    {
        perror("Failed to open file");
        return 1;
    }

    //////////////////////////////////////////////////////////
    // Lexer
    //////////////////////////////////////////////////////////

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
        case '!':
            if (*next(c, 1) == '=')
                addToken(TOK_BANG_EQUAL, "!=");
            else
                addToken(TOK_BANG, "!");
            c++;
            break;
        case '=':
            if (*next(c, 1) == '=')
            {
                addToken(TOK_EQUAL_EQUAL, "==");
                c++;
            }
            else
                addToken(TOK_EQUAL, "=");
            break;
        case '<':
            if (*next(c, 1) == '=')
            {
                addToken(TOK_LESS_EQUAL, "<=");
                c++;
            }
            else
                addToken(TOK_LESS, "<");
            break;
        case '>':
            if (*next(c, 1) == '=')
            {
                addToken(TOK_GREATER_EQUAL, ">=");
                c++;
            }
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

    ///////////////////////////////////////////////////////////////////////////
    // Parser &  AST
    ////////////////////////////////////////////////////////////////////////////

    auto program = make_shared<RootNode>();

    for (auto token = tokens.begin(); token != tokens.end(); token++)
    {
        // Declaration
        if (token->type == TOK_INTEGER)
        {
            token++;
            if (token != tokens.end() && token->type == TOK_IDENTIFIER)
            {
                string var_name = token->val;
                token++;
                if (token != tokens.end() && token->type == TOK_SEMICOLON)
                {
                    program->addNode(make_shared<DeclarationNode>(var_name));
                    std::cout << "Added a declaration node\n";
                }
            }
        }

        // Assignment
        else if (token->type == TOK_IDENTIFIER)
        {
            string var_name = token->val;
            token++;
            if (token != tokens.end() && token->type == TOK_EQUAL)
            {
                token++;
                shared_ptr<ASTNode> lhs;
                if (token != tokens.end())
                {
                    if (token->type == TOK_NUMBER)
                        lhs = make_shared<LiteralNode>(stoi(token->val));
                    else if (token->type == TOK_IDENTIFIER)
                        lhs = make_shared<VariableNode>(token->val);
                    token++;

                    if (token->type == TOK_PLUS | token->type == TOK_MINUS)
                    {
                        string op = token->val;
                        token++;
                        shared_ptr<ASTNode> rhs;
                        if (token->type == TOK_NUMBER)
                            rhs = make_shared<LiteralNode>(stoi(token->val));
                        else if (token->type == TOK_IDENTIFIER)
                            rhs = make_shared<VariableNode>(token->val);
                        auto expression = make_shared<BinExprNode>(op, lhs, rhs);
                        token++;
                        if (token != tokens.end() && token->type == TOK_SEMICOLON)
                        {
                            program->addNode(make_shared<AssignmentNode>(var_name, expression));
                            cout << "Added an assignment node\n";
                        }
                    }
                }

                // Variable Assignment
                else if (token != tokens.end() && token->type == TOK_SEMICOLON)
                {
                    program->addNode(make_shared<AssignmentNode>(var_name, lhs));
                    cout << "Added an assignment Node\n";
                }
            }
        }

        else if (token->type == TOK_IF)
        {
            token++;
            if (token != tokens.end() && token->type == TOK_LEFT_PAREN)
            {
                token++;
                if (token->type == TOK_IDENTIFIER || token->type == TOK_NUMBER)
                {
                    token++;
                    if (token->type == TOK_EQUAL_EQUAL ||
                        token->type == TOK_BANG_EQUAL ||
                        token->type == TOK_LESS ||
                        token->type == TOK_LESS_EQUAL ||
                        token->type == TOK_GREATER ||
                        token->type == TOK_GREATER_EQUAL)
                    {
                        token++;
                        if (token->type == TOK_NUMBER || token->type == TOK_IDENTIFIER)
                        {
                            token++;
                            if (token->type == TOK_RIGHT_PAREN)
                            {
                                token++;
                                cout << "Found Conditional\nt";
                            }
                        }
                    }
                }
            }
        }

        else if (token->type == TOK_EOF)
        {
            break;
        }
        else
        {
            cout << "Unexpected token: " << token->val << '\n';
        }
    }

    // for(auto node: program->nodes){
    //     cout<<node;
    // }
    // for (auto tok : tokens)
    //     cout << tok.type << ':' << tok.val << '\n';
    // return 0;
}