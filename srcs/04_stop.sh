#! /bin/bash

docker ps --filter ancestor=bv/ft_server -q | xargs -I % docker exec % service supervisor stop

