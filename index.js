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

<<<<<<< HEAD
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
=======
var lamps = [{ id: 0, ip: "http://192.168.2.109", type: "SonOff", name: "One", status: false, position: { x: "100", y: "100" }, activationRadius: "200" },
{ id: 1, ip: "http://192.168.2.109", type: "SonOff", name: "Two", status: false, position: { x: "200", y: "100" }, activationRadius: "200" },
{ id: 2, ip: "http://192.168.2.109", type: "SonOff", name: "Three", status: false, position: { x: "300", y: "100" }, activationRadius: "200" },
{ id: 3, ip: "http://192.168.2.109", type: "SonOff", name: "Four", status: false, position: { x: "400", y: "100" }, activationRadius: "200" },
{ id: 1, ip: "192.168.2.79", type: "Hue",hueUsername:"CKz4-mVVsU856bBd2GLQ4VxqAzgd8ik2aYsBR1Ow", status: false, position: { x: "400", y: "100" }, activationRadius: "200" },
{ id: 6, ip: "192.168.2.79", type: "Hue",hueUsername:"CKz4-mVVsU856bBd2GLQ4VxqAzgd8ik2aYsBR1Ow", status: false, position: { x: "600", y: "100" }, activationRadius: "200" }];

var Tags = [
	{id:"2EFC36BB5F",name:"Card1",position: { x: "600", y: "100" }},
	{id:"20CA36BB67",name:"Card2",position: { x: "600", y: "100" }},
	{id:"E2F36BB60",name:"Card3",position: { x: "600", y: "100" }},
	{id:"311936BBA5",name:"Card4",position: { x: "600", y: "100" }},
	{id:"336736BBD9",name:"Card5",position: { x: "600", y: "100" }},
	{id:"B2B8BE299D",name:"Card6",position: { x: "600", y: "100" }},
	{id:"88435A118",name:"Tag1",position: { x: "600", y: "100" }},
	{id:"884AA127",name:"Tag2",position: { x: "600", y: "100" }},
	{id:"8847BA156",name:"Tag3",position: { x: "600", y: "100" }},
];

var userList = [{id:1,color:{r:255,g:0,b:0}},
                {id:2,color:{r:0,g:255,b:0}}]

var chairs = [
    { id: "1", state: 0, position: { x: "400", y: "100" }, user:userList[0]},
    { id: "2", state: 0, position: { x: "100", y: "100" }, user:userList[1]}
];
>>>>>>> a4fc031276f197a353c9142ffdb50f7fe65347a7

app.get('/', (req, res) => res.send('Hello World!'));

app.get('/lamp/:lampid', function (req, res) {
    res.send(req.params);
    var l = lamps.find(e => e.id == req.params.lampid && e.type == "Hue");
    //sendingChair.state = parseInt(req.params.zustand);
    console.log(req.params);
    refreshHue(l);
});

<<<<<<< HEAD
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
	
=======
app.get('/rfidState/:chairId/:tagID', function (req, res) {
	//console.log(req);
	var sendingChair = chairs.find(e=>e.id == req.params.chairId);
	var foundTag = Tags.find(e=>e.id == req.params.tagID); //NFC tag das der Stuhl ausgelsen hat
	sendingChair.position = foundTag.position;
	console.log(sendingChair);
	res.send(req.params);
>>>>>>> a4fc031276f197a353c9142ffdb50f7fe65347a7
	refreshLamps();
});
/*
app.get('/rfidState/:chairId/:position', function (req, res) {
    //console.log(req);
<<<<<<< HEAD
    var sendingStuhl = chairs.find(e => e.id == req.params.stuhlId);
=======
    var sendingChair = chairs.find(e => e.id == req.params.chairId);
>>>>>>> a4fc031276f197a353c9142ffdb50f7fe65347a7
    var pos = req.params.position.split("_");
    sendingChair.position = { x: pos[0], y: pos[1] };
    console.log(sendingChair);
    res.send(req.params);
    refreshLamps();
});*/

app.post('/lightsensor', function (req, res) {
  console.log(req.body.brightness);
  res.send(req.body.brightness);
});

app.listen(80, () => console.log('Example app listening on port 8080!'));

function refreshLamps() {
    var changeLightBlubs; // lightbulbs that needs to be changed
    //chairs.map(s=> s);
<<<<<<< HEAD
    changeLightBlubs = lampen.map(
=======
    changeLightBlubs = lamps.map(
>>>>>>> a4fc031276f197a353c9142ffdb50f7fe65347a7
        l => l.nextStatus = getNextStatus(l)
    );
    //changeLightBlubs = lamps.filter(l => distance(l.position, chair.position) < l.activationRadius);
    lamps.map(l => {
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
<<<<<<< HEAD
    var stuhleInRangeOfLight = chairs.filter(s => s.state != 0 && distance(l.position, s.position) < l.activationRadius);
    if(stuhleInRangeOfLight.length > 0){
=======
    var chairInRangeOfLight = chairs.filter(s => s.state != 0 && distance(l.position, s.position) < l.activationRadius);
    if(chairInRangeOfLight.length > 0){
>>>>>>> a4fc031276f197a353c9142ffdb50f7fe65347a7
        switch (l.type) {
            case "SonOff":
                return true;
                break;
            case "Hue":
                return ( chairInRangeOfLight.reduce( function( sum, key ){ //returns the avrage colorvalue of each user sitting on the Stuhl
                    sum.r += key.user.color.r/chairInRangeOfLight.length;
                    sum.g += key.user.color.g/chairInRangeOfLight.length;
                    sum.b += key.user.color.b/chairInRangeOfLight.length;
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
        console.log("can't reach: " + lamp.type + " " + lamp.id);
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

