#include <algorithm>
#include <iostream>
#include <list>
#include <utility>

#include <cassert>
#include <string.h>

#include "message.h"
#include "rpc.h"
#include "receiver.h"

// Name conflict on OS X with get_terminate()
// using namespace std;

class BinderReceiver : public NetworkReceiver {
    std::list<ServerLocation> server_priorities;

    // using unordered types requires setting up a custom hash function
    FunctionLocations function_locations;

    void process_registration(int fd, message *m);
    void process_request(int fd, message *m);
    void process_cache_request(int fd, message *m);
    void process_terminate();

    // Variables used for termination
    std::list<int> server_fds;

    protected:
        virtual void extra_setup();
        virtual void process_message(int fd, message *m);

    public:
        void print_registrations();
};

void BinderReceiver::extra_setup() {
    std::cout << "BINDER_ADDRESS " << hostname << std::endl;
    std::cout << "BINDER_PORT " << port << std::endl;
}

void BinderReceiver::print_registrations() {
    for (
        FunctionLocations::iterator func_loc_it=function_locations.begin();
        func_loc_it != function_locations.end();
        func_loc_it++
    )
    {
        CompleteFunction c = func_loc_it->first;
        //cerr << "Name: " << c.first << " Args: " << c.second << endl;
        //cerr << "Servers: " << endl;

        for (
            std::set<ServerLocation>::iterator server_loc_it=func_loc_it->second.begin();
            server_loc_it != func_loc_it->second.end();
            server_loc_it++
        )
        {
            //cerr << '\t' << server_loc_it->first << ":" << server_loc_it->second << endl;
        }
    }
}


void BinderReceiver::process_registration(int fd, message *m) {
    char hostname[MAX_HOSTNAME_LEN];
    char port[MAX_PORT_LEN];
    char name[MAX_FUNCTION_NAME_LEN];

    unsigned int offset = METADATA_LEN;

    strncpy(hostname, m->buf + offset, MAX_HOSTNAME_LEN);
    offset += MAX_HOSTNAME_LEN;

    strncpy(port, m->buf + offset, MAX_PORT_LEN);
    offset += MAX_PORT_LEN;

    strncpy(name, m->buf + offset, MAX_FUNCTION_NAME_LEN);
    offset += MAX_FUNCTION_NAME_LEN;

    //cerr << "Hostname is: " << hostname << endl;
    //cerr << "Port is: " << port << endl;
    //cerr << "Name is: " << name << endl;

    std::string arguments = get_argTypes_string((int *)(m->buf + offset));
    //cerr << "Arguments :" << arguments << std::endl;

    CompleteFunction complete_function = std::make_pair(name, arguments);
    ServerLocation server_location = std::make_pair(hostname, port);

    // First, make sure we don't have an entry for this server already
    // otherwise, insert it at the head of the list so it serves first
    if (find(server_priorities.begin(), server_priorities.end(), server_location) == server_priorities.end()) {
        server_priorities.push_front(server_location);
    }

    // Check the std::map to see if we have this function already registered
    if (function_locations.count(complete_function)) {
            assert(! function_locations[complete_function].count(server_location));

            function_locations[complete_function].insert(server_location);
    }
    else {
        function_locations[complete_function] = { server_location };
    }

    // Add this server's file descriptor, so we can send a terminate message to it
    server_fds.push_back(fd);

    //Send back a sucess message to the server
    send_reply(fd, get_register_success(REGISTRATION_SUCCESSFUL));
}

void BinderReceiver::process_request(int fd, message *m) {
    char name[MAX_FUNCTION_NAME_LEN];

    unsigned int offset = METADATA_LEN;

    strncpy(name, m->buf + offset, MAX_FUNCTION_NAME_LEN);
    offset += MAX_FUNCTION_NAME_LEN;


    std::string arguments = get_argTypes_string((int *)(m->buf + offset));
    CompleteFunction complete_function = std::make_pair(name, arguments);


    ServerLocation *s = NULL;

    for (std::list<ServerLocation>::iterator it = server_priorities.begin(); it != server_priorities.end(); it++) {
        if (function_locations[complete_function].count(*it)) {
            server_priorities.push_back(*it);
            server_priorities.erase(it);
            s = &server_priorities.back();
            break;
        }
    }

    if (s) {
        send_reply(fd, get_loc_success(s->first.c_str(), s->second.c_str()));
    }
    else {
        send_reply(fd, get_loc_failure(REASON_NO_MATCHING_SERVERS));
    }
}

void BinderReceiver::process_cache_request(int fd, message *m) {
    char name[MAX_FUNCTION_NAME_LEN];

    unsigned int offset = METADATA_LEN;

    strncpy(name, m->buf + offset, MAX_FUNCTION_NAME_LEN);
    offset += MAX_FUNCTION_NAME_LEN;

    std::string arguments = get_argTypes_string((int *)(m->buf + offset));
    CompleteFunction complete_function = std::make_pair(name, arguments);

    if (function_locations.count(complete_function)) {
        send_reply(fd, get_cache_loc_success(function_locations[complete_function]));
    } else {
        send_reply(fd, get_cache_loc_failure(REASON_NO_MATCHING_SERVERS));
    }
}

void BinderReceiver::process_terminate() {
    for (std::list<int>::iterator it = server_fds.begin(); it != server_fds.end(); it++) {
        send_reply(*it, get_terminate());
    }

    exit(0);
}

void BinderReceiver::process_message(int fd, message *m) {
    //cerr << "A message was received with total length: " << received_messages[fd].offset << std::endl;
    //cerr << "Message type is: " << *((int *)(m->buf) + 1) << std::endl;

    switch (*((int *)(m->buf) + 1)) {
        case REGISTER:
            process_registration(fd, m);
            break;
        case LOC_REQUEST:
            process_request(fd, m);
            break;
        case CACHE_LOC_REQUEST:
            process_cache_request(fd, m);
            break;
        case TERMINATE:
            process_terminate();
            break;
        default:
            throw UNRECOGNIZED_MESSAGE_TYPE;
            break;
    }

    delete[] m->buf;
}

int main(int argc, char **argv) {
    BinderReceiver b;

    b.run();

    return 0;
}
