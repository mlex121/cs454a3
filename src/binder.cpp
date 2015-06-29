#include <iostream>

#include "receiver.h"

using namespace std;

class BinderReceiver : public NetworkReceiver {
    protected:
        virtual void extra_setup();
};

void BinderReceiver::extra_setup() {
    cout << "SERVER_ADDRESS " << hostname << endl;
    cout << "SERVER_PORT " << ntohs(addr_info.sin_port) << endl;
}

int main(int argc, char **argv) {
    BinderReceiver b; 

    b.run();

    return 0;
}
