const char webSiteContent[] PROGMEM = R"=====(
<html>
<meta charset="utf-8">
<meta name="mobile-web-app-capable" content="yes">
<meta name="viewport" content="user-scalable=no, width=device-width, initial-scale=1.0, maximum-scale=1.0"/>
<link rel="stylesheet" href="https://www.w3schools.com/w3css/4/w3.css">
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">

<script>
  InitWebSocket()
  function InitWebSocket() {
    websock = new WebSocket("ws://" + window.location.hostname + ":88/");
    websock.onmessage = function(evt) {
      JSONobj = JSON.parse(evt.data);
      document.getElementById('btn').innerHTML = JSONobj.controlOn;
      document.getElementById('freeMemory').innerHTML = 'Memória: ' + JSONobj.freeMemory + ' bytes';
      document.getElementById('freeSpace').innerHTML = 'Espaço: ' + JSONobj.freeSpace + ' bytes';
      document.getElementById('timeOn').innerHTML = 'Tempo ligado: ' + JSONobj.timeOn;

      if(JSONobj.controlOn == 'ON') {
        document.getElementById('btn').style.background = '#E3E3E3';
        document.getElementById('btn').style.color = '#434343';
        document.getElementById('btn').style["boxShadow"]="0px 0px 0px 0px #E3E3E3";
      }
      else {
        document.getElementById('btn').style.background = '#434343';
        document.getElementById('btn').style.color = '#E3E3E3';
        document.getElementById('btn').style["boxShadow"]="0px 0px 0px 8px #434343";
      }
    }
  }
  function button() {
    if(document.getElementById('btn').innerHTML === 'OFF') {
      websock.send(websock.send(btn));
    }
  }
</script>
<style>
body, html {
  height: 98%;
  background-color:#E8EAF6;
  font: 10px arial, sans-serif;
  font-weight: bold;
  overscroll-behavior: none;
  overflow: hidden;
}
.card{
  display: flex;
  justify-content: center;
  align-items: center;
  height: 70%;
}
#header{
  position: absolute;
  width: 100%;
  height: 70px;
  box-shadow: 0px 4px 4px rgba(0, 0, 0, 0.25);
  background-color:#00796B;
  font: 50px arial, sans-serif;
  text-decoration:none;
}
h5{
  position: absolute;
  top: 20%;
  font: 40px arial, sans-serif;
  font-weight: bold;
  line-height: 30px;
  text-align: center;
  color: #B2DFDB ;
}
i{
  position: absolute;
  left: 85%;
  right: 5%;
  top: 20%;
  color: #B2DFDB;
  }
#mid{
  display: flex;
  justify-content: center;
  align-items: center;
  height: 100%;
  background: #009688;
}
#btn{
  display: inline-block;
  text-decoration:none;
  background: #000000;
  color: #000000;
  font: 100px arial, sans-serif;
  line-height: 320px;
  width: 320px;
  height: 320px;
  border-radius: 20%;
  text-align: center;
  box-shadow: 0px 0px 0px 8px #8cd460;
  border: solid 2px rgba(255,255,255,0.47);
  transition: 0.4s;  
}
#footer{
  position: absolute;
  width: 100%;
  height: 150px;
  bottom: 0px;
  background: #00796B;
  box-shadow: 0px -4px 4px rgba(0, 0, 0, 0.25);
  font-weight: bold;
  color: #B2DFDB;
}
</style>
<body>
  <div id="header" class="card">
    <h5>Controle Portão</h5>
    <a href="."><i href="." class="fa fa-refresh"></i></a>
    </div>
<div id="mid">
  <a href="#" id="btn" ONCLICK='button()'></a>
</div>
<div id="footer">
  <h2 id="freeMemory"></h2>
  <h2 id="freeSpace"></h2>
  <h2 id="timeOn"></h2>
</div>
</body>
<html>
)=====";
