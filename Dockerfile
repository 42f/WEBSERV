# Build a wordpress server

FROM debian:10

MAINTAINER bvalette <bvalette@student.42.fr>

WORKDIR /tmp

RUN apt-get update
RUN apt-get install -y apt-utils
RUN apt-get install -y wget
RUN apt-get install -y gnupg
RUN apt-get install -y lsb-release
ADD ./srcs/script.sh /usr/bin/script.sh

RUN chmod 755 /usr/bin/script.sh

EXPOSE 80

ADD https://dev.mysql.com/get/mysql-apt-config_0.8.13-1_all.deb /tmp
RUN DEBIAN_FRONTEND=noninteractive dpkg -i /tmp/mysql-apt-config_0.8.13-1_all.deb 
RUN apt -y update
#RUN DEBIAN_FRONTEND=noninteractive apt-get -y install mysql-server
RUN ["script.sh"]

