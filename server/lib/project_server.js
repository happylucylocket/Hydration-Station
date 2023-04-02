"use strict";
/*
 * Respond to commands over a websocket to relay UDP commands to a local program
 */

var socketio = require('socket.io');
var io;
var fs   = require('fs');
var dgram = require('dgram');

exports.listen = function(server) {
	io = socketio.listen(server);
	io.set('log level 1');

	io.sockets.on('connection', function(socket) {
		handleCommand(socket);
	});
};

function handleCommand(socket) {
	// Pased string of comamnd to relay
	socket.on('daUdpCommand', function(data) {
		// Info for connecting to the local process via UDP
		var PORT = 8088;
		var HOST = '192.168.7.2';
		var buffer = new Buffer(data);

		var client = dgram.createSocket('udp4');
		client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
			if (err) 
				throw err;
			// console.log('UDP message sent to ' + HOST +':'+ PORT);
		});

		client.on('listening', function () {
			var address = client.address();
			// console.log('UDP Client: listening on ' + address.address + ":" + address.port);
		});
		// Handle an incoming message over the UDP from the local application.
		client.on('message', function (message, remote) {
			// console.log("UDP Client: message Rx" + remote.address + ':' + remote.port +' - ' + message);

			var reply = message.toString('utf8')
			socket.emit('commandReply', reply);
			
			client.close();

		});
		client.on("UDP Client: close", function() {
			console.log("closed");
		});
		client.on("UDP Client: error", function(err) {
			console.log("error: ",err);
		});
	});

	socket.on('updateTimer', function(data) {
		// Info for connecting to the local process via UDP
		var PORT = 8088;
		var HOST = '192.168.7.2';
		var buffer = new Buffer(data);

		var client = dgram.createSocket('udp4');
		client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
			if (err) 
				throw err;
		});

		client.on('listening', function () {
			var address = client.address();
			// console.log('UDP Client: listening on ' + address.address + ":" + address.port);
		});
		// Handle an incoming message over the UDP from the local application.
		client.on('message', function (message, remote) {
			// console.log("UDP Client: message Rx" + remote.address + ':' + remote.port +' - ' + message);

			var reply = message.toString('utf8')
			socket.emit('timerContents', reply);

			client.close();

		});
		client.on("UDP Client: error", function(err) {
			console.log("error: ",err);
		});
	});

	socket.on('updateVolume', function(data) {
		// Info for connecting to the local process via UDP
		var PORT = 8088;
		var HOST = '192.168.7.2';
		var buffer = new Buffer(data);

		var client = dgram.createSocket('udp4');
		client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
			if (err) 
				throw err;
			// console.log('UDP message sent to ' + HOST +':'+ PORT);
		});

		client.on('listening', function () {
			var address = client.address();
			// console.log('UDP Client: listening on ' + address.address + ":" + address.port);
		});
		// Handle an incoming message over the UDP from the local application.
		client.on('message', function (message, remote) {
			// console.log("UDP Client: message Rx" + remote.address + ':' + remote.port +' - ' + message);
			var reply = message.toString('utf8')
			socket.emit('volumeContents', reply);

			client.close();

		});
		client.on("UDP Client: error", function(err) {
			console.log("error: ",err);
		});
		
	});

	socket.on('updateCountdown', function(data) {
		// Info for connecting to the local process via UDP
		var PORT = 8088;
		var HOST = '192.168.7.2';
		var buffer = new Buffer(data);

		var client = dgram.createSocket('udp4');
		client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
			if (err) 
				throw err;
			// console.log('UDP message sent to ' + HOST +':'+ PORT);
		});

		client.on('listening', function () {
			var address = client.address();
			// console.log('UDP Client: listening on ' + address.address + ":" + address.port);
		});
		// Handle an incoming message over the UDP from the local application.
		client.on('message', function (message, remote) {
			// console.log("UDP Client: message Rx" + remote.address + ':' + remote.port +' - ' + message);
			var reply = message.toString('utf8')
			socket.emit('timeRemaining', reply);

			client.close();

		});
		client.on("UDP Client: error", function(err) {
			console.log("error: ",err);
		});
		
	});

	// socket.on('checkRunning', function(data) {
	// 	var errorTimer = setTimeout(function() {
	// 		socket.emit('serverError', 
	// 					"NO RESPONSE");
	// 		}, 5000);
	// 	// Info for connecting to the local process via UDP
	// 	var PORT = 8088;
	// 	var HOST = '192.168.7.2';
	// 	var buffer = new Buffer(data);

	// 	var client = dgram.createSocket('udp4');
	// 	client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
	// 		if (err) 
	// 			throw err;
	// 		console.log('UDP message sent to ' + HOST +':'+ PORT);
	// 	});

	// 	client.on('listening', function () {
	// 		var address = client.address();
	// 		console.log('UDP Client: listening on ' + address.address + ":" + address.port);
	// 	});
	// 	// Handle an incoming message over the UDP from the local application.
	// 	client.on('message', function (message, remote) {
	// 		console.log("UDP Client: message Rx" + remote.address + ':' + remote.port +' - ' + message);
	// 		clearTimeout(errorTimer);
	// 		client.close();

	// 	});
	// 	client.on("UDP Client: error", function(err) {
	// 		console.log("error: ",err);
	// 	});
	// });
};
