CC=gcc
CFLAGS=-Wall -std=gnu99
EXECUTABLES=move_face face_controller_test servo_controller_test servo_controllerTest rpi_uart_test

all: $(EXECUTABLES) 

move_face: move_face.o face_controller.o servo_controller.o rpi_uart.o
	$(CC) $(CFLAGS) -o move_face move_face.o face_controller.o servo_controller.o rpi_uart.o
face_controller_test: face_controller_test.o face_controller.o servo_controller.o rpi_uart.o
	$(CC) $(CFLAGS) -o face_controller_test face_controller_test.o face_controller.o servo_controller.o rpi_uart.o

servo_controller_test: servo_controller_test.o servo_controller.o rpi_uart.o
	$(CC) $(CFLAGS) -o servo_controller_test servo_controller_test.o servo_controller.o rpi_uart.o

servo_controllerTest: servo_controllerTest.o servo_controller.o rpi_uart.o
	$(CC) $(CFLAGS) -o servo_controllerTest servo_controllerTest.o servo_controller.o rpi_uart.o

rpi_uart_test: rpi_uart_test.o rpi_uart.o
	$(CC) $(CFLAGS) -o rpi_uart_test rpi_uart_test.o rpi_uart.o

.PHONY = clean 
clean: 
	-rm -rf *.o $(EXECUTABLES) *~
