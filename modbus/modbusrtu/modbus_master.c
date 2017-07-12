#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
 
#include <modbus.h>
 
#define MODBUS_SERIAL_DEV           "/dev/ttyO1"
#define MODBUS_SERIAL_BAUDRATE      9600    /* 9600, 38400, 115200, ... */
#define MODBUS_SERIAL_PARITY        'N'     /* 'N', 'E', or 'O' */
#define MODBUS_SERIAL_DATABITS      8       /* 5, 6, 7, or 8 */
#define MODBUS_SERIAL_STOPBITS      1       /* 1 or 2 */
#define MODBUS_DEVICE_ID            1
#define MODBUS_TIMEOUT_SEC          3
#define MODBUS_TIMEOUT_USEC         0
#define MODBUS_DEBUG                ON
#define MODBUS_RO_BITS              32
#define MODBUS_RW_BITS              32
#define MODBUS_RO_REGISTERS         64
#define MODBUS_RW_REGISTERS         64
 
int main(int argc, char *argv[])
{
   
  //Create a new RTU context with proper serial parameters (in this example,
  //device name /dev/ttyS0, baud rate 9600, no parity bit, 8 data bits, 1 stop bit)
  modbus_t *ctx = modbus_new_rtu("/dev/ttyO1", 9600, 'N', 8, 1);  
  if (!ctx) {
    fprintf(stderr, "Failed to create the context: %s\n", modbus_strerror(errno));
    exit(1);
  }

  if (modbus_connect(ctx) == -1) {
    fprintf(stderr, "Unable to connect: %s\n", modbus_strerror(errno));
    modbus_free(ctx);
    exit(1);
  }

  //Set the Modbus address of the remote slave (to 3)
  modbus_set_slave(ctx, 3);

  uint16_t reg[5];// will store read registers values
  int i;
  
  //Read 5 holding registers starting from address 10
  int num = modbus_read_registers(ctx, 10, 5, reg);
  if (num != 5) {// number of read registers is not the one expected
    fprintf(stderr, "Failed to read: %s\n", modbus_strerror(errno));
  }

  modbus_close(ctx);
  modbus_free(ctx);
	
  for(i=0;i<num;i++)
   printf("req:%x \n",reg[i]);
  
  printf("done...\n");
  return 0;
}


