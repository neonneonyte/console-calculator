#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/*
ERROR CODES:
0 == SUCCESS
1 == INVALID MATH (THE PROGRAM RAN AS INTENDED THOUGH!)
2 == FAILURE (PROGRAM DIDN'T RUN AS INTENDED!)
*/

#define tonumber(ch) (ch - '0')

struct eval {
    double* table;
    unsigned int length;
    unsigned int capacity;
} eval;

void addtable(struct eval* const dst, const double val) {
    if (dst->length == dst->capacity) {
        double* tmp = malloc((dst->capacity + 8) * sizeof(double));
        if (!tmp) {
            fputs("Failed to allocate memory", stdout);
            free(dst->table);
            dst->table = (void*)0;
            free(eval.table);
            exit(2);
        }
        for (unsigned int idx = 0; idx < dst->length; ++idx)
            tmp[idx] = dst->table[idx];
        free(dst->table);
        dst->table = tmp;
    }
    dst->table[dst->length++] = val;
}

double getnumber(char* const src);

double parenthesis(char* const src) {
    struct eval sec_eval = {(void*)0, 0, 0};

    char ch = *src;
    addtable(&sec_eval, getnumber(&ch));

    while (ch != ')' && ch != EOF && ch != '\n' && ch != '\r' && ch != '\f') {
        switch (ch) {
            case '+':
            case '-':
                ch = getchar();
                addtable(&sec_eval, getnumber(&ch));
                break;
            case '*':
            {
                ch = getchar();
                double number = getnumber(&ch);
                if (!number)
                    break;
                sec_eval.table[sec_eval.length - 1] *= number;
            } break;
            case '/':
            {
                ch = getchar();
                double number = getnumber(&ch);
                if (!number) {
                    fputs("Cannot divide by zero!", stdout);
                    free(sec_eval.table);
                    free(eval.table);
                    exit(1);
                }
                sec_eval.table[sec_eval.length - 1] /= number;
            } break;
            case '(':
            {
                ch = getchar();
                double number = parenthesis(&ch);
                if (!number)
                    break;
                sec_eval.table[sec_eval.length - 1] *= number;
            } break;
            case ' ':
                ch = getchar();
                break;
        }
    }

    if (ch != ')') {
        fputs("Invalid math expression", stdout);
        free(sec_eval.table);
        free(eval.table);
        exit(1);
    }

    double result = sec_eval.table[0];
    for (unsigned int idx = 1; idx < sec_eval.length; ++idx)
        result += sec_eval.table[idx];
    *src = getchar();
    free(sec_eval.table);
    return result;
}

double getnumber(char* const src) {
    char ch = *src;
    double result = 0.0;
    uint8_t neg = 0;

    // Skip whitespaces
    while (ch == ' ' || ch == '\t') {
        ch = getchar();
    }

    // Should number be negated?
    if (ch == '-') {
        neg = 1;
        ch = getchar();
    }

    // Is it a number?
    if (ch >= '0' && ch <= '9') {
        result = tonumber(ch);
        for (ch = getchar(); (ch >= '0' && ch <= '9'); ch = getchar()) {
            result *= 10;
            result += tonumber(ch);
        }
        if (ch == ',' || ch == '.') {
            ch = getchar();
            if (ch >= '0' && ch <= '9') {
                float fpoint = 0.1;
                do {
                    result += (tonumber(ch) * fpoint);
                    fpoint *= 0.1;
                    ch = getchar();
                } while (ch >= '0' && ch <= '9');
            }
        }
        *src = ch;
        return neg ? -result : result;
    }
    // or is it a parenthesis? 
    else if (ch == '(') {
        ch = getchar();
        result = parenthesis(&ch);
        *src = ch;
        return result;
    }
    // or is it something else? if so, then that means it's an invalid math expression
    else {
        fputs("Invalid math expression\n", stdout);
        free(eval.table);
        exit(1);
    }
}

int main() {
    eval.table = (void*)0;
    eval.length = eval.capacity = 0;

    char ch = getchar();
    addtable(&eval, getnumber(&ch));

    while (ch != EOF && ch != '\n' && ch != '\r' && ch != '\f') {
        switch (ch) {
            case '+':
            case '-':
                ch = getchar();
                addtable(&eval, getnumber(&ch));
                break;
            case '*':
            {
                ch = getchar();
                double number = getnumber(&ch);
                if (!number)
                    break;
                eval.table[eval.length - 1] *= number;
            } break;
            case '/':
            {
                ch = getchar();
                double number = getnumber(&ch);
                if (!number) {
                    fputs("Cannot divide by zero\n", stdout);
                    free(eval.table);
                    exit(1);
                }
                eval.table[eval.length - 1] /=  number;
            } break;
            case '(':
            {
                ch = getchar();
                double number = parenthesis(&ch);
                if (!number)
                    break;
                eval.table[eval.length - 1] *= number;
            } break;
            case '\t':
            case ' ':
                ch = getchar();
                break;
            default:
                fputs("Invalid math expression", stdout);
                free(eval.table);
                exit(1);
        }
    }

    double result = eval.table[0];
    for (unsigned int idx = 1; idx < eval.length; ++idx)
        result += eval.table[idx];
    free(eval.table);
    printf("Result: %f\n", result);
    exit(0);
}