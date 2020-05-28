let url = "ws://" + window.location.hostname + ":1337/";
let button;
let timer;
let btn_sound;
let connected = false;
let visible = true;
let state;
let buttonBg;
let relogio = setInterval('doPing()', 250);

function init() {
  timer = document.getElementById("timeOn");
  button = document.getElementById("btn");
  btn_sound = document.getElementById('tickSound');
  buttonBg = document.getElementsByClassName('buttonHolder')[0];
  wsConnect(url);
}
init();
function wsConnect(url) {
  if(!visible) return;
  websocket = new WebSocket(url);
  websocket.onopen = function(evt) { onOpen(evt) };
  websocket.onclose = function(evt) { onClose(evt) };
  websocket.onmessage = function(evt) { onMessage(evt) };
  websocket.onerror = function(evt) { onError(evt) };
}
function onOpen(evt) {
  console.log("Connected");
  connected = true;
  buttonBg.style.backgroundColor = 'rgba(255,0,0,0)';
}
function onClose(evt) {
  console.log("Disconnected");
  connected = false;
  setTimeout(function() { wsConnect(url) }, 2000);
  buttonBg.style.backgroundColor = 'rgba(255,0,0,0.5)';
}
function onMessage(evt) {
  console.log("Received: " + evt.data);
  JSONobj = JSON.parse(evt.data);
  state = JSONobj.controlState;
  timer.innerHTML = `Ligado há: ${JSONobj.timeOn}`; 
  buttonBg.style.backgroundColor = 'rgba(255,0,0,0)';
    if(state == 'ON') {;
      btn_sound.play();
      button.style.animation = 'buttonClick .1s forwards';
      button.style.color = 'red';
    }
    else {
      btn_sound.pause();
      btn_sound.currentTime = 0;
      button.style.animation = 'buttonRelease .1s forwards';
      button.style.color = 'springgreen';
    }
}
function onError(evt) {
  console.log("ERROR: " + evt.data);
}
function doSend(message) {
  if(connected){
    console.log("Sending: " + message);
    websocket.send(message);
  }
  else {
    console.log("Offline");
  }
}
button.onclick = function() {
  if(state == 'OFF')
    doSend("setControl");
}
function doPing() {
  doSend("ping");
}
function wspause() {
  visible = false;
  clearInterval(relogio);
  buttonBg.style.backgroundColor = 'rgba(255,0,0,0.5)';
 }
function wscontinue() {
  visible = true;
  clearInterval(relogio);
  relogio = setInterval('doPing()', 250);
  if (!connected)
  setTimeout(function() { wsConnect(url) }, 2000);
}
const handleChange = (e) => {
  document.hidden ? wspause() : wscontinue();
}
document.addEventListener("visibilitychange", handleChange);
window.oncontextmenu = function(event) {
  event.preventDefault();
  event.stopPropagation();
  return false;
};