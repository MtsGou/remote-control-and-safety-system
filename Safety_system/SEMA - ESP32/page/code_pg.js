/* -------- JAVASCRIPT ----------*/

// VARIAVEL PARA INFORMAR ESTADO DO RELE 
// 0 ----> NAO ACIONADO // 1----> ACIONADO
var PAGE = 0;

// VARIAVEL PARA INFORMAR ERRO DO STM
var ERRO_STM = 0;

// VARIAVEL PARA INFORMAR SE FREIO FOI ACIONADO		
// POR PARTES DO SISTEMA (STM32)		
var stm_acionado = 0;

// INICIALIZACAO DOS STATEMENTS
var statement1 = "EM CASO DE EMERG&EcircNCIA, PRESSIONE O BOT&AtildeO ABAIXO ";
var statement2 = "Aten&ccedil&atildeo! S&oacute pressionar em caso de emerg&ecircncia.";
var statement3 = "Ao pressionar, o freio ser&aacute acionado.";
var statement4 = "";

// ENDERECO DA CONEXAO WS WEBSOCKET
var gateway = `ws://${window.location.hostname}/ws`;

// VARIAVEL SOCKET
var websocket;

var t = `<!DOCTYPE html>
<html>
<head>

<!---------------------------------
-------------REPORT----------------
-----------------------------------
-------------------------------!-->

<!-------STYLE------!-->

<style>

/*-------CSS--------*/

@font-face {
  font-family: 'Overpass';
  font-style: normal;
  font-weight: 400;
  src: url('Overpass-bold.ttf');
}

h1 {
  text-align: center;
  font-family: 'Overpass';
  font-style: normal;
  font-weight: bold;
  letter-spacing: -2px;
}


h2 { 
font-family: 'Overpass';
font-style: normal;
font-weight: normal;
text-align: center;
font-size: 25px;
letter-spacing: -2px;
}

.content {
padding: 30px;
max-width: 600px;
margin: 0 auto;
}

.classp1 {font-family: 'Overpass'; text-align: center; font-size: 18px;}

.classp2 {font-family: trebuchet ms, sans-serif; text-align: left; font-size: 16px;}

/*--TOPO--*/
.topnav {
overflow: hidden;
background-color: Gray;
}

/*--RODAPE--*/

#container {
position:relative;
}

#body {
padding:10px;
padding-bottom:60px;   /* Height of the footer */
}

</style>
 
<!-----TITULO DA PAGINA----!-->

<title> RELATORIO SEMA </title>
</head>
<body style = "background-color: white; margin: 0;">

<!-----CABECALHO TOPO------!-->
<div class = "topnav">
</br>
<h1><strong> RELAT&OacuteRIO - SISTEMA DE EMERG&EcircNCIA E MONITORAMENTO DO AUT&OcircNOMO (SEMA) </strong> </h1>
<h2> &copy FAKE&COMPANY Constru&ccedil&atildeo S/A.</h2>
</div>

</br>

<div>
<p class = "classp1">Relat&oacuterio obtido do dia:</p>
<p class = "classp1" id="GetDate_time"></p>
</br>
<div><p class = "classp2">%ReportCont%</p></div>

<p style = "text-align: center;"> Acesse o 
<a href = https://www.sejda.com/pt/html-to-pdf> link </a> para converter para PDF. </p>

<script>
const date_now = new Date();
document.getElementById("GetDate_time").innerHTML = date_now;
<\/script>

</body>
</html>`;
   
var f = "Relatorio_Operacao_SEMA.html";

var BUFFER_REPORT = "";

window.addEventListener('load',onLoad); //quando a pagina carregar (load)

function onLoad(event){
  initWebSocket(); //INICIALIZAR A CONEXAO WEBSOCKET
  document.getElementById("statement1").innerHTML = statement1;
  document.getElementById("statement2").innerHTML = statement2;
  document.getElementById("statement3").innerHTML = statement3;
  document.getElementById("statement4").innerHTML = statement4;
}

function initWebSocket(){

console.log('Tentando abrir uma conexao websocket...');
websocket = new WebSocket(gateway);
websocket.onopen    = onOpen;
websocket.onclose   = onClose;
websocket.onmessage = onMessage;
}

  function onOpen(event) {
    console.log('Conexao Estabelecida.');
  }
  
  function onClose(event) {
    console.log('Conexao Encerrada.');
    setTimeout(initWebSocket, 2000 
    /*2 segundos para reconectar*/);
  }
  
  function onMessage(event) {
    var state;
    var statement4 = ""; 
    if (event.data == "1"){
      PAGE = 1;
    }
    else if (event.data == "0"){
      PAGE = 0;
    }
    else if (event.data == "ERRO! PROBLEMA NO HARDWARE STM32. FREIO FOI ACIONADO!"){
      console.log(event.data);
      ERRO_STM = 1;
    }
    else if (event.data == "Comunicacao com STM recuperada."){
      console.log(event.data);
      ERRO_STM = 0;
    }
    else if (event.data == "CPU ORDER TO EMERGENCY BREAK"){
      PAGE = 1;
      console.log(event.data);
      statement4 = event.data;
    }
    else if (event.data == "Freio acionado pelo STM."){
      stm_acionado = 1;
      console.log(event.data);
    }
    else if (event.data == "Freio desativado pelo STM."){
      stm_acionado = 2;
      console.log(event.data);
    }
    else if (event.data == "DOWNLOAD REPORT"){
      download(f, t);
    }
    else if (event.data.startsWith("REPORT DATA:")){
      BUFFER_REPORT = event.data;
      const reg = /REPORT DATA: .*[/]p>/
      t = t.replace(reg, BUFFER_REPORT);
    }
    else if (event.data == "ABORTAR COMANDO CPU. FREIO ACIONADO."){
      PAGE = 1;
      console.log(event.data);
      statement4 = event.data;
    }
    else {
      console.log(event.data);
      statement4 = event.data;
    }
    
    if (PAGE){state = "DESARMAR";} else {state = "STOP";}
    AtualizarStatements();
    document.getElementById("state").innerHTML = state;
    document.getElementById("statement1").innerHTML = statement1;
    document.getElementById("statement2").innerHTML = statement2;
    document.getElementById("statement3").innerHTML = statement3;
    document.getElementById("statement4").innerHTML = statement4;
  }

function initButton() {
  Alerta(); // ALERTA DE CONFIRMACAO DA ACAO
  AtualizarStatements();
  document.getElementById("statement1").innerHTML = statement1;
  document.getElementById("statement2").innerHTML = statement2;
  document.getElementById("statement3").innerHTML = statement3;
}

function Alerta()
{
  if (PAGE == 0){
    PAGE = 1;
    websocket.send('ON');
   }
  else if (PAGE == 1){
    
     var r=confirm("Desarmar o sistema?");
     if (r==true)
       {
         PAGE = 0;
         websocket.send('OFF');
       }
     else
       {
         PAGE = 1;
       }  
   }
}

function AtualizarStatements(){

  if (PAGE == 1){
    if (!ERRO_STM){
      statement1 = "O SISTEMA DE FRENAGEM DE EMERG&EcircNCIA FOI ACIONADO.";
    }
    else {statement1 = "O SISTEMA DE FRENAGEM DE EMERG&EcircNCIA FOI ACIONADO DEVIDO A FALHA DE HARDWARE. STM NAO RESPONDE.";}    
      statement2 = "Para restaurar o mecanismo de frenagem,";
      statement3 = "Selecione \"Desarmar\"";
    } 
  else if (PAGE == 0){
    if (!ERRO_STM){
      statement1 = "EM CASO DE EMERG&EcircNCIA, PRESSIONE O BOT&AtildeO ABAIXO.";
    }
    else {statement1 = "EM CASO DE EMERG&EcircNCIA, PRESSIONE O BOT&AtildeO ABAIXO. FALHA DE HARDWARE. STM NAO RESPONDE.";}  
      statement2 = "Aten&ccedil&atildeo! S&oacute pressionar em caso de emerg&ecircncia.";
      statement3 = "Ao pressionar, o freio ser&aacute acionado.";
    }
  if (stm_acionado == 1){
    statement3 += " Freio STM acionado!";
  }
  else if (stm_acionado == 2){
    statement3 += " Freio STM desativado!";
  }

}

function download(f, t) {
		var e = document.createElement('a');
		e.setAttribute('href','data:text/html;charset=utf-8, '
		+ encodeURIComponent(t));
		e.setAttribute('download', f);
		document.body.appendChild(e);
		e.click();
		document.body.removeChild(e);
}

document.getElementById("Download").addEventListener("click", function() {
		download(f, t);
}, false);

