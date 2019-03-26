/*
 * main implementation: use this 'C' sample to create your own application
 *
 */


#include "S32K144.h"
#include "can_driver.h"
#include "transceiver.h"
#include "clocks_and_modes.h"

#define SEND_ID				(0x511)

int main(void)
{

	uint16_t ID;
	uint32_t msg[16];
	uint8_t msg_size;
	uint16_t timestamp;
	uint8_t DLC;
	WDOG_disable();

	/*********************** NOTE ***************************/
	/** This module is taken from the driver example FlexCAN*/
	/********************************************************/
	/** From here **********************************************************************************************/
	SOSC_init_8MHz();       /* Initialize system oscillator for 8
	   MHz xtal */
	SPLL_init_160MHz();     /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	NormalRUNmode_80MHz();  /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
	/** To here **********************************************************************************************/

	CAN_Init(CAN0, CAN_CTRL1_SPEED_500KBPS);

	/*********************** NOTE ***************************/
	/** This module is taken from the driver example FlexCAN*/
	/********************************************************/
	/** From here **********************************************************************************************/
	PORT_init();             /* Configure ports */
	LPSPI1_init_master();    /* Initialize LPSPI1 for communication with MC33903 */
	LPSPI1_init_MC33903();   /* Configure SBC via SPI for CAN transceiver operation */
	/** To here **********************************************************************************************/


	while(1)
	{
		if(CAN_get_rx_status(CAN0))
		{
			CAN_receive_message(CAN0, &ID, msg, &msg_size, &timestamp, &DLC);



			CAN_send_message(CAN0, ID + 1, msg, msg_size, DLC);
		}
	}

	return 0;
}
