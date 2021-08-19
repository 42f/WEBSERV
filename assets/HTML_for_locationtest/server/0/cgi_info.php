<!doctype html>

<html lang="en">

<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<style>
		h1 {
			text-align: center;
			font-family: arial, sans-serif;
		}

		p {
			/* text-align: center; */
		}

		div {
			text-align: center;
		}
	</style>
	<title>ABOUT</title>
	<meta name="description" content="Coucou">
	<meta name="author" content="team ABC">

	<meta property="og:title" content="A Basic HTML5 Template">
	<meta property="og:type" content="website">
	<meta property="og:description" content="A webserv">
	<meta property="og:image" content="image.png">

	<link rel="icon" href="/imgs/favicon.ico">


</head>


<body>
	<h1>
		Here are the CGI infos base on the meta-variables: <br />
	</h1>
	<p><?php echo "GATEWAY_INTERFACE : ", $_SERVER['GATEWAY_INTERFACE']; ?> </p>
	<p><?php echo "SERVER_PROTOCOL : ", $_SERVER['SERVER_PROTOCOL']; ?> </p>
	<p><?php echo "SCRIPT_FILENAME : ", $_SERVER['SCRIPT_FILENAME']; ?> </p>
	<p><?php echo "SCRIPT_NAME : ", $_SERVER['SCRIPT_NAME']; ?> </p>
	<p><?php echo "REDIRECT_STATUS : ", $_SERVER['REDIRECT_STATUS']; ?> </p>
	<p><?php echo "PATH_INFO : ", $_SERVER['PATH_INFO']; ?> </p>

</body>

</html>