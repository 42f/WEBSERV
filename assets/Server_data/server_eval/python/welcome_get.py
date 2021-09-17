import cgi, cgitb

form = cgi.FieldStorage()

name = form.getvalue('name')
email = form.getvalue('email')
print("<center>HelloWorld! From welcome_get.py<br/>")
print("<br/>")
print(name)
print("<br/>")
print(email)
print("</center>")
