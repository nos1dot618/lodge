#ifndef LODGE_H_
#define LODGE_H_

#include <stdio.h>

#define LogInfo(fmt, ...) fprintf(stdout, "info: "fmt"\n", ##__VA_ARGS__)
#define LogFatal(fmt, ...) fprintf(stderr, "error: "fmt"\n", ##__VA_ARGS__)
#define ExitFatal(fmt, ...)						\
	do {										\
		LogFatal(fmt, ##__VA_ARGS__);			\
		exit(EXIT_FAILURE);						\
	} while (0)

#endif // LODGE_H_
