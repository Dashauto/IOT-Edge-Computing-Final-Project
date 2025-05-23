/**
 * @file      BootMain.c
 * @brief     Main file for the ESE5160 bootloader. Handles updating the main application
 * @details   Main file for the ESE5160 bootloader. Handles updating the main application
 * @author    Eduardo Garcia
 * @author    Nick M-G
 * @date      2024-03-03
 * @version   2.0
 * @copyright Copyright University of Pennsylvania
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "conf_example.h"
#include "sd_mmc_spi.h"
#include <asf.h>
#include <string.h>

#include "ASF/sam0/drivers/dsu/crc32/crc32.h"
#include "SD Card/SdCard.h"
#include "SerialConsole/SerialConsole.h"
#include "Systick/Systick.h"

/******************************************************************************
 * Defines
 ******************************************************************************/
#define APP_START_ADDRESS           ((uint32_t) 0x12000)                    ///< Start of main application. Must be address of start of main application
#define APP_START_RESET_VEC_ADDRESS (APP_START_ADDRESS + (uint32_t) 0x04)   ///< Main application reset vector address

/******************************************************************************
 * Structures and Enumerations
 ******************************************************************************/

struct usart_module cdc_uart_module;   ///< Structure for UART module connected to EDBG (used for unit test output)

/******************************************************************************
 * Local Function Declaration
 ******************************************************************************/
static void jumpToApplication(void);
static bool StartFilesystemAndTest(void);
static void configure_nvm(void);

/******************************************************************************
 * Global Variables
 ******************************************************************************/
// INITIALIZE VARIABLES
char test_file_name[] = "0:sd_mmc_test.txt";   ///< Test TEXT File name
char test_bin_file[] = "0:sd_binary.bin";      ///< Test BINARY File name
Ctrl_status status;                            ///< Holds the status of a system initialization
FRESULT res;                                   // Holds the result of the FATFS functions done on the SD CARD TEST
FATFS fs;                                      // Holds the File System of the SD CARD
FIL file_object;                               // FILE OBJECT used on main for the SD Card Test


char testA_bin_file[] = "0:TestA.bin";
char testB_bin_file[] = "0:TestB.bin";
char flagA_txt_file[] = "0:FlagA.txt";
char flagB_txt_file[] = "0:FlagB.txt";
char flagC_txt_file[] = "0:Hello World.txt";
FIL file_objectA;
FIL file_objectB;
FIL file_objectFlagC;
FIL file_objectFlagA;
FIL file_objectFlagB;
FRESULT resA;
FRESULT resB;
FRESULT flagC;
FRESULT flagA;
FRESULT flagB;
FILINFO fno;
void clean_buffer();
void read_bin_file(FIL file_object);

/******************************************************************************
 * Global Functions
 ******************************************************************************/

/**
* @fn		int main(void)
* @brief	Main function for ESE5160 Bootloader Application

* @return	Unused (ANSI-C compatibility).
* @note		Bootloader code initiates here.
*****************************************************************************/

int main(void) {

    /*1.) INIT SYSTEM PERIPHERALS INITIALIZATION*/
    system_init();
    delay_init();
    InitializeSerialConsole();
    system_interrupt_enable_global();

    /* Initialize SD MMC stack */
    sd_mmc_init();

    // Initialize the NVM driver
    configure_nvm();

    irq_initialize_vectors();
    cpu_irq_enable();

    // Configure CRC32
    dsu_crc32_init();

    SerialConsoleWriteString("ESE5160 - ENTER BOOTLOADER");   // Order to add string to TX Buffer

    /*END SYSTEM PERIPHERALS INITIALIZATION*/

    /*2.) STARTS SIMPLE SD CARD MOUNTING AND TEST!*/

    // EXAMPLE CODE ON MOUNTING THE SD CARD AND WRITING TO A FILE
    // See function inside to see how to open a file
    SerialConsoleWriteString("\x0C\n\r-- SD/MMC Card Example on FatFs --\n\r");

    if (StartFilesystemAndTest() == false) {
        SerialConsoleWriteString("SD CARD failed! Check your connections. System will restart in 5 seconds...");
        delay_cycles_ms(5000);
        system_reset();
    } else {
        SerialConsoleWriteString("SD CARD mount success! Filesystem also mounted. \r\n");
    }

    /*END SIMPLE SD CARD MOUNTING AND TEST!*/

    /*3.) STARTS BOOTLOADER HERE!*/
    // Students - this is your mission!

    SerialConsoleWriteString("Creating file: HelloWorld.txt ... \r\n");
	flagC_txt_file[0] = LUN_ID_SD_MMC_0_MEM +'0';
	flagC = f_open(&file_objectFlagC,(char const *)flagC_txt_file, FA_CREATE_ALWAYS | FA_WRITE);

    if (flagC != FR_OK) {
        LogMessage(LOG_INFO_LVL, "[FAIL] res %d\r\n", res);
    }
    f_close(&file_objectFlagC);
    SerialConsoleWriteString("[OK]\r\n");

    //testA_bin_file[0] = LUN_ID_SD_MMC_0_MEM +'0';
    //testB_bin_file[0] = LUN_ID_SD_MMC_0_MEM +'0';
    //flagA_txt_file[0] = LUN_ID_SD_MMC_0_MEM +'0';
    //flagB_txt_file[0] = LUN_ID_SD_MMC_0_MEM +'0';
//
    //flagA = f_stat((char const *)flagA_txt_file, &fno);
    //flagB = f_stat((char const *)flagB_txt_file, &fno);
//
    //if(flagA == FR_OK) {
        //// delete flagA
        //flagA_txt_file[0] = LUN_ID_SD_MMC_0_MEM +'0';
        //f_unlink((char const *) flagA_txt_file);
//
        //// run TestA
        //resA = f_open(&file_objectA,(char const *)testA_bin_file,FA_READ);
        //if(resA == FR_OK) {
            //read_bin_file(file_objectA);
            //f_close(&file_objectA);
        //}
        //else{
            //f_close(&file_objectA);
        //}
        //SerialConsoleWriteString("\r\n run TestA \r\n");
    //}
    //else if(flagB == FR_OK) {
        //// delete flagB
        //flagB_txt_file[0] = LUN_ID_SD_MMC_0_MEM +'0';
        //f_unlink((char const *) flagB_txt_file);
        //
        //// run TestB
        //resB = f_open(&file_objectB,(char const *)testB_bin_file,FA_READ);
        //if(resB == FR_OK) {
            //read_bin_file(file_objectB);
            //f_close(&file_objectB);
        //}
        //else{
            //f_close(&file_objectB);
        //}
        //SerialConsoleWriteString("\r\n run TestB \r\n");
    //}
    //else {
        //// run TestA
        //resA = f_open(&file_objectA,(char const *)testA_bin_file,FA_READ);
        //if(resA == FR_OK) {
            //read_bin_file(file_objectA);
            //f_close(&file_objectA);
        //}
        //else{
            //f_close(&file_objectA);
        //}
        //SerialConsoleWriteString("\r\n run TestA \r\n");
    //}

    /* END BOOTLOADER HERE!*/

    // 4.) DEINITIALIZE HW AND JUMP TO MAIN APPLICATION!
    SerialConsoleWriteString("ESE5160 - EXIT BOOTLOADER");   // Order to add string to TX Buffer
    delay_cycles_ms(100);                                    // Delay to allow print

    // Deinitialize HW - deinitialize started HW here!
    DeinitializeSerialConsole();   // Deinitializes UART
    sd_mmc_deinit();               // Deinitialize SD CARD

    // Jump to application
    jumpToApplication();

    // Should not reach here! The device should have jumped to the main FW.
}

/******************************************************************************
 * Static Functions
 ******************************************************************************/

/**
 * function      static void StartFilesystemAndTest()
 * @brief        Starts the filesystem and tests it. Sets the filesystem to the global variable fs
 * @details      Jumps to the main application. Please turn off ALL PERIPHERALS that were turned on by the bootloader
 *				before performing the jump!
 * @return       Returns true is SD card and file system test passed. False otherwise.
 ******************************************************************************/
static bool StartFilesystemAndTest(void) {
    bool sdCardPass = true;
    uint8_t binbuff[256];

    // Before we begin - fill buffer for binary write test
    // Fill binbuff with values 0x00 - 0xFF
    for (int i = 0; i < 256; i++) {
        binbuff[i] = i;
    }

    // MOUNT SD CARD
    Ctrl_status sdStatus = SdCard_Initiate();
    if (sdStatus == CTRL_GOOD)   // If the SD card is good we continue mounting the system!
    {
        SerialConsoleWriteString("SD Card initiated correctly!\n\r");

        // Attempt to mount a FAT file system on the SD Card using FATFS
        SerialConsoleWriteString("Mount disk (f_mount)...\r\n");
        memset(&fs, 0, sizeof(FATFS));
        res = f_mount(LUN_ID_SD_MMC_0_MEM, &fs);   // Order FATFS Mount
        if (FR_INVALID_DRIVE == res) {
            LogMessage(LOG_INFO_LVL, "[FAIL] res %d\r\n", res);
            sdCardPass = false;
            goto main_end_of_test;
        }
        SerialConsoleWriteString("[OK]\r\n");

        // Create and open a file
        SerialConsoleWriteString("Create a file (f_open)...\r\n");

        test_file_name[0] = LUN_ID_SD_MMC_0_MEM + '0';
        res = f_open(&file_object, (char const *) test_file_name, FA_CREATE_ALWAYS | FA_WRITE);

        if (res != FR_OK) {
            LogMessage(LOG_INFO_LVL, "[FAIL] res %d\r\n", res);
            sdCardPass = false;
            goto main_end_of_test;
        }

        SerialConsoleWriteString("[OK]\r\n");

        // Write to a file
        SerialConsoleWriteString("Write to test file (f_puts)...\r\n");

        if (0 == f_puts("Test SD/MMC stack\n", &file_object)) {
            f_close(&file_object);
            LogMessage(LOG_INFO_LVL, "[FAIL]\r\n");
            sdCardPass = false;
            goto main_end_of_test;
        }

        SerialConsoleWriteString("[OK]\r\n");
        f_close(&file_object);   // Close file
        SerialConsoleWriteString("Test is successful.\n\r");

        // Write binary file
        // Read SD Card File
        test_bin_file[0] = LUN_ID_SD_MMC_0_MEM + '0';
        res = f_open(&file_object, (char const *) test_bin_file, FA_WRITE | FA_CREATE_ALWAYS);

        if (res != FR_OK) {
            SerialConsoleWriteString("Could not open binary file!\r\n");
            LogMessage(LOG_INFO_LVL, "[FAIL] res %d\r\n", res);
            sdCardPass = false;
            goto main_end_of_test;
        }

        // Write to a binaryfile
        SerialConsoleWriteString("Write to test file (f_write)...\r\n");
        uint32_t varWrite = 0;
        if (0 != f_write(&file_object, binbuff, 256, &varWrite)) {
            f_close(&file_object);
            LogMessage(LOG_INFO_LVL, "[FAIL]\r\n");
            sdCardPass = false;
            goto main_end_of_test;
        }

        SerialConsoleWriteString("[OK]\r\n");
        f_close(&file_object);   // Close file
        SerialConsoleWriteString("Test is successful.\n\r");

    main_end_of_test:
        SerialConsoleWriteString("End of Test.\n\r");

    } else {
        SerialConsoleWriteString("SD Card failed initiation! Check connections!\n\r");
        sdCardPass = false;
    }

    return sdCardPass;
}

/**
 * function      static void jumpToApplication(void)
 * @brief        Jumps to main application
 * @details      Jumps to the main application. Please turn off ALL PERIPHERALS that were turned on by the bootloader
 *				before performing the jump!
 * @return
 ******************************************************************************/
static void jumpToApplication(void) {
    // Function pointer to application section
    void (*applicationCodeEntry)(void);

    // Rebase stack pointer
    __set_MSP(*(uint32_t *) APP_START_ADDRESS);

    // Rebase vector table
    SCB->VTOR = ((uint32_t) APP_START_ADDRESS & SCB_VTOR_TBLOFF_Msk);

    // Set pointer to application section
    applicationCodeEntry = (void (*)(void))(unsigned *) (*(unsigned *) (APP_START_RESET_VEC_ADDRESS));

    // Jump to application. By calling applicationCodeEntry() as a function we move the PC to the point in memory pointed by applicationCodeEntry,
    // which should be the start of the main FW.
    applicationCodeEntry();
}

/**
 * function      static void configure_nvm(void)
 * @brief        Configures the NVM driver
 * @details
 * @return
 ******************************************************************************/
static void configure_nvm(void) {
    struct nvm_config config_nvm;
    nvm_get_config_defaults(&config_nvm);
    config_nvm.manual_page_write = false;
    nvm_set_config(&config_nvm);
}

/**
 * function      void clean_buffer()
 * @brief        Cleans the buffer
 * @details
 * @return
*/
void clean_buffer() {
    // define variables
	struct nvm_parameters parameters;
	nvm_get_parameters (&parameters);
	char helpStr[64]; 
	snprintf(helpStr, 63,"NVM Info: Number of Pages %d. Size of a page: %d bytes. \r\n", parameters.nvm_number_of_pages, parameters.page_size);
	SerialConsoleWriteString(helpStr);

    // erase the first 288 pages
	for(int i = 0; i < (parameters.nvm_number_of_pages / 4) - 288; i++){
		enum status_code nvmError = nvm_erase_row((APP_START_ADDRESS + i * 256));
		
        // check for errors
		if(nvmError != STATUS_OK)
		{
			snprintf(helpStr, 63,"NVM ERROR: Erase error at row %d \r\n", i + 288);
			SerialConsoleWriteString(helpStr);
			return;
		}
	}

    // check if the test page is erased
	for(int iter = 0; iter < 256; iter++)
	{
        //Pointer pointing to address APP_START_ADDRESS + iter
		char *a = (char *)(APP_START_ADDRESS + iter); 
		if(*a != 0xFF)
		{
			SerialConsoleWriteString("Error - test page is not erased!");
			break;
		}
	}
}

/**
 * function      void read_bin_file(FIL file_object)
 * @brief        Reads a binary file from the SD card and writes it to the NVM
 * @details
 * @return
*/
void read_bin_file(FIL file_object) {
    // Define variables
	char helpStr[64]; 
	uint32_t resultCrcSd = 0;
	uint32_t resultCrcNVM = 0;
    // Clean the buffer
	clean_buffer();
	uint8_t readBuffer[256];
    int i =0;
    // set the pointer to the start of the file
    UINT NumBytesRead = 256;
    // Read the file
    // if the number of bytes read is less than 256, then we have reached the end of the file
    while(NumBytesRead == 256){
        res = f_read(&file_object, &readBuffer[0],  256, &NumBytesRead);
            SerialConsoleWriteString("read success \r\n");
        if(res != FR_OK){
            SerialConsoleWriteString("read error \r\n");
            f_close(&file_object);
            return false;
        }

        // set the current address
        int cur_address = APP_START_ADDRESS + (i * 256);
        
        // fill the rest of the buffer with 0xFF
        if(NumBytesRead < 256){
            for(int iter = NumBytesRead; iter < 256; iter++){
                readBuffer[iter] = 0xFF;
            }
        }

        // write the buffer to the NVM
        res = nvm_write_buffer(APP_START_ADDRESS + (i * 256), &readBuffer[0], 64);
        res = nvm_write_buffer(APP_START_ADDRESS + (i * 256) + 64, &readBuffer[64], 64);
        res = nvm_write_buffer(APP_START_ADDRESS +(i * 256) + 128, &readBuffer[128], 64);
        res = nvm_write_buffer(APP_START_ADDRESS + (i * 256) + 192, &readBuffer[192], 64);

        // calculate the CRC of the SD card chunck
        *((volatile unsigned int*) 0x41007058) &= ~0x30000UL;
        dsu_crc32_cal(readBuffer, 256, &resultCrcSd);
        *((volatile unsigned int*) 0x41007058) |= 0x20000UL;
        
        dsu_crc32_cal(APP_START_ADDRESS + (i*256), 256, &resultCrcNVM);
        
        i++;
    }

    // compare the CRC of the SD card and the NVM
    if(resultCrcSd != resultCrcNVM){
        SerialConsoleWriteString("CRC ERROR\r\n");
        snprintf(helpStr, 63,"CRC SD CARD: %d  CRC NVM: %d \r\n", resultCrcSd, resultCrcNVM);
        SerialConsoleWriteString(helpStr);
    }
}
