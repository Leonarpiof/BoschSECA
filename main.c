/*
 * main implementation: use this 'C' sample to create your own application
 *
 */


#include "S32K144.h"
#include "can_driver.h"

#define SEND_ID				(0x511)

int main(void)
{

	uint16_t ID;
	uint32_t msg[16];
	uint8_t msg_size;
	uint16_t timestamp;

	CAN_Init(CAN0, speed_500kbps);

	while(1)
	{
		if(CAN_get_rx_status(CAN0))
		{
			CAN_receive_message(CAN0, &ID, msg, &msg_size, &timestamp);

			CAN_send_message(CAN0, ID, msg, msg_size);
		}
	}

	return 0;
}
