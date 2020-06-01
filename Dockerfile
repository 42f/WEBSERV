# Build a wordpress server

FROM debian:10

MAINTAINER bvalette <bvalette@student.42.fr>

# DEFINES
ENV DEBIAN_FRONTEND="noninteractive"
ENV MYSQL_ROOT_PASSWORD="user42"
ENV MYSQL_DATABASE="wordpress"
ENV MYSQL_USER="user"
ENV MYSQL_PASSWORD="42"

#EXPOSE PORTS FOR NGINX AND PHPYADMIN
EXPOSE 80
EXPOSE 8080

# INSTALL ALL PACKAGED
ADD ./srcs/sources /tmp/
ADD ./srcs/packages_script.sh /tmp/
RUN cat /tmp/sources >> /etc/apt/sources.list
RUN chmod 755 /tmp/packages_script.sh
RUN /tmp/packages_script.sh

# GET WORDPRESS
ADD https://wordpress.org/latest.tar.gz /tmp
RUN tar -xf /tmp/latest.tar.gz && mv wordpress /var/www/
COPY ./srcs/www.conf /etc/php/7.3/fpm/pool.d
# CONFIGURE WORDPRESS
ADD ./srcs/wp-config.php /var/www/wordpress/

# CONFIGURE SITES FOR NGINX
RUN rm /etc/nginx/sites-available/default /etc/nginx/sites-enabled/default
ADD ./srcs/my_site /etc/nginx/sites-available/
ADD ./srcs/phpmyadmin /etc/nginx/sites-available/
RUN ln -s /etc/nginx/sites-available/my_site /etc/nginx/sites-enabled/
RUN ln -s /etc/nginx/sites-available/phpmyadmin /etc/nginx/sites-enabled/

RUN nginx -t

RUN mkdir -p /var/run/php
ADD ./srcs/supervisor.conf /etc/supervisor/conf.d/
RUN service mysql start \
	&& mysql -u root -p -e "CREATE DATABASE ${MYSQL_DATABASE};" \
	&& mysql -u root -p -e "GRANT ALL ON ${MYSQL_DATABASE}.* TO ${MYSQL_USER}@localhost IDENTIFIED BY '${MYSQL_PASSWORD}';"

# DEFAULT COMMAND : supervisor
CMD /usr/bin/supervisord
