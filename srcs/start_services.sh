#! /bin/bash

service mysql start
/usr/sbin/php-fpm7.3 start
service nginx start


#/usr/sbin/nginx -g "daemon off;"

service nginx status
service php7.3-fpm status
service mysql status

