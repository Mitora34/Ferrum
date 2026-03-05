#include <iostream>
#include <string>

using namespace std;

enum TokenTypes{
    IDENTIFIER, NUMBER, ASSIGN, PLUS, MINUS, MUL, DIV, SEMICOLON, END_OF_FILE,
    GREATER, LESS, GR_EQUAL, LS_EQUAL, IS_EQUAL_TO, NOT_EQUAL_TO, MINUS_EQUAL, PLUS_EQUAL, MUL_EQUAL, DIV_EQUAL, DECREASE, INCREASE, NOT,
    IF, ELSE, ELSE_IF, OUTPUT, INPUT,
    LPAREN, RPAREN, LBRACE, RBRACE
};

struct Token{
    TokenTypes type;
    string value;
};

class Lexer{
    private:
        string source;
        int pos = 0;
    public:
        Lexer(string src) : source(src){}

        char current_char(){
            if (pos >= source.length()){
                return '\0';
            }
            return source[pos];
        }

        Token next_token(){
            while(isspace(current_char())){
                pos++;
            }

            char c = current_char();

            
            if (isdigit(c) || c == '.'){
                string num;
                bool has_dot = false;
                bool has_digit = false;

                while (isdigit(current_char()) || current_char() == '.'){
                    if (current_char() == '.'){
                        if (has_dot){
                            cout<<"[InvalidNumberError]: Number must contain only one decimal point\n";
                            exit(1);
                        }
                    has_dot = true;
                    }
                    else {
                        has_digit = true;
                    }

                    num += source[pos++];
                }

                if (!has_digit){
                    cout<<"[InvalidNumberError]: Invalid number format\n";
                    exit(1);
                }

                return {NUMBER, num};
            }

            else if (isalpha(c)){
                string id;
                while(isalnum(current_char())){
                    id += source[pos++];
                }

                if (id == "if") return {IF, id};
                else if (id == "elif") return {ELSE_IF, id};
                else if (id == "else") return {ELSE, id};
                else if (id == "output") return {OUTPUT, id};
                else if (id == "input") return {INPUT, id};
                else return {IDENTIFIER, id};
            }

            else if (c == '#'){
                pos++;
                while (current_char() != '\n' && current_char() != '\0') pos++;
                return next_token();
            }

            else if (c == '>'){
                pos++;
                if (current_char() == '='){
                    pos++;
                    return {GR_EQUAL, ">="};
                }
                return {GREATER, ">"};
            }

            else if (c == '<'){
                pos++;
                if (current_char() == '='){
                    pos++;
                    return {LS_EQUAL, "<="};
                }
                return {LESS, "<"};
            }

            else if (c == '='){
                pos++;
                if (current_char() == '='){
                    pos++;
                    return {IS_EQUAL_TO, "=="};
                }
                return {ASSIGN, "="};
            }

            else if (c == '!'){
                pos++;
                if (current_char() == '='){
                    pos++;
                    return {NOT_EQUAL_TO, "!="};
                }
                return {NOT, "!"};
            }

            else if (c == '-'){
                pos++;
                if (current_char() == '='){
                    pos++;
                    return {MINUS_EQUAL, "-="};
                }
                else if (current_char() == '-'){
                    pos++;
                    return {DECREASE, "--"};
                }
                return {MINUS, "-"};
            }

            else if (c == '+'){
                pos++;
                if (current_char() == '='){
                    pos++;
                    return {PLUS_EQUAL, "+="};
                }
                else if (current_char() == '+'){
                    pos++;
                    return {INCREASE, "++"};
                }
                return {PLUS, "+"};
            }

            else if (c == '*'){
                pos++;
                if (current_char() == '='){
                    pos++;
                    return {MUL_EQUAL, "*="};
                }
                return {MUL, "*"};
            }

            else if (c == '/'){
                pos++;
                if (current_char() == '='){
                    pos++;
                    return {DIV_EQUAL, "/="};
                }
                return {DIV, "/"};
            }

            switch (c){
                case ';': pos++; return {SEMICOLON, ";"};
                case '(': pos++; return {LPAREN, "("};
                case ')': pos++; return {RPAREN, ")"};
                case '{': pos++; return {LBRACE, "{"};
                case '}': pos++; return {RBRACE, "}"};
            }

            return {END_OF_FILE, ""};
        }
};