#! /bin/bash

docker ps --filter ancestor=bv/ft_server -q | xargs -I % docker exec % sed -i 's/autoindex on/autoindex off/' /etc/nginx/sites-available/wordpress
docker ps --filter ancestor=bv/ft_server -q | xargs -I % docker exec % nginx -s reload
