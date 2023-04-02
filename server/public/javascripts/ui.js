// function showError() {
//     var message = getError()
//     $('#error-box').show()
//     $('#error-text').html(message)
// }

// Client-side interactions with the browser.

"use strict"

// Make connection to server when web page is fully loaded.
var socket = io.connect();

function updateTimer() {
	socket.emit('updateTimer', 'timer');
}

function updateWaterVolume() {
	socket.emit('updateVolume', 'water volume');
}

function updateCountdown() {
	socket.emit('updateCountdown', 'time remaining');
}

// Hide error box until error occurs
function checkNotOn() {
	if (isError) {
		showError();
	}
	else {
		$('#error-box').hide();
	}
}

var isError = false;

function showError() {
	$('#error-box').show();
	$('#error-text').html("SERVER ERROR: No response from beat-box application. Is it running?");
}

$(document).ready(function() {
    window.setInterval(function() {updateCountdown()}, 1000);
	window.setInterval(function() {updateTimer()}, 500);
	window.setInterval(function() {updateWaterVolume()}, 500);
	// window.setInterval(function() {checkServer()}, 500);

	$('#error-box').hide();

    $('#alarmOff').click(function(){
		sendCommandViaUDP("silence");
	});

	$('#cup').click(function(){
		sendCommandViaUDP("cup");
	});

	$('#mug').click(function(){
		sendCommandViaUDP("mug");
	});

	$('#bottle').click(function(){
		sendCommandViaUDP("bottle");
	});

	$('#bigbottle').click(function(){
		sendCommandViaUDP("big bottle");
	});

	$('#timerUp').click(function(){
		sendCommandViaUDP("timer up");
	});

	$('#timerDown').click(function(){
		sendCommandViaUDP("timer down");
	});

	$('#stop').click(function(){
		sendCommandViaUDP("quit");
		// isError = true;
	});
	
	socket.on('timeRemaining', function(result) {
		var time = new Date(null);
		time.setSeconds(result);
		var res = time.toISOString().slice(11, 19);
        $('#timeRemaining').html(res);
	});

	socket.on('timerContents', function(result) {
		var time = new Date(null);
		time.setSeconds(result);
		var res = time.toISOString().slice(11, 19);
        $('#timer').html(res);
	});

	socket.on('volumeContents', function(result) {
		$('#volumeid').html(result);
	});

	// socket.on('serverError', function(result) {
	// 	console.log(typeof(result))
	// 	if (result == "NO RESPONSE") {
	// 		isError = true;
	// 		$('#error-box').show();
	// 		$('#error-text').html("SERVER ERROR: No response from application. Is it running?");
	// 	}
	// 	else {
	// 		$('#error-box').hide();
	// 		isError = false;
	// 	}

	// });
});

function sendCommandViaUDP(message) {
	socket.emit('daUdpCommand', message);
};