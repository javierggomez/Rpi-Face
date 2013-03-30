//    Copyright (C) 2013 Javier García, Julio Alberto González

//    This file is part of Rpi-Face.
//    Rpi-Face is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    Rpi-Face is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with Rpi-Face.  If not, see <http://www.gnu.org/licenses/>.

// Envía el voto al servidor usando el método POST.
$(document).ready(function() {
	$('#vote_plus').on('click', function() {
		$('#vote_hidden').val('1'); // valor del voto como parámetro oculto
		$('#vote_form').submit(); // enviar formulario
	});
	$('#vote_minus').on('click', function() {
		$('#vote_hidden').val('0'); // valor del voto como parámetro oculto
		$('#vote_form').submit(); // enviar formulario
	});
});