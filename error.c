#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include "error.h"

void warning(const char *fmt, ...) {
	va_list	args;

	fflush(stdout);
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fflush(stderr);
	return;
}

void fatal_error(const char *fmt, ...) {
	va_list	args;

	fflush(stdout);
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	die(EXIT_FAILURE);
}

void die(int how) {
	warning("exiting (code = %d), goodbye...\n", how);
	exit(how);
}
