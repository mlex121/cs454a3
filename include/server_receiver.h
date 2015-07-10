#ifndef SERVER_RECEIVER_H
#define SERVER_RECEIVER_H

#include <map>

#include <semaphore.h>

#include "receiver.h"

typedef std::map<CompleteFunction, skeleton> SkeletonLocations;

class ServerReceiver : public NetworkReceiver {
    sem_t message_read_avail;
    sem_t message_write_avail;

    SkeletonLocations skeleton_locations;

    void process_execute(int fd, message *m);

    protected:
        virtual void process_message(int fd, message *m);
        virtual void extra_setup();

    public:
        void add_skeleton(char *name, int *argTypes, skeleton f);
};

#endif
