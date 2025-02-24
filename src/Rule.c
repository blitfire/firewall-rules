//
// Created by george on 26/10/23.
//

#include "Rule.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "myBool.h"

void make_rule(Rule *rule, char *ip_str, char *port_str) {
    get_ip(rule, ip_str);
    get_ports(rule, port_str);
    check_rule(rule);
}

int compare_rules(const void* first, const void* second) {
    const Rule* rule1 = first;
    const Rule* rule2 = second;
    unsigned int rule1_ip_index = 2 - rule1->ip_count;
    unsigned int rule2_ip_index = 2 - rule2->ip_count;
    for (int i=0; i < 4; i++) {
        if (rule1->ip_nums[rule1_ip_index][i] > rule2->ip_nums[rule2_ip_index][i]) return 1;
        if (rule1->ip_nums[rule1_ip_index][i] < rule2->ip_nums[rule2_ip_index][i]) return -1;
    }

    unsigned int rule1_port_index = 2 - rule1->port_count;
    unsigned int rule2_port_index = 2 - rule2->port_count;
    if (rule1->ports[rule1_port_index] > rule2->ports[rule2_port_index]) return 1;
    if (rule1->ports[rule1_port_index] < rule2->ports[rule2_port_index]) return -1;
    return 0;
}

void get_rule_string(Rule* rule, char *out_str) {
    char ip_1[MAX_IP_LENGTH], ip_2[MAX_IP_LENGTH], ports[MAX_PORT_LENGTH];
    if (rule->ip_count == 2) {
        ip_to_str(rule->ip_nums[0], ip_1);
        strcat(out_str, ip_1);
        strcat(out_str, "-");
    }
    ip_to_str(rule->ip_nums[1], ip_2);
    strcat(out_str, ip_2);
    strcat(out_str, " ");

    if (rule->port_count == 2) {
        sprintf(ports, "%ld-%ld", rule->ports[0], rule->ports[1]);
    } else sprintf(ports, "%ld", rule->ports[1]);

    strcat(out_str, ports);
}

void ip_to_str(const long nums[], char *out_str) {
    for (int i=0; i < 4; i++) {
        if (i < 3) out_str += sprintf(out_str, "%ld.", nums[i]);
        else out_str += sprintf(out_str, "%ld", nums[i]);
    }
}

bool follows_rule(Rule* rule, char* packet_ip, char *packet_port) {
    Rule temp_rule;
    make_rule(&temp_rule, packet_ip, packet_port);

    if ((temp_rule.ports[1] > rule->ports[1]) || (temp_rule.ports[1] < rule->ports[0])) return false;

    if (rule->ip_count == 1) {
        for (int i=0; i < 4; i++) {
            if (rule->ip_nums[1][i] != temp_rule.ip_nums[1][i]) return false;
        }
        return true;
    }
    bool above_lower = false;
    bool below_upper = false;
    for (int i=0; i < 4 && !(above_lower && below_upper); i++) {
        if (rule->ip_nums[0][i] < temp_rule.ip_nums[1][i]) above_lower = true;
        if ((rule->ip_nums[0][i] > temp_rule.ip_nums[1][i]) && !above_lower) return false;

        if (rule->ip_nums[1][i] > temp_rule.ip_nums[1][i]) below_upper = true;
        if ((rule->ip_nums[1][i] < temp_rule.ip_nums[1][i]) && !below_upper) return false;
    }
    return true;
}

void check_rule(Rule* rule) {
    rule->ill_formed = check_ip(rule) && check_port_range(rule);
}

void get_ip(Rule *rule, char *ip) {
    char* ip_copy;
    if ((ip_copy= calloc(strlen(ip), sizeof(char))) == NULL) {
        printf("Failed to create node: Allocation failed.");
        exit(-1);
    }
    strcpy(ip_copy, ip);

    rule->ip_count = strchr(ip_copy, '-') ? 2 : 1;
    long num;
    char delimiters[] = "-.";
    char* token = strtok(ip_copy, delimiters);

    for (int i = 0; token != NULL; i++) {
        num = strtol(token, NULL, 10);
        if (rule->ip_count == 2) rule->ip_nums[i / 4][i % 4] = num;
        else rule->ip_nums[1][i] = num;
        token = strtok(NULL, delimiters);
    }
    free(ip_copy);
}

void get_ports(Rule *rule, const char *port_range) {
    long first = strtol(port_range, NULL, 10), second;
    char* hyphen_index = strchr(port_range, '-');
    if (hyphen_index) {
        second = strtol(hyphen_index+1, NULL, 10);
        rule->ports[0] = first;
        rule->ports[1] = second;
        rule->port_count = 2;
    } else {
        rule->ports[0] = 0;
        rule->ports[1] = first;
        rule->port_count = 1;
    }
}

bool check_ip(Rule *rule) {
    // CHECK FOR RANGE
    long num1, num2;
    if (rule->ip_count == 1) {
        for (int i=0; i < 4; i++) {
            num1 = rule->ip_nums[1][i];
            if (num1 > 255 || num1 < 0) return true;
        }
        return false;
    }
    for (int i=0; i < 4; i++) {
        num1 = rule->ip_nums[0][i];
        num2 = rule->ip_nums[1][i];
        if (num1 > 255 || num1 < 0 || num2 > 255 || num2 < 0) return false;

        if (num1 > num2) return true;
        else if (num1 < num2) return false;
    }
    return false;
}

bool check_port_range(Rule *rule) {
    return (rule->ports[0] < rule->ports[1]) &&
           ((rule->ports[0] < MAX_PORT_VALUE) ||
            (rule->ports[1] < MAX_PORT_VALUE));
}

bool is_ip_str(const char* ip_str) {
    if (strlen(ip_str) > MAX_IP_LENGTH) return false;

    char curr;
    int num_count=0;
    int dot_count=0;
    int dash_count=0;
    for (int i=0; ip_str[i] != '\0'; i++) {
        curr = ip_str[i];
        if (curr == '.') {
            if (num_count == 0) return false;
            num_count = 0;
            dot_count++;
            if (dot_count > 3) return false;
        } else if (curr >= '0' && curr <= '9') {
            num_count++;
            if (num_count > 3) return false;
        } else if (curr == '-') {
            if (num_count == 0 || dot_count != 3 || dash_count > 0) return false;
            num_count = 0;
            dot_count = 0;
            dash_count++;
        } else return false;
    }
    return (bool) (dot_count == 3 && num_count > 0);
}

bool is_port_str(const char* port_str) {
//    if (strlen(port_str) > MAX_PORT_LENGTH) return false;

    char curr;
    int num_count=0;
    int dash_count=0;
    for (int i=0; port_str[i] != '\0'; i++) {
        curr = port_str[i];
        if (curr == '-') {
            if (num_count == 0 || dash_count > 0) return false;
            num_count = 0;
            dash_count++;
        } else if (curr >= '0' && curr <= '9') {
            num_count++;
            if (num_count > 5) return false;
        } else return false;
    }
    return (bool) (num_count > 0);
}

