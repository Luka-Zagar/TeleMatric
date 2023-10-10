<?php

$hostname = "localhost";
$username = "luka-root";
$password = "luka-root-128";
$database = "Data-flow";

$conn = mysqli_connect($hostname, $username, $password, $database);

if (!$conn) {
	die("Connection failed: " . mysqli_connect_error());
}

echo "Database connection is OK <br>";

if (isset($_POST["lat"]) && isset($_POST["lon"])) {
	
	$ID = 69;
	$Latitude = $_POST["lat"];
	$Longitude = $_POST["lon"];
	$Height = 420;
	$Direction = 0;
	$Speed = 0.0;
	$Satellites = 0;

	$sql = "INSERT INTO `GPS-Data`(`ID`, `Latitude`, `Longitude`, `Height`, `Direction`, `Speed`, `N. of satellites`) 
	VALUES ('$ID', '$Latitude', '$Longitude', '$Height', '$Direction', '$Speed', '$Satellites')";

	if (mysqli_query($conn, $sql)) {
		echo "New record created successfully";
	} else {
		echo "Error: " . $sql . "<br>" . mysqli_error($conn);
	}
}

?>
