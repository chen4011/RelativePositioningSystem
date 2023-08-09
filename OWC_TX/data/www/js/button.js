import { startRecord, stopRecord, clearRecord} from './recordData.js';
import { resetMPU, countDown } from './sensorData.js';

window.startClick = startRecord;
window.stopClick = stopRecord;
window.clearClick = clearRecord;
window.resetMPUPD = resetMPUPD;
window.resetMPULED = resetMPULED;
window.calcuDist = calcuDist;
window.espRestart = espRestart;
window.LEDMode = LEDMode;
window.copyRecord = copyRecord;

var calcuButton = document.getElementById("calcuDist");

function resetMPUPD() {
    resetMPU("PD");

    var msg = {"type": "resetMPUPD", "val": 1};
    ws.send(JSON.stringify(msg));
    // console.log("Send to ESP32: " + JSON.stringify(msg));
}

function resetMPULED() {
    resetMPU("LED");

    var msg = {"type": "resetMPULED", "val": 1};
    ws.send(JSON.stringify(msg));
}

function calcuDist() {
    wsStatus.innerText = "Calculating...";
    
    calcuButton.innerText = "Calculating...";
    calcuButton.setAttribute("disabled", "disabled");
    calcuButton.style.backgroundColor = "#cccccc"
    var msg = {"type": "calcuDist", "val": 1};
    ws.send(JSON.stringify(msg));

    countDown();
}

function espRestart() {
    var msg = {"type": "espRestart", "val": 1};
    ws.send(JSON.stringify(msg));
    // if(confirm('Restart the ESP32. Reload the page?') == true){
    //     window.location.reload();
    // }
}

function LEDMode() {
    var msg = {"type": "ledMode", "val": 1};
    ws.send(JSON.stringify(msg));
}

function copyRecord() {
    
    var copyText= document.getElementById("ttable");
    var body = document.body, range, sel;
    if (document.createRange && window.getSelection) {
        range = document.createRange();
        sel = window.getSelection();
        sel.removeAllRanges();
        try {
            range.selectNodeContents(copyText);
            sel.addRange(range);
        } catch (e) {
            range.selectNode(copyText);
            sel.addRange(range);
        }
    } else if (body.createTextRange) {
        range = body.createTextRange();
        range.moveToElementText(copyText);
        range.select();
    }
    document.execCommand("Copy");
}