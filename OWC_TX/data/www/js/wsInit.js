// Create a web socket conncetion
var host = window.location.host;
var url = "ws://" + host + "/ws";
var ws = new WebSocket(url);
ws.binaryType = "arraybuffer";

var wsStatus = document.getElementById("status");

ws.onopen = wsOpen;
ws.onclose = wsClose;

function wsOpen(e){
    wsStatus.innerText = "Connected";
}

function wsClose(e){
    wsStatus.innerText = "Disconnected";
    if(confirm('Disconnect with ESP32. Reload the page?') == true){
        window.location.reload();
    }
}