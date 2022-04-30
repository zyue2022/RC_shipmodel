/* ArduinoProMini-6通道接收机 
 * PWM输出 -> 引脚2~5
 * 10 -> 信号灯
 * 
 * NRF24L01 | Arduino
 * CE    -> 7
 * CSN   -> 8
 * MOSI  -> 11
 * MISO  -> 12
 * SCK   -> 13
 * IRQ   -> 无连接
 * VCC   -> 小于3.6V
 * GND   -> GND */
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
int ch_1 = 0, ch_2 = 0, ch_3 = 0, ch_4 = 0;
Servo ch1;
Servo ch2;
Servo ch3;
Servo ch4;
struct Signal
{
    byte joystick1_x;
    byte joystick1_y;
    byte joystick2_x;
    byte joystick2_y;
};
Signal data;
const uint64_t pipeIn = 0x141450196; //对频地址，与发射端地址相同
RF24 radio(3, 2);

// 信号丢失时，关闭操作
void ResetData()
{
    data.joystick1_x = 127;
    data.joystick1_y = 127;
    data.joystick2_x = 127;
    data.joystick2_y = 127;
}
void setup()
{
    //设置PWM信号输出引脚
    ch1.attach(4);
    ch2.attach(5);
    ch3.attach(10);
    ch4.attach(6);
    //配置NRF24L01模块
    ResetData();
    radio.begin();
    //radio.setAutoAck(false);
    //radio.setDataRate(RF24_250KBPS);
    radio.setPALevel(RF24_PA_LOW);
    //radio.setChannel(108);
    radio.openReadingPipe(1, pipeIn); //与发射端地址相同
    radio.startListening();           //接收模式
    pinMode(9, OUTPUT);              //LED输出
    digitalWrite(9, HIGH);
    Serial.begin(115200);
}
unsigned long lastRecvTime = 0;
void recvData()
{
    while (radio.available())
    {
        radio.read(&data, sizeof(Signal)); //接收数据
        lastRecvTime = millis();           //当前时间ms
    }
}
void loop()
{
    recvData();
    unsigned long now = millis();
    if (now - lastRecvTime > 1000)
    {
        ResetData(); //两次接收超过1s表示失去信号，输出reset值
        //Serial.print("无信号");
        digitalWrite(9, LOW);
    }
    else
    {
        //Serial.print("有信号");
        digitalWrite(9, HIGH);
    }

    /*Serial.print("\t");
    Serial.print(data.joystick1_x);
    Serial.print("\t");
    Serial.print(data.joystick1_y);
    Serial.print("\t");
    Serial.print(data.joystick2_x);
    Serial.print("\t");
    Serial.println(data.joystick2_y);*/

    ch_1 = map(data.joystick1_x, 0, 255, 1000, 2000); // 将0~255映射到1000~2000，即1ms~2ms/20ms的PWM输出
    ch_2 = map(data.joystick1_y, 0, 255, 600, 2400);
    ch_3 = map(data.joystick2_x, 0, 255, 1000, 2000);
    ch_4 = map(data.joystick2_y, 0, 255, 1000, 2000);

    /*Serial.print("\t");
    Serial.print(ch_1);
    Serial.print("\t");
    Serial.print(ch_2);
    Serial.print("\t");
    Serial.print(ch_3);
    Serial.print("\t");
    Serial.println(ch_4);
    Serial.print("\n");*/

    // 将PWM信号输出至引脚
    ch1.writeMicroseconds(ch_1);
    ch2.writeMicroseconds(ch_2);
    ch3.writeMicroseconds(ch_3);
    ch4.writeMicroseconds(ch_4);
}
