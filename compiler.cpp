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
                    token->type == TOK_NOT_EQUAL)

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

static int blockCount = 0;
static vector<string> symbols;

void generate(shared_ptr<RootNode> program, string label = "_start")
{
    ofstream file;
    file.open ("generated.asm");
    file<<".text\n\n";
    file<<label<<":\n";
    for (auto node : program->nodes)
    {
        if (auto dec_node = dynamic_pointer_cast<DeclarationNode>(node))
            symbols.push_back(dec_node->name); 
        else if (auto assign_node = dynamic_pointer_cast<AssignmentNode>(node))
        {
            if (auto exp_node = dynamic_pointer_cast<BinExprNode>(assign_node->exp))
            {
                if (auto rhs = dynamic_pointer_cast<LiteralNode>(exp_node->RHS))
                    file<<"ldi A "<< rhs->value<<'\n';
                else if (auto rhs = dynamic_pointer_cast<VariableNode>(exp_node->RHS))
                    file<<"lda %"<< rhs->name << '\n';
                file<<"mov B A\n";
                if (auto lhs = dynamic_pointer_cast<LiteralNode>(exp_node->LHS))
                    file<<"ldi A "<<lhs->value<<'\n';
                else if (auto lhs = dynamic_pointer_cast<VariableNode>(exp_node->LHS))
                    file<<"lda %"<< lhs->name << '\n';
                if(exp_node->op == "+")
                    file<<"add\n";
                else 
                    file<<"sub\n";
                file<<"sta %"<<assign_node->name<<'\n';
            }
            else if (auto lit_node = dynamic_pointer_cast<LiteralNode>(assign_node->exp))
                file<<"ldi A "<< lit_node->value<<'\n'<<"sta %"<<assign_node->name<<'\n';
            else if (auto var_node = dynamic_pointer_cast<VariableNode>(assign_node->exp))
                file<<"lda %"<< var_node->name << '\n'<<"sta %"<<assign_node->name<<'\n';
        }
        else if (auto cond_node = dynamic_pointer_cast<ConditionalNode>(node))
        {
            auto condition = dynamic_pointer_cast<ConditionNode>(cond_node->condition);
            if (auto rhs = dynamic_pointer_cast<LiteralNode>(condition->RHS))
                file<<"ldi A "<< rhs->value<<'\n';
            else if (auto rhs = dynamic_pointer_cast<VariableNode>(condition->RHS))
                file<<"lda %"<< rhs->name << '\n';
            file<<"mov B A\n";
            if (auto lhs = dynamic_pointer_cast<LiteralNode>(condition->LHS))
                file<<"ldi A "<<lhs->value<<'\n';
            else if (auto lhs = dynamic_pointer_cast<VariableNode>(condition->LHS))
                file<<"lda %"<< lhs->name << '\n';
            file<<"cmp\n";
            if(condition->comp == "=")
                    file<<"jz %BLOCK"<<blockCount;
                else 
                    file<<"jnz %BLOCK"<<blockCount;
            auto block = dynamic_pointer_cast<BlockNode>(cond_node->block);
            generate(block, "BLOCK"+blockCount);
            blockCount++;
        }
        else
            cerr << "Unknown Node Type\n";
    }
    file<<"\nEND:\nhlt\n";
    file<<"\n.data\n";
    for(auto i:symbols)
        file<<i<<"=0\n";
}

void parse(vector<Token> &tokens, shared_ptr<RootNode> program){
    for (auto token = tokens.begin(); token->type != TOK_EOF; token++)
    {
        if (parseDeclaration(token, tokens.end(), program))
            continue;
        else if (parseAssignment(token, tokens.end(), program))
            continue;
        else if (parseConditional(token, tokens.end(), program))
            continue;
        else if (token->type != TOK_EOF)
            break;
        else
        {
            cerr << "Unexpected token " << token->val;
            break;
        }
    }
}


int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Invalid Syntax.....\nSyntax: <executable> <file>\n";
        return 1;
    }
    ifstream file(argv[1]);
    if (!file)
    {
        perror("Failed to open file");
        return 1;
    }
    tokens = tokenize(file);
    auto root = make_shared<RootNode>();
    parse(tokens, root);
    generate(root);

    // for(auto tok : tokens){
    //     cout<<tok.val<<' ';
    // }
}