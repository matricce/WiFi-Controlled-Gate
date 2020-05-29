let url = "ws://" + window.location.hostname + ":1337/";
let button;
let timer;
let btn_sound;
let connected = 0;
let visible = 1;
let state;
let buttonBg;
let clock;
let pingInterval = 250;
let untilResponse = 2000;
let timeout;

function init() {
  timer = document.getElementById("timeOn");
  button = document.getElementById("btn");
  btn_sound = document.getElementById('tickSound');
  buttonBg = document.getElementsByClassName('buttonHolder')[0];
  clock = setInterval('doPing()', pingInterval);
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
  setConnected();
}
function onClose(evt) {
  setDisconnected('onClose');
  setTimeout(function() { wsConnect(url) }, untilResponse);
}
function onMessage(evt) {
  // console.log("Received: " + evt.data);
  console.log("Received");
  JSONobj = JSON.parse(evt.data);
  state = JSONobj.controlState;
  timer.innerHTML = `Ligado há: ${JSONobj.timeOn}`; 
  buttonBg.style.backgroundColor = 'rgba(255,0,0,0)';
  clearTimeout(timeout);
  timeout = setTimeout('setDisconnected(\'timeout\')', untilResponse);
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
    // console.log("Sending: " + message);
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
function onPause() {
  visible = false;
  clearInterval(clock);
  clearTimeout(timeout);//???
  buttonBg.style.backgroundColor = 'rgba(255,0,0,0.5)';
 }
function onResume() {
  visible = true;
  clearInterval(clock);
  clock = setInterval('doPing()', pingInterval);
  if (!connected)
    wsConnect(url);
}
const handleChange = (e) => {
  document.hidden ? onPause() : onResume();
}
document.addEventListener("visibilitychange", handleChange);
function setConnected() {
  console.log("Connected");
  connected = true;
  buttonBg.style.backgroundColor = 'rgba(0,0,0,0)';
}
function setDisconnected(reason) {
  console.log("Disconnected, Reason: " + reason);
  connected = false;
  buttonBg.style.backgroundColor = 'rgba(255,0,0,0.5)';
}
window.oncontextmenu = function(event) {
  event.preventDefault();
  event.stopPropagation();
  return false;
};
