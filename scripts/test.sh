#!/bin/bash

killall binder &> /dev/null
killall server &> /dev/null

./binder > outfile &

# Pretty sure this is actually using the old outfile handle or soemthing similar
for _ in {0..1}; do
  read line;
  export $(echo $line | sed 's/ /=/' ) ;
done < outfile

#echo $SERVER_ADDRESS
#echo $SERVER_PORT

./server
