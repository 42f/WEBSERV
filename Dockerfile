# Build a wordpress server

FROM debian:10

MAINTAINER bvalette <bvalette@student.42.fr>

ENV DEBIAN_FRONTEND="noninteractive"
ENV MYSQL_ROOT_PASSWORD="user42"
ENV MYSQL_DATABASE="wordpress"
ENV MYSQL_USER="user"
ENV MYSQL_PASSWORD="42"


EXPOSE 80
EXPOSE 8080

# INSTALL ALL PACKAGED
ADD ./srcs/sources /tmp/
ADD ./srcs/packages_script.sh /tmp/
RUN cat /tmp/sources >> /etc/apt/sources.list
RUN chmod 755 /tmp/packages_script.sh
RUN /tmp/packages_script.sh

# SETUP MY_SITE
RUN mkdir /var/www/my_site
ADD https://wordpress.org/latest.tar.gz /tmp
RUN cd /tmp && tar -xf latest.tar.gz && mv wordpress /var/www/
COPY ./srcs/www.conf /etc/php/7.3/fpm/pool.d
ADD ./srcs/wp-config.php /var/www/wordpress/
RUN rm /etc/nginx/sites-available/default /etc/nginx/sites-enabled/default
ADD ./srcs/my_site /etc/nginx/sites-available/
ADD ./srcs/phpmyadmin /etc/nginx/sites-available/
RUN ln -s /etc/nginx/sites-available/my_site /etc/nginx/sites-enabled/
RUN ln -s /etc/nginx/sites-available/phpmyadmin /etc/nginx/sites-enabled/
ADD ./srcs/setup.sql /tmp/

RUN nginx -t

RUN mkdir -p /var/run/php
ADD ./srcs/supervisor.conf /etc/supervisor/conf.d/
CMD /usr/bin/supervisord
RUN service mysql start \
	&& mysql -u root -p -e "CREATE DATABASE ${MYSQL_DATABASE};" \
	&& mysql -u root -p -e "GRANT ALL ON ${MYSQL_DATABASE}.* TO ${MYSQL_USER}@localhost IDENTIFIED BY '${MYSQL_PASSWORD}';"
#RUN  -e WORDPRESS_DB_PASSWORD=<password> --name wordpress --link wordpressdb:mysql -p 80:80 -v "$PWD/html":/var/www/html -d wordpress




#ADD ./srcs/start_services.sh /tmp/
#RUN chmod 755 /tmp/start_services.sh
#RUN /tmp/start_services.sh && tail -f /dev/null








#ADD ./srcs/script.sh /usr/bin/srcs/
#RUN chmod 755 /usr/bin/srcs/script.sh
#RUN locate www.conf | xargs cat -n | grep "listen = " 
#RUN cat /usr/bin/srcs/location_directive > /etc/nginx/sites-available/my_site

