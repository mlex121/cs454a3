#!/bin/bash

#Server Failure test
./launch_script.sh ../bin/server_error 0.5 
read
tmux kill-window 

#Client Round Robin test
./launch_script.sh ../bin/server2 0.4 ../bin/server 0.5 ../bin/client_round_robin 1
read
tmux kill-window 


#Client Failure test
./launch_script.sh ../bin/server 0.5 ../bin/client_error 1
read
tmux kill-window 

#Long test
./launch_script.sh ../bin/server 0.5 ../bin/client_long 1 ../bin/client 2 ../bin/client_terminator 3

read
tmux kill-window 

#Basic Test
./launch_script.sh ../bin/server 0.5 ../bin/client 1

read
tmux kill-window 

#Basic Termination Test
./launch_script.sh ../bin/server 0.5 ../bin/client_terminator 1

read
tmux kill-window 
