#!/usr/bin/env python
import os

print "Content-Type: text/html"
print
print """\
<html>
<body>
<h2>Hello World!</h2>
</body>
</html>
"""


for param in os.environ.keys():
   print "<b>%20s</b>: %s<\br>" % (param, os.environ[param])
