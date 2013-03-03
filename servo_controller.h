#ifndef SERVO_CONTROLLER
#define SERVO_CONTROLLER
int servo_read(int fd, char *buffer, int nChars, int useconds);
int servo_initialize();
int servo_setServoPosition(int fd, int servo, unsigned char position);
int servo_setAllServosPositions(int fd, unsigned const char *positions);
int servo_turnOff(int fd, int servo);
int servo_setAsHome(int fd, int servo);
int servo_goToHome(int fd, int servo);
int servo_setWidthResolution(int fd, int servo, char resolution);
unsigned char servo_getPosition(int fd, int servo, int *error);
int servo_getTorque(int fd, int servo, int *error);
int servo_setBaudRate(int fd, int rate);
void servo_close(int fd);
#endif
