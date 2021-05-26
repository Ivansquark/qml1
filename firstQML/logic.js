// myfunction.js
.pragma library


var a = "Connect"
var connectedState = false;

function newConnectState(x) {
    a = x;
    if(x === "Connect"){
        connectedState = true;
    } else {
        connectedState = false;
    }
    return a;
}


