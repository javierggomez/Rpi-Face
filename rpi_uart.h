#ifndef RPI_UART
#define RPI_UART
int uart_initialize(int read, int write, int flags);
void uart_close(int fd);
void uart_setBaudRate(int fd, int baudRate);
void uart_setBlockForInput(int fd, int nChars);
int uart_read(int fd, char *buffer, int nChars);
int uart_write(int fd, char *string, int nChars);

#endif
