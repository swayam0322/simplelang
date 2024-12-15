#include <bits/stdc++.h>
#include <memory>

using namespace std;

typedef enum
{
    TOK_INTEGER,
    TOK_IDENTIFIER,
    TOK_NUMBER,
    TOK_ASSIGN,
    TOK_PLUS,
    TOK_MINUS,
    TOK_IF,
    TOK_EQUAL,
    TOK_NOT_EQUAL,
    TOK_GREATER,
    TOK_LESS,
    TOK_LEFT_PAREN,
    TOK_RIGHT_PAREN,
    TOK_LEFT_BRACE,
    TOK_RIGHT_BRACE,
    TOK_SEMICOLON,
    TOK_UNKNOWN,
    TOK_EOF,
} TokenType;

typedef struct
{
    TokenType type;
    string val;
} Token;

static vector<Token> tokens;
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

class ConditionNode : public ASTNode
{
public:
    string comp;
    shared_ptr<ASTNode> LHS, RHS;

    ConditionNode(string comp, shared_ptr<ASTNode> lhs, shared_ptr<ASTNode> rhs)
        : comp(comp), LHS(move(lhs)), RHS(move(rhs)) {}
};

class BlockNode : public RootNode
{
public:
    vector<shared_ptr<ASTNode>> nodes;

    void addNode(shared_ptr<ASTNode> stmt)
    {
        nodes.push_back(stmt);
    }
};

class ConditionalNode : public ASTNode
{
public:
    shared_ptr<ASTNode> condition, block;
    ConditionalNode(shared_ptr<ASTNode> cond, shared_ptr<ASTNode> blk) : condition(cond), block(blk) {}
};

bool parseDeclaration(vector<Token>::iterator &token, vector<Token>::iterator end, shared_ptr<RootNode> program)
{
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
                // std::cout << "Added a declaration node\n";
                return true;
            }
        }
    }
    return false;
}

shared_ptr<ASTNode> parseExpression(vector<Token>::iterator &token, vector<Token>::iterator end, shared_ptr<RootNode> program)
{
    shared_ptr<ASTNode> lhs;
    if (token->type == TOK_NUMBER)
        lhs = make_shared<LiteralNode>(stoi(token->val));
    else if (token->type == TOK_IDENTIFIER)
        lhs = make_shared<VariableNode>(token->val);
    token++;

    // Expression assignment
    if (token->type == TOK_PLUS || token->type == TOK_MINUS)
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
        return expression;
    }
    return lhs;
}

bool parseAssignment(vector<Token>::iterator &token, vector<Token>::iterator end, shared_ptr<RootNode> program)
{
    if (token->type == TOK_IDENTIFIER)
    {
        string var_name = token->val;
        token++;
        if (token != tokens.end() && token->type == TOK_ASSIGN)
        {
            token++;
            auto expression = parseExpression(token, tokens.end(), program);
            if (token != tokens.end() && token->type == TOK_SEMICOLON)
            {
                program->addNode(make_shared<AssignmentNode>(var_name, expression));
                // cout << "Added an assignment Node\n";
                return true;
            }
        }
    }
    return false;
}

bool parseConditional(vector<Token>::iterator &token, vector<Token>::iterator end, shared_ptr<RootNode> program);

shared_ptr<BlockNode> parseBlock(vector<Token>::iterator &token, vector<Token>::iterator end, shared_ptr<BlockNode> blk)
{
    while (token->type != TOK_RIGHT_BRACE)
    {
        if (parseDeclaration(token, tokens.end(), blk))
            token++;
        else if (parseAssignment(token, tokens.end(), blk))
            token++;
        else if (parseConditional(token, tokens.end(), blk))
            token++;
        else if (token->type == TOK_EOF)
        {
            break;
        }
        else
        {
            // cout << "Unexpected token: " << '\n';
            token++;
        }
    }
    return blk;
}

bool parseConditional(vector<Token>::iterator &token, vector<Token>::iterator end, shared_ptr<RootNode> program)
{
    if (token->type == TOK_IF)
    {
        token++;
        if (token != tokens.end() && token->type == TOK_LEFT_PAREN)
        {
            token++;
            shared_ptr<ASTNode> lhs;
            if (token->type == TOK_IDENTIFIER || token->type == TOK_NUMBER)
            {
                if (token->type == TOK_NUMBER)
                    lhs = make_shared<LiteralNode>(stoi(token->val));
                else if (token->type == TOK_IDENTIFIER)
                    lhs = make_shared<VariableNode>(token->val);
                token++;
                if (token->type == TOK_EQUAL ||
                    token->type == TOK_NOT_EQUAL ||
                    token->type == TOK_LESS ||
                    token->type == TOK_GREATER)
                {
                    string comp = token->val;
                    token++;
                    shared_ptr<ASTNode> rhs;
                    if (token->type == TOK_NUMBER)
                        rhs = make_shared<LiteralNode>(stoi(token->val));
                    else if (token->type == TOK_IDENTIFIER)
                        rhs = make_shared<VariableNode>(token->val);
                    token++;
                    if (token->type == TOK_RIGHT_PAREN)
                    {
                        auto condition = make_shared<ConditionNode>(comp, lhs, rhs);
                        token++;
                        if (token->type == TOK_LEFT_BRACE)
                        {
                            token++;
                            // cout << "New Block opened"<<token->val<<'\n';
                            auto block = parseBlock(token, tokens.end(), make_shared<BlockNode>());
                            program->addNode(make_shared<ConditionalNode>(condition, block));
                            token++;
                            // cout << "Added Conditional Block"<<"\n";
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

vector<Token> tokenize(ifstream &file)
{
    vector<Token> tokens;
    auto addToken = [&tokens](TokenType t, string s)
    {
        tokens.push_back({.type = t, .val = s});
    };
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
            addToken(TOK_NOT_EQUAL, "!");
            c++;
            break;
        case '<':
            addToken(TOK_LESS, "<");
            c++;
            break;
        case '>':
            addToken(TOK_GREATER, ">");
            c++;
            break;
        case '=':
            addToken(TOK_EQUAL, "=");
            c++;
            break;
        case ':':
            addToken(TOK_ASSIGN, ":");
            c++;
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
                if (buffer == "var")
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
                addToken(TOK_UNKNOWN, "$");
        }
    }
    addToken(TOK_EOF, "\0");
    return tokens;
}

void printTree(shared_ptr<RootNode> program)
{
    for (auto node : program->nodes)
    {
        if (auto dec_node = dynamic_pointer_cast<DeclarationNode>(node))
            cout << "Declaration(" << dec_node->name << ")\n";
        else if (auto assign_node = dynamic_pointer_cast<AssignmentNode>(node))
        {
            cout << "Assignment(" << assign_node->name << ',';
            if (auto exp_node = dynamic_pointer_cast<BinExprNode>(assign_node->exp))
            {
                cout << "Expression(";
                if (auto lhs = dynamic_pointer_cast<LiteralNode>(exp_node->LHS))
                    cout << "Literal(" << lhs->value << ')';
                else if (auto lhs = dynamic_pointer_cast<VariableNode>(exp_node->LHS))
                    cout << "Variable(" << lhs->name << ')';
                cout << exp_node->op;
                if (auto rhs = dynamic_pointer_cast<LiteralNode>(exp_node->RHS))
                    cout << "Literal(" << rhs->value << ')';
                else if (auto rhs = dynamic_pointer_cast<VariableNode>(exp_node->RHS))
                    cout << "Variable(" << rhs->name << ')';
                cout << ")";
            }
            else if (auto lit_node = dynamic_pointer_cast<LiteralNode>(assign_node->exp))
                cout << "Literal(" << lit_node->value << ")";
            else if (auto var_node = dynamic_pointer_cast<VariableNode>(assign_node->exp))
                cout << "Variable(" << var_node->name << ")";
            cout << ")\n";
        }
        else if (auto cond_node = dynamic_pointer_cast<ConditionalNode>(node))
        {
            auto condition = dynamic_pointer_cast<ConditionNode>(cond_node->condition);
            cout << "Condition(";
            if (auto lhs = dynamic_pointer_cast<LiteralNode>(condition->LHS))
                cout << "Literal(" << lhs->value << ')';
            else if (auto lhs = dynamic_pointer_cast<VariableNode>(condition->LHS))
                cout << "Variable(" << lhs->name << ')';
            cout << ' ' << condition->comp << ' ';
            if (auto rhs = dynamic_pointer_cast<LiteralNode>(condition->RHS))
                cout << "Literal(" << rhs->value << ')';
            else if (auto rhs = dynamic_pointer_cast<VariableNode>(condition->RHS))
                cout << "Variable(" << rhs->name << ')';
            cout << ")\n";
            auto block = dynamic_pointer_cast<BlockNode>(cond_node->block);
            printTree(block);
        }
        else
            cout << "Unknown Node Type\n";
    }
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
    tokens = tokenize(file);
    auto program = make_shared<RootNode>();
    for (auto token = tokens.begin(); token->type != TOK_EOF; token++)
    {
        if (parseDeclaration(token, tokens.end(), program))
            continue;
        else if (parseAssignment(token, tokens.end(), program))
            continue;
        else if (parseConditional(token, tokens.end(), program))
            continue;
        else if(token->type != TOK_EOF) break;
        else{
            cout<<"Unexpected token "<< token->val;
            break;
        }
    }
    printTree(program);
    // for(auto tok : tokens){
    //     cout<<tok.val<<' ';
    // }
}