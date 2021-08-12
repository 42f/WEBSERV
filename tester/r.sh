#!/bin/bash

counter=0
while [ $counter -le 257 ]
do
	curl -s -S -v 127.0.0.1:18000/bonjour > /dev/null & 
	ret=$?
	if [ $ret -ne 0 ]; then
		kill $!
		exit 1
	fi
	((counter++))
done
