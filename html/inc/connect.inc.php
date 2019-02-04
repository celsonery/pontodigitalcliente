<?
/*
	Arquivo de conexao com o banco de dados (connect.inc.php)
	criado por: Celso Nery (celso.nery@brfree.com.br)
	variaveis de conexao e gerais	
*/

 // Definindo as variaveis
  $server = "localhost";
  $usuario = "uponto";
  $senha = "senha_do_bd";
  $banco = "ponto";

  // Conectando, escolhendo o banco de dados
   $conn = @mysql_connect("$server", "$usuario", "$senha")
       or die('Imposs&iacute;vel conectar ao servidor - Contate o <a href=\"mailto:celso.nery@gmail.com\">Administrador</a>');
	   
	$bd = @mysql_select_db($banco) or die('N&atilde;o foi poss&iacute;vel selecionar o banco de dados');
?>
