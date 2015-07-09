#!/bin/bash

while read line; do
    export $(echo $line | sed 's/ /=/' ) ;
done < outfile

