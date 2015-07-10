#!/bin/sh

tmux kill-window -t +1
tmux new-window -t $SESSION:+1 -c ./bin

tmux split-window -h -c ./bin
tmux split-window -h -c ./bin

tmux select-pane -t 0
tmux send-keys "cd ../build/ && make && cd ../bin && ./binder > outfile" Enter

tmux select-pane -t 1
tmux send-keys "sleep 2 && source ../scripts/get_vars.sh && ./client" Enter

tmux select-pane -t 2
tmux send-keys "sleep 1 && source ../scripts/get_vars.sh && ./server" Enter

