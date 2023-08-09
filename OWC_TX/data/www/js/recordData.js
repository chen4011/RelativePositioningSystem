
var start = false;
var index = 1, delay = 0, count = 0;
var terminalIndex = document.getElementById("terminalIndex");

var tbody = document.getElementById("tbody");
var tr = document.createElement("tr");
var td = document.createElement("td");

function startRecord(){
    start = true;
}

function stopRecord(){
    start = false;
}

function clearRecord(){
    reset();
    document.getElementById("index").innerText = 0;
    // document.getElementById("signal1").innerText = "";
}

function reset(){
    start = false;
    index = 1;
    delay = 0;
    tbody.innerText = "";
    tr = document.createElement("tr");
}

function recordData(current){
    if (start){
        if (delay < 2) {        // Wait for arm move
            delay += 1;
            return;
        }
        if (index % 5 == 1){    // New position
            count = Math.ceil(index/ 5);
            document.getElementById("index").innerText = count;

            td = document.createElement("td");
            td.innerText = count;
            tr.appendChild(td);
        }
        for(var i = 0; i < current.length; i++) {   // Copy 3 PDs data to table
            td = document.createElement("td");
            td.innerText = current[i].toFixed(5);
            tr.appendChild(td);
        }
        tbody.appendChild(tr);
        
        if (index % 5 == 0){    // The end of 5 times record, Crate a new row
            tr = document.createElement("tr");
            delay = 0;
            if (count == terminalIndex.value) {
                start = false;
            }
        }
        
        index += 1;
    }
    
}

function recordSignal(signal) {     // !!! Need to uncomment line 86(<span id = "signal1"...>) in index.html
    if(start){
        document.getElementById("signal1").innerText += "signal(1, :) = [";

        for(var i = 0; i < signal.length; i++) {
            document.getElementById("signal1").innerText += signal[i].toFixed(5) + ", ";
        }
        document.getElementById("signal1").innerText += "];\n\n";
    }
    
}

export { recordData, startRecord, stopRecord, clearRecord, recordSignal};