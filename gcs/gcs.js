// instance objects: ros, pub/sub- constructor , text boxes

// WHen submit is clicked, send that value to pico

// One subscriber, parse text into boxes, make sure its being processed line by line - .split to separate, 


var allSpeed = $("#allSpeed"); // $(#id)
var imusensor = document.getElementById("imuSensorData");
var temp = document.getElementById("tempData");
var pressure = document.getElementById("pressureData");
var servo1 = document.getElementById("servo1");
var servo2 = document.getElementById("servo2");
var servo3 = document.getElementById("servo3");
var servo4 = document.getElementById("servo4");
var motor = document.getElementById("motorData");

var outServo1 = 90;
var outServo2 = 90;
var outServo3 = 90;
var outServo4 = 90;
var outMotorS = 1; // 0-255
var outMotorD = 1;

// Connecting ROS + ros_bridge
var ros = new ROSLIB.Ros({
    url : 'ws://localhost:9090'
});

// PICO Subscribers
var commandPub = new ROSLIB.Topic({
    ros: ros,
    name: 'pico/command',
    messageType : "std_msgs/String"

});

// Subscriber from Libre from PICO
var picoSub = new ROSLIB.Topic({
    ros: ros,
    name: "pico/output",
    messageType: "std_msgs/String"
});

//Wait for document to load before starting jquery
//(Make jquery work)
$("document").ready(() => {

    ros.on('connection', () => {
        console.log("Connected to ROS2.");
        $("#status").text("Success!");
    });
    
    ros.on('error', (error) => {
        console.log("Error connecting: ", error);
        $("#status").text(`Error: ${error}`);
    });
    
    ros.on('close', () => {
        console.log('connection closed.');
        $("#status").text("Closed.");
    });

    picoSub.subscribe((message) => {
        //servo1.text(message.data);
        console.log(`Recieved from pico: ${message.data}`);
    });


    /*$("#btn-submitAllSpeed").click(() => {
        commandPub.publish({
            data: "setallservos " + $("#allSpeed").val().trim()
        });
        //console.log("button pushed");
    });*/

    $("#btn-picoManualCommand").click(() => {
        commandPub.publish({
            data: $("#txt-picoManualCommand").val().trim()
        });
    });
});






// document.addEventListener('keypress', (event) => {
//     var name = event.key;
//     var code = event.code;
    

// }
