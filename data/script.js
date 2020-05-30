let url = "ws://" + window.location.hostname + ":1337/";
let button;
let timer;
let clickSoundOk;
let clickSoundBad;
let connected = 0;
let visible = 1;
let state;
let buttonBg;
let pingInterval = 250; //tempo entre pings
let untilReconnect = 2000; //tempo que entre cada tentativa de reconexão com o websocket
let untilDisconnect = 2000; //tempo dado até o app considerar sem respostas e portanto desconexão com o websocket
let untilCloseApp = 40000; //tempo dado até o app considerar que deve ser encerrado por falta de resposta (máximo 40s)
let pingResponse;
let closeApp;
let disconnect;
let reconnect;
let websocket;
const CONNECTING = 0;
const OPEN = 1;
const CLOSING = 2;
const CLOSED = 3; 

CONNECTING
function init() {
  timer = document.getElementById("timeOn");
  button = document.getElementById("btn");
  clickSoundOk = document.getElementById('clickSoundOk');
  clickSoundBad = document.getElementById('clickSoundBad');
  buttonBg = document.getElementsByClassName('buttonHolder')[0];
  pingResponse = setInterval('doPing()', pingInterval);
  clickSoundBad.volume = 0.5;
  wsConnect(url, 'init');
}
init();
function wsConnect(url, reason) {
  if(!visible) return;
  console.log('Se conectando, motivo: ' + reason);
  if(websocket) console.log('Status 1: ' + websocket.readyState);
  if(websocket && (websocket.readyState === OPEN || websocket.readyState === CONNECTING)) return; 
  websocket = new WebSocket(url);
  console.log('Status 2: ' + websocket.readyState);
  websocket.onopen = function(evt) { onOpen(evt) };
  websocket.onclose = function(evt) { onClose(evt) };
  websocket.onmessage = function(evt) { onMessage(evt) };
  websocket.onerror = function(evt) { onError(evt) };
}
function onOpen(evt) {
  setConnected();
}
function onClose(evt) {
  setDisconnected('onClose');
  clearTimeout(reconnect);
  reconnect = setTimeout(function() { wsConnect(url, 'onClose') }, untilReconnect);
}
function onMessage(evt) {
  // console.log("Received: " + evt.data);
  console.log("Received");
  JSONobj = JSON.parse(evt.data);
  state = JSONobj.controlState;
  timer.innerHTML = `Ligado há: ${JSONobj.timeOn}`; 
  buttonBg.style.backgroundColor = 'rgba(255,0,0,0)';
  clearTimeout(disconnect);
  disconnect = setTimeout('setDisconnected(\'timeout\')', untilDisconnect);
  clearTimeout(closeApp);
  closeApp = setTimeout(function(){window.close();}, untilCloseApp);
    if(state == 'ON') {;
      clickSoundOk.play();
      button.style.animation = 'buttonClick .1s forwards';
      button.style.color = 'red';
    }
    else {
      clickSoundOk.pause();
      clickSoundOk.currentTime = 0;
      button.style.animation = 'buttonRelease .1s forwards';
      button.style.color = 'springgreen';
    }
}
function onError(evt) {
  console.log("ERROR: " + evt.data);
}
function doSend(message) {
  if(connected){
    // console.log("Sending: " + message);
    websocket.send(message);
  }
}
button.onclick = function() {
  if(!connected) {
    clickSoundBad.play();
    return;
  }
  if(state == 'OFF')
    doSend("setControl");
}
function doPing() {
  doSend("ping");
}
function onPause() {
  console.log('onPause');
  visible = false;
  clearInterval(pingResponse);  
  clearTimeout(disconnect);
  buttonBg.style.backgroundColor = 'rgba(255,0,0,0.5)';
 }
function onResume() {
  console.log('onResume');
  visible = true;
  clearTimeout(reconnect); // reseta o setTimeout do onClose evitando uma reconexão em cima de outra
  clearInterval(pingResponse);
  pingResponse = setInterval('doPing()', pingInterval);
   if (!connected)
     wsConnect(url, 'onResume');
}
function setConnected() {
  console.log("Connected");
  connected = true;
  buttonBg.style.backgroundColor = 'rgba(0,0,0,0)';
}
function setDisconnected(reason) {
  console.log("Disconnected, Reason: " + reason);
  connected = false;
  websocket.close();
  buttonBg.style.backgroundColor = 'rgba(255,0,0,0.5)';
}
const handleChange = (e) => {
  document.hidden ? onPause() : onResume();
}
document.addEventListener("visibilitychange", handleChange);
window.oncontextmenu = function(event) {
  event.preventDefault();
  event.stopPropagation();
  return false;
};