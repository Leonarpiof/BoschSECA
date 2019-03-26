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

#define CAN_CODE_MASK			(0x07000000)
#define CAN_CODE_SHIFT			(24)

#define CAN_TIMESTAMP_MASK		(0x0000FFFF)
#define CLEAR_MB_4				(0x00000010)

#define BIT_MASK				(1)

static uint32_t RxCODE;
static uint32_t RxID;
static uint32_t RxLENGTH;
static uint32_t RxDATA[16];
static uint32_t RxTIMESTAMP;

void CAN_Init(CAN_Type* base, CAN_speed_t speed)
{
	/** Coutner to clean the RAM*/
	uint8_t counter;

	if(CAN0 == base)
	{
		/** Enables the peripheral clock*/
		PCC->PCCn[PCC_FlexCAN0_INDEX] |= PCC_PCCn_CGC_MASK;
	}

	else if(CAN1 == base)
	{
		/** Enables the peripheral clock*/
		PCC->PCCn[PCC_FlexCAN1_INDEX] |= PCC_PCCn_CGC_MASK;
	}

	else if(CAN2 == base)
	{
		PCC->PCCn[PCC_FlexCAN2_INDEX] |= PCC_PCCn_CGC_MASK;
	}

	/** Disables the module*/
	base->MCR |= CAN_MCR_MDIS_MASK;
	/** Sets the clock source to the oscillator clock*/
	//TODO: Funciona con peripheral clock?
	base->CTRL1 &= (~CAN_CTRL1_CLKSRC_MASK);
	/** Enables the module*/
	base->MCR &= (~CAN_MCR_MDIS_MASK);

	/** Waits for the module to enter freeze mode*/
	while(!((base->MCR & CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT));

	//TODO: Probar todas las velocidades
	/** Configures the speed, and other parameters*/
	base->CTRL1 |= speed;

	for(counter = INIT_VAL ; MAX_MSG_BUFFERS > counter ; counter ++)
	{
		base->RAMn[counter] = INIT_VAL;

		if(MAX_FILTER_BUFFERS > counter)
		{
			base->RXIMR[counter] = CHECK_ALL_MESSAGES;
		}
	}

	CAN0->RXMGMASK = GLOBAL_ACCEPTANCE_MASK;

	//TODO: ¿Debe estar esto aqui?¿Cómo configurar los buffers de recepción de manera más dinámica?
	//TODO: ¿Es posible configurar "Cualquier ID" y revisar el ID fuera de la función?
	/************************************************************************************************/
	base->RAMn[ 4*MSG_BUF_SIZE + 0] = ENABLE_RX_BUFF; /* Msg Buf 4, word 0: Enable for reception */

#ifdef NODE_A                                   /* Node A receives msg with std ID 0x511 */
	base->RAMn[ 4*MSG_BUF_SIZE + 1] = 0x14440000; /* Msg Buf 4, word 1: Standard ID = 0x111 */
#else                                           /* Node B to receive msg with std ID 0x555 */
	base->RAMn[ 4*MSG_BUF_SIZE + 1] = 0x15540000; /* Msg Buf 4, word 1: Standard ID = 0x555 */
#endif
	/***********************************************************************************************/

	/** CAN FD not used*/
	base->MCR |= 0x0000001F;       /* Negate FlexCAN 1 halt state for 32 MBs */

	/** Waits for the module to exit freeze mode*/
	while ((base->MCR && CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT);
	/** Waits for the module to be ready*/
	while ((base->MCR && CAN_MCR_NOTRDY_MASK) >> CAN_MCR_NOTRDY_SHIFT);
}

void CAN_send_message(CAN_Type* base, uint16_t ID, uint32_t* msg, uint8_t msg_size)
{
	uint8_t counter = INIT_VAL;
	uint8_t DLC = INIT_VAL;

	/** ID can only be of 11 bits*/
	ID &= STD_ID_MASK;
	/** Sets the DLC*/
	DLC = msg_size / 4;

	/** Clears CAN 0 MB 0 interruption flag*/
	base->IFLAG1 = CLEAR_MB_0;

	/** Sets the message in the CAN tx buffer*/
	for(counter = 0 ; counter < msg_size ; counter ++)
	{
		base->RAMn[2 + counter] = (*msg);
		msg ++;
	}

	/** Sets the ID to the bits 28-18 (ID bits for standard format)*/
	base->RAMn[1] = (ID << 16);

	/** Sets the CAN command to transmit*/
	base->RAMn[0] = (DLC << CAN_WMBn_CS_DLC_SHIFT) | TX_BUFF_TRANSMITT;
}

void CAN_receive_message(CAN_Type* base, uint16_t* ID, uint32_t* msg, uint8_t* msg_size, uint16_t* timestamp)
{
	uint8_t counter = INIT_VAL;

	/** Gets the rx code*/
	RxCODE = (base->RAMn[16] & CAN_CODE_MASK) >> CAN_CODE_SHIFT;
	/** Gets ID*/
	RxID = (base->RAMn[17] & CAN_WMBn_ID_ID_MASK) >> CAN_WMBn_ID_ID_SHIFT;
	/** Gets the DLC*/
	RxLENGTH = (base->RAMn[16] & CAN_WMBn_CS_DLC_MASK) >> CAN_WMBn_CS_DLC_SHIFT;

	/** Gets each of the data*/
	for(counter = INIT_VAL ; counter < RxLENGTH ; counter ++)
	{
		RxDATA[counter] = base->RAMn[18 + counter];
		/** Sets the data to the msg pointer*/
		(*msg) = RxDATA[counter];
		msg ++;
	}

	/** Gets the time stamp*/
	RxTIMESTAMP = (base->RAMn[0] & CAN_TIMESTAMP_MASK);

	/** Clears the reception flag*/
	base->IFLAG1 = CLEAR_MB_4;

	/** Returns the data*/
	(*ID) = RxID;
	(*msg_size) = RxLENGTH;
	(*timestamp) = RxTIMESTAMP;
}

/** Gets the flag of the RX buffer*/
CAN_rx_status_t CAN_get_rx_status(CAN_Type* base)
{
	return ((CAN_rx_status_t)((base->IFLAG1 >> 4) & BIT_MASK));
}

/** Gets the flag of the RX buffer*/
CAN_tx_status_t CAN_get_tx_status(CAN_Type* base)
{
	return((CAN_tx_status_t)(base->IFLAG1 & BIT_MASK));
}
