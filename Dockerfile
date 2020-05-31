# Build a wordpress server

FROM debian:10

MAINTAINER bvalette <bvalette@student.42.fr>

ENV DEBIAN_FRONTEND noninteractive
EXPOSE 80

ADD ./srcs/sources /tmp/
RUN cat /tmp/sources >> /etc/apt/sources.list
RUN apt-get update
RUN apt-get install -y apt-utils
RUN apt-get upgrade
RUN apt-get install -y nginx
RUN apt-get install -y php7.3-fpm
RUN apt-get install -y supervisor
RUN apt-get install -y mariadb-server
RUN apt-get install -y phpmyadmin

RUN rm /etc/nginx/sites-available/default /etc/nginx/sites-enabled/default
ADD ./srcs/my_site /etc/nginx/sites-available/
RUN ln -s /etc/nginx/sites-available/my_site /etc/nginx/sites-enabled/my_site
RUN mkdir /var/www/my_site
RUN echo "<?php\n phpinfo();\n ?>" > /var/www/my_site/index.php
RUN nginx -t
RUN service nginx restart

ADD ./srcs/www.conf /etc/php/7.3/fpm/pool.d
ADD ./srcs/supervisor.conf /etc/supervisor/conf.d/

#RUN mysql_secure_installation
CMD ["/usr/bin/supervisord"]








#ADD ./srcs/script.sh /usr/bin/srcs/
#RUN chmod 755 /usr/bin/srcs/script.sh
#RUN locate www.conf | xargs cat -n | grep "listen = " 
#RUN cat /usr/bin/srcs/location_directive > /etc/nginx/sites-available/my_site
#RUN sed 's#listen =[.]##' /etc/sphp/7.3/fpm/pool.d/www.conf | grep listen

