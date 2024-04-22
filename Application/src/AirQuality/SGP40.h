  /******************************************************************************
  * @file    SGP40.h
  * @author  yn wz
  * @brief   SGP40 driver file
  * @date    2024-04-03
  ******************************************************************************/
  #ifndef SGP40_H
  #define SGP40_H

  #ifdef __cplusplus
  extern "C" {
	  #endif
	  
/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stddef.h>
#include <math.h>

/******************************************************************************
 * Defines
 ******************************************************************************/
#define SGP40_ADDR 0x59
#define SGP40_CMD_Execute_Self_Test 0x280e
#define SGP40_CMD_Execute_Self_Test1 0x28
#define SGP40_CMD_Execute_Self_Test2 0x0e
#define SGP40_CMD_Turn_Heater_Off 0x3615

#define SGP40_CMD_MEASURE_RAW_WORDS 1
#define SGP40_CMD_MEASURE_RAW 0x260f
#define SGP40_CMD_MEASURE_RAW1 0x26
#define SGP40_CMD_MEASURE_RAW2 0x0f

/* command and constants for reading the serial ID */
#define SGP40_CMD_GET_SERIAL_ID_DURATION_US 500
#define SGP40_CMD_GET_SERIAL_ID_WORDS 3
#define SGP40_CMD_GET_SERIAL_ID 0x3682
#define SGP40_CMD_GET_SERIAL_ID1 0x36
#define SGP40_CMD_GET_SERIAL_ID2 0x82

/* command and constants for reading the featureset version */
#define SGP40_CMD_GET_FEATURESET_DURATION_US 1000
#define SGP40_CMD_GET_FEATURESET_WORDS 1
#define SGP40_CMD_GET_FEATURESET 0x202f

/* command and constants for measuring raw signals */
#define SGP40_CMD_MEASURE_RAW_DURATION_US 100000
#define SGP40_DEFAULT_HUMIDITY 0x8000
#define SGP40_DEFAULT_TEMPERATURE 0x6666
#define SGP40_SERIAL_ID_NUM_BYTES 6

#define WAIT_TIME 0xff


/******************************************************************************
 * Structures and Enumerations
 ******************************************************************************/


// #define SHTC3_WAKEUP_CMD 0x3517
// #define SHTC3_WAKEUP_CMD1 0x35
// #define SHTC3_WAKEUP_CMD2 0x17
// #define SHTC3_SLEEP_CMD 0xB098
// #define SHTC3_SOFT_RESET_CMD 0x805D
// #define SHTC3_ID_REG 0xEFC8

// #define WAIT_TIME 0xff

// #define SHT3_TH_NM_NCS_MEASURE_CMD 0x7866  ///< Command to measure temperature first, then RH, in normal power mode, no clock stretching
// #define SHT3_TH_NM_NCS_MEASURE_CMD1 0x78   ///< Command to measure temperature first, then RH, in normal power mode, no clock stretching
// #define SHT3_TH_NM_NCS_MEASURE_CMD2 0x66  ///< Command to measure temperature first, then RH, in normal power mode, no clock stretching

// #define SHT3_TH_LPM_NCS_MEASURE_CMD 0x609C  ///< Command to measure temperature first, then RH, in low power mode, no clock stretching

// #define SHT3_HT_NM_NCS_MEASURE_CMD 0x58E0 ///< Command to measure RH first, then temperature, in normal power mode, no clock stretching
// #define SHT3_HT_LPM_NCS_MEASURE_CMD 0x401A  ///< Command to measure RH first, then temperature, in low power mode, no clock stretching

// #define SHT3_TH_NM_CS_MEASURE_CMD 0x7CA2  ///< Command to measure temperature first, then RH, in normal power mode, clock stretching
// #define SHT3_TH_LPM_CS_MEASURE_CMD 0x6458 ///< Command to measure temperature first, then RH, in low power mode, clock stretching

// #define SHT3_HT_NM_CS_MEASURE_CMD 0x5C24 ///< Command to measure RH first, then temperature, in normal power mode,  clock stretching
// #define SHT3_HT_LPM_CS_MEASURE_CMD 0x44DE  ///< Command to measure RH first, then temperature, in low power mode, clock stretching

/******************************************************************************
 * Structures and Enumerations
 ******************************************************************************/

/******************************************************************************
 * Global Function Declaration
 ******************************************************************************/
//int SHTC3_Init(void);
//int SHTC3_SendI2cCommand(uint8_t *buf, uint8_t size);
//int SHTC3_Read_Data(uint8_t *buf, uint8_t size);
//int SHTC3_Init(uint8_t *buffer, uint8_t count);
int SGP40_Init(void);
int32_t SGP40_Read_Default_Data(uint8_t *buffer, uint8_t count);
int SGP40_Self_Test(void);

#ifdef __cplusplus
}
#endif

#endif /*SHTC3_DRIVER_H */

