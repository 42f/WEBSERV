#!/bin/bash

counter=0
while [ $counter -le 10000 ]
do
	curl -s -S 127.0.0.1:18000/bonjour > /dev/null & 
	((counter++))
done
