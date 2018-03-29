var express = require('express');
var http = require('http');
var philipsHueID = '/api/CKz4-mVVsU856bBd2GLQ4VxqAzgd8ik2aYsBR1Ow/lights/';

var server = express();
server.use('/', express.static(__dirname + '/'));
server.get('/switch', function(req, res, next) {
	//var querystring = require('querystring');
    var data = '{"on":false}';
    
	var options = {
		host: '192.168.2.189',
        port: 80,
        path: philipsHueID + 1 + '/state',
		method: 'PUT',
		headers: {
			'Content-Type': 'application/json',
			'Content-Length': Buffer.byteLength(data)
		}
	};

	var hueReq = http.request(options, function(res) {
		res.setEncoding('utf8');
		res.on('data', function (chunk) {
			console.log("body: " + chunk);
		});
	});
	hueReq.write(data);
	hueReq.end();
	res.send("test32");
});

server.listen(8686, function () {
  console.log('Server is listening on port 8686.')
})
