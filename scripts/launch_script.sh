#!/bin/sh

cd ../build

if ! make; then
    exit 1
fi


tmux kill-window -t +1
tmux new-window -t $SESSION:+1 -c ./bin

tmux send-keys "./binder > outfile" Enter

while [ "$1" ]; do

    tmux split-window -h -c ./bin
    tmux send-keys "sleep $2 && source ../scripts/get_vars.sh && $1" Enter

    shift
    shift
done

