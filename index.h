const char webSiteContent[] PROGMEM = R"=====(
<html>
<title>Controle Portão</title>
<link rel="icon" href="https://t4.ftcdn.net/jpg/01/85/91/59/240_F_185915956_Pw79jOJmiCb9OcHTP927IoKrdO8UHM7W.jpg">
<meta charset="utf-8">
<meta name="mobile-web-app-capable" content="yes">
<meta name="viewport" content="user-scalable=no, width=device-width, initial-scale=1.0, maximum-scale=1.0"/>
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
<style>
body{
  background-color:#E8EAF6;
  margin:auto;
  font-family: Arial, sans-serif;
  color: #B2DFDB;
  font-weight: bold;
}
#header{
  position: absolute;
  width: 100%;
  height: 70px;
  box-shadow: 0px 4px 4px rgba(0, 0, 0, 0.25);
  background-color:#00796B;
  text-decoration:none;
}
h1{
  margin-top: 15px;
  text-align: center;
}
#refresh{
  display: flex;
  margin-left:85%;
  margin-right:0px;
  margin-top: -60px;
  width:43px;
  font-size: 50px;
}
.fa-refresh{
  display: inline-block;
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
  display: table;
  text-decoration:none;
  background: #434343;
  color: #434343;
  line-height: 320px;
  width: 320px;
  height: 320px;
  border-radius: 20%;
  box-shadow: 0px 0px 0px 8px #434343;
  border: solid 2px rgba(255,255,255,0.47);
  transition: 0.3s;
}
.fa-bolt{
  font-size:220px;
  display: table-cell;
  vertical-align: middle;
  text-align: center;
}
#footer{
  position: absolute;
  width: 100%;
  height: 80px;
  bottom: 0px;
  background: #00796B;
  box-shadow: 0px -4px 4px rgba(0, 0, 0, 0.25);
}
</style>
<body ONLOAD="forceReload()">
  <div id="header">
    <h1>Controle Portão</h1>
    <div id="refresh">
      <a href="#" ONCLICK='location.reload(0)'><i href="." class="fa fa-refresh"></i></a>
    </div>
  </div>
<div id="mid">
  <a href="#" id="btn"><i class="fa fa-bolt"></i></a>
</div>
<div id="footer">
  <h4 id="timeOn"></h4>
</div>
</body>
<script>
  var websock;
  var t0 = 0;
  var t1 = 0;
  var state = 0;
  var button = document.getElementById('btn');
  const socketMessageListener = (event) => {
    t0 = performance.now();
    JSONobj = JSON.parse(event.data);
    state = JSONobj.controlOn;
    document.getElementById('timeOn').innerHTML = `Ligado há: ${JSONobj.timeOn}`; 
      if(JSONobj.controlOn == 'ON') {
        button.style.background = '#E3E3E3';
        button.style.color = '#434343';
        button.style.boxShadow = '0px 0px 0px 0px #E3E3E3';
      }
      else {
        button.style.background = '#434343';
        button.style.color = '#E3E3E3';
        button.style.boxShadow = '0px 0px 0px 8px #434343';
      }
  };

  const socketOpenListener = (event) => {
    console.log('Connected');
  };

  const socketCloseListener = (event) => {;
    if (websock) {
      console.error('Disconnected.');
    }
    websock = new WebSocket("ws://" + window.location.hostname + ":88/");
    websock.addEventListener('open', socketOpenListener);
    websock.addEventListener('message', socketMessageListener);
    websock.addEventListener('close', socketCloseListener);
  };

  socketCloseListener();   
button.onclick = function (event) {
  if(state === 'OFF') {
    websock.send('LEDonoff=ON');
  }
}
function forceReload() {
  setInterval(verify, 2000);
}
function verify() {
  if(t1 - t0 > 5000) {
    location.reload(0);
  }
  t1 = performance.now();
}
</script>
<html>
)=====";
