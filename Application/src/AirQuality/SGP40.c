
  /******************************************************************************
  * @file    SHTC3.h
  * @author  Sahil Mangaonkar and Siddhant Mathur
  * @brief   SHTC3 driver file
  * @date    2023-04-20
  ******************************************************************************/
  
/******************************************************************************
 * Includes
 ******************************************************************************/

#include "SGP40.h"
#include "i2c_master.h"
#include "i2c_master_interrupt.h"
#include "I2cDriver\I2cDriver.h"
#include "stdint.h"
#include "SerialConsole.h"

uint8_t msgOutImu[64];
//static int32_t temp_write(uint8_t *bufp, uint16_t len);
//static int32_t temp_read(uint8_t *bufp, uint16_t len);
int SGP40_Init(void);
int32_t SGP40_Read_Default_Data(uint8_t *buffer, uint8_t count);
int SGP40_Self_Test(void);


I2C_Data SGP40Data;
/******************************************************************************
 * Functions
 ******************************************************************************/

/**
 * @fn		int SGP40_Init(void)
 * @brief	Sending serial command to initialize
 * @details 	Assumes I2C is already initialized

 * @return		Returns 0 if no errors.
 * @note
 */
int SGP40_Init(void){

    uint8_t buffer[64];
	uint8_t buffer1[64];
    //Sending wakeup command to initialize
    uint8_t cmd[] = {SGP40_CMD_GET_SERIAL_ID1, SGP40_CMD_GET_SERIAL_ID2};

    // SGP40Data.address = SGP40_ADDR;
    // SGP40Data.msgOut = (const uint8_t *) &cmd[0];
    // SGP40Data.lenOut = sizeof(cmd);
    // SGP40Data.lenIn = 9; // Response length including CRC [bytes]
    // int32_t error = I2cWriteDataWait(&SGP40Data, WAIT_TIME);

    SGP40Data.address = SGP40_ADDR;
    SGP40Data.msgOut = (const uint8_t*) &cmd[0];
    SGP40Data.lenOut = sizeof(cmd);
    SGP40Data.msgIn = buffer;
    SGP40Data.lenIn = 9; // Response length including CRC [bytes]

    int error = I2cReadDataWait(&SGP40Data, WAIT_TIME, WAIT_TIME);

    if (ERROR_NONE != error) {
        SerialConsoleWriteString("SGP Get Serial Fail!/r/n");
        return error;
    }

    char *bufPtr = (char *)buffer1;
    int len = 0;
    for (size_t i = 0; i < 9; ++i) {
        // Ensure not to overflow buffer1
        if (len < sizeof(buffer1) - 3) {
            len += snprintf(bufPtr + len, sizeof(buffer1) - len, "%02X", buffer[i]);
        }
    }

    SerialConsoleWriteString("Serial Number: ");
    SerialConsoleWriteString(buffer1);
    SerialConsoleWriteString("\r\n");

    //snprintf((char *) buffer1, sizeof(buffer1), "Serial Number : %d\r\n", buffer);
	  //SerialConsoleWriteString(buffer1);

    // //Self Test
    // uint8_t cmd[] = {SGP40_CMD_Execute_Self_Test1, SGP40_CMD_Execute_Self_Test1};
    // SGP40Data.msgOut = (const uint8_t *) &cmd[0];
    // SGP40Data.lenOut = sizeof(cmd);
    // SGP40Data.lenIn = 0;
    // int32_t error = I2cWriteDataWait(&SGP40Data, WAIT_TIME);
    // if (ERROR_NONE != error) {
    //     SerialConsoleWriteString("SGP Self Test Fail!/r/n");
    // }

    return error;
}

/**
 * @fn		int SGP40_Self_Test(void)
 * @brief	Sending self test command to initialize
 * @details 	Assumes I2C is already initialized

 * @return		Returns 0 if no errors.
 * @note
 */
int SGP40_Self_Test(void){

    uint8_t buf[64];
	uint8_t buffer1[64];

    //Self Test
    uint8_t cmd[] = {SGP40_CMD_Execute_Self_Test1, SGP40_CMD_Execute_Self_Test2};
    SGP40Data.address = SGP40_ADDR;
    SGP40Data.msgOut = (const uint8_t*) &cmd[0];
    SGP40Data.lenOut = sizeof(cmd);
    SGP40Data.msgIn = buf;
    SGP40Data.lenIn = 3;
    int error = I2cReadDataWait(&SGP40Data, 32, WAIT_TIME);
    if (ERROR_NONE != error) {
        SerialConsoleWriteString("SGP Self Test Fail!\r\n");
    }
    // if(buffer[1] == 0xd4){}

    char *bufPtr = (char *)buffer1;
    int len = 0;
    for (size_t i = 0; i < 3; ++i) {
        // Ensure not to overflow buffer1
        if (len < sizeof(buffer1) - 3) {
            len += snprintf(bufPtr + len, sizeof(buffer1) - len, "%02X", buf[i]);
        }
    }
    SerialConsoleWriteString("self test result: ");
    SerialConsoleWriteString(buffer1);
    SerialConsoleWriteString("\r\n");

    return error;
}

/**
 * @fn		int32_t SHTC3_Read_Data(uint8_t *buffer, uint8_t count)
 * @brief	//Sending command to measure temperature first, then RH, in normal power mode, no clock stretching
 * @details 	Assumes I2C is already initialized
 * @param[in] 	uint8_t *buffer, uint8_t count
 * @return		Returns 0 if no errors.
 * @note
 */
int32_t SGP40_Read_Default_Data(uint8_t *buffer, uint8_t count) {
	
    //Sending command to measure temperature first, then RH, in normal power mode, no clock stretching
    uint8_t cmd[] = {SGP40_CMD_MEASURE_RAW1, SGP40_CMD_MEASURE_RAW2, 0x80, 0x00, 0xA2, 0x66, 0x66, 0x93};
    SGP40Data.address = SGP40_ADDR;
    SGP40Data.msgOut = (const uint8_t*) &cmd[0];
    SGP40Data.lenOut = sizeof(cmd);
    SGP40Data.msgIn = buffer;
    SGP40Data.lenIn = sizeof(buffer);

    int error = I2cReadDataWait(&SGP40Data, WAIT_TIME, WAIT_TIME);

    if (ERROR_NONE != error) {
      SerialConsoleWriteString("Error reading SGP data!/r/n");
    }
    return error;
}


/**
 * @fn		int32_t SGP40_Read_withTH_Data(uint8_t *buffer, uint8_t count, uint8_t humidity, uint8_t temperature)
 * @brief	Sending command to measure VOC index with known temperature and humidity
 * @details 	Assumes I2C is already initialized
 * @param[in] 	uint8_t *buffer, uint8_t count, uint8_t humidity, uint8_t temperature
 * @return		Returns 0 if no errors.
 * @note
*/
int32_t SGP40_Read_withTH_Data(uint8_t *buffer, uint8_t count, uint8_t humidity, uint8_t temperature) {
	
    // temp/ticks = (temperature + 45)*65535/175
    uint16_t temperature_ticks = (temperature + 45) * (65535 / 175);

    // RH/ticks = humidity * 65535 / 100 + 1
    uint16_t humidity_ticks = humidity * (65535 / 100) + 1;

    //Sending command to measure temperature first, then RH, in normal power mode, no clock stretching
    uint8_t cmd[] = {SGP40_CMD_MEASURE_RAW1, SGP40_CMD_MEASURE_RAW2, 
                    (uint8_t)(temperature_ticks >> 8), (uint8_t)(temperature_ticks & 0xFF), 0xA2, 
                    (uint8_t)(humidity_ticks >> 8), (uint8_t)(humidity_ticks & 0xFF), 0x93};
    SGP40Data.address = SGP40_ADDR;
    SGP40Data.msgOut = (const uint8_t*) &cmd[0];
    SGP40Data.lenOut = sizeof(cmd);
    SGP40Data.msgIn = buffer;
    SGP40Data.lenIn = sizeof(buffer);

    int error = I2cReadDataWait(&SGP40Data, WAIT_TIME, WAIT_TIME);

    if (ERROR_NONE != error) {
      SerialConsoleWriteString("Error reading SGP data!/r/n");
    }
    return error;
}
