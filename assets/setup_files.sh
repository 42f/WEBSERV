#! /bin/bash

rm -rf /tmp/server_eval /tmp/cgi_bin

cp -rf ./assets/Server_data/* /tmp
mkdir -p /tmp/cgi_bin

cp ./assets/cgi_bin/py-cgi /tmp/cgi_bin/py-cgi

case "$OSTYPE" in
        darwin*)        echo "MACOS cgi setting"; cp -rf ./assets/cgi_bin/* /tmp/cgi_bin/ ;;
        *)              echo "LINUX cgi setting"; ln -s /usr/bin/php-cgi /tmp/cgi_bin/php-cgi ; cp -rf ./assets/cgi_bin/ubuntu_cgi_tester /tmp/cgi_bin/ubuntu_cgi_tester ;;
esac

ln -s /usr/bin/python /tmp/cgi_bin/python
