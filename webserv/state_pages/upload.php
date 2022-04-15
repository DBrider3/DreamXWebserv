<?php
$target_dir = "./uploads/";
$target_file = $target_dir . basename($_FILES["fileToUpload"]["name"]);
$target_file2 = $target_dir . basename($_FILES["fileToUpload2"]["name"]);
$uploadOk = 1;
$imageFileType = strtolower(pathinfo($target_file,PATHINFO_EXTENSION));
$imageFileType2 = strtolower(pathinfo($target_file2,PATHINFO_EXTENSION));
// Check if image file is a actual image or fake image
if(isset($_POST["submit"])) {
    $check = getimagesize($_FILES["fileToUpload"]["tmp_name"]);
    if($check !== false) {
        echo "File is an image - " . $check["mime"] . ".";
        $uploadOk = 1;
    } else {
        echo "File is not an image.";
        $uploadOk = 0;
    }
    $check = getimagesize($_FILES["fileToUpload2"]["tmp_name"]);
    if($check !== false) {
        echo "File is an image - " . $check["mime"] . ".";
        $uploadOk = 1;
    } else {
        echo "File is not an image.";
        $uploadOk = 0;
    }
}
// Check if file already exists
if (file_exists($target_file) || file_exists($target_file2)) {
    echo "Sorry, file already exists.";
    $uploadOk = 0;
}
// Check file size
if ($_FILES["fileToUpload"]["size"] > 10000000000 || $_FILES["fileToUpload2"]["size"] > 10000000000) {
	echo "Sorry, your file is too large.";
    $uploadOk = 0;
}
// Allow certain file formats
if(($imageFileType != "jpg" && $imageFileType != "png" && $imageFileType != "jpeg"
&& $imageFileType != "gif" && $imageFileType != "txt") &&  ($imageFileType2 != "jpg" && $imageFileType2 != "png" && $imageFileType2 != "jpeg"
&& $imageFileType2 != "gif" && $imageFileType2 != "txt")) {
    echo "hello file1 name $target_file \n";
    echo "hello file2 name $target_file2 \n";
    echo "Sorry, only JPG, JPEG, PNG & GIF files are allowed.";
    $uploadOk = 0;
}
// Check if $uploadOk is set to 0 by an error
if ($uploadOk == 0) {
    echo "Sorry, your file was not uploaded.";
// if everything is ok, try to upload file
} else {
    if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $target_file)) {
        echo "<p>The file ". basename( $_FILES["fileToUpload"]["name"]). " has been uploaded.</p>";
		echo "<br><img src=/uploads/". basename( $_FILES["fileToUpload"]["name"]). ">";
		echo "<br><button type='button' onclick='history.back()'>돌아가기</button>";
    } else {
        echo "<p>Sorry, there was an error uploading your file.</p>";
		echo "<br><button type='button' onclick='history.back()'>돌아가기</button>";
    }
    if (move_uploaded_file($_FILES["fileToUpload2"]["tmp_name"], $target_file)) {
        echo "<p>The file ". basename( $_FILES["fileToUpload2"]["name"]). " has been uploaded.</p>";
		echo "<br><img src=/uploads/". basename( $_FILES["fileToUpload2"]["name"]). ">";
		echo "<br><button type='button' onclick='history.back()'>돌아가기</button>";
    } else {
        echo "<p>Sorry, there was an error uploading your file.</p>";
		echo "<br><button type='button' onclick='history.back()'>돌아가기</button>";
    }
}
phpinfo(INFO_ENVIRONMENT);
phpinfo(INFO_VARIABLES);
?>
