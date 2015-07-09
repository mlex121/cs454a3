#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <utility>

#include <cassert>
#include <string.h>

#include "rpc.h"
#include "receiver.h"

using namespace std;

typedef pair<string, string> ServerLocation;
typedef map<CompleteFunction, set<ServerLocation>> FunctionLocations;

class BinderReceiver : public NetworkReceiver {
    list<ServerLocation> server_priorities;
    
    // using unordered types requires setting up a custom hash function
    FunctionLocations function_locations;

    void process_registration(int fd, message *m);
    void process_request(int fd, message *m);

    protected:
        virtual void extra_setup();
        virtual void process_message(int fd);

    public:
        void print_registrations();
};

void BinderReceiver::extra_setup() {
    cout << "BINDER_ADDRESS " << hostname << endl;
    cout << "BINDER_PORT " << port << endl;
}

void BinderReceiver::print_registrations() {
    for (
        FunctionLocations::iterator func_loc_it=function_locations.begin();
        func_loc_it != function_locations.end();
        func_loc_it++
    )
    {
        CompleteFunction c = func_loc_it->first;
        cerr << "Name: " << c.first << " Args: " << c.second << endl;
        cerr << "Servers: " << endl;

        for (
            set<ServerLocation>::iterator server_loc_it=func_loc_it->second.begin();
            server_loc_it != func_loc_it->second.end();
            server_loc_it++
        )
        {
            cerr << '\t' << server_loc_it->first << ":" << server_loc_it->second << endl;
        }
    }
}


void BinderReceiver::process_registration(int fd, message *m) {
    int message_length = *((int *)m);

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

    /*
    cerr << "Hostname is: " << hostname << endl;
    cerr << "Port is: " << port << endl;
    cerr << "Name is: " << name << endl;
    */

    string arguments = get_argTypes_string((int *)(m->buf + offset));
    //cerr << "Arguments :" << arguments << endl;

    CompleteFunction complete_function = make_pair(name, arguments);
    ServerLocation server_location = make_pair(hostname, port);

    // First, make sure we don't have an entry for this server already
    // otherwise, insert it at the head of the list so it serves first
    if (find(server_priorities.begin(), server_priorities.end(), server_location) == server_priorities.end()) {
        server_priorities.push_front(server_location);
    }

    // Check the map to see if we have this function already registered
    if (function_locations.count(complete_function)) {
            // We should not have this server already registered for this function
            // if this were the case, the server should have simply updated it local
            // database without sending a request to the binder
            assert(! function_locations[complete_function].count(server_location));

            function_locations[complete_function].emplace(server_location);
    }
    else {
        function_locations[complete_function] = { server_location };
    }
}

void BinderReceiver::process_request(int fd, message *m) {
    int message_length = *((int *)m);

    char name[MAX_FUNCTION_NAME_LEN];

    unsigned int offset = METADATA_LEN;

    strncpy(name, m->buf + offset, MAX_FUNCTION_NAME_LEN);
    offset += MAX_FUNCTION_NAME_LEN;

    cerr << "Name is: " << name << endl;

    string arguments = get_argTypes_string((int *)(m->buf + offset));
    CompleteFunction complete_function = make_pair(name, arguments);

    //cerr << "Arguments :" << arguments << endl;

    ServerLocation *s = NULL;

    for (list<ServerLocation>::iterator it = server_priorities.begin(); it != server_priorities.end(); it++) {
        if (function_locations[complete_function].count(*it)) {
            server_priorities.push_back(*it);
            server_priorities.erase(it);
            s = &server_priorities.back();
            break;
        }
    }

    if (s) {
        cerr << "Hostname: " << s->first << " Port: " << s->second << endl;
        //send_reply(fd, get_loc_success(s->first, s->second));
    }
    else {

    }
}

void BinderReceiver::process_message(int fd) {
    cerr << "A message was received with total length: " << received_messages[fd].offset << endl;

    message *m = (message *)(&received_messages[fd].buf);

    //cerr << "Message type is: " << *((int *)(m->buf) + 1) << endl;

    switch (*((int *)(m->buf) + 1)) {
        case REGISTER:
            process_registration(fd, m);
            //print_registrations();
            break;
        case LOC_REQUEST:
            process_request(fd, m);
            break;
        default:
            // TODO throw something
            break;
    }

    delete[] received_messages[fd].buf;
    received_messages.erase(fd);
}

int main(int argc, char **argv) {
    BinderReceiver b; 

    b.run();

    return 0;
}
