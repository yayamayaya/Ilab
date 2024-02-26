#include <stdio.h>
#include <math.h>
#include <assert.h>

typedef struct
{
    double a;
    double b;
    double c;

    double x1;
    double x2;
    int solNum;
} kvUr;

int coeffInput(kvUr *KV);
int Solution(kvUr *ur);
void linearEq(double k, double b, double* x);
void bZeroSolution(kvUr *ur);
void cZeroSolution(kvUr *ur);
void Discriminant(kvUr* ur, double *D);
void solutionPrint(kvUr ur);

void main()
{
    kvUr KV = {0};
    //Запись коэффицентов
    coeffInput(&KV);
    //Решение уравнения
    Solution(&KV);
    //Печать решений
    solutionPrint(KV);
}

int coeffInput(kvUr *KV)
{
    printf("Enter coefficients:\nEnter a:\n");
    while (scanf("%lf", &KV-> a) == 0)
        printf("Try again.\n");

    printf("Enter b:\n");
    while (scanf("%lf", &KV-> b) == 0)
        printf("Try again.\n");
    
    printf("Enter c:\n");
    while (scanf("%lf", &KV-> c) == 0)
        printf("Try again.\n");

    printf("Coefficients were read successfully. Equation is:\n %.2lfx^2 + %.2lfx + %.2lf = 0", KV-> a, KV-> b, KV-> c);
}

int Solution(kvUr *ur)
{
    if(ur-> a == 0)
    {
        assert(ur-> a == 0);
        linearEq(ur-> b, ur-> c, &ur-> x1);  
        ur-> solNum = 1;      
    }
    else if(ur-> b == 0)
        bZeroSolution(ur);
    else if(ur-> c == 0)
        cZeroSolution(ur);
    else
    {
        double D = 0;
        Discriminant(ur, &D);
        if(D < 0)
            ur-> solNum = 0;
        else if(D == 0)
        {
            ur-> x1 = - ur-> b / (2 * ur-> a);            
            ur-> solNum = 1;
        }
        else
        {
            ur-> x1 = - (ur-> b + sqrt(D)) / (2 * ur-> a);
            ur-> x2 = - (ur-> b - sqrt(D)) / (2 * ur-> a);
            ur-> solNum = 2;
        }
    }
}

void linearEq(double k, double b, double* x)
{
    assert(k != 0);
    *x = - b / k;
}

void bZeroSolution(kvUr *ur)
{
    assert(ur-> a != 0);
    assert(ur-> b == 0);
    if(ur-> c == 0)
        ur-> solNum = 1;
    else
    {
        ur-> x1 = sqrt(- ur-> c / ur-> a);     
        ur-> x2 = -sqrt(- ur-> c / ur-> a);   
        ur-> solNum = 2;
    }
}

void cZeroSolution(kvUr *ur)
{
    assert(ur-> c == 0);
    if(ur-> a == 0 || ur-> b == 0)
        ur-> solNum = 1;
    else
    {
        linearEq(ur-> a, ur-> b, &ur-> x2);
        ur-> solNum = 2;
    }
}

void Discriminant(kvUr* ur, double *D)
{
    *D = ur-> b * ur-> b - 4 * ur-> a * ur-> c;
}

void solutionPrint(kvUr ur)
{
    assert(0 <= ur.solNum && ur.solNum <= 2);
    switch (ur.solNum)
    {
    case 0:
        printf("\n>> Equation don't have solutions");
        break;
    case 1:
        printf("\n>> Equation have 1 solution:\n> x = %.3lf", ur.x1);
        break;
    case 2:
        printf("\n>> Equation have 2 solutions:\n> x1 = %.3lf,\n> x2 = %.3lf", ur.x1, ur.x2);        
        break;
    }
}