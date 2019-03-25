/*!
 	 \file can_driver.c

 	 \brief This is the source file of the CAN device driver for
 	 	 	 the S32K144EVB development board. All the initialization
 	 	 	 and control functions are found in this source.

 	 \author HEMI team
 	 	 	 Arpio Fernandez, Leon 				ie702086@iteso.mx
 	 	 	 Barragan Alvarez, Daniel 			ie702554@iteso.mx
 	 	 	 Delsordo Bustillo, Jose Ricardo	ie702570@iteso.mx

 	 \date 	27/03/2019
 */

#include "can_driver.h"
#include "S32K144.h"

#define INIT_VAL				(0)
#define MAX_MSG_BUFFERS			(128)
#define MAX_FILTER_BUFFERS		(16)
#define CHECK_ALL_MESSAGES		(0xFFFFFFFF)

#define MSG_BUF_SIZE			(4)

#define GLOBAL_ACCEPTANCE_MASK	(0x1FFFFFFF)
#define ENABLE_RX_BUFF			(0x04000000)

#define PARAM_OK				(0)
#define PARAM_NOT_OK			(1)

#define CLEAR_MB_0				(0x00000001)
#define STD_ID_MASK				(0x000007FF)

#define TX_BUFF_TRANSMITT		(0x0C400000)

return_codes_t CAN_Init(CAN_init_t config)
{
	/** Coutner to clean the RAM*/
	uint8_t counter;
	uint8_t param_ok = PARAM_OK;

	/** Default return value*/
	return_codes_t retval = CAN_success;

	if(config.speed != speed_50kbps ||
			config.speed != speed_100kbps ||
			config.speed != speed_250kbps ||
			config.speed != speed_500kbps)
	{
		param_ok = PARAM_NOT_OK;
	}


	if(CAN_0 == config.alternative && PARAM_OK == param_ok)
	{
		/** Enables the peripheral clock*/
		PCC->PCCn[PCC_FlexCAN0_INDEX] |= PCC_PCCn_CGC_MASK;

		/** Disables the Clock*/
		CAN0->MCR |= CAN_MCR_MDIS_MASK;
		/** Sets the clock source to the oscillator clock*/
		//TODO: Funciona con peripheral clock?
		CAN0->CTRL1 &= (~CAN_CTRL1_CLKSRC_MASK);
		/** Enables the module*/
		CAN0->MCR &= (~CAN_MCR_MDIS_MASK);

		/** Waits for the module to enter freeze mode*/
		while(!((CAN0->MCR & CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT));

		//TODO: Probar todas las velocidades
		/** Configures the speed, and other parameters*/
		CAN0->CTRL1 |= config.speed;

		for(counter = INIT_VAL ; MAX_MSG_BUFFERS > counter ; counter ++)
		{
			CAN0->RAMn[counter] = INIT_VAL;

			if(MAX_FILTER_BUFFERS > counter)
			{
				CAN0->RXIMR[counter] = CHECK_ALL_MESSAGES;
			}
		}

		CAN0->RXMGMASK = GLOBAL_ACCEPTANCE_MASK;

		//TODO: ¿Debe estar esto aqui?¿Cómo configurar los buffers de recepción de manera más dinámica?
		/************************************************************************************************/
		CAN0->RAMn[ 4*MSG_BUF_SIZE + 0] = ENABLE_RX_BUFF; /* Msg Buf 4, word 0: Enable for reception */


	#ifdef NODE_A                                   /* Node A receives msg with std ID 0x511 */
		CAN0->RAMn[ 4*MSG_BUF_SIZE + 1] = 0x14440000; /* Msg Buf 4, word 1: Standard ID = 0x111 */
	#else                                           /* Node B to receive msg with std ID 0x555 */
		CAN0->RAMn[ 4*MSG_BUF_SIZE + 1] = 0x15540000; /* Msg Buf 4, word 1: Standard ID = 0x555 */
	#endif
		/***********************************************************************************************/

		/** CAN FD not used*/
		CAN0->MCR |= 0x0000001F;       /* Negate FlexCAN 1 halt state for 32 MBs */

		/** Waits for the module to exit freeze mode*/
		while ((CAN0->MCR && CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT);
		/** Waits for the module to be ready*/
		while ((CAN0->MCR && CAN_MCR_NOTRDY_MASK) >> CAN_MCR_NOTRDY_SHIFT);
	}

	else if(CAN_1 == config.alternative && PARAM_OK == param_ok)
	{
		/** Enables the peripheral clock*/
		PCC->PCCn[PCC_FlexCAN1_INDEX] |= PCC_PCCn_CGC_MASK;

		/** Disables the module*/
		CAN1->MCR |= CAN_MCR_MDIS_MASK;
		/** Sets the clock source to the oscillator clock*/
		//TODO: Funciona con peripheral clock?
		CAN1->CTRL1 &= (~CAN_CTRL1_CLKSRC_MASK);
		/** Enables the module*/
		CAN1->MCR &= (~CAN_MCR_MDIS_MASK);

		/** Waits for the module to enter freeze mode*/
		while(!((CAN1->MCR & CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT));

		//TODO: Probar todas las velocidades
		/** Configures the speed, and other parameters*/
		CAN1->CTRL1 |= config.speed;

		for(counter = INIT_VAL ; MAX_MSG_BUFFERS > counter ; counter ++)
		{
			CAN1->RAMn[counter] = INIT_VAL;

			if(MAX_FILTER_BUFFERS > counter)
			{
				CAN1->RXIMR[counter] = CHECK_ALL_MESSAGES;
			}
		}

		CAN1->RXMGMASK = GLOBAL_ACCEPTANCE_MASK;

		//TODO: ¿Debe estar esto aqui?¿Cómo configurar los buffers de recepción de manera más dinámica?
		/************************************************************************************************/
		CAN1->RAMn[ 4*MSG_BUF_SIZE + 0] = ENABLE_RX_BUFF; /* Msg Buf 4, word 0: Enable for reception */


	#ifdef NODE_A                                   /* Node A receives msg with std ID 0x511 */
		CAN1->RAMn[ 4*MSG_BUF_SIZE + 1] = 0x14440000; /* Msg Buf 4, word 1: Standard ID = 0x111 */
	#else                                           /* Node B to receive msg with std ID 0x555 */
		CAN1->RAMn[ 4*MSG_BUF_SIZE + 1] = 0x15540000; /* Msg Buf 4, word 1: Standard ID = 0x555 */
	#endif
		/***********************************************************************************************/

		/** CAN FD not used*/
		CAN1->MCR |= 0x0000001F;       /* Negate FlexCAN 1 halt state for 32 MBs */

		/** Waits for the module to exit freeze mode*/
		while ((CAN1->MCR && CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT);
		/** Waits for the module to be ready*/
		while ((CAN1->MCR && CAN_MCR_NOTRDY_MASK) >> CAN_MCR_NOTRDY_SHIFT);
	}

	else
	{
		retval = CAN_parameter_error;
	}

	return retval;
}

return_codes_t CAN_send_message(CAN_alternative_t alt, uint16_t ID, uint32_t* msg, uint8_t msg_size)
{
	return_codes_t retval = CAN_success;
	uint8_t counter = 0;
	uint8_t DLC = 0;

	/** ID can only be of 11 bits*/
	ID &= STD_ID_MASK;
	/** Sets the DLC*/
	DLC = msg_size / 4;

	if(CAN_0 == alt)
	{
		/** Clears CAN 0 MB 0 interruption flag*/
		CAN0->IFLAG1 = CLEAR_MB_0;

		/** Sets the message in the CAN tx buffer*/
		for(counter = 0 ; counter < msg_size ; counter ++)
		{
			CAN0->RAMn[2 + counter] = (*msg);
			msg ++;
		}

		/** Sets the ID to the bits 28-18 (ID bits for standard format)*/
		CAN0->RAMn[1] = (ID << 16);

		/** Sets the CAN command to transmitt*/
		CAN0->RAMn[0] = (DLC << CAN_WMBn_CS_DLC_SHIFT) | TX_BUFF_TRANSMITT;
	}

	else if(CAN_1 == alt)
	{
		/** Clears CAN 0 MB 0 interruption flag*/
		CAN1->IFLAG1 = CLEAR_MB_0;

		/** Sets the message in the CAN tx buffer*/
		for(counter = 0 ; counter < msg_size ; counter ++)
		{
			CAN1->RAMn[2 + counter] = (*msg);
			msg ++;
		}

		/** Sets the ID to the bits 28-18 (ID bits for standard format)*/
		CAN1->RAMn[1] = (ID << 16);

		/** Sets the CAN command to transmitt*/
		CAN1->RAMn[0] = (DLC << CAN_WMBn_CS_DLC_SHIFT) | TX_BUFF_TRANSMITT;
	}

	else
	{
		retval = CAN_parameter_error;
	}

	return retval;
}
