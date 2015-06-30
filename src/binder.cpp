#include <algorithm>
#include <iostream>
#include <list>
#include <unordered_map>
#include <set>
#include <utility>

#include <assert.h>
#include <string.h>

#include "rpc.h"
#include "receiver.h"

using namespace std;

typedef pair<string, string> ServerLocation;

class BinderReceiver : public NetworkReceiver {

    list<ServerLocation> server_priorities;
    
    // using unordered set requires setting up a custom hash function
    unordered_map<string, set<ServerLocation> > function_locations;

    void process_registration(message *m);

    protected:
        virtual void extra_setup();
        virtual void process_message(int fd);
};

void BinderReceiver::extra_setup() {
    cout << "SERVER_ADDRESS " << hostname << endl;
    cout << "SERVER_PORT " << ntohs(addr_info.sin_port) << endl;
}


void BinderReceiver::process_registration(message *m) {

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
    cerr << "Arguments :" << arguments << endl;

    string complete_function_name = name + ':' + arguments;

    ServerLocation server_location = make_pair(hostname, port);

    // First, make sure we don't have an entry for this server already
    // otherwise, insert it at the head of the list so it serves first
    if (find(server_priorities.begin(), server_priorities.end(), server_location) == server_priorities.end()) {
        //server_priorities.push_front(server_location);
    }

    // Check the map to see if we have this function already registered
    if (function_locations.count(complete_function_name)) {
            // We should not have this server already registered for this function
            // if this were the case, the server should have simply updated it local
            // database without sending a request to the binder
            assert(! function_locations[complete_function_name].count(server_location));

            function_locations[complete_function_name].emplace(server_location);
    }
    else {
        function_locations[complete_function_name] = { server_location };
    }
}

void BinderReceiver::process_message(int fd) {
    cerr << "A message was received with total length: " << received_messages[fd].offset << endl;

    message *m = (message *)(&received_messages[fd].buf);

    switch (*((int *)(m->buf) + 1)) {
        case REGISTER:
            process_registration(m);
            break;
        default:
            // TODO throw something
            break;
    }

    delete received_messages[fd].buf;
    received_messages.erase(fd);
}

int main(int argc, char **argv) {
    BinderReceiver b; 

    b.run();

    return 0;
}
