#!/bin/bash

if [ $1 == "fg" ]
then
    ./talk $2 A
    
    ./talk $2 B

    ./talk $2 C
else
    (trap 'kill 0' SIGINT; ./talk $2 A & ./talk $2 B & ./talk $2 C &)
fi

exit 0