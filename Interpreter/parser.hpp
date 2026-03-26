#include "lexer.hpp"
#include "riopython.hpp"
#include <map>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <variant>
#include <vector>
#include <type_traits>
#include <unordered_set>

using AllValue = std::variant<long double, long long, std::string, bool, char>;

struct Function{
    std::vector<std::string> params;
    std::string body_code;
};
std::map<std::string, Function> user_functions;

class Parser{
    private:
        Lexer lexer;
        Token current_token;
        std::map<std::string, AllValue>& memory;
    public:
        Parser(Lexer l, std::map<std::string, AllValue>& mem) : lexer(l), memory(mem){
            current_token = lexer.next_token();
        }

        //Checks the type and returns it as a string, not like numeration as 0 or 32
        std::string GetTokenName(TokenTypes type){
            switch(type){
                case IDENTIFIER: return "IDENTIFIER";
                case TYPE_INT: return "TYPE_INT";
                case TYPE_FLOAT: return "TYPE_FLOAT";
                case TYPE_BOOL: return "TYPE_BOOL";
                case TYPE_STRING: return "TYPE_STRING";
                case TYPE_CHAR: return "TYPE_CHAR";
                case INT: return "INT";
                case FLOAT: return "FLOAT";
                case STRING: return "STRING";
                case BOOLEAN: return "BOOLEAN";
                case CHAR: return "CHAR";
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
                case COMMA: return "COMMA";
                case END_OF_FILE: return "END_OF_FILE";
                case IF: return "IF";
                case ELSE: return "ELSE";
                case ELSE_IF: return "ELSE_IF";
                case OUTPUT: return "OUTPUT";
                case INPUT: return "INPUT";
                case IMPORT: return "IMPORT";
                case WHILE: return "WHILE";
                case FN: return "FN";
                case LPAREN: return "LPAREN";
                case RPAREN: return "RPAREN";
                case LBRACE: return "LBRACE";
                case RBRACE: return "RBRACE";
                case LBRACKET: return "LBRACKET";
                case RBRACKET: return "RBRACKET";
                default: return "UNKNOWN_TOKEN";
            }
        }

        //Moves the token to the next one if the current token type is matches with expected one
        void eat(TokenTypes type){
            if (current_token.type == type){
                current_token = lexer.next_token();
            }
            else{
                std::cout << "[UnexpectedTokenError]: Expected " << GetTokenName(type) << " but got " << GetTokenName(current_token.type);
                exit(1);
            }
        }

        //Function that parses arithmetical expressions such as unary minus, identifier, and brackets with numbers too
        AllValue parse_factor(){
            if (current_token.type == MINUS){
                eat(MINUS);

                AllValue val;
                if (std::holds_alternative<long long>(val)) return -std::get<long long>(val);
                return -std::get<long double>(val);
            }
            else if (current_token.type == FLOAT){
                long double value = std::stold(current_token.value);
                eat(FLOAT);
                return value;
            }
            else if (current_token.type == INT){
                long long value = std::stoll(current_token.value);
                eat(INT);
                return value;
            }
            else if (current_token.type == IDENTIFIER){
                std::string name = current_token.value;
                eat(IDENTIFIER);
                if(current_token.type == LPAREN){
                    eat(LPAREN);

                    std::vector<AllValue> args;

                    if (current_token.type != RPAREN){
                        args.push_back(parse_expression());

                        while (current_token.type == COMMA){
                            eat(COMMA);
                            args.push_back(parse_expression());
                        }
                    }
                    eat(RPAREN);

                    return call_builtin_math(name, args);
                }
                if(memory.count(name)){
                    return memory[name];
                }
                else{
                    std::cout << "[RuntimeError]: Variable " << name << " is not initialized";
                    exit(1);
                }
            }
            else if (current_token.type == LPAREN){
                eat(LPAREN);
                AllValue value = parse_expression();
                eat(RPAREN);
                return value;
            }
            else{
                std::cout << "[UnexpectedTokenError]: Unexpected " << GetTokenName(current_token.type);
                exit(1);
            }
        }

        //Function that parses multiplication and division arithmetical operators
        AllValue parse_term(){
            AllValue value = parse_factor();

            while (current_token.type == MUL || current_token.type == DIV){
                TokenTypes op = current_token.type;
                eat(op);

                AllValue right = parse_factor();

                value = std::visit([op](auto& left, auto& right) -> AllValue{
                    using T1 = std::decay_t<decltype(left)>;
                    using T2 = std::decay_t<decltype(right)>;

                    if constexpr (std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>){
                        if (op == MUL) return left * right;
                        if (op == DIV && right == 0) {std::cout << "[ZeroDivisionError]: division by zero"; exit(1);}
                        return left / right;
                    }
                    else throw std::runtime_error("Math Error");
                }, value, right);
            }
        }

        //Function that parses summation and subtraction arithmetical operators, but before it checks for multiplication and division
        AllValue parse_expression(){
            AllValue value = parse_term();

            while (current_token.type == PLUS || current_token.type == MINUS){
                TokenTypes op = current_token.type;
                eat(op);

                AllValue right = parse_factor();

                value = std::visit([op](auto& left, auto& right) -> AllValue{
                    using T1 = std::decay_t<decltype(left)>;
                    using T2 = std::decay_t<decltype(right)>;

                    if constexpr (std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>){
                        return (op == PLUS) ? (a + b) : (a - b);
                    }
                    else throw std::runtime_error("Math Error");
                }, value, right);
            }

            return value;
        }

        //Function that parses the comparison and statements
        AllValue parse_comparison(){
            AllValue left = parse_expression();

            switch(current_token.type){
                case GREATER: eat(GREATER); return left > parse_expression();
                case GR_EQUAL: eat(GR_EQUAL); return left >= parse_expression();
                case LESS: eat(LESS); return left < parse_expression();
                case LS_EQUAL: eat(LS_EQUAL); return left <= parse_expression();
                case IS_EQUAL_TO: eat(IS_EQUAL_TO); return left == parse_expression();
                case NOT_EQUAL_TO: eat(NOT_EQUAL_TO); return left != parse_expression();
                default: return left;
            }
        }

        //Parses logical operators such as '||' and '&&'
        AllValue parse_logic(){
            AllValue left = parse_comparison();

            while (current_token.type == AND || current_token.type == OR){
                TokenTypes op = current_token.type;

                eat(op);
                
                AllValue right = parse_comparison();

                if (op == AND) std::get<bool>(left) = std::get<bool>(left) && std::get<bool>(right);
                else std::get<bool>(left) = std::get<bool>(left) || std::get<bool>(right);
            }

            return left;
        }

        //Skips block until the next RBRACE: '}'
        void skip_block(){
            int brace_depth = 1;
            while (brace_depth > 0 && current_token.type != END_OF_FILE){
                current_token = lexer.next_token();
                if (current_token.type == LBRACE) brace_depth++;
                else if (current_token.type == RBRACE) brace_depth--;
            }
        }

        //Function that calls built-in C++ mathematics
        AllValue call_builtin_math(std::string name, std::vector<AllValue> arg){
            int e;
            long double fppart;

            //---------INT---------

            auto i = [](const AllValue& v) {return std::get<long long>(v);};

            // Root and power
            if (name == "sqrt") return sqrt(i(arg[0]));
            if (name == "cbrt") return cbrt(i(arg[0]));
            if (name == "pow") return pow(i(arg[0]), i(arg[1]));

            // Trigonometrical functions
            if (name == "sin") return sin(i(arg[0]));
            if (name == "cos") return cos(i(arg[0]));
            if (name == "tan") return tan(i(arg[0]));

            // Inversed trigonometrical functions
            if (name == "asin") return asin(i(arg[0]));
            if (name == "acos") return acos(i(arg[0]));
            if (name == "atan") return atan(i(arg[0]));
            if (name == "atan2") return atan2(i(arg[0]), i(arg[1]));

            // Hyperbolic functions
            if (name == "sinh") return sinh(i(arg[0]));
            if (name == "cosh") return cosh(i(arg[0]));
            if (name == "tanh") return tanh(i(arg[0]));

            // Inversed hyperbolic functions
            if (name == "asinh") return asinh(i(arg[0]));
            if (name == "acosh") return acosh(i(arg[0]));
            if (name == "atanh") return atanh(i(arg[0]));

            // Round and absolute value
            if (name == "ceil") return ceil(i(arg[0]));
            if (name == "floor") return floor(i(arg[0]));
            if (name == "abs") return fabs(i(arg[0]));

            //Exponents and Logarithms
            if (name == "exp") return exp(i(arg[0]));
            if (name == "fmod") return fmod(i(arg[0]), i(arg[1]));
            if (name == "frexp") return frexp(i(arg[0]), &e);
            if (name == "ldexp") return ldexp(i(arg[0]), i(arg[1]));
            if (name == "log") return log(i(arg[0]));
            if (name == "log10") return log10(i(arg[0]));
            if (name == "modf") return modf(i(arg[0]), &fppart);

            //---------FLOAT---------

            auto f = [](const AllValue& v) {return std::get<long double>(v);};

            // Root and power
            if (name == "sqrtf") return sqrtl(f(arg[0]));
            if (name == "cbrtf") return cbrtl(f(arg[0]));
            if (name == "powf") return powl(f(arg[0]), f(arg[1]));

            // Trigonometrical functions
            if (name == "sinf") return sinl(f(arg[0]));
            if (name == "cosf") return cosl(f(arg[0]));
            if (name == "tanf") return tanl(f(arg[0]));

            // Inversed trigonometrical functions
            if (name == "asinf") return asinl(f(arg[0]));
            if (name == "acosf") return acosl(f(arg[0]));
            if (name == "atanf") return atanl(f(arg[0]));
            if (name == "atan2f") return atan2l(f(arg[0]), f(arg[1]));

            // Hyperbolic functions
            if (name == "sinhf") return sinhl(f(arg[0]));
            if (name == "coshf") return coshl(f(arg[0]));
            if (name == "tanhf") return tanhl(f(arg[0]));

            // Inversed hyperbolic functions
            if (name == "asinhf") return asinhl(f(arg[0]));
            if (name == "acoshf") return acoshl(f(arg[0]));
            if (name == "atanhf") return atanhl(f(arg[0]));

            // Round and absolute value
            if (name == "ceilf") return ceill(f(arg[0]));
            if (name == "floorf") return floorl(f(arg[0]));
            if (name == "absf") return fabsl(f(arg[0]));

            //Exponents and Logarithms
            if (name == "expf") return expl(f(arg[0]));
            if (name == "fmodf") return fmodl(f(arg[0]), f(arg[1]));
            if (name == "frexpf") return frexpl(f(arg[0]), &e);
            if (name == "ldexpf") return ldexpl(f(arg[0]), f(arg[1]));
            if (name == "logf") return logl(f(arg[0]));
            if (name == "log10f") return log10l(f(arg[0]));
            if (name == "modff") return modfl(f(arg[0]), &fppart);

            //---------Extra---------

            if (name == "int"){
                return std::visit([](auto&& arg) -> AllValue{
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>){
                        return std::stoll(arg);
                    }
                    return (long long) arg;
                }, arg[0]);
            }

            if (name == "float"){
                return std::visit([](auto&& arg) -> AllValue{
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>){
                        return std::stold(arg);
                    }
                    return (long double) arg;
                }, arg[0]);
            }

            // My own arithmetical functions
            if (name == "sum") return Mitora34::riopython::Math::Arithmetics::sum(arg[0], arg[1]);
            if (name == "sub") return Mitora34::riopython::Math::Arithmetics::sub(arg[0], arg[1]);
            if (name == "mul") return Mitora34::riopython::Math::Arithmetics::mul(arg[0], arg[1]);
            if (name == "div") return Mitora34::riopython::Math::Arithmetics::div(arg[0], arg[1]);

            // Exception throw, if no matches found 
            std::cout << "[UnknownBuiltinFunctionError]: Unknown function '" << name << "'\n";
            exit(1);
        }

        AllValue call_builtin_file_tools(std::string name, std::vector<AllValue> args){

            auto s = [](const AllValue& v){return std::get<std::string>(v);};

            if (name == "write"){
                std::string path = s(args[0]);
                std::string content = s(args[1]);

                std::ofstream file(path);

                if (file.is_open()){
                    file << content;
                    return true;
                }
                return false;
            }
            if (name == "read"){
                std::string path = s(args[0]);

                std::ifstream file(path);

                if (file.is_open()){
                    std::string str;
                    std::string file_contents;
                    while (std::getline(file, str)){
                        file_contents += str + "\n";
                    }
                    file.close();
                    return file_contents;
                }
                return false;
            }
            if (name == "append"){
                std::string path = s(args[0]);
                std::string contents = s(args[1]);
                
                std::ofstream file(path, std::ios::app);

                if (file.is_open()){
                    file << contents;
                    return true;
                }
                return false;
            }
            if (name == "exists") return std::filesystem::exists(s(args[0]));

            if (name == "clear"){
                std::string path = s(args[0]);
                std::ofstream file(path, std::ios::trunc);
                return file.is_open();
            }
        }

        AllValue execute_user_function(const Function& fn, std::vector<AllValue> args){
            std::map<std::string, AllValue> local_memory;

            for (size_t i = 0; i < fn.params.size() && i < args.size(); ++i){
                local_memory[fn.params[i]] = args[i];
            }

            Lexer fn_lexer(fn.body_code);
            Parser fn_parser(fn_lexer, local_memory);
            return fn_parser.run();
        }

        //Checks the type(int)
        bool is_int(const AllValue& v){
            return std::holds_alternative<long long>(v);
        }
        //Checks the type(float)
        bool is_float(const AllValue& v){
            return std::holds_alternative<long double>(v);
        }
        //Checks the type(bool)
        bool is_bool(const AllValue& v){
            return std::holds_alternative<bool>(v);
        }
        //Checks the type(char)
        bool is_char(const AllValue& v){
            return std::holds_alternative<char>(v);
        }
        //Checks the type(string)
        bool is_string(const AllValue& v){
            return std::holds_alternative<std::string>(v);
        }
        //Function that turns integers and floats to the boolean 
        bool to_bool(AllValue v){
            if (v.index() == 1) return std::get<long long>(v) != 0;         // INT
            if (v.index() == 0) return std::get<long double>(v) != 0.0L;   // FLOAT
            if (v.index() == 3) return std::get<bool>(v);                  // BOOL
            if (v.index() == 2) return !std::get<std::string>(v).empty();  // STRING
            if (v.index() == 4) return std::get<char>(v) != '\0';          // CHAR
    
            return false; // На всякий случай
        }

        //Copy of the run function needed only for the while cycle
        AllValue run_until(TokenTypes type){

            AllValue last_val = 0.0L;
            bool last_if_executed = false;
            bool last_else_if_executed = false;

            while (current_token.type != END_OF_FILE || current_token.type != type){

                if (current_token.type == TYPE_INT){
                    eat(TYPE_INT);
                    std::string var_name = current_token.value;
                    eat(IDENTIFIER);
                    if (current_token.type == SEMICOLON){
                        memory[var_name] = 0;
                        std::cout << "[SYSTEM]: Variable '" << var_name << "' on pos: " << lexer.GetPos() << " on row: " << lexer.GetRow() << " was initialized as 0 successfully\n";
                        eat(SEMICOLON);
                    }
                    else{
                        eat(ASSIGN);
                        AllValue val = parse_expression();
                        if (!is_int(val)){
                            std::cout << "[SyntaxError]: on pos:" << lexer.GetPos() << " on row: " << lexer.GetRow() << " int type can contain only numbers\n";
                            exit(1);
                        }
                        eat(SEMICOLON);

                        memory[var_name] = val;

                        std::cout << "[SYSTEM]: Successfully write data into '" << var_name << "'\n";
                    }
                }

                else if (current_token.type == TYPE_FLOAT){
                    eat(TYPE_FLOAT);
                    std::string var_name = current_token.value;
                    eat(IDENTIFIER);
                    if (current_token.type == SEMICOLON){
                        memory[var_name] = 0.0L;
                        std::cout << "[SYSTEM]: Variable '" << var_name << "' on pos: " << lexer.GetPos() << " on row: " << lexer.GetRow() << " was initialized as 0.0 successfully\n";
                        eat(SEMICOLON);
                    }
                    else{
                        eat(ASSIGN);
                        AllValue val = parse_expression();
                        if (!is_float(val)){
                            std::cout << "[SyntaxError]: on pos:" << lexer.GetPos() << " on row: " << lexer.GetRow() << " float type can contain only numbers\n";
                            exit(1);
                        }
                        eat(SEMICOLON);

                        memory[var_name] = val;
                        
                        std::cout << "[SYSTEM]: Successfully write data into '" << var_name << "'\n";
                    }
                }

                else if (current_token.type == TYPE_STRING){
                    eat(TYPE_STRING);
                    std::string var_name = current_token.value;
                    eat(IDENTIFIER);
                    if (current_token.type == SEMICOLON){
                        memory[var_name] = "";
                        std::cout<<"[SYSTEM]: Variable '" << var_name << "' on pos: " << lexer.GetPos() << " on row: " << lexer.GetRow() << " was initialized as \"\" successfully\n";
                        eat(SEMICOLON);
                    }
                    else{
                        eat(ASSIGN);
                        std::string val = current_token.value;
                        eat(STRING);
                        eat(SEMICOLON);

                        memory[var_name] = val;

                        std::cout << "[SYSTEM]: Successfully write data into '" << var_name << "'\n";
                    }
                }
                
                else if (current_token.type == TYPE_BOOL){
                    eat(TYPE_BOOL);
                    std::string var_name = current_token.value;
                    eat(IDENTIFIER);
                    if (current_token.type == SEMICOLON){
                        memory[var_name] = false;
                        std::cout << "[SYSTEM]: Variable '" << var_name << "' on pos: " << lexer.GetPos() << " on row: " << lexer.GetRow() << " was initialized as 'false' successfully\n";
                        eat(SEMICOLON);
                    }
                    else{
                        eat(ASSIGN);
                        AllValue val = parse_expression();
                        if (!is_bool(val)){
                            std::cout << "[SyntaxError]: on pos:" << lexer.GetPos() << " on row: " << lexer.GetRow() << " bool type can contain only true/false\n";
                            exit(1);
                        }
                        eat(SEMICOLON);

                        memory[var_name] = val;

                        std::cout << "[SYSTEM]: Successfully write data into '" << var_name << "'\n";
                    }
                }

                else if (current_token.type == TYPE_CHAR){
                    eat(TYPE_CHAR);
                    std::string var_name = current_token.value;
                    eat(IDENTIFIER);
                    if (current_token.type == SEMICOLON){
                        memory[var_name] = '\0';
                        std::cout<<"[SYSTEM]: Variable '" << var_name << "' on pos: " << lexer.GetPos() << " on row: " << lexer.GetRow() << " was initialized as '\0' successfully\n";
                        eat(SEMICOLON);
                    }
                    else{
                        eat(ASSIGN);
                        AllValue val = parse_expression();
                        if (!is_char(val)){
                            std::cout << "[SyntaxError]: on pos:" << lexer.GetPos() << " on row: " << lexer.GetRow() << " char type can contain only one symbol\n";
                            exit(1);
                        }
                        eat(SEMICOLON);

                        memory[var_name] = val;

                        std::cout << "[SYSTEM]: Successfully write data into '" << var_name << "'\n";
                    }
                }

                else if (current_token.type == IDENTIFIER){
                    std::string name = current_token.value;
                    eat(IDENTIFIER);
                    if (current_token.type == LPAREN){
                        eat(LPAREN);
                        if (current_token.type != RPAREN && current_token.type != IDENTIFIER){
                            std::cout << "[UnexpectedTokenError]: expected " << GetTokenName(RPAREN) << " or " << GetTokenName(IDENTIFIER) << " but got "<<GetTokenName(current_token.type) <<  " on pos: " << lexer.GetPos() << " on row: " << lexer.GetRow() << "\n";
                            exit(1);
                        }
                        
                        std::vector<AllValue> args;
                        if (current_token.type != RPAREN) args.push_back(parse_expression());
                        while (current_token.type == COMMA){
                            eat(COMMA);
                            args.push_back(parse_expression());
                        }
                        eat(RPAREN);
                        eat(SEMICOLON);

                        static std::unordered_set<std::string> math_funcs = {
                            "sqrt", "cbrt", "pow", "sin", "cos", "tan", "asin", "acos", "atan", "atan2", "sinh", "cosh", "tanh", "asinh", "acosh", "atanh", "ceil", "floor", "abs", "exp", "fmod", "frexp", "ldexp", "log", "log10", "modf",
                            "sqrtf", "cbrtf", "powf", "sinf", "cosf", "tanf", "asinf", "acosf", "atanf", "atan2f", "sinhf", "coshf", "tanhf", "asinhf", "acoshf", "atanhf", "ceilf", "floorf", "absf", "expf", "fmodf", "frexpf", "ldexpf", "logf", "log10f", "modff"
                        };
                        static std::unordered_set<std::string> file_funcs = {
                            "write", "read", "append", "exists", "clear"
                        };

                        if (args.empty()){
                            std::cout << "[ArgumentError]: not enough arguments";
                            exit(1);
                        }

                        if (math_funcs.count(name)) call_builtin_math(name, args);
                        else if (file_funcs.count(name)) call_builtin_file_tools(name, args);
                        else if (user_functions.count(name)) execute_user_function(user_functions[name], args);
                        else {std::cout << "[FunctionError]: function: " << name << " was not found"; exit(1);}
                    }
                }

                else if (current_token.type == OUTPUT){
                    eat(OUTPUT);
                    eat(LPAREN);
                    
                    AllValue val = parse_expression();

                    std::visit([](auto&& arg){std::cout << arg << "\n";}, val);
                    
                    eat(RPAREN);
                    eat(SEMICOLON);
                }

                else if (current_token.type == INPUT){
                    eat(INPUT);
                    eat(LPAREN);
                    if(current_token.type != IDENTIFIER){
                        std::cout << "[SyntaxError]: input() function expects " << GetTokenName(IDENTIFIER) << ", but got " << GetTokenName(current_token.type) << " on pos: " << lexer.GetPos() << " on row: " << lexer.GetRow() << "\n";
                        exit(1);
                    }
                    
                    std::string var_name = current_token.value;

                    eat(IDENTIFIER);
                    eat(RPAREN);
                    eat(SEMICOLON);

                    std::cout << "[INPUT]: Enter the value for " << var_name << ": ";
                    std::string user_input;
                    std::cin >> user_input;
                    memory[var_name] = user_input;
                    std::cout << "[SYSTEM]: Succesfully written data into '" << var_name << "'\n";
                }

                else if (current_token.type == IMPORT){
                    eat(IMPORT);
                    
                    if (current_token.type != IDENTIFIER){
                        std::cout << "[SyntaxError]: on pos: " << lexer.GetPos() << " on row: " << lexer.GetRow() << " 'import' keyword expects file name";
                        exit(1);
                    }

                    std::string filename = current_token.value;
                    eat(IDENTIFIER);

                    std::ifstream file(filename);
                    if(!file){
                        std::cout << "[ImportError]: Cannot open file: '" << filename;
                        exit(1);
                    }

                    std::stringstream buffer;
                    buffer << file.rdbuf();
                    std::string code = buffer.str();

                    Lexer lex(code);
                    Parser parser(lex, memory);
                    parser.run();
                }

                else if (current_token.type == IF){
                    eat(IF);
                    eat(LPAREN);
    
                    AllValue condition_value = parse_logic();

                    eat(RPAREN);
                    eat(LBRACE);

                    if (std::holds_alternative<bool>(condition_value)){
                        if (std::get<bool>(condition_value)) last_if_executed = true;
                    }
                    else skip_block();

                    last_else_if_executed = false;
                }

                else if (current_token.type == ELSE_IF){
                    eat(ELSE_IF);
                    eat(LPAREN);

                    AllValue condition_value = parse_logic();

                    eat(RPAREN);
                    eat(LBRACE);

                    if (std::holds_alternative<bool>(condition_value)){
                        if (!last_if_executed && std::get<bool>(condition_value)) last_else_if_executed = true;
                    }
                    else skip_block();
                }

                else if (current_token.type == ELSE){
                    eat(ELSE);
                    eat(LBRACE);

                    if (!last_if_executed && !last_else_if_executed){}
                    else skip_block();
                }

                else if (current_token.type == WHILE){
                    eat(WHILE);
                    eat(LPAREN);

                    size_t start_pos = lexer.GetPos();
                    Lexer condition_lexer = lexer;
                    AllValue condition = parse_logic();

                    eat(RPAREN);
                    eat(LBRACE);

                    size_t body_start = lexer.GetPos();

                    if (to_bool(condition)) {
                        lexer.SetPos(start_pos);
                        current_token = lexer.next_token();
                        condition = parse_logic();

                        while(std::get<bool>(condition) != false){
                            lexer.SetPos(body_start);
                            current_token = lexer.next_token();
                            run_until(RBRACE);
                            lexer.SetPos(start_pos);
                            condition = parse_logic();
                        }
                    }
                    else skip_block();
                }

                else if (current_token.type == FN){
                    eat(FN);
                    
                    std::string func_name = current_token.value;

                    eat(IDENTIFIER);

                    Function new_func;

                    eat(LPAREN);

                    if (current_token.type == IDENTIFIER){
                        new_func.params.push_back(current_token.value);
                        eat(IDENTIFIER);
                        while (current_token.type == COMMA){
                            eat(COMMA);
                            new_func.params.push_back(current_token.value);
                            eat(IDENTIFIER);
                        }
                    }

                    eat(RPAREN);
                    eat(LBRACE);

                    std::stringstream code_body;
                    while (current_token.type != RBRACE && current_token.type != END_OF_FILE){
                        code_body << current_token.value << " ";
                        current_token = lexer.next_token();
                    }
                    new_func.body_code = code_body.str();

                    eat(RBRACE);

                    user_functions[func_name] = new_func;

                    std::cout << "[SYSTEM]: Successfully saved an user function: " << func_name << nl;
                }

                else if (current_token.type == RBRACE) eat(RBRACE);
            }
        }

        //Main function of parser
        AllValue run(){
            run_until(END_OF_FILE);
        }
};
