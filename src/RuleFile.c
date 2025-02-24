//
// Created by george on 26/10/23.
//

#include "RuleFile.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "Rule.h"
#include "myBool.h"


void open_rule_file(RuleFile *new_rules, const char *filename) {
    new_rules->fp = fopen(filename, "r+");
    if (new_rules->fp == NULL) {
        printf("Cannot open new_rules.\n");
        exit(-1);
    }
}

void close_rule_file(RuleFile* rule_file) {
    fclose(rule_file->fp);
}

void write_rule_line(RuleFile* rule_file, char *ip_str, char *port_str) {
    char rule_str[MAX_RULE_LENGTH];
    sprintf(rule_str, "%s %s", ip_str, port_str);

    fseek(rule_file->fp, 0, 2);
    fprintf(rule_file->fp, "%s\n", rule_str);
}

bool read_rule_line(RuleFile* rule_file, char *ip_out, char *port_out) {
    char line_buffer[MAX_RULE_LENGTH];
    if (fgets(line_buffer, sizeof(line_buffer), rule_file->fp)) {
        sscanf(line_buffer, "%s %s\n", ip_out, port_out);
        return true;
    }
    return false;
}

bool delete_rule_line(RuleFile* rule_file, char *ip_str, char *port_str) {
    rewind(rule_file->fp);

    char rule_str[MAX_RULE_LENGTH];
    sprintf(rule_str, "%s %s\n", ip_str, port_str);
    long line_length = (long) strlen(rule_str);

    char line_buffer[MAX_RULE_LENGTH];
    bool found = false;

    // Try and find the right line to delete
    while (fgets(line_buffer, sizeof(line_buffer), rule_file->fp) &&
           !(found=(bool) (strcmp(rule_str, line_buffer) == 0))) {}
    if (!found) return false;

    // bounce between the read and write locations copying each character one by one
    char temp = (char) getc(rule_file->fp);
    while (temp != EOF) {
        fseek(rule_file->fp, -(line_length + 1), 1);
        putc(temp, rule_file->fp);

        fseek(rule_file->fp, line_length, 1);
        temp = (char) getc(rule_file->fp);
    }
    // truncate to remove left-over garbage
    if (ftruncate(fileno(rule_file->fp), ftell(rule_file->fp) - line_length) != 0) {
        printf("Error truncating the file.");
        close_rule_file(rule_file);
        exit(-1);
    }
    return true;
}