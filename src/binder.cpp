#include <iostream>
#include <map>

#include <string.h>

#include "receiver.h"

using namespace std;

class BinderReceiver : public NetworkReceiver {

    //std::map<int, message_assembly> received_messages;


    protected:
        virtual void extra_setup();
        virtual void process_message(int fd);
};

void BinderReceiver::extra_setup() {
    cout << "SERVER_ADDRESS " << hostname << endl;
    cout << "SERVER_PORT " << ntohs(addr_info.sin_port) << endl;
}

void process_registration(message *m) {

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

    cerr << "Hostname is: " << hostname << endl;
    cerr << "Port is: " << port << endl;

    cerr << "Message port is: " << m->buf + 2 + METADATA_LEN + MAX_HOSTNAME_LEN << endl;

    cerr << "Name is: " << name << endl;

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
