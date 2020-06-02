#! /bin/bash

apt-get update
apt-get install -y apt-utils
apt-get upgrade
apt-get install -y nginx
apt-get install -y php7.3-fpm
apt-get install -y supervisor
apt-get install -y phpmyadmin
apt-get install -y mariadb-server
apt-get install -y locate && updatedb
apt-get install -y vim
apt-get install -y curl
curl -O https://raw.githubusercontent.com/wp-cli/builds/gh-pages/phar/wp-cli.phar
chmod +x wp-cli.phar
mv wp-cli.phar /usr/local/bin/wp
wp cli update
