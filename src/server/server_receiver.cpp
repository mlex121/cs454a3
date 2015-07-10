#include <cassert>
#include <iostream>

#include <string.h>

#include "server_receiver.h"
#include "message.h"

using namespace std;

void ServerReceiver::extra_setup() {
    sem_init(&execute_request_read_avail, 0, 0);
    sem_init(&execute_request_write_avail, 0, 1);

    has_initialized = false;
}

void ServerReceiver::rpcInit() {
    has_initialized = true;

    // Spawn worker threads
    for (int i=0; i < MAX_CONCURRENCY; i++) {
        pthread_t tid;
        int ret = pthread_create(&tid, NULL, &ServerReceiver::process_execute, (void *)(this));
        if (ret) {
            // FIXME throw something
            cout << "Error: Unable to spawn worker thread." << endl;
        }

        workers[i] = tid;
    }

    run();
}

void ServerReceiver::add_skeleton(char *name, int *argTypes, skeleton f) {
    // FIXME are we initialized???
    // if yes, don't let this happen
    
    CompleteFunction c = make_pair(name, get_argTypes_string(argTypes));

    int count = skeleton_locations.count(c);

    // Set the location to our current function regardless
    skeleton_locations[c] = f;

    // FIXME
    // check the spec for clarity on this
    // If we had a previous function, throw a warning
    if (count) {
        throw  PREVIOUSLY_REGISTERED_FUNCTION;
    }
}

void *ServerReceiver::process_execute(void *arg) {
    ServerReceiver *sr = (ServerReceiver *)arg;

    while(true)  {
        sem_wait(&sr->execute_request_read_avail);
        execute_request e = sr->execute_requests.front();
        sr->execute_requests.pop_front();
        sem_post(&sr->execute_request_write_avail);

        message *m = e.m;
        char name[MAX_FUNCTION_NAME_LEN];
        unsigned int offset = METADATA_LEN;

        strncpy(name, m->buf + offset, MAX_FUNCTION_NAME_LEN);
        offset += MAX_FUNCTION_NAME_LEN;

        int *argTypes = (int *)(m->buf + offset);
        int arg_count = get_argtypes_len(argTypes);
        offset += arg_count * sizeof(*argTypes);

        //Do not count the trailing zero
        void **args = new void*[arg_count - 1];

        void **args_iterator = args;
        int *argTypes_iterator = argTypes;

        while (*argTypes_iterator) {
            *args_iterator = (void *)(m->buf + offset);

            unsigned int arg_len = *argTypes_iterator & ARG_LEN_MASK;
            if (arg_len == 0) arg_len = 1;
            arg_len *= ARG_SIZES[get_argtype(*argTypes_iterator)];

            cerr << "Arg_len is " << arg_len ;
            cerr << " Argtype is: " << ARG_NAMES[get_argtype(*argTypes_iterator)];
            cerr << endl;

            offset += arg_len;
            
            args_iterator++;
            argTypes_iterator++;
        }

        cerr << "Name is " << name << endl;
        cerr << "Arg Count is " << arg_count << endl;

        assert(offset == *((int *)m->buf));

        CompleteFunction c = make_pair(name, get_argTypes_string(argTypes));

        SkeletonLocations::iterator it = sr->skeleton_locations.find(c);
        if(it != sr->skeleton_locations.end()) {
            int ret_val = it->second(argTypes, args);

            if (!ret_val) {
                sr->send_reply(e.fd, get_execute(EXECUTE_SUCCESS, name, argTypes, (const void **)args));
            }
            else {
                cerr << "Sending back execute failure" << endl;
                sr->send_reply(e.fd, get_execute_failure(REASON_FUNCTION_RETURNED_ERROR));
            }
        }
        else {
            //FIXME function not found
        }
    }

}

/*
void ServerReceiver::process_execute(int fd, message *m) {
    cerr << "Got an execute message" << endl;

    char name[MAX_FUNCTION_NAME_LEN];

    unsigned int offset = METADATA_LEN;

    strncpy(name, m->buf + offset, MAX_FUNCTION_NAME_LEN);
    offset += MAX_FUNCTION_NAME_LEN;

    int *argTypes = (int *)(m->buf + offset);

    // Do not count the trailing zero
    int arg_count = get_argtypes_len(argTypes);

    offset += arg_count * sizeof(*argTypes);

    //Do not count the trailing zero
    void **args = new void*[arg_count - 1];

    void **args_iterator = args;
    int *argTypes_iterator = argTypes;


    while (*argTypes_iterator) {
        *args_iterator = (void *)(m->buf + offset);

        unsigned int arg_len = *argTypes_iterator & ARG_LEN_MASK;
        if (arg_len == 0) arg_len = 1;
        arg_len *= ARG_SIZES[get_argtype(*argTypes_iterator)];

        cerr << "Arg_len is " << arg_len ;
        cerr << " Argtype is: " << ARG_NAMES[get_argtype(*argTypes_iterator)];
        cerr << endl;

        offset += arg_len;
        
        args_iterator++;
        argTypes_iterator++;
    }

    cerr << "Size is " << *((int *)m->buf) << endl;
    //cerr << "Offset is " << offset << endl;

    assert(offset == *((int *)m->buf));

    cerr << "Name is " << name << endl;
    cerr << "Arg Count is " << arg_count << endl;

    CompleteFunction c = make_pair(name, get_argTypes_string(argTypes));

    SkeletonLocations::iterator it = skeleton_locations.find(c);
    if(it != skeleton_locations.end()) {
        int ret_val = it->second(argTypes, args);

        if (!ret_val) {
            send_reply(fd, get_execute(EXECUTE_SUCCESS, name, argTypes, (const void **)args));
        }
        else {
            message *m = get_execute_failure(REASON_FUNCTION_RETURNED_ERROR);
            cerr << "Sending back execute failure" << endl;
            send_reply(fd, m);
        }
    }
    else {
        //FIXME function not found
    }
}
*/

void ServerReceiver::process_message(int fd, message *m) {
    switch (*((int *)(m->buf) + 1)) {
        case EXECUTE:
            execute_request e;
            e.fd = fd;
            e.m = m;


            sem_wait(&execute_request_write_avail);
            execute_requests.push_back(e);
            sem_post(&execute_request_read_avail);

            break;

        default:
            delete[] m->buf;
            break;
    }
}

/*
void ServerSender::run() {
    pthread_t worker;
    string s;

    int ret = pthread_create(&worker, NULL, &ServerSender::dispatch, (void *)(this));
    if (ret) {
        cout << "Error: Unable to spawn worker thread." << endl;
    }

    cout << "Working" << endl;

    while (getline(cin, s)) {
        if (s.length()) {
            sem_wait(&write_avail);
            client_data.push_back(s);
            sem_post(&read_avail);
        }
    }
}
*/

/*
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
*/

/*
void* ServerSender::dispatch(void *arg) {
  ServerSender *c = (ServerSender *)arg;

  int size;
  char buf[MAX_DATA_LEN];
  message *m;

  string s;

  while(true) {
    //Lock the client data structure while we grab the first item
    sem_wait(&c->read_avail);
    s = c->client_data.front();
    c->client_data.pop_front();
    sem_post(&c->write_avail);

    //Send data to server
    if (send(c->sock_fd, s.c_str(), s.length(), 0) == -1) {
        perror("send");
    }

    //Receive server reply
    if ((size = recv(c->sock_fd, buf, MAX_DATA_LEN, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    m = (message*)(buf);

    //cout << "Length: " << m->length << endl;
    cout << "Server: " << m->s << endl;

    //Wait at least two seconds before attempting to send more data
    sleep(2);
  }

  pthread_exit(0);
}
*/
