/*!
 	 \file can_driver.h

 	 \brief This is the header file of the CAN device driver for
 	 	 	 the S32K144EVB development board. All the initialization
 	 	 	 and control functions are found in this header.

 	 \author HEMI team
 	 	 	 Arpio Fernandez, Leon 				ie702086@iteso.mx
 	 	 	 Barragan Alvarez, Daniel 			ie702554@iteso.mx
 	 	 	 Delsordo Bustillo, Jose Ricardo	ie702570@iteso.mx

 	 \date 	27/03/2019
 */

#ifndef CAN_DRIVER_H_
#define CAN_DRIVER_H_

#include "S32K144.h"

#define CAN_CTRL1_SPEED_500KBPS			(0x00DB0006)
#define CAN_CTRL1_SPEED_250KBPS			(0x01DB0006)
#define CAN_CTRL1_SPEED_100KBPS			(0x04DB0006)
#define CAN_CTRL1_SPEED_50KBPS			(0x09DB0006)

typedef enum
{
	speed_500kbps = CAN_CTRL1_SPEED_500KBPS,
	speed_250kbps = CAN_CTRL1_SPEED_250KBPS,
	speed_100kbps = CAN_CTRL1_SPEED_100KBPS,
	speed_50kbps = CAN_CTRL1_SPEED_50KBPS,
}CAN_speed_t;

typedef enum
{
	rx_not_interrupted,
	rx_interrupted
}CAN_rx_status_t;

typedef enum
{
	tx_not_interrupted,
	tx_interrupted
}CAN_tx_status_t;

void CAN_Init(CAN_Type* base, CAN_speed_t speed);

void CAN_send_message(CAN_Type* base, uint16_t ID, uint32_t* msg, uint8_t msg_size);

void CAN_receive_message(CAN_Type* base, uint16_t* ID, uint32_t* msg, uint8_t* msg_size, uint16_t* timestamp);

CAN_rx_status_t CAN_get_rx_status(CAN_Type* base);

#endif /* CAN_DRIVER_H_ */
