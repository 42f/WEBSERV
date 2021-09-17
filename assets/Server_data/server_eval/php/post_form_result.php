<!DOCTYPE html>
<html lang="en" >
<head>
  <meta charset="UTF-8">
  <title>Webserv Project</title>
    <link href='https://fonts.googleapis.com/css?family=Open+Sans:400,800' rel='stylesheet' type='text/css'>
  <link href='https://fonts.googleapis.com/css?family=Poiret+One' rel='stylesheet' type='text/css'><link rel="stylesheet" href="/css/style.css">

</head>
<body>
<!-- partial:index.partial.html -->
<div class="full-screen">
  <div>
    <h2>
    Welcome <?php echo $_POST["name"]; ?><br>
    Your email address is: <?php echo $_POST["email"]; ?><br>
    The sum of your numbers is <?php
      $sum = (int)$_POST["one"] + (int)$_POST["two"];
      echo $sum;
    ?>
  </h2>
    <a class="button-line" href="/">Go back Home</a>
  </div>
</div>
<!-- partial -->

</body>
</html>



