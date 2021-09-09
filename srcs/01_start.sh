#! /bin/bash
VOLUME=$(ls -d $PWD/volume)
docker run --rm -it -p 80:80 -p 8080:8080 -p443:443 -i --mount type=bind,src=$VOLUME,dst=/var/www/volume bv/ft_server
