//
// Created by george on 26/10/23.
//

#ifndef SOCKETS_RULEFILE_H
#define SOCKETS_RULEFILE_H
#include <stdio.h>
#include <stdlib.h>
#include "myBool.h"
#include "Rule.h"

typedef struct RuleFile {
    FILE* fp;

} RuleFile;

void open_rule_file(RuleFile *new_rules, const char *filename);
void close_rule_file(RuleFile* rule_file);
void write_rule_line(RuleFile* rule_file, char *ip_str, char *port_str);
bool read_rule_line(RuleFile* rule_file, char *ip_out, char *port_out);
bool delete_rule_line(RuleFile* rule_file, char *ip_str, char *port_str);

#endif //SOCKETS_RULEFILE_H
