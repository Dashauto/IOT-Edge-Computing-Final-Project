# IOT-Edge-Computing-Final-Project

## Tasks

1. cli

2. wifi - send to queue operating state machine  - receive from queue senor readings

3. sensor  - send to queue sensor readings  -  receive from queue operating state machine

   - receive from queue current time

4. clock  -  send to queue time  -  receive from queue adjusted time

- priority: cli > clock > sensor = wifi

```

xQueueWifiState = xQueueCreate(5, sizeof(uint32_t));

xQueueSensorBuffer = xQueueCreate(5, sizeof(struct SensorDataPacket));

xQueueOperateState = xQueueCreate(2, sizeof(uint32_t));  // 2 states

xQueueTimeInfo = xQueueCreate(5, sizeof(struct TimeStruct));  // time data

```

Wifi send & receive procedure:

将XXX改为合适的内容：

1. 在头文件中定义要传的struct

2. 声明[QueueHandle_t xQueueXXXInfo = NULL;] 以及 [xQueueXXXInfo = xQueueCreate(5, sizeof(struct TimeXXX));] 跨Task传参handler

3. 添加[int WifiAddXXXToQueue(struct TimeSinceBoot *time)]函数作为跨Task传参函数

4. 在 [static void MQTT_HandleTransactions(void)]函数中添加[MQTT_HandleXXXMessages()]函数

5. 补全[MQTT_HandleXXXMessages()]函数

6. 在头文件中定义 MQTT topic [#define XXX_TOPIC "xxx"]

7. 定义 [void SubscribeHandlerXXXTopic(MessageData *msgData)]函数


## 0424 update

   奇怪的bug：SubscribeHandlerClockTopic function converting time error

   当输入时间为 12:00 AM - 02:46 AM 时，改function会将毫秒转换为错误的小时与分钟
