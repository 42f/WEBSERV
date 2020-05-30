# Build a wordpress server

FROM debian:10

MAINTAINER bvalette <bvalette@student.42.fr>

RUN apt-get update
RUN apt-get upgrade
# RUN apt-get -y install nginx mariadb-server mariadb-client php-cgi php-common php-fpm php-pear php-mbstring php-zip php-net-socket php-gd php-xml-util php-gettext php-mysql php-bcmath unzip wget git

ADD ./srcs/script.sh /usr/bin/script.sh

RUN chmod 755 /usr/bin/script.sh

EXPOSE 80

ENTRYPOINT ["script.sh"]

VOLUME /volume/data

ENV MYSQL_ROOT_PASSWORD root
