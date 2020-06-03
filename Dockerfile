## FT_SERVER PROJECT : build a wordpress server

FROM debian:10

MAINTAINER bvalette <bvalette@student.42.fr>

# DEFINES
ENV DEBIAN_FRONTEND="noninteractive"
ENV MYSQL_ROOT_PASSWORD="user42"
ENV MYSQL_DATABASE="wordpress"
ENV MYSQL_USER="user"
ENV MYSQL_PASSWORD="42"


# EXPOSE PORTS FOR NGINX AND PHPYADMIN
EXPOSE 80
EXPOSE 8080


# UPDATE CONTAINER TIMEZONE FOR SSL
ENV TZ Europe/Paris
RUN cp /usr/share/zoneinfo/Europe/Paris /etc/localtime


# INSTALL ALL PACKAGED
ADD ./srcs/conf_files/sources /tmp/
ADD ./srcs/scripts/packages_script.sh /tmp/
RUN cat /tmp/sources >> /etc/apt/sources.list
RUN chmod 755 /tmp/packages_script.sh
RUN /tmp/packages_script.sh


# GENERATE SSL KEY
RUN  openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
	-subj /C=FR/ST=IDF/L=Paris/O=FT_SERVER/OU=CeProjetSentLePoney/CN=127.0.0.1 \
	-keyout /etc/ssl/private/nginx-selfsigned.key \
	-out /etc/ssl/certs/nginx-selfsigned.crt
COPY ./srcs/conf_files/self-signed.conf /etc/nginx/snippets/


# GET WORDPRESS
COPY ./srcs/assets/latest.tar.gz /tmp
RUN tar -xf /tmp/latest.tar.gz && mv wordpress /var/www/
COPY ./srcs/conf_files/www.conf /etc/php/7.3/fpm/pool.d
# CONFIGURE WORDPRESS
ADD ./srcs/conf_files/wp-config.php /var/www/wordpress/


# LINK PHPMYADMIN
RUN cp -rf /usr/share/phpmyadmin /var/www/


# CONFIGURE SITES FOR NGINX
RUN rm  /etc/nginx/sites-enabled/default
ADD ./srcs/conf_files/wordpress /etc/nginx/sites-available/
ADD ./srcs/conf_files/phpmyadmin /etc/nginx/sites-available/
RUN ln -s /etc/nginx/sites-available/wordpress /etc/nginx/sites-enabled/default
RUN ln -s /etc/nginx/sites-available/phpmyadmin /etc/nginx/sites-enabled/
# RUN echo "<h1 style="text-align:center">FT_SERVER Kikoo<br/><br/><img src="https://media.giphy.com/media/l3V0uEmPgKpjZH6ve/source.gif" width="1440" height="1065"></h1>" > /var/www/index.html
RUN nginx -t

# CONFIGURE PHPMYADMIN
RUN mkdir -p /var/run/php
ADD ./srcs/sql/create_db.sql /tmp
ADD ./srcs/conf_files/config.ini.php /tmp
RUN service mysql start \
	&& mysql -u root -p -e "CREATE DATABASE ${MYSQL_DATABASE};" \ 
	&& mysql -u root -p -e "GRANT ALL PRIVILEGES ON *.* TO ${MYSQL_USER}@localhost IDENTIFIED BY '${MYSQL_PASSWORD}';" \
	&& mysql -u root -p -e "GRANT ALL PRIVILEGES ON *.* TO phpmysql@localhost IDENTIFIED BY '${MYSQL_PASSWORD}';" \
	&& mysql -u root -p -e "FLUSH PRIVILEGES;" \
	&& mv /tmp/config.ini.php /etc/phpmyadmin/ \
	&& mysql < /tmp/create_db.sql  

# DEFAULT COMMAND : supervisor
ADD ./srcs/conf_files/supervisor.conf /etc/supervisor/conf.d/
CMD /usr/bin/supervisord
