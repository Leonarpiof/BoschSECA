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
