/**************************************************************************/ /**
 * @file      WifiHandler.h
 * @brief     File to handle HTTP Download and MQTT support
 * @author    Eduardo Garcia
 * @date      2020-01-01

 ******************************************************************************/

#pragma once

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "MQTTClient/Wrapper/mqtt.h"
#include "SerialConsole.h"
#include "asf.h"
#include "driver/include/m2m_wifi.h"
#include "iot/http/http_client.h"
#include "main.h"
#include "secret.h"  // NOTE: You must make this file and include your Wi-Fi settings
#include "socket/include/socket.h"
#include "stdio_serial.h"
#include "Global.h"

/******************************************************************************
 * Defines
 ******************************************************************************/

#define WIFI_MQTT_INIT 0        ///< State for Wifi handler to Initialize MQTT Connection
#define WIFI_MQTT_HANDLE 1      ///< State for Wifi handler to Handle MQTT Connection
#define WIFI_DOWNLOAD_INIT 2    ///< State for Wifi handler to Initialize Download Connection
#define WIFI_DOWNLOAD_HANDLE 3  ///< State for Wifi handler to Handle Download Connection

#define WIFI_TASK_SIZE 500
#define WIFI_PRIORITY (configMAX_PRIORITIES - 4)

/** IP address parsing. */
#define IPV4_BYTE(val, index) ((val >> (index * 8)) & 0xFF)

/** Content URI for download. */
#define MAIN_HTTP_FILE_URL "http://74.249.109.202/TestA.bin"  ///< Change me to the URL to download your OTAU binary file from!

/** Maximum size for packet buffer. */
#define MAIN_BUFFER_MAX_SIZE (512)
/** Maximum file name length. */
#define MAIN_MAX_FILE_NAME_LENGTH (64)
/** Maximum file extension length. */
#define MAIN_MAX_FILE_EXT_LENGTH (8)
/** Output format with '0'. */
#define MAIN_ZERO_FMT(SZ) (SZ == 4) ? "%04d" : (SZ == 3) ? "%03d" : (SZ == 2) ? "%02d" : "%d"
#define GAME_SIZE 20  ///< Number of plays in game

typedef enum {
    NOT_READY = 0,         /*!< Not ready. */
    STORAGE_READY = 0x01,  /*!< Storage is ready. */
    WIFI_CONNECTED = 0x02, /*!< Wi-Fi is connected. */
    GET_REQUESTED = 0x04,  /*!< GET request is sent. */
    DOWNLOADING = 0x08,    /*!< Running to download. */
    COMPLETED = 0x10,      /*!< Download completed. */
    CANCELED = 0x20        /*!< Download canceled. */
} download_state;


/**
 * @brief Structure to hold time since boot
 * TIME_INFO_SEND: send current time to wifihandeler
 * TIME_INFO_ADJUST: receive adjusted time from web
 * TIME_INFO_SET_OPEN: receive curtain open time from web
 * TIME_INFO_SET_CLOSE: receive curtain close time from web
*/
typedef enum {
    TIME_INFO_SEND, // send current time to wifihandeler
    TIME_INFO_ADJUST, // receive adjusted time from web
    TIME_INFO_SET_OPEN, // receive curtain open time from web
    TIME_INFO_SET_CLOSE // receive curtain close time from web
} TimeType;

/**
* @brief Structure to hold time strcut
* type: type of time
* hours: hours
* minutes: minutes
* seconds: seconds
* milliseconds: milliseconds
*/
struct TimeInfo {
    TimeType type;
    uint32_t hours;
    uint32_t minutes;
    uint32_t seconds;
    uint32_t milliseconds;
};

/**
 * @brief Structure to hold sensor data
    * light_intensity: light intensity
    * temperature: temperature
    * humidity: humidity
    * VOCvalue: VOC value
*/
struct SensorDataPacket{
    uint16_t light_intensity;
    uint16_t temperature;
    uint16_t humidity;
    uint16_t VOCvalue;
};

typedef enum {
	MANUAL, // manual decision based on user input
    SMART,  // smart decision based on photoresistor
    TIMER, // timer decision based on time set by user
	DUMMY
} ModeType;


struct ButtonDataPacket {
	ModeType mode;
	uint8_t openButton;
	uint8_t closeButton;
};


/* Max size of UART buffer. */
#define MAIN_CHAT_BUFFER_SIZE 64

/* Max size of MQTT buffer. */
#define MAIN_MQTT_BUFFER_SIZE 512

/* Limitation of user name. */
#define MAIN_CHAT_USER_NAME_SIZE 64

/* Chat MQTT topic. */
#define SENSOR_TOPIC "Sensor"
#define Button_TOPIC "Button"
#define TIME_TOPIC "Time"
#define TIME_ADJUST_TOPIC "Adjusted_Time"
#define TIME_TO_OPEN "Open_Time"
#define TIME_TO_CLOSE "Close_time"

#define LED_TOPIC_LED_OFF "false"
#define LED_TOPIC_LED_ON "true"

// Cloud MQTT User
#define CLOUDMQTT_USER_ID "azureuser"

// Cloud MQTT PASSWORD
#define CLOUDMQTT_USER_PASSWORD "BrsJBNVoQBl7"

#define CLOUDMQTT_PORT 1883

/*
 * A MQTT broker server which was connected.
 * m2m.eclipse.org is public MQTT broker.
 * WZ Azure MQTT Broker: 52.179.188.39
 * YX Azure MQTT Broker: 74.249.109.202
 */
static const char main_mqtt_broker[] = "74.249.109.202";

#define STRING_EOL "\r\n"
#define STRING_HEADER                                                                 \
    "-- HTTP file downloader example --" STRING_EOL "-- " BOARD_NAME " --" STRING_EOL \
    "-- Compiled: "__DATE__                                                           \
    " "__TIME__                                                                       \
    " --" STRING_EOL

/******************************************************************************
 * Structures and Enumerations
 ******************************************************************************/

/******************************************************************************
 * Global Function Declaration
 ******************************************************************************/

void vWifiTask(void *pvParameters);
void init_storage(void);

void SubscribeHandlerClockTopic(MessageData *msgData);
void SubscribeHandlerButtonTopic(MessageData *msgData);
void SubscribeHandlerOpenTopic(MessageData *msgData);
void SubscribeHandlerCloseTopic(MessageData *msgData);

void WifiHandlerSetState(uint8_t state);
int WifiAddTimeToQueue(struct TimeInfo *time);
int wifiAddSensorDataToQueue(struct SensorDataPacket *sensorPacket);

void configure_extint_channel(void);
void configure_extint_callbacks(void);

bool button_status(void);

#ifdef __cplusplus
}
#endif
