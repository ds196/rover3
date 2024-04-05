/**
 * *A.S.T.R.A.
 * Ground Control Station JS
 */


//    num joint   close dir  min  max
//Servo 1 (claw):     -      80   n/a
//Servo 2 (wrist):    -      n/a  n/a
//Servo 3 (elbow):    +      n/a  135
//Servo 4 (shoulder): +      70   150
//Motor:   1-left, 2-right

var outServo1 = 90;
var outServo2 = 90;
var outServo3 = 90;
var outServo4 = 90;
var outMotorS = 255; // 0-255
var outMotorD = 1;
var outMotorStopped = false;
const servoIncrement = 1;
const motorIncrement = 20;

let forward = 6;
let up = 0;
let kineIncrement = .5;
let keysCurrPressed = [];

// Connecting ROS + ros_bridge
var ros = new ROSLIB.Ros({
    url : "ws://localhost:9090"
});

// PICO Subscribers
var commandPub = new ROSLIB.Topic({
    ros: ros,
    name: "pico/command",
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

    $("#btn-picoManualCommand").click(() => {
        commandPub.publish({
            data: $("#txt-picoManualCommand").val().trim()
        });
    });

    // Subscriber from Libre from PICO
    var listener = new ROSLIB.Topic({
        ros: ros,
        name: "pico/listener",
        messageType: "std_msgs/String"
    });
    
    listener.subscribe('/talker', 'std_msgs/String', (msg) => {
        txt-tpSensorData.text(message.data);
        console.log("Received message on " + listener.name + ": " + message.data);

    });

    picoSub.subscribe((message) => {
        //servo1.text(message.data);
        console.log(`Recieved from pico: ${message.data}`);
        let textAlreadyThere = $("#txt-picoSerialLog").val(); // There should already be a newline at the end of textAlreadyThere
        $("#txt-picoSerialLog").val(textAlreadyThere + message.data);
        if(message.data.startsWith("Temp")) {
            $("#txt-tpSensorData").val(message.data);
        }
        if(message.data.startsWith("Pressure")) {
            let prevData = $("#txt-tpSensorData").val();
            $("#txt-tpSensorData").val(prevData+message.data);
        }
        if(message.data.startsWith("Accel")) {
            $("#txt-imuSensorData").val(message.data);
        }
    });

    updateServoText();
    updateMotorText();

    $("#btn-updateTP").click(() => {
        commandPub.publish({
            data: "pollbmp"
        });
    });

    $("#btn-updateIMU").click(() => {
        commandPub.publish({
            data: "pollbno"
        });
    });

    $("#btn-resetServos").click(() => {
        outServo1 = 90;
        outServo2 = 90;
        outServo3 = 90;
        outServo4 = 90;
        console.log(`Reset servos: ${outServo1}, ${outServo2}, ${outServo3}, ${outServo4}`);
        setServos();
        updateServoText();
    });

    $("#btn-closeClaw").click(() => {
        outServo1 = 180;
        setServos();
        updateServoText();
    });

    $("#btn-openClaw").click(() => {
        outServo1 = 0;
        setServos();
        updateServoText();
    });

    $("#btn-sendServos").click(() => {
        setServos();
        updateServoText();
    });

    // Keydown
    $(document).on("keydown", (event) => {
        // if you press the z character key, the event.key returns z and event.code returns KeyZ
        console.log(`key=${event.key}, code=${event.code}`);


        //Servos
        if(["KeyA", "KeyS", "KeyD", "KeyF", 
            "KeyZ", "KeyX", "KeyC", "KeyV"].indexOf(event.code) > -1) 
        {
            if(!isKeyPressed(event.code)) {
                keyPress(event.code);
                switch(event.code) {
                    //Claw
                    case "KeyA":
                        startServo(1, "+", servoIncrement*3, event.code);
                        break;
                    case "KeyZ":
                        startServo(1, "-", servoIncrement*3, event.code);
                        break;
                    //Wrist
                    case "KeyS":
                        startServo(2, "-", servoIncrement*5, event.code);
                        break;
                    case "KeyX":
                        startServo(2, "+", servoIncrement*5, event.code);
                        break;
                    //Elbow
                    case "KeyD":
                        startServo(3, "+", servoIncrement*2, event.code);
                        break;
                    case "KeyC":
                        startServo(3, "-", servoIncrement*2, event.code);
                        break;
                    //Shoulder
                    case "KeyF":
                        startServo(4, "+", servoIncrement*(2/3), event.code);
                        break;
                    case "KeyV":
                        startServo(4, "-", servoIncrement*(2/3), event.code);
                        break;
                }
            }
        }

        //Inverse kinematics
        if(["KeyI", "KeyK", "KeyU", "KeyO"].indexOf(event.code) > -1) {
            if(!isKeyPressed(event.code)) {
                keyPress(event.code);
                switch(event.code) {
                    //Forward/backward
                    case "KeyI":
                        forward += kineIncrement;
                        break;
                    case "KeyK":
                        forward -= kineIncrement;
                        break;
                    //Up/down
                    case "KeyU":
                        up -= kineIncrement;
                        break;
                    case "KeyO":
                        up += kineIncrement;
                        break;
                }
                // Limits
                if(up < 0) up = 0;
                else if(up > 10) up = 10;
                if(forward < 6) forward = 6;
                else if(forward > 11) forward = 11;
                //Trig
                let angles = findAngles(forward, up);
                console.log(`forward=${forward}, up=${up}, t1=${angles[0].round(1)}, t2=${angles[1].round(1)}`);
                //Convert to numbers servos will understand
                let shoulderAngle = radToDeg(angles[0]).round(1);
                let elbowAngle = radToDeg(angles[1]).round(1);
                outServo4 = 90+(90-shoulderAngle)*(2/3); // 90+(90-shoulderAngle)
                outServo3 = 90+(90-elbowAngle)*2;
                
                setServos();
                updateServoText();
            }
        }
        

        //Motor control
        if(["ArrowUp", "ArrowDown", "ArrowLeft", "ArrowRight"].indexOf(event.code) > -1) 
        {
            switch(event.code) {
                case "ArrowUp":
                    outMotorS += motorIncrement;
                    if(outMotorS > 255) outMotorS = 255;
                    break;
                case "ArrowDown":
                    outMotorS -= motorIncrement;
                    if(outMotorS < 30) outMotorS = 30;
                    break;
                case "ArrowLeft":
                    outMotorD = 1;
                    outMotorStopped = false;
                    setMotor();
                    break;
                case "ArrowRight":
                    outMotorD = 2;
                    outMotorStopped = false;
                    setMotor();
                    break;
            }
            updateMotorText();
        }

        //Motor explicit stop
        if(event.code == "KeyM") {
            commandPub.publish({
                data: "stopmotor"
            });
            outMotorStopped = true;
            updateMotorText();
        }
        
    });

    
    $(document).on("keyup", (event) => {
        keyRelease(event.code);
        //Stop motor mvmt when key released
        if (event.code == "ArrowRight" 
            || event.code == "ArrowLeft" 
            || event.code == "KeyG" 
            || event.code == "KeyJ")
        {
            outMotorStopped = true;
            commandPub.publish({
                data: `stopmotor`
            });
            updateMotorText();
        }
    });
    
    /**
     * Updates the info text box for the servo positions
     */
    function updateServoText() {
        document.getElementById("txt-servoPosData").innerHTML = `Claw: ${outServo1}\nWrist: ${outServo2}\nElbow: ${outServo3}\nShoulder: ${outServo4}`;
    }

    /**
     * Tells pico to set all servos based on their outServo[num]
     */
    function setServos() {
        //    num joint   close dir  min  max
        //Servo 1 (claw):     -      80   180
        //Servo 2 (wrist):    -       0   180
        //Servo 3 (elbow):    +       0   135
        //Servo 4 (shoulder): +      70   150
        //Motor:   1-left, 2-right

        //Enforce servo lims
        //Claw
        if(outServo1 < 1) {
            outServo1 = 1;
        } else
        if(outServo1 > 180) {
            outServo1 = 180;
        }
        //Wrist
        if(outServo2 < 1) {
            outServo2 = 1;
        } else
        if(outServo2 > 180) {
            outServo2 = 180
        }
        //Elbow
        if(outServo3 < 1) {
            outServo3 = 1;
        } else
        if(outServo3 > 135) {
            outServo3 = 135;
        }
        //Shoulder
        if(outServo4 < 70) {
            outServo4 = 70;
        } else
        if(outServo4 > 150) {
            outServo4 = 150;
        }
        //Send to pico all at once
        commandPub.publish({
            data: `sr ${outServo1} ${outServo2} ${outServo3} ${outServo4}`
        });
    }

    /**
     * Updates the info text box for the motor speed and direction
     */
    function updateMotorText() {
        if(outMotorStopped) {
            document.getElementById("txt-motorData").innerHTML = `Speed: ${outMotorS} (STOPPED)\nDirection: ${outMotorD}`;
        } else if(!outMotorStopped) {
            document.getElementById("txt-motorData").innerHTML = `Speed: ${outMotorS}\nDirection: ${outMotorD}`;
        }
    }

    /**
     * Tells the pico to set the motor to outMotorS speed
     * and outMotorD direction
     */
    function setMotor() {
        //Enforce motor lims
        if(outMotorS < 30) {
            outMotorS = 30;
        } else
        if(outMotorS > 255) {
            outMotorS = 255;
        }
        //Send to Pico
        commandPub.publish({
            data: `setmotor ${outMotorS} ${outMotorD}`
        });
    }

    /**
     * Returns whether or not the specified key is being
     * tracked as currently held down or not
     * @param {string} keyCode 
     * @returns 
     */
    function isKeyPressed(keyCode) {
        let index = keysCurrPressed.indexOf(keyCode);
        if(index > -1) { // key currently pressed
            return true;
        } else if(index == -1) { // key not already pressed
            return false;
        }
    }
    /**
     * Tracks the specified key as being pressed
     * @param {string} keyCode 
     * @returns 
     */
    function keyPress(keyCode) {
        let index = keysCurrPressed.indexOf(keyCode);
        if(index > -1) { // key currently pressed
            return true;
        } else if(index == -1) { // key not already pressed
            //Track that the key is being pressed
            keysCurrPressed.push(keyCode);
            return false;
        }
    }
    /**
     * Removes the specified key from being tracked as
     * being pressed
     * @param {string} keyCode 
     * @returns 
     */
    function keyRelease(keyCode) {
        let index = keysCurrPressed.indexOf(keyCode);
        if(index > -1) { // key currently pressed
            keysCurrPressed.splice(index, 1); // remove 1 item at index
            return true;
        } else if(index == -1) { // key not already pressed
            return false;
        }
    }

    /**
     * Starts incrementing a specific servo by increment
     * every 300ms in dir direction
     * @param {number} servoNum 
     * @param {string} dir 
     * @param {number} increment 
     * @param {string} keyCode 
     * @returns 
     */
    function startServo(servoNum, dir, increment, keyCode) {
        if(!isKeyPressed(keyCode)) return;
        switch(servoNum) {
            case 1:
                if(dir == "-") outServo1 -= increment;
                else if(dir == "+") outServo1 += increment;
                break;
            case 2:
                if(dir == "-") outServo2 -= increment;
                else if(dir == "+") outServo2 += increment;
                break;
            case 3:
                if(dir == "-") outServo3 -= increment;
                else if(dir == "+") outServo3 += increment;
                break;
            case 4:
                if(dir == "-") outServo4 -= increment;
                else if(dir == "+") outServo4 += increment;
                break;
        }
        setServos();
        updateServoText();

        setTimeout(startServo, 300, servoNum, dir, increment, keyCode)
    }

    /**
     * Finds angles for shoulder, elbow, and wrist
     * for x inches forward and y inches up from shoulder joint
     * @param {number} x 
     * inches forward from shoulder joint
     * @param {number} y 
     * inches up from shoulder joint
     * @returns {Array}
     * shoulder, elbow, wrist
     */
    function findAngles(x, y) {
        let humerus = 5; //inches, a
        let forearm = 7; //inches, b
    
        let t1=1000,t2=1000,t3=1000; //stupid values for debugging
    
        //Up and forward
        if (x > 0 && y > 0) {
            let diag =  Math.sqrt(x**2 + y**2);
            let alpha = Math.atan(x/y);
            let beta =  Math.atan(y/x);
            let A = Math.acos( (forearm**2 + diag**2 - humerus**2) / (2 * forearm * diag) );
            let D = Math.acos( (humerus**2 + forearm**2 - diag**2) / (2 * humerus * forearm) );
            let B = 180-A-D
            //let B = Math.asin( (forearm / humerus) * Math.sin(A) );
            //let D = Math.asin( (diag    / humerus) * Math.sin(A) );
            //let A = Math.asin( (humerus/diag) * Math.sin(D) );
            //let B = Math.asin( (forearm/diag) * Math.sin(D) );
    
            t1 = B + beta;
            t2 = D;
            t3 = A + alpha; // 270-t3 = wrist angle
        //Forward but not up
        } else if(x > 0 && y == 0) {
            t2 = Math.acos( (forearm**2 + humerus**2 - x**2) / (2*forearm*humerus) );
            t1 = Math.asin( (forearm/x) * Math.sin(t2));
            t3 = Math.asin( (humerus/x) * Math.sin(t2));
        //Nope
        } else {
            t1 = 90;
            t2 = 90;
            t3 = 90;
        }
        
        //t1 = shoulder, t2 = elbow, t3 = wrist
        return [t1, t2, t3];
    }

});

/**
 * Converts degrees to radians
 * @param {number} rad 
 * @returns {number}
 */
function radToDeg(rad) {
    return rad * (180/Math.PI);
}

/**
 * Rounds to d decimal places
 * @param {number} d 
 * @returns {number}
 */
Number.prototype.round = function(d) {
    return Math.round((this + Number.EPSILON)*(10**d))/(10**d);
};
