alert("Start0");
$(document).ready(function() {
	alert("Start");
	var recognition=null;
	if (!('webkitSpeechRecognition' in window)) {
		alert("Error");
		$('#record>img').attr('title', 'La función de reconocimiento de voz solo es soportada por Chrome 25 o superior.');
	} else {
		alert("Correct");
		$('#message').attr('placeholder', 'Escribe un mensaje o pincha en el micrófono y habla');
 		recognition = new webkitSpeechRecognition();
 		alert("Created recognition");
 		recognition.lang='es-ES';
		//recognition.onstart = function() {
		//}
  		recognition.onresult = function(event) {
  			alert("Received result");
  			var message='';
  			for (var i=event.resultIndex;i<event.results.length;i++) {
  				message+=event.results[i][0].transcript;
  			}
  			$('#message').val(message);
  		};
  		recognition.onerror = function(event) { 
  			alert('Error: '+event.error);
  		 };
  		//recognition.onend = function() { 
  		//}
  	}
  	$('#record').on('click', function() {
  		alert("Recognition start");
  		if (recognition==null) {
  			alert('La función de reconocimiento de voz solo es soportada por Chrome 25 o superior.');
  			return;
  		}
  		$('#message').val('');
  		$('#message').attr('placeholder', 'Habla ahora');
  		recognition.start();
  	});
});