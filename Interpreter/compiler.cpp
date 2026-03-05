#include <iostream>
#include <fstream>
#include <filesystem>
#include "parser.hpp"

int main(int argc, char* argv[]){
    std::cout<<"Welcome to the Ferrum language's interpreter!\n";
    std::string path;
    std::cout<<"Input the file path(suffix must be .fer): ";
    std::cin>>path;

    if (std::filesystem::exists(path)){
        if (std::filesystem::is_directory(path)){
            std::cout<<"It is a directory, not a file!";
            exit(1);
        }
        std::cout<<"Interpreting the file...\n";

        std::ifstream input_file(path);
        if (!input_file.is_open()){
            std::cout<<"Unable to open the file!";
            exit(1);
        }

        std::string str;
        std::string file_contents;
        while (std::getline(input_file, str)){
            file_contents += str;
            file_contents.push_back('\n');
        }

        Lexer lex(file_contents);
        Parser parser(lex);
        parser.run();
    }
    else{
        std::cout<<"File not found!";
        exit(1);
    }
}