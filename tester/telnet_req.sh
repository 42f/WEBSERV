#! /bin/bash

request() {
	echo "open $1 $2"
	sleep 2
	echo "$5 $4 HTTP/1.0"
	echo "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1.4) Gecko/20070515 Firefox/2.0.0.4"
	echo "Host: $3"
	echo
	echo
	sleep 2
}

if [ $# -eq 0 ]; then
    echo "No arguments provided"
    echo "./telnet_req.sh [IP] [PORT] [HOST_HEADER] [TARGET] [METHOD]"
    echo "ex.: ./telnet_req.sh [127.0.0.1] [18000] [localhost] [/] [GET]"
    exit 1
else
	request $1 $2 $3 $4 $5 | telnet
fi

