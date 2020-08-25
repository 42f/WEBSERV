<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/4/4e/Docker_%28container_engine%29_logo.svg/1920px-Docker_%28container_engine%29_logo.svg.png"
     style="text-align:center" width=200px />

# ft_server
First Docker project for School 42.
Small project which deploys a Wordpress website on the local network. It runs with a mysql db and nginx server.

# Usage

### Build and deploy

`$> docker build -t ft_server .`

**replace with your own password**:

`$> docker run -rm -d -p 80:80 -e "MYSQL_PASSWORD"=[PASSWORD] -p 8080:8080 -p443:443 bv/ft_server`

All websites will be accessible on `http://localhost:80` and `https://localhost:443`

### Cleanup

`$> docker kill $(docker ps -q -n 1 -f "ancestor=ft_server")`

`$> docker rmi $(docker images -q bv/ft_server)`

# Notes for improvements

Security is not optimal (it's the least I can say...) and could be much improved with password in a docker secret.
Currently the wordpress deploys as a brand new, but it would be easy to have it already configured.

## Disclaimer

It is well understood that a container should contain only the bare minimum for one service, this project packs 4 stuff in a container, it is done only to comply with the project framework... For myself I would, of course, use kubernetes to deploy several containers. 
