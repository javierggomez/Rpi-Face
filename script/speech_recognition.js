$(document).ready(function() {
  var recognition=null;
  var recognizing=false;
  if (!('webkitSpeechRecognition' in window)) {
    //el navegador no soporta reconocimiento
    $('#message').prop('placeholder', 'Escribe un mensaje');
    $('#record>img').prop('title', 'La función de reconocimiento de voz solo es soportada por Chrome 25 o superior.');
    //$('#record').prop('disabled', true);
    $('#record>img').prop('src', '/img/img_microphone_disabled.ico');
  } else {
    // El navegador soporta reconocimiento
    $('#message').prop('placeholder', 'Escribe un mensaje o pincha en el micrófono y habla');
    recognition = new webkitSpeechRecognition(); // crear objeto
    recognition.lang='es-ES';
    // Ejecutado al iniciar reconocimiento
    recognition.onstart = function() {
      recognizing=true;
      $('#message').val(''); // borrar texto
      $('#message').prop('placeholder', 'Habla ahora');
    }
    // Ejecutado cuando el servidor devuelve el resultado del reconocimiento
    recognition.onresult = function(event) {
        //obtener resultado
        var message='';
        for (var i=event.resultIndex;i<event.results.length;i++) {
          message+=event.results[i][0].transcript; 
        }
        $('#message').val(message); // mostrarlo
    };
    // Ejecutado si hay error de reconocimiento
    recognition.onerror = function(event) {
        recognizing=false;
        $('#message').prop('placeholder', 'Escribe un mensaje o pincha en el micrófono y habla');
        $('#message').val(''); 
        alert('Error: '+event.error); // avisar del error
       };
    // Ejecutado al finalizar reconocimiento
    recognition.onend = function() { 
        recognizing=false;
        $('#message').prop('placeholder', 'Escribe un mensaje o pincha en el micrófono y habla');
      }
    }
    // Ejecutado al pulsar el botón del micrófono
    $('#record').on('click', function() {
      if (recognition==null) {
        alert('La función de reconocimiento de voz solo es soportada por Chrome 25 o superior.');
        return;
      }
      // Cambiar estado de reconocimiento
      if (!recognizing) {
        recognition.start();
      } else {
        recognition.stop();
      }
    });
});