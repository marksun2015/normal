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
 
/*
 * modbus_mapping_new():
 * The modbus_mapping_new() function shall allocate four arrays to store
 * bits, input bits, registers, inputs registers.  
 *
 * for master (r/w)
 * Input Discrte(r),Coils(r/w),Input Registers(r),Holding Registers(r/w) 
 *
 * for slave (r/w)
 * Input Discrte(r/w),Coils(r/w),Input Registers(r/w),Holding Registers(r/w) 
 */

int main(int argc, char *argv[])
{
 //Prepare a Modbus mapping with 30 holding registers
 //(plus no output coil, one input coil and two input registers)
 //This will also automatically set the value of each register to 0
 	modbus_mapping_t *mapping = modbus_mapping_new(0, 1, 30, 2);
 	if (!mapping) {
     	fprintf(stderr, "Failed to allocate the mapping: %s\n", modbus_strerror(errno));
         exit(1);
     }

 	//Example: set register 12 to integer value 623
 	//Example: expect reply to master from register tab_registers[10] ~ [15]  
	mapping->tab_registers[12] = 623;

	modbus_t *ctx = modbus_new_rtu("/dev/ttyO1", 9600, 'N', 8, 1);
	if (!ctx) {
		fprintf(stderr, "Failed to create the context: %s\n", modbus_strerror(errno));
		exit(1);
	}

	//Set the Modbus address of this slave (to 3)
	modbus_set_slave(ctx, 3);


	if (modbus_connect(ctx) == -1) {
		fprintf(stderr, "Unable to connect: %s\n", modbus_strerror(errno));
		modbus_free(ctx);
		exit(1);
	}


	uint8_t req[MODBUS_RTU_MAX_ADU_LENGTH];// request buffer
	int len;// length of the request/response
	int i;
	while(1) {
		len = modbus_receive(ctx, req);//receive request from master and reply mapping 
		if (len == -1) break;

		len = modbus_reply(ctx, req, len, mapping); //replay registers 10 ~ 15
		if (len == -1) break;
	}
	printf("Exit the loop: %s\n", modbus_strerror(errno));

	modbus_mapping_free(mapping);
	modbus_close(ctx);
	modbus_free(ctx);
  	return 0;
}


