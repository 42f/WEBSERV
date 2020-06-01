CREATE DATABASE wordpress;
GRANT ALL PRIVILEGES ON wordpress.* TO 'root'@'localhost';
FLUSH PRIVILEGES;
UPDATE mysql.user SET plugin = 'mysql_native_password' WHERE user='root';
