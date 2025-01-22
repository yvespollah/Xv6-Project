#include "kernel/types.h"
#include "user.h"


// Function to evaluate an expression with two operands and an operator
uint evaluate(char operator, uint operand1, uint operand2) {
    switch (operator) {
        case '+': return operand1 + operand2;
        case '-': return operand1 - operand2;
        case '*': return operand1 * operand2;
        case '/': 
            if (operand2 != 0) return operand1 / operand2;
            printf("Error: Division by zero\n");
            return 0;
        default:
            printf("operand1 = %d\n", operand1);
            printf("operand2 = %d\n", operand2);
            printf("Error: Unsupported operator '%c'\n", operator);
            printf("Usage: calc \"A+B\" \t Note: No spaces allowed\n");
            return 0;
    }
}

/**
 * Remove all spaces from a string in-place.
 *
 * The function iterates over the string, copying non-space characters to
 * the front of the string.  Finally, it null-terminates the string.
 *
 * Requires: expression is a null-terminated string.
 *
 * Effects: Modifies expression by removing all spaces.
 *
 * Returns: nothing.
 
void strip_all_spaces(char *expression) {
    int j = 0;
    for (int i = 0; expression[i] != '\0'; i++) {
        if (expression[i] != ' ') {
            expression[j++] = expression[i];
        }
    }
    expression[j] = '\0';  // Null-terminate the string
}

*/

void strip_quotes(char *expression) {
    int len = strlen(expression);
    if (len > 1 && expression[0] == '"' && expression[len - 1] == '"') {
        // Shift characters left to remove starting quote
        for (int i = 1; i < len - 1; i++) {
            expression[i - 1] = expression[i];
        }
        expression[len - 2] = '\0';  // Null-terminate after removing ending quote
    }
}

/**
 * Copies at most n characters of src to dest, padding with zeros
 * if src is shorter than n characters.  If src is longer than n
 * characters, no null terminator is appended.
 *
 * Returns a pointer to dest.
 */
char *strncpy(char *dest, const char *src, uint n) {
    uint i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}



// Function to parse and evaluate a simple expression
int parse_and_evaluate(const char *expression) {
    uint operand1 = 0, operand2 = 0;
    char operator = 0;

    // Manual parsing of the input string
    int i = 0;
    
    while (expression[i] == ' ') i++;  // Skip leading spaces
    
    while (expression[i] >= '0' && expression[i] <= '9') {
        operand1 = operand1 * 10 + (expression[i] - '0');
        i++;
    }

    // Skip spaces
    while (expression[i] == ' ') i++;

    // Extract operator
    operator = expression[i];
    i++;

    // Skip spaces
    while (expression[i] == ' ') i++;

    // Extract second operand
    while (expression[i] >= '0' && expression[i] <= '9') {
        operand2 = operand2 * 10 + (expression[i] - '0');
        i++;
    }

    // Evaluate the expression
    return evaluate(operator, operand1, operand2);
}

int main(int argc, char *argv[]) {
   if (argc != 2) {
        printf("Usage: calc <A+B> Note: No spaces allowed\n");
        exit(1);
    }

//    const char *expression = argv[1];

    char expression[100];
    strncpy(expression, argv[1], sizeof(expression) - 1);
    expression[sizeof(expression) - 1] = '\0';  // Ensure null termination

 //   strip_all_spaces(expression); // Remove spaces

    strip_quotes(expression);  // Remove quotes if present
    int result = parse_and_evaluate(expression);

    printf("Result: %d\n", result);
    exit(0);
}
