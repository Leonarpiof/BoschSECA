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

#define CAN_CTRL1_SPEED_500KBPS			(0x00DB0006)
#define CAN_CTRL1_SPEED_250KBPS			(0x01DB0006)
#define CAN_CTRL1_SPEED_100KBPS			(0x04DB0006)
#define CAN_CTRL1_SPEED_50KBPS			(0x09DB0006)

typedef enum
{
	CAN_success,
	CAN_parameter_error
}return_codes_t;

typedef enum
{
	CAN_0,
	CAN_1
}CAN_alternative_t;

typedef enum
{
	speed_500kbps = CAN_CTRL1_SPEED_500KBPS,
	speed_250kbps = CAN_CTRL1_SPEED_250KBPS,
	speed_100kbps = CAN_CTRL1_SPEED_100KBPS,
	speed_50kbps = CAN_CTRL1_SPEED_50KBPS,
}CAN_speed_t;

typedef struct
{
	CAN_alternative_t alternative;
	CAN_speed_t speed;
}CAN_init_t;

return_codes_t CAN_Init(CAN_init_t config);

#endif /* CAN_DRIVER_H_ */
