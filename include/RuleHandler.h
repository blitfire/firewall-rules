//
// Created by george on 01/11/23.
//

#ifndef SOCKETS_RULEHANDLER_H
#define SOCKETS_RULEHANDLER_H
#include "RuleList.h"
//#include "RuleFile.h"
#include "Rule.h"
#include "myBool.h"

#define MAX_LINE_LENGTH 64
#define INVALID_ADD "Invalid rule\n"
#define INVALID_CHECK "Illegal IP address or port specified\n"
#define INVALID_DELETE "Rule invalid\n"

typedef struct RuleHandler {
//    RuleFile file;
    RuleList list;
} RuleHandler;

void start_rule_handler(RuleHandler* handler);
void stop_rule_handler(RuleHandler* handler);

void stream(RuleHandler *handler, char *buffer);
void check(RuleHandler* handler, char* ip, char* port, char* status);
void add_rule(RuleHandler *handler, char *ips, char *ports, char *status);
void delete_rule(RuleHandler *handler, char *ips, char *ports, char *status);

bool check_input(char* ip, char* port);
#endif //SOCKETS_RULEHANDLER_H
