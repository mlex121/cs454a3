#ifndef SERVER_RECEIVER_H
#define SERVER_RECEIVER_H

#include <list>
#include <map>

#include <semaphore.h>

#include "receiver.h"

#define MAX_CONCURRENCY 10

typedef std::map<CompleteFunction, skeleton> SkeletonLocations;

struct execute_request {
    int fd;
    message *m;
};

class ServerReceiver : public NetworkReceiver {
    bool has_initialized;

    sem_t execute_request_read_avail;
    sem_t execute_request_write_avail;

    SkeletonLocations skeleton_locations;

    pthread_t workers[MAX_CONCURRENCY];
    std::list<execute_request> execute_requests;
    static void *process_execute(void *arg);

    protected:
        virtual void process_message(int fd, message *m);
        virtual void extra_setup();

    public:
        void add_skeleton(char *name, int *argTypes, skeleton f);
        void rpcInit();
};

#endif
