$(document).ready(function() {
	var recognition=null;
  var recognizing=false;
	if (!('webkitSpeechRecognition' in window)) {
    $('#message').prop('placeholder', 'Escribe un mensaje');
		$('#record>img').prop('title', 'La función de reconocimiento de voz solo es soportada por Chrome 25 o superior.');
    //$('#record').prop('disabled', true);
    $('#record>img').prop('src', '/img/img_microphone_disabled.ico');
	} else {
		$('#message').prop('placeholder', 'Escribe un mensaje o pincha en el micrófono y habla');
 		recognition = new webkitSpeechRecognition();
 		recognition.lang='es-ES';
		recognition.onstart = function() {
      recognizing=true;
      $('#message').val('');
      $('#message').prop('placeholder', 'Habla ahora');
		}
  		recognition.onresult = function(event) {
  			var message='';
  			for (var i=event.resultIndex;i<event.results.length;i++) {
  				message+=event.results[i][0].transcript;
  			}
  			$('#message').val(message);
  		};
  		recognition.onerror = function(event) {
        recognizing=false;
        $('#message').prop('placeholder', 'Escribe un mensaje o pincha en el micrófono y habla');
        $('#message').val(''); 
  			alert('Error: '+event.error);
  		 };
  		recognition.onend = function() { 
        recognizing=false;
        $('#message').prop('placeholder', 'Escribe un mensaje o pincha en el micrófono y habla');
  		}
  	}
  	$('#record').on('click', function() {
  		if (recognition==null) {
  			alert('La función de reconocimiento de voz solo es soportada por Chrome 25 o superior.');
  			return;
  		}
      if (!recognizing) {
        recognition.start();
      } else {
        recognition.stop();
      }
  	});
});