#ifndef FACE_CONTROLLER
#define FACE_CONTROLLER
extern const unsigned char FACE_HAPPY[8];
extern const unsigned char FACE_SAD[8];
extern const unsigned char FACE_SURPRISE[8];
extern const unsigned char FACE_ANGRY[8];
extern const unsigned char FACE_NEUTRAL[8];
int face_initialize();
int face_setAsHome(int fd);
int face_goToHome(int fd);
int face_setFace(int fd, unsigned const char *positions);
int face_turnOff(int fd);
void face_close(int fd);
#endif
