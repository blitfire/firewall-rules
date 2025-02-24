#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "myBool.h"
#include "RuleHandler.h"

#define THREAD_IN_USE     0
#define THREAD_FINISHED   1
#define THREAD_AVAILABLE  2
#define THREADS_ALLOCATED 10

#define BUFFER_LENGTH   64
#define MAX_CONNECTIONS 10

// Compound type for passing to the threads
struct threadArgs_t {
    RuleHandler* rule_data;
    int socket_fd;
    int thread_index;
};

// this is only necessary for proper termination of threads
struct threadInfo_t {
    pthread_t pthread_info;
    pthread_attr_t attributes;
    int status;
};

void error(char *msg); // error helper
void* process_request(void *args);
int find_thread_index();
void* wait_for_threads(void *args);

int return_value = 0; // compiler satisfaction

struct threadInfo_t *server_threads = NULL;
int thread_count = 0;
pthread_rwlock_t thread_lock =  PTHREAD_RWLOCK_INITIALIZER;
pthread_cond_t thread_condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t thread_end_lock = PTHREAD_MUTEX_INITIALIZER; // I think this is just for managing the finishing of threads

pthread_rwlock_t rule_lock = PTHREAD_RWLOCK_INITIALIZER;

int main(int argc, char* argv[]) {

    int socket_fd, port_num;
    int result;
    socklen_t client_len;
    struct sockaddr_in6 server_addr, client_addr;
    pthread_t wait_info;
    pthread_attr_t wait_attributes;

    if (argc < 2) {
        fprintf (stderr,"ERROR, no port provided\n");
        exit(1);
    }
    // create socket
    socket_fd = socket (AF_INET6, SOCK_STREAM, 0);
    if (socket_fd < 0) error("ERROR opening socket");

    bzero((char *) &server_addr, sizeof(server_addr));
    port_num = atoi(argv[1]); // get the port number from the args
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_addr = in6addr_any;
    server_addr.sin6_port = htons(port_num);

    // then bind the socket
    if (bind(socket_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0)
        error("ERROR on binding");
    // listen on connection, with a max backlog size of MAX_CONNECTIONS
    listen (socket_fd,MAX_CONNECTIONS);
    client_len = sizeof(client_addr);

    // creating separate thread waiting for other threads to finish
    if (pthread_attr_init(&wait_attributes)) {
        fprintf (stderr, "Creating initial thread attributes failed!\n");
        exit (1);
    }
    result = pthread_create (&wait_info, &wait_attributes, wait_for_threads, NULL);
    if (result != 0) {
        fprintf (stderr, "Initial Thread creation failed!\n");
        exit (1);
    }
    RuleHandler data;
    start_rule_handler(&data);
    // MAIN SERVER LOOP
    struct threadArgs_t* thread_args;
    int available_thread_index;
    while (1) {
        if ((thread_args = (struct threadArgs_t*) malloc(sizeof(struct threadArgs_t))) == NULL) {
            fprintf (stderr, "Memory allocation failed!\n");
            exit(1);
        }
        thread_args->socket_fd = accept(socket_fd, (struct sockaddr*) &client_addr, &client_len);
        if (thread_args < 0) error("ERROR on accept");

        available_thread_index = find_thread_index();
        thread_args->thread_index = available_thread_index;
        thread_args->rule_data = &data;
        if (pthread_attr_init(&(server_threads[available_thread_index].attributes))) {
            fprintf (stderr, "Creating thread attributes failed!\n");
            exit (1);
        }

        // process the request
        result = pthread_create (&(server_threads[available_thread_index].pthread_info),
                                 &(server_threads[available_thread_index].attributes),
                                 process_request, (void *) thread_args);
        if (result != 0) {
            fprintf (stderr, "Thread creation failed!\n");
            exit(1);
        }
    }
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void* process_request(void *args) {
    char message_buffer[BUFFER_LENGTH];
    struct threadArgs_t *thread_args;
    long n;

    thread_args = (struct threadArgs_t *) args;
    bzero(message_buffer, BUFFER_LENGTH);
    n = read(thread_args->socket_fd, message_buffer, BUFFER_LENGTH - 1); // -1 for \0
    if (n < 0) error("Error reading from socket.");

    // Client application ensures that the input is in the correct format with the correct number of arguments for a given mode
    // Client only checks to see if the mode is a single character
    char mode;
    if (message_buffer[1] == ' ') mode = message_buffer[0];
    else mode = 'X';

    char ip_str[MAX_IP_LENGTH];
    char port_str[MAX_PORT_LENGTH];
    char* stream_buffer;
    bool finished_streaming = false;
    switch (mode) {
        case 'A':
            // +2 to move past the mode and space
            sscanf(message_buffer + 2, "%s %s", ip_str, port_str);
            if (is_ip_str(ip_str) && is_port_str(port_str)) {
                pthread_rwlock_wrlock(&rule_lock);
                add_rule(thread_args->rule_data, ip_str, port_str, message_buffer);
                pthread_rwlock_unlock(&rule_lock);
            } else sprintf(message_buffer, INVALID_ADD);

            break;

        case 'C':
            sscanf(message_buffer + 2, "%s %s", ip_str, port_str);
            if (is_ip_str(ip_str) && is_port_str(port_str)) {
                pthread_rwlock_wrlock(&rule_lock);
                check(thread_args->rule_data, ip_str, port_str, message_buffer);
                pthread_rwlock_unlock(&rule_lock);
            } else sprintf(message_buffer, INVALID_CHECK);
            break;

        case 'D':
            sscanf(message_buffer + 2, "%s %s", ip_str, port_str);
            if (is_ip_str(ip_str) && is_port_str(port_str)) {
                pthread_rwlock_wrlock(&rule_lock);
                delete_rule(thread_args->rule_data, ip_str, port_str, message_buffer);
                pthread_rwlock_unlock(&rule_lock);
            } else sprintf(message_buffer, INVALID_DELETE);
            break;

        case 'L':
            if ((stream_buffer= (char *) malloc(thread_args->rule_data->list.line_count * MAX_LINE_LENGTH * sizeof(char))) == NULL) {
                printf("Allocation failed.\n");
                exit(-1);
            }
            bzero(stream_buffer, strlen(stream_buffer));

            pthread_rwlock_rdlock(&rule_lock);
            stream(thread_args->rule_data, stream_buffer);
            pthread_rwlock_unlock(&rule_lock);

            n = write(thread_args->socket_fd, stream_buffer, strlen(stream_buffer));
            if (n < 0) error("ERROR writing to socket\n");

            finished_streaming = true;
            free(stream_buffer);
            break;

        default:
            sprintf(message_buffer, "Invalid Request\n");
            break;
    }
    if (!finished_streaming) {
        n = write(thread_args->socket_fd, message_buffer, BUFFER_LENGTH);
        if (n < 0) error("ERROR writing to socket\n");
    }
    server_threads[thread_args->thread_index].status = THREAD_FINISHED;
    pthread_cond_signal(&thread_condition);
    close(thread_args->socket_fd);
    free(thread_args);
    pthread_exit(&return_value);
}

// This is copied directly from the example, which states it requires no modification
void* wait_for_threads(void *args) {
    int i;
    while (1) {
        pthread_mutex_lock(&thread_end_lock);
        pthread_cond_wait(&thread_condition, &thread_end_lock);
        pthread_mutex_unlock(&thread_end_lock);

        // START OF CRITICAL REGION
        pthread_rwlock_rdlock(&thread_lock);
        for (i=0; i < thread_count; i++) {
            if (server_threads[i].status == THREAD_FINISHED) {
                // makes the thread available
                if (pthread_join(server_threads[i].pthread_info, NULL) != 0) {
                    fprintf (stderr, "thread joining failed, exiting\n");
                    exit(1);
                }
                server_threads[i].status = THREAD_AVAILABLE;
            }
        }
        pthread_rwlock_unlock(&thread_lock);
        // END OF CRITICAL REGION
    }
}

// All this does is find an available thread for us to use
int find_thread_index() {
    int i, tmp;
    // Hopefully finds one out of the ones already allocated
    for (i = 0; i < thread_count; i++) {
        if (server_threads[i].status == THREAD_AVAILABLE) {
            server_threads[i].status = THREAD_IN_USE;
            return i;
        }
    }
    // no available thread found; need to allocate more threads
    pthread_rwlock_wrlock(&thread_lock);
    thread_count += THREADS_ALLOCATED;
    server_threads = realloc(server_threads, (thread_count * sizeof(struct threadInfo_t)));
    pthread_rwlock_unlock (&thread_lock);
    if (server_threads == NULL) {
        fprintf (stderr, "Memory allocation failed\n");
        exit (1);
    }
    // initialise thread status and return first available thread that we just allocated
    for (tmp = i+1; tmp < thread_count; tmp++) {
        server_threads[tmp].status = THREAD_AVAILABLE;
    }
    server_threads[i].status = THREAD_IN_USE;
    return i;
}
