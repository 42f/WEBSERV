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
p {text-align: center;}
div {text-align: center;}
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
		Webserver php infos here: <br/>
		<input type="button" value="Go back!" onclick="history.back()"> <br/>
		<?php phpinfo(); ?>
	</h1>
</body>
</html>
