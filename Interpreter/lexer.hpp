#include <iostream>
#include <string>

enum TokenTypes{
    IDENTIFIER, ASSIGN, PLUS, MINUS, MUL, DIV, SEMICOLON, COMMA, NEWLINE, END_OF_FILE,
    GREATER, LESS, GR_EQUAL, LS_EQUAL, IS_EQUAL_TO, NOT_EQUAL_TO, MINUS_EQUAL, PLUS_EQUAL, MUL_EQUAL, DIV_EQUAL, DECREASE, INCREASE, NOT, OR, AND,
    IF, ELSE, ELSE_IF, OUTPUT, INPUT, IMPORT, WHILE, FN, RETURN,
    TYPE_INT, TYPE_FLOAT, TYPE_STRING, TYPE_BOOL, TYPE_CHAR, INT, FLOAT, STRING, BOOLEAN, CHAR,
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET
};

struct Token{
    TokenTypes type;
    std::string value;
};

class Lexer{
    private:
        std::string source;
        size_t cursor = 0;
        int pos = 1;
        int row = 1;

        void advance(){
            while (cursor < source.length() && isspace(source[cursor])){
                if (source[cursor] == '\n'){
                    row++;
                    pos = 1;
                }else{
                    pos++;
                }
                cursor++;            
            }
        }
    public:
        Lexer(std::string src) : source(src){}

        char current_char(){
            if (cursor >= source.length()) return '\0';

            return source[cursor];
        }

        Token next_token(){
            while(cursor < source.length() && isspace(current_char())) advance();

            char c = current_char();

            switch (c){
                case ';': advance(); return {SEMICOLON, ";"};
                case '(': advance(); return {LPAREN, "("};
                case ')': advance(); return {RPAREN, ")"};
                case '{': advance(); return {LBRACE, "{"};
                case '}': advance(); return {RBRACE, "}"};
                case '[': advance(); return {LBRACKET, "["};
                case ']': advance(); return {RBRACKET, "]"};
                case ',': advance(); return {COMMA, ","};
            }

            
            if (isdigit(c) || c == '.'){
                std::string num;
                bool has_dot = false;
                bool has_digit = false;

                while (isdigit(current_char()) || current_char() == '.'){
                    if (current_char() == '.'){
                        if (has_dot){
                            std::cout<<"[InvalidNumberError]: Number must contain only one decimal point\n";
                            exit(1);
                        }
                    has_dot = true;
                    }
                    else has_digit = true;

                    num += source[cursor++];
                }

                if (!has_digit){
                    std::cout<<"[InvalidNumberError]: Invalid number format\n";
                    exit(1);
                }
                if (has_dot) return {FLOAT, num};
                else return {INT, num};
            }

            else if (isalpha(c)){
                std::string id;
                while(isalnum(current_char()) || current_char() == '_'){
                    id += source[cursor++];
                }

                if (id == "if") return {IF, id};
                else if (id == "elif") return {ELSE_IF, id};
                else if (id == "else") return {ELSE, id};
                else if (id == "output") return {OUTPUT, id};
                else if (id == "input") return {INPUT, id};
                else if (id == "import") return {IMPORT, id};
                else if (id == "while") return {WHILE, id};
                else if (id == "return") return {RETURN, id};
                else if (id == "fn") return {FN, id};
                else if (id == "int") return {TYPE_INT, id};
                else if (id == "float") return {TYPE_FLOAT, id};
                else if (id == "boolean") return {TYPE_BOOL, id};
                else if (id == "string") return {TYPE_STRING, id};
                else if (id == "char") return {TYPE_CHAR, id};
                else if (id == "true") return {BOOLEAN, "true"};
                else if (id == "false") return {BOOLEAN, "false"};
                else return {IDENTIFIER, id};
            }

            else if (c == '#'){
                advance();
                while (current_char() != '\n' && current_char() != '\0') advance();
                return next_token();
            }

            else if (c == '>'){
                advance();
                if (current_char() == '='){
                    advance();
                    return {GR_EQUAL, ">="};
                }
                return {GREATER, ">"};
            }

            else if (c == '<'){
                advance();
                if (current_char() == '='){
                    advance();
                    return {LS_EQUAL, "<="};
                }
                return {LESS, "<"};
            }

            else if (c == '='){
                advance();
                if (current_char() == '='){
                    advance();
                    return {IS_EQUAL_TO, "=="};
                }
                return {ASSIGN, "="};
            }

            else if (c == '!'){
                advance();
                if (current_char() == '='){
                    advance();
                    return {NOT_EQUAL_TO, "!="};
                }
                return {NOT, "!"};
            }

            else if (c == '|'){
                advance();
                if (current_char() == '|'){
                    advance();
                    return {OR, "||"};
                }
                std::cout<<"[UnexpectedTokenError]: Unexpected token '|', perhaps you have meant '||'?";
                exit(1);
            }

            else if (c == '&'){
                advance();
                if (current_char() == '&'){
                    advance();
                    return {AND, "&&"};
                }
                std::cout<<"[UnexpectedTokenError]: Unexpected token '&', perhaps you have meant '&&'?";
                exit(1);
            }

            else if (c == '-'){
                advance();
                if (current_char() == '='){
                    advance();
                    return {MINUS_EQUAL, "-="};
                }
                else if (current_char() == '-'){
                    advance();
                    return {DECREASE, "--"};
                }
                return {MINUS, "-"};
            }

            else if (c == '+'){
                advance();
                if (current_char() == '='){
                    advance();
                    return {PLUS_EQUAL, "+="};
                }
                else if (current_char() == '+'){
                    advance();
                    return {INCREASE, "++"};
                }
                return {PLUS, "+"};
            }

            else if (c == '*'){
                advance();
                if (current_char() == '='){
                    advance();
                    return {MUL_EQUAL, "*="};
                }
                return {MUL, "*"};
            }

            else if (c == '/'){
                advance();
                if (current_char() == '='){
                    advance();
                    return {DIV_EQUAL, "/="};
                }
                return {DIV, "/"};
            }

            else if (c == '"'){
                advance();
                std::string str_lit;

                while (current_char() != '"' && current_char() != '\0') str_lit += source[cursor++];
                
                if (current_char() == '"') {advance(); return {STRING, str_lit};}
                
                else {std::cout<<"[MissingSymbolError]: string was not closed with \""; exit(1);}
            }

            else if (c == '\''){
                advance();
                char char_lit = current_char();
                if (std::string(1, char_lit) == ""){
                    std::cout<<"[CharError]: char literal cannot be empty";
                    exit(1);
                }
                advance();
                if (current_char() != '\''){
                    std::cout<<"[CharError]: char type must contain only one symbol and closed with '";
                    exit(1);
                }
                else return {CHAR, std::string(1, char_lit)};
            }

            else if (c == '\0') return {END_OF_FILE, ""};
            
            else{
                std::cout<<"[UnexpectedTokenError]: Unknown character: '"<<c<<"'\n";
                exit(1);
            }
        }

        int GetPos() const {
            return pos;
        }

        int GetRow() const {
            return row;
        }

        int GetCursor() const {
            return cursor;
        }

        void SetPos(int p){
            pos = p;
        }
};
