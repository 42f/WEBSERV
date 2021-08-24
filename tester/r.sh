#!/bin/bash

counter=0
while [ $counter -le 25007 ]
do
	curl -s -S -v -H "User-agent:" google.fr > /dev/null & 
	ret=$?
	if [ $ret -ne 0 ]; then
		kill $!
		exit 1
	fi
	((counter++))
done
