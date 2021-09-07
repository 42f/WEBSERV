<html>
<body>

Welcome <?php echo $_POST["name"]; ?><br>
Your email address is: <?php echo $_POST["email"]; ?><br>
The sum of your numbers is <?php
    $sum = (int)$_POST["one"] + (int)$_POST["two"];
    echo $sum;
?>
</body>
</html>
