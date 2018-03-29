let huejay = require('huejay');
const express = require('express');
const hue = require('node-hue-api');

const http = require('http');
const app = express();
var lampen = [{ id: 0, ip: "http://192.168.2.109", type: "SonOff", name: "One", status: false, position: { x: "100", y: "100" }, activationRadius: "200" },
{ id: 1, ip: "http://192.168.2.109", type: "SonOff", name: "Two", status: false, position: { x: "200", y: "100" }, activationRadius: "200" },
{ id: 2, ip: "http://192.168.2.109", type: "SonOff", name: "Three", status: false, position: { x: "300", y: "100" }, activationRadius: "200" },
{ id: 3, ip: "http://192.168.2.109", type: "SonOff", name: "Four", status: false, position: { x: "400", y: "100" }, activationRadius: "200" },
{ id: 1, ip: "192.168.2.79", type: "Hue",hueUsername:"CKz4-mVVsU856bBd2GLQ4VxqAzgd8ik2aYsBR1Ow", status: false, position: { x: "500", y: "100" }, activationRadius: "200" },
{ id: 6, ip: "192.168.2.79", type: "Hue",hueUsername:"CKz4-mVVsU856bBd2GLQ4VxqAzgd8ik2aYsBR1Ow", status: false, position: { x: "600", y: "100" }, activationRadius: "200" }];

var userList = [{id:1,color:{r:255,g:255,b:0}},
                {id:2,color:{r:0,g:255,b:0}}]

//var lampen = [{id: 0,ip:"http://192.168.3.112",type:"SonOff", name:"One", status: "false",position:{ x: "100", y: "100"},activationRadius:"200"}];
var stuhle = [
    { id: "1", state: 0, position: { x: "400", y: "100" }, user:userList[0]},
    { id: "2", state: 0, position: { x: "100", y: "100" }, user:userList[1] }
];
//test
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
    var sendingStuhl = stuhle.find(e => e.id == req.params.stuhlId);
    sendingStuhl.state = parseInt(req.params.zustand);
    console.log(req.params);

    //console.log(res1);
    //console.log(httpGet);
    refreshLamps();
});

app.get('/rfidState/:stuhlId/:position', function (req, res) {
    //console.log(req);
    var sendingStuhl = stuhle.find(e => e.id == req.params.stuhlId);
    var pos = req.params.position.split("_");
    sendingStuhl.position = { x: pos[0], y: pos[1] };
    console.log(sendingStuhl);
    res.send(req.params);
    refreshLamps();
});

app.post('/lichtsensor', function (req, res) {
  res.send("test");
});

app.listen(80, () => console.log('Example app listening on port 80!'));

function refreshLamps() {
    var changeLightBlubs; // lightbulbs that needs to be changed
    //stuhle.map(s=> s);
    changeLightBlubs = lampen.map(l => l.nextStatus = ((stuhle.filter(s => s.state != 0 && distance(l.position, s.position) < l.activationRadius).length) > 0));
    //changeLightBlubs = lampen.filter(l => distance(l.position, chair.position) < l.activationRadius);
    lampen.map(l => {
        if (l.nextStatus != l.status) {
            console.log("switch: " + l.type + " " + l.id + " " + l.nextStatus);
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

    var api = new HueApi(lamp.ip, lamp.hueUsername),
    state = lightState.create().on().rgb(255,0,0);

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

