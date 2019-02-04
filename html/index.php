<?php
	session_start();
	
	setlocale(LC_ALL, 'pt_BR');
	
	//echo "iniciando sessao<br>";
	if(isset($_SESSION['ses_mes']))
	{
		//echo "sessao setada<br>";
		$nome_mes = $_SESSION['ses_mes'];
		echo "Mes selecionado ",$nome_mes;
	}else{
		//echo "sessao nao setada escolhendo<br>";
		$nome_mes = strftime("%m");
		//echo $nome_mes,"<br>";
	}
	
	if(isset($_SESSION['ses_host']))
	{
		$host = $_SESSION['ses_host'];
		//echo "<br>Sessao setada - host : ", $host;
	}else{
		$host = "*";
		//echo "<br>Sessao nao setada - host : *";
	}
	require_once("inc/connect.inc.php");
	$titulo_maq = "#5F61BB";
	$chegada = "#CADBFB";
	$saida = "#F3FDB0";
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
  <title>Ponto Digital</title>
  <meta name="GENERATOR" content="Quanta Plus">
  <meta name="AUTHOR" content="Celso Nery">
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
  <link rel="stylesheet" type="text/css" href="inc/ponto.css">
</head>
<body>
		<form action="filtro.php" enctype="application/x-www-form-urlencoded" method="post">
		
  <table border="0" cellpadding="2" cellspacing="0" width="100%" height="20" bgcolor="#c8c8c8">
    <tr> 
      <td width="64">Maquina: </td>
      <td width="95" align="left"> <select name="host" size="1">
          <option value="*"<? if($host == "*"){ echo " selected";}?>>Todas</option>
          <?
								$qry_host = mysql_query("select * from maquinas");
								
								while($res_host = mysql_fetch_array($qry_host))
								{
									echo '<option value="',$res_host[host],'"';
									if($host == $res_host[host]) echo " selected";
									echo '>',$res_host[host],'</option>';
								}
						?>
        </select></td>
      <td width="33">Mes:</td>
      <td width="120" align="left"><select name="mes" size="1">
          <option value="01" <? if($nome_mes == "01"){ echo "SELECTED";}?>>Janeiro</option>
          <option value="02"<? if($nome_mes == "02"){ echo "SELECTED";}?>>Fevereiro</option>
          <option value="03"<? if($nome_mes == "03"){ echo "SELECTED";}?>>Mar&ccedil;o</option>
          <option value="04"<? if($nome_mes == "04"){ echo "SELECTED";}?>>Abril</option>
          <option value="05"<? if($nome_mes == "05"){ echo "SELECTED";}?>>Maio</option>
          <option value="06"<? if($nome_mes == "06"){ echo "SELECTED";}?>>Junho</option>
          <option value="07"<? if($nome_mes == "07"){ echo "SELECTED";}?>>Julho</option>
          <option value="08"<? if($nome_mes == "08"){ echo "SELECTED";}?>>Agosto</option>
          <option value="09"<? if($nome_mes == "09"){ echo "SELECTED";}?>>Setembro</option>
          <option value="10"<? if($nome_mes == "10"){ echo "SELECTED";}?>>Outubro</option>
          <option value="11"<? if($nome_mes == "11"){ echo "SELECTED";}?>>Novembro</option>
          <option value="12"<? if($nome_mes == "12"){ echo "SELECTED";}?>>Dezembro</option>
        </select> </td>
      <td width="461">&nbsp;</td>
      <td width="45"><input type="submit" value="OK"></td>
      <td width="91">Ano Calendario:</td>
      <td width="34">2010</td>
    </tr>
  </table>
		</form>
		<br><br>
<?php
	// Mostra maquinas
	
	if($host != '*')
	{
		$qry_host2 = mysql_query("select * from maquinas where host = '$host'");
	}else{
		$qry_host2 = mysql_query("select * from maquinas");
	}
	
	echo '<table border="0" cellspacing="2"><tr>';
	while($res_host2 = mysql_fetch_array($qry_host2))
	{
		echo '<td valign="top">';
		
		//$qry_pt = mysql_query("select * from entrada where host = '$res_host2[host]' and mes = '$nome_mes' and ano = '2010' order by data");
		$qry_pt = mysql_query("select * from controle where host = '$res_host2[host]' and chegada >= '2010-$nome_mes%' and chegada <= '2010-$nome_mes-31%' order by chegada");
		echo '<table border="1" width="200" cellpadding="2" cellspacing="2"><tr><td colspan="3" bgcolor="',$titulo_maq,'"><font color="white"><b>',$res_host2[host],' / ',$res_host2[ip],'</b></font></td></tr>';
		echo '<tr bgcolor="#B0B1FD"><td>Data</td><td>Chegada</td><td>Sa&iacute;da</td></tr>';
		while($res_pt = mysql_fetch_array($qry_pt))
		{
			echo '<tr><td>',date('d/m/Y',strtotime($res_pt[chegada])),'</td><td bgcolor="',$chegada,'">',date('H:i:s',strtotime($res_pt[chegada])),'</td><td bgcolor="',$saida,'">',date('H:i:s',strtotime($res_pt[saida])),'</tr>';
		}
		echo '</table>&nbsp;</td>';
	}
	echo '</tr></table>';
	
	mysql_free_result($qry_host);
	mysql_free_result($qry_host2);
	mysql_free_result($qry_pt);
	mysql_close(conn);
?>

</body>
</html>
