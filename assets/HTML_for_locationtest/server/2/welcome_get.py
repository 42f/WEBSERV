import cgi, cgitb

form = cgi.FieldStorage()

name = form.getvalue('name')
email = form.getvalue('email')
print(name)
print(email)
