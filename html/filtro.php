<?php
	session_start();
	
	$_SESSION['ses_mes'] = $_REQUEST['mes'];
	$_SESSION['ses_host'] = $_REQUEST['host'];
	
	header("location:index.php");
?>