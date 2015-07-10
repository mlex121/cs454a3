#ifndef SERVER_RECEIVER_H
#define SERVER_RECEIVER_H

#include <map>

#include "receiver.h"

typedef std::map<CompleteFunction, skeleton> SkeletonLocations;

class ServerReceiver : public NetworkReceiver {

    SkeletonLocations skeleton_locations;

    void process_execute(int fd, message *m);

    protected:
        virtual void process_message(int fd, message *m);

    public:
        void add_skeleton(char *name, int *argTypes, skeleton f);
};

#endif
