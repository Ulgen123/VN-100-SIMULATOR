#ifndef PARSER_H
#define PARSER_H
#include <stdbool.h>

void parser(const char *msg);
static bool validateMessage(const char *msg);
char *extractRegiserNumber(const char *msg);
static void parserRegisterCommand(const char *msg);
static void parserYPRcommand(const char *msg);
char *extractHeader(const char *msg);
#endif /* PARSER_H */