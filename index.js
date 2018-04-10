let huejay = require('huejay');
const express = require('express');
const hue = require('node-hue-api');
const bodyParser = require('body-parser');
var fs = require('fs');


const http = require('http');
const app = express();
// parse application/x-www-form-urlencoded
app.use(bodyParser.urlencoded({ extended: false }));

// parse application/json
app.use(bodyParser.json());

var lampen,userList,Tags,chairs;

fs.readFile('rfidTags.json', 'utf8', function (err, data) {
    if (err) throw err;
    Tags = JSON.parse(data);
  });
fs.readFile('lamps.json', 'utf8', function (err, data) {
    if (err) throw err;
    lampen = JSON.parse(data);
});

fs.readFile('userList.json', 'utf8', function (err, data) {
    if (err) throw err;
    userList = JSON.parse(data);
    fs.readFile('chairs.json', 'utf8', function (err, data) {
        if (err) throw err;
        chairs = JSON.parse(data);
        chairs[0].user = userList[0]; // ToDo: System that is capable of
        chairs[1].user = userList[1]; // seeing wich user is sitting on the chair
    });
});

app.get('/', (req, res) => res.send('Hello World!'));

app.get('/lamp/:lampid', function (req, res) {
    res.send(req.params);
    var l = lampen.find(e => e.id == req.params.lampid && e.type == "Hue");
    //sendingStuhl.state = parseInt(req.params.zustand);
    console.log(req.params);
    refreshHue(l);
});

app.get('/sitState/:stuhlId/:zustand', function (req, res) {
    res.send(req.params);
    var sendingStuhl = chairs.find(e => e.id == req.params.stuhlId);
    sendingStuhl.state = parseInt(req.params.zustand);
    console.log(req.params);

    //console.log(res1);
    //console.log(httpGet);
    refreshLamps();
});

app.get('/rfidState/:stuhlId/:tagID', function (req, res) {
    console.log("got rfidState request");
    res.send("req.params");
	var sendingStuhl = chairs.find(e=>e.id == req.params.stuhlId);
	var foundTag = Tags.find(e=>e.id == req.params.tagID); //NFC tag das der Stuhl ausgelsen hat
	sendingStuhl.position = foundTag.position;
	console.log("Found Tag:" + JSON.stringify( foundTag));
	
	refreshLamps();
});
/*
app.get('/rfidState/:stuhlId/:position', function (req, res) {
    //console.log(req);
    var sendingStuhl = chairs.find(e => e.id == req.params.stuhlId);
    var pos = req.params.position.split("_");
    sendingStuhl.position = { x: pos[0], y: pos[1] };
    console.log(sendingStuhl);
    res.send(req.params);
    refreshLamps();
});*/

app.post('/lichtsensor', function (req, res) {
  console.log(req.body.brightness);
  res.send(req.body.brightness);
});

app.listen(80, () => console.log('Example app listening on port 80!'));

function refreshLamps() {
    var changeLightBlubs; // lightbulbs that needs to be changed
    //chairs.map(s=> s);
    changeLightBlubs = lampen.map(
        l => l.nextStatus = getNextStatus(l)
    );
    //changeLightBlubs = lampen.filter(l => distance(l.position, chair.position) < l.activationRadius);
    lampen.map(l => {
        if (l.nextStatus != l.status) {
            console.log("switch: " + l.type + " " + l.id + " " + JSON.stringify(l.nextStatus));
            switch (l.type) {
                case "SonOff":
                    //refreshSonOff(l);
                    break;
                case "Hue":
                    refreshHue(l);
                    break;
                default:
                    console.log("Unkonown Lamp Device");
                    break;
            }
            l.status = l.nextStatus;
        }
    });
}

function getNextStatus(l){
    var stuhleInRangeOfLight = chairs.filter(s => s.state != 0 && distance(l.position, s.position) < l.activationRadius);
    if(stuhleInRangeOfLight.length > 0){
        switch (l.type) {
            case "SonOff":
                return true;
                break;
            case "Hue":
                return ( stuhleInRangeOfLight.reduce( function( sum, key ){ //returns the avrage colorvalue of each user sitting on the Stuhl
                    sum.r += key.user.color.r/stuhleInRangeOfLight.length;
                    sum.g += key.user.color.g/stuhleInRangeOfLight.length;
                    sum.b += key.user.color.b/stuhleInRangeOfLight.length;
                    return sum;
                }, {r:0,g:0,b:0} ));
                break;
            default:
                console.log("Unkonown Lamp Device");
                return true;
                break;
        }
    }else{
        return (false);
    }
}

function refreshSonOff(lamp) {
    var httpGet = lamp.ip + "/";
    httpGet += lamp.status ? "power" : "nopower";
    //httpGet +="/";
    httpGet += lamp.name;
    console.log("httpGet: " + httpGet);
    try {
        var res1 = http.get(httpGet);
    } catch (error) {
        console.log("cant reach: " + lamp.type + " " + lamp.id);
    }

}

function refreshHue(lamp) {
    var hue = require("node-hue-api"),
    HueApi = hue.HueApi,
    lightState = hue.lightState;
    
    var state2 = lamp.nextStatus;
    var api = new HueApi(lamp.ip, lamp.hueUsername);
    var state = (lamp.nextStatus==false)?lightState.create().off():lightState.create().on().rgb(lamp.nextStatus.r,lamp.nextStatus.g,lamp.nextStatus.b);

    try {
        api.setLightState(lamp.id, state)
        .then(displayResult)
        .done();
    } catch (error) {
        console.log("can't reach: " + lamp.type + " " + lamp.id);
    }
}

var displayResult = function(result) {
    console.log(JSON.stringify(result, null, 2));
};

function distance(p1, p2) {
    var a = p1.x - p2.x;
    var b = p1.y - p2.y;
    return Math.sqrt(a * a + b * b);
}

