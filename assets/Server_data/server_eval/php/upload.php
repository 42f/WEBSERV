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
    <?php
      $target_dir = "/tmp/server_eval/uploads_dir/";
      $target_file = $target_dir . basename($_FILES["fileToUpload"]["name"]);
      $uploadOk = 1;
      $imageFileType = strtolower(pathinfo($target_file,PATHINFO_EXTENSION));
      // Check if image file is a actual image or fake image
      $uploadOk = 1;
      if ($uploadOk == 0) {
      echo "<h2>Sorry, your file was not uploaded.</h2>";
      // if everything is ok, try to upload file
      } else {
      if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $target_file)) {
       echo "
        <div id=\"note\">
          You can delete the file with:<br>
          curl -vX DELETE localhost:18000/uploaded_files/". htmlspecialchars( basename( $_FILES["fileToUpload"]["name"])) ."
        </div>";

      echo "<h2>The file ". htmlspecialchars( basename( $_FILES["fileToUpload"]["name"])). " has been uploaded.</h2>";
      } else {
      echo "<h2>Sorry, there was an error uploading your file.</h2>";
      }
      }
    ?>
    <a class="button-line" href="/uploaded_files">See autoindex of Uploads</a>
    <a class="button-line" href="/">Go back Home</a>
  </div>
</div>
<!-- partial -->

</body>
</html>



