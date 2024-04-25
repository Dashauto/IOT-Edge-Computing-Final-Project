# IOT-Edge-Computing-Final-Project

## Tasks

1. cli

2. wifi - send to queue operating state machine  - receive from queue senor readings

3. sensor  - send to queue sensor readings `xQueueSensorBuffer`  -  receive from queue operating state machine

   - receive from queue current time

4. clock  -  send to queue time `xQueueTimeInfo`  -  receive from queue adjusted time `xQueueTimeAdjInfo`

- priority: cli > clock > sensor = wifi

```

xQueueWifiState = xQueueCreate(5, sizeof(uint32_t));

xQueueSensorBuffer = xQueueCreate(5, sizeof(struct SensorDataPacket));

xQueueOperateState = xQueueCreate(2, sizeof(uint32_t));  // 2 states

xQueueTimeInfo = xQueueCreate(5, sizeof(struct TimeStruct));  // time data

```

Wifi send & receive procedure:

将XXX和YYY改为合适的内容：

1. 在头文件中定义要传的struct

2. 声明[QueueHandle_t xQueueXXXInfo = NULL;] 以及 [xQueueXXXInfo = xQueueCreate(5, sizeof(struct XXX));] 跨Task传参handler

   1. 如果需要实现数据双向传输，即既能接收数据又能发送数据，遵循以下步骤

      1. 再声明一个[QueueHandle_t xQueueYYYInfo = NULL;]作为接收端 

      2. 将 [xQueueYYYInfo = xQueueCreate(5, sizeof(struct YYY));] 同时放置于 vWifiTask 与 信息传递终点的Task 记得添加头文件

3. 添加[int WifiAddXXXToQueue(struct TimeSinceBoot *time)]函数作为跨Task传参函数

4. 在头文件中定义 MQTT topic [#define XXX_TOPIC "xxx"]

5. 在 [static void MQTT_HandleTransactions(void)]函数中添加[MQTT_HandleXXXMessages()]函数

6. 补全[MQTT_HandleXXXMessages()]函数

7. 以上为传送教程，以下为接收教程

8. 定义 [void SubscribeHandlerXXXTopic(MessageData *msgData)]函数

9. 在 [static void mqtt_callback(struct mqtt_module *module_inst, int type, union mqtt_data *data)] 函数下的 [if (data->connected.result == MQTT_CONN_RESULT_ACCEPT)] 判断条件下添加 [mqtt_subscribe(module_inst, XXX_TOPIC, 2, SubscribeHandlerXXXTopic);]



## 0424 update

   奇怪的bug：SubscribeHandlerClockTopic function converting time error

   当输入时间为 12:00 AM - 02:46 AM 时，该function会将毫秒转换为错误的小时与分钟

## 0424 update

   static void manual_open(void){} needs to be finished

   static void timer_open(void){} needs to be finished


