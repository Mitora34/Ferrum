#pragma once

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <cmath>

#define nl '\n'
#define tab '\t'

namespace Mitora34{

    namespace riopython{

    //const double pi = 3.1415926;
    //const double e = 2.718281828;

    const int square_degree = 360;
    const int rectangle_degree = 360;
    const int circle_degree = 360;
    const int triangle_degree = 180;

    class IO{
        public:

            template <typename T>

            //Outputs any type in python style
            static void print(T anything){
                std::cout<<anything;
            }

            template <typename T>

            //Output any type in C# style
            static void ConsoleWriteLine(T anything){
                std::cout<<anything<<nl;
            }

            template <typename T>

            //Input anything in python style
            static void input(T& anything){
                std::cin>>anything;
            }

            template <typename T>

            //Input anything in C# style
            static void Scan(T& words){
                std::cin>>words;
            }

    };

    class ArraySort{
        public:

            template <typename T>

            //Minimal value in array
            static T min(T* arr, int len){
                T minimal = *arr;
                for(int i = 0; i < len; i++){
                    if(minimal > *(arr + i)){
                        minimal = *(arr + i);
                    }
                }
                return minimal;
            }

            template <typename T>

            //Maximal value in array
            static T max(T* arr, int len){
                T maximal = *arr;
                for(int i = 0; i < len; i++){
                    if(maximal < *(arr + i)){
                        maximal = *(arr + i);
                    }
                }
                return maximal;
            }

    };

    class Math{
        public:
            class Algebra{
                public:

                    template <typename T>

                    //Formula of Abbriviated Multipliction
                    static T am(T num1, T num2){
                        return (num1 + num2) * (num1 - num2);
                    }

                    //Gauss's formula
                    static double gauss(int num){
                        return num * (num + 1) / 2;
                    }

                    //Greatest Common Divisor
                    static int gcd(int num, int nums){
                        while (nums != 0){
                            int temp = nums;
                            nums = num % nums;
                            num = temp;
                        }
                    return num;
                    }

                    //Least Common Multiply
                    static int lcm(int num, int nums){
                        return abs(num * nums) / gcd(num, nums);
                    }

            };

            class Arithmetics{
                public:

                    template <typename T>

                    //Summarizing numbers
                    static T sum(T num1, T num2){
                        return num1 + num2;
                    }

                    template <typename T>

                    //Subbing numbers
                    static T sub(T num1, T num2){
                        return num1 - num2;
                    }

                    template <typename T>

                    //Multiplication of numbers
                    static T mul(T num1, T num2){
                        return num1 * num2;
                    }

                    template <typename T>

                    //Dividing numbers
                    static T div(T num1, T num2){
                        return num1 / num2;
                    }

                    template <typename T>

                    //Comparing numbers
                    static T cmp(T num1, T num2){
                        return (num1 > num2) ? num1 : num2;
                    }

                    template <typename T>

                    //Increase number
                    static T inc(T anynum){
                        return ++anynum;
                    }

                    template <typename T>

                    //Decrease number
                    static T dec(T anynum){
                        return --anynum;
                    }

                    //Number checking
                    static void numcheck(){
                        int num;
                        std::cout<<"Enter a num:";
                        std::cin>>num;
                        std::cout<<((num % 2 == 0) ? "Even" : "Odd");
                    }

                    //Number reversing
                    static int reverse(int num){
                        int reversed = 0;
                        while(num != 0){
                            int rnum = num % 10;
                            reversed = reversed * 10 + rnum;
                            num /= 10;
                        }
                        return reversed;
                    }

                    //Number palindrome checking
                    static bool is_palindrome(int num) {
                        // Negatives aren't palindromes
                        if (num < 0) return false;
                        return num == reverse(num);
                    }

            };

            class Geometry{
                public:

                    template <typename T>

                    //Perimeter for square
                    static T sper(T length){
                        return length * 4;
                    }

                    template <typename T>

                    //Perimeter for rectangle
                    static T rper(T length, T width){
                        return (length + width) * 2;
                    }

                    template <typename T>

                    //Perimeter for triangle
                    static T tper(T fside, T sside, T thside){
                        return fside + sside + thside;
                    }

                    template <typename T>

                    //Square for square
                    static T square(T length){
                        return length * length;
                    }

                    template <typename T>

                    //Square for rectangle
                    static T rsquare(T length, T width){
                        return length * width;
                    }

                    template <typename T>

                    //Square for triangle
                    static T tsquare(T base, T height){
                        return 0.5 * base * height;
                    }

                    template <typename T>

                    //Volume for cube
                    static T vcube(T length){
                        return length * length * length;
                    }

                    template <typename T>

                    //Volume for parallelepiped
                    static T vparal(T length, T width, T height){
                        return length * width * height;
                    }

                    template <typename T>

                    //Volume for pyramid
                    static T vpyramid(T base_area, T height) {
                        return (base_area * height) / 3.0; 
                    }

                    template <typename T>

                    //Circle length
                    static T clen(T radius){
                        return 2 * M_PI * radius;
                    }

                    template <typename T>

                    //Square for circle
                    static T csquare(T radius){
                        return M_PI * (radius * radius);
                    }

                    template <typename T>

                    //Volume for sphere
                    static T svolume(T radius){
                        return (4.0 / 3.0) * M_PI * (radius * radius * radius);
                    }
            };

    };

    class Cryptography{
        public:

            //Ceasar Encryption
            std::string ceasarenc(const std::string& word, int shift){
                std::string res = word;
                for(char& c : res){
                    if(isalpha(c)){
                        char base = islower(c)? 'a' : 'A';
                        c = (c - base + shift) % 26 + base;
                    }
                }
                return res;
            }

            //Ceasar Decryption
            std::string ceasardec(const std::string& word, int shift){
                return ceasarenc(word, 26 - shift);
            }

    };

    //StopWatch function
    void stwatch(){
        int time;
        std::cout<<"Enter time in seconds:";
        std::cin>>time;
        while(time > 0){
            sleep(1);
            time--;
            std::cout<<time<<nl;
        }

        std::cout<<"Time out!!!"<<nl;
        #if defined(__APPLE__) && defined(__MACH__)
            system("say time is over");
        #endif
    }

    }
}