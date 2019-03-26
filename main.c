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

	PORT_init();             /* Configure ports */
	LPSPI1_init_master();    /* Initialize LPSPI1 for communication with MC33903 */
	LPSPI1_init_MC33903();   /* Configure SBC via SPI for CAN transceiver operation */

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
