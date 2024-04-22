
  /******************************************************************************
  * @file    SHTC3.h
  * @author  Sahil Mangaonkar and Siddhant Mathur
  * @brief   SHTC3 driver file
  * @date    2023-04-20
  ******************************************************************************/
  
/******************************************************************************
 * Includes
 ******************************************************************************/

#include "SHTC3.h"
#include "i2c_master.h"
#include "i2c_master_interrupt.h"
#include "I2cDriver\I2cDriver.h"
#include "stdint.h"
#include "SerialConsole.h"

uint8_t msgOutImu[64];
static int32_t temp_write(uint8_t *bufp, uint16_t len);
static int32_t temp_read(uint8_t *bufp, uint16_t len);
int32_t SHTC3_Read_Data(uint8_t *buffer, uint8_t count);
int SHTC3_Init(void);


I2C_Data SHTC3Data;
/******************************************************************************
 * Functions
 ******************************************************************************/

/**
 * @fn		int SHTC3_Init(void)
 * @brief	Sending wakeup command to initialize
 * @details 	Assumes I2C is already initialized

 * @return		Returns 0 if no errors.
 * @note
 */
int SHTC3_Init(void){
	
	 //Sending wakeup command to initialize
     uint8_t cmd[] = {SHTC3_WAKEUP_CMD1, SHTC3_WAKEUP_CMD2};

     SHTC3Data.address = SHTC3_ADDR;
     SHTC3Data.msgOut = (const uint8_t *) &cmd[0];
     SHTC3Data.lenOut = sizeof(cmd);
     SHTC3Data.lenIn = 0;
     int32_t error = I2cWriteDataWait(&SHTC3Data, WAIT_TIME);
	
     return error;
}

/**
 * @fn		int32_t SHTC3_Read_Data(uint8_t *buffer, uint8_t count)
 * @brief	//Sending command to measure temperature first, then RH, in normal power mode, no clock stretching
 * @details 	Assumes I2C is already initialized

 * @return		Returns 0 if no errors.
 * @note
 */
int32_t SHTC3_Read_Data(uint8_t *buffer, uint8_t count){
	
	//Sending command to measure temperature first, then RH, in normal power mode, no clock stretching
	uint8_t cmd[] = {SHT3_TH_NM_NCS_MEASURE_CMD1, SHT3_TH_NM_NCS_MEASURE_CMD2};
	SHTC3Data.address = SHTC3_ADDR;
	SHTC3Data.msgOut = (const uint8_t*) &cmd[0];
	SHTC3Data.lenOut = sizeof(cmd);
	SHTC3Data.msgIn = buffer;
	SHTC3Data.lenIn = sizeof(buffer);

	int error = I2cReadDataWait(&SHTC3Data, WAIT_TIME, WAIT_TIME);

	if (ERROR_NONE != error) {
		SerialConsoleWriteString("Error reading Seesaw counts!/r/n");
	}
	return error;
}
