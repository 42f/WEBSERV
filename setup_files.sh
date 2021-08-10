#! /bin/bash

mkdir -p /tmp/server/{0..3}
touch /tmp/server/{0..3}/index.html
find /tmp/server -type f | xargs -I % sh -c 'sed s#_#%# ./assets/HTML_pages/index.html > %'
