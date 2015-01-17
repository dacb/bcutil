#include <math.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	double e, Q;

	printf("e\t\tQ\t\taccuracy\n");
	for (Q = 0.1; Q <= 60; Q += 10) {
		e = exp((log(10.0) * Q) / -10.0);
		printf("%f\t%f\t%f\n", Q, e, 1-e);
	}
	return 1;
}
