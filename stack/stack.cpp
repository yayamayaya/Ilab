#include "stack.h"

// type traits

template<>
const int Stack<int>::poison = 0xDD;
template<>
const long long int Stack<long long int>::poison = 0xFDEADF;
template<>
const double Stack<double>::poison = 0xFFDEAD;
template<>
const float Stack<float>::poison = 0xDA;
template<>
const char Stack<char>::poison = 0xD;

const char *msgNoArgs = "%-25s| %-20s| %-20s|\n";
const char *msgOneArg = "%-25s| %-20d| %-20s|\n";


void data_print(FILE *fileName, const char num)
{
    fprintf(fileName, "%d\n", num);
}

void data_print(FILE *fileName, const int num)
{
    fprintf(fileName, "%d\n", num);
}

void data_print(FILE *fileName, const double num)
{
    fprintf(fileName, "%.2f\n", num);
}

void data_print(FILE *fileName, const long long int num)
{
    fprintf(fileName, "%lld\n", num);
}