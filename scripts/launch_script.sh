#!/bin/sh

cd ../build

if ! make; then
    exit 1
fi

tmux kill-window -t +1
tmux new-window -t $SESSION:+1 -c ./bin

tmux split-window -h -c ./bin
tmux split-window -h -c ./bin

tmux select-pane -t 0
tmux send-keys "./binder > outfile" Enter

tmux select-pane -t 1
tmux send-keys "sleep 1 && source ../scripts/get_vars.sh && ./client" Enter

tmux select-pane -t 2
tmux send-keys "sleep 0.5 && source ../scripts/get_vars.sh && ./server" Enter

