#!/bin/bash

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
