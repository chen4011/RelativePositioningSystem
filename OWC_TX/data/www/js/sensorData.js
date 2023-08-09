import { recordData, recordSignal } from './recordData.js';

// Create a web socket conncetion
// var host = window.location.host;
// var url = "ws://" + host + "/ws";
// var ws = new WebSocket(url);

var distance = document.getElementById("distance");
var calcuButton = document.getElementById("calcuDist");

var countTime = 60;
var timeoutID;



// Read data from esp32 and show on html
ws.onmessage = updateData;

function updateData(event){
    let msg = JSON.parse(event.data);
    var value = Object.values(msg.val);
    var type = msg.type;
    
    if(type == "resetESP32") {
        // console.log("Press Reset button on ESP32");
        wsStatus.innerText = "Press Reset button on ESP32";
    } else if(type == "distCalcuFinish") {
        countTime = 60;
        clearTimeout(timeoutID);
        wsStatus.innerText = "Connected";
        calcuButton.innerText = "Calculate distance";
        calcuButton.removeAttribute("disabled");
        calcuButton.style.backgroundColor = "#003366";
        // distance.innerText = value[0].toFixed(3);
    } else if(type == "signal") {
        recordSignal(value);
    } else {
        if(type == "current") recordData(value);

        for(var i = 0; i < value.length; i++) {
            // console.log(value[i].toFixed(3));
            document.getElementById(type+ (i+1).toString()).innerText = value[i].toFixed(3);
        }
    }
    
}

function resetMPU(MPUtype) {
    for(var i = 1; i <= 3; i++) {
        document.getElementById("rot"+ MPUtype+ i.toString()).innerText = "Reset...";
    }
}

function countDown() {
    countTime -= 1;
    
    wsStatus.innerText = "Calculating..." + countTime;
    calcuButton.innerText = "Calculating..." + countTime;

    timeoutID = setTimeout(countDown, 1000);
}

export {resetMPU, countDown};