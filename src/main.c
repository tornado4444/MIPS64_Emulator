#include <stdio.h>

#include "mips64/core.h"

int divide(int a, int b) {
	return a / b;
}

int main(int argc, char** argv) {
	int a = 10; int b = 5;
	printf("Hello World!:)\n");
	printf("Result: %d", divide(a, b));
	return 0;
}
