# IOT-Edge-Computing-Final-Project

## Tasks

1. cli

2. wifi - send to queue operating state machine  - receive from queue senor readings

3. sensor  - send to queue sensor readings  -  receive from queue operating state machine

   - receive from queue current time

4. clock  -  send to queue time  -  receive from queue adjusted time

- priority: cli > clock > sensor = wifi

'''

xQueueWifiState = xQueueCreate(5, sizeof(uint32_t));

xQueueSensorBuffer = xQueueCreate(5, sizeof(struct SensorDataPacket));

xQueueOperateState = xQueueCreate(2, sizeof(uint32_t));  // 2 states

xQueueTimeInfo = xQueueCreate(5, sizeof(struct TimeStruct));  // time data

'''
