#include "lexer.hpp"
#include <map>

using namespace std;

class Parser{
    private:
        Lexer lexer;
        Token current_token;
        map<string, long double> memory;
    public:
        Parser(Lexer l) : lexer(l){
            current_token = lexer.next_token();
        }

        string GetTokenName(TokenTypes type){
            switch(type){
                case IDENTIFIER: return "IDENTIFIER";
                case NUMBER: return "NUMBER";
                case ASSIGN: return "ASSIGN";
                case PLUS: return "PLUS";
                case MINUS: return "MINUS";
                case MUL: return "MULTIPLICATION";
                case DIV: return "DIVISION";
                case GREATER: return "GREATER";
                case LESS: return "LESS";
                case GR_EQUAL: return "GR_EQUAL";
                case LS_EQUAL: return "LS_EQUAL";
                case IS_EQUAL_TO: return "IS_EQUAL_TO";
                case NOT_EQUAL_TO: return "NOT_EQUAL_TO";
                case MINUS_EQUAL: return "MINUS_EQUAL";
                case PLUS_EQUAL: return "PLUS_EQUAL";
                case MUL_EQUAL: return "MUL_EQUAL";
                case DIV_EQUAL: return "DIV_EQUAL";
                case DECREASE: return "DECREASE";
                case INCREASE: return "INCREASE";
                case NOT: return "NOT";
                case OR: return "OR";
                case AND: return "AND";
                case SEMICOLON: return "SEMICOLON";
                case END_OF_FILE: return "END_OF_FILE";
                case IF: return "IF";
                case ELSE: return "ELSE";
                case ELSE_IF: return "ELSE_IF";
                case OUTPUT: return "OUTPUT";
                case INPUT: return "INPUT";
                case LPAREN: return "LPAREN";
                case RPAREN: return "RPAREN";
                case LBRACE: return "LBRACE";
                case RBRACE: return "RBRACE";
                default: return "UNKNOWN_TOKEN";
            }
        }

        void eat(TokenTypes type){
            if (current_token.type == type){
                current_token = lexer.next_token();
            }
            else{
                cout<<"[UnexpectedTokenError]: Expected "<<GetTokenName(type)<<" but got "<<GetTokenName(current_token.type);
                exit(1);
            }
        }

        long double parse_factor(){
            if (current_token.type == MINUS){
                eat(MINUS);
                return -parse_factor();
            }
            else if (current_token.type == NUMBER){
                long double value = stold(current_token.value);
                eat(NUMBER);
                return value;
            }
            else if (current_token.type == IDENTIFIER){
                string name = current_token.value;
                eat(IDENTIFIER);
                if(memory.count(name)){
                    return memory[name];
                }
                else{
                    cout<<"[RuntimeError]: Variable "<<name<<" is not initialized";
                    exit(1);
                }
            }
            else if (current_token.type == LPAREN){
                eat(LPAREN);
                long double value = parse_expression();
                eat(RPAREN);
                return value;
            }
            else{
                cout<<"[UnexpectedTokenError]: Unexpected "<<GetTokenName(current_token.type);
                exit(1);
            }
        }

        long double parse_term(){
            long double value = parse_factor();

            while (current_token.type == MUL || current_token.type == DIV){
                TokenTypes op = current_token.type;
                eat(op);

                long double right = parse_factor();

                if (op == MUL) value *= right;
                else value /= right;
            }

            return value;
        }

        long double parse_expression(){
            long double value = parse_term();

            while (current_token.type == PLUS || current_token.type == MINUS){
                TokenTypes op = current_token.type;
                eat(op);

                long double right = parse_term();

                if (op == PLUS) value += right;
                else value -= right;
            }

            return value;
        }

        long double parse_comparison(){
            long double left = parse_expression();

            if (current_token.type == GREATER){
                eat(GREATER);
                return left > parse_expression();
            }

            else if (current_token.type == LESS){
                eat(LESS);
                return left < parse_expression();
            }

            else if (current_token.type == GR_EQUAL){
                eat(GR_EQUAL);
                return left >= parse_expression();
            }

            else if (current_token.type == LS_EQUAL){
                eat(LS_EQUAL);
                return left <= parse_expression();
            }

            else if (current_token.type == IS_EQUAL_TO){
                eat(IS_EQUAL_TO);
                return left == parse_expression();
            }

            else if (current_token.type == NOT_EQUAL_TO){
                eat(NOT_EQUAL_TO);
                return left != parse_expression();
            }

            return left;
        }

        long double parse_logic(){
            long double left = parse_comparison();

            while (current_token.type == AND || current_token.type == OR){
                TokenTypes op = current_token.type;

                eat(op);
                
                long double right = parse_comparison();

                if (op == AND) left = left && right;
                else left = left || right;
            }

            return left;
        }

        void skip_block(){
            int brace_depth = 1;
            while (brace_depth > 0 && current_token.type != END_OF_FILE){
                current_token = lexer.next_token();
                if (current_token.type == LBRACE) brace_depth++;
                else if (current_token.type == RBRACE) brace_depth--;
            }
        }

        void run(){

            bool last_if_executed = false;
            bool last_else_if_executed = false;

            while (current_token.type != END_OF_FILE){
                if (current_token.type == IDENTIFIER){
                    string var_name = current_token.value;
                    eat(IDENTIFIER);
                    eat(ASSIGN);
                    long double val = parse_expression();
                    eat(SEMICOLON);

                    memory[var_name] = val;
                    cout<<"[SYSTEM]: Successfully write data into "<<"'"<<var_name<<"'"<<endl;
                }

                else if (current_token.type == OUTPUT){
                    eat(OUTPUT);
                    eat(LPAREN);
                    if (current_token.type == IDENTIFIER){
                        string var_name = current_token.value;
                        eat(IDENTIFIER);

                        if (memory.count(var_name)){
                            cout<<"Programm's stdout: "<<memory[var_name]<<endl;
                        }
                        else{
                            cout<<"[RuntimeError]: Variable "<<var_name<<" is not initialized";
                            exit(1);
                        }
                    }

                    else if (current_token.type == NUMBER){
                        string val = current_token.value;
                        eat(NUMBER);
                        cout<<"Programm's stdout: "<<stold(val)<<endl;
                    }

                    else{
                        cout<<"[SyntaxError]: output() function expects "<<GetTokenName(NUMBER)<<" or "<<GetTokenName(IDENTIFIER)<<", but got "<<GetTokenName(current_token.type);
                        exit(1);
                    }
                    eat(RPAREN);
                    eat(SEMICOLON);
                }

                else if (current_token.type == INPUT){
                    eat(INPUT);
                    eat(LPAREN);
                    if(current_token.type != IDENTIFIER){
                        cout<<"[SyntaxError]: input() function expects "<<GetTokenName(IDENTIFIER)<<", but got "<<GetTokenName(current_token.type);
                        exit(1);
                    }
                    
                    else{
                        string var_name = current_token.value;
                        eat(IDENTIFIER);
                        long double user_input;
                        cout<<"[INPUT]: Enter the value for "<<var_name<<": ";
                        cin>>user_input;
                        memory[var_name] = user_input;
                        cout<<"[SYSTEM]: Succesfully written data into "<<"'"<<var_name<<"'\n";
                    }
                    eat(RPAREN);
                    eat(SEMICOLON);
                }

                else if (current_token.type == IF){
                    eat(IF);
                    eat(LPAREN);
    
                    long double condition_value = parse_logic();

                    eat(RPAREN);
                    eat(LBRACE);
    
                    if (condition_value) last_if_executed = true;
                    else {
                        skip_block();
                    }

                    eat(RBRACE);
                    last_else_if_executed = false;
                }

                else if (current_token.type == ELSE_IF){
                    eat(ELSE_IF);
                    eat(LPAREN);

                    long double condition_value = parse_logic();

                    eat(RPAREN);
                    eat(LBRACE);

                    if (!last_if_executed && !last_else_if_executed){
                        cout << "[InvalidMemberStartError]: 'else if/else' could not be executed without 'if' member declaration";
                        exit(1);
                    }

                    if (!last_if_executed && condition_value){
                        last_else_if_executed = true;
                        last_if_executed = true;
                    } 
                    else {
                        skip_block();
                    }

                    eat(RBRACE);
                }

                else if (current_token.type == ELSE){
                    eat(ELSE);
                    eat(LBRACE);

                    if (!last_if_executed && !last_else_if_executed){
                        cout << "[InvalidMemberStartError]: 'else if/else' could not be executed without 'if' member declaration";
                        exit(1);
                    }

                    if (!last_if_executed && !last_else_if_executed){
                        last_if_executed = true;
                    }
                    else {
                        skip_block();
                    }

                    eat(RBRACE); 
                }
            }
        }
};
