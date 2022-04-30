/* 
 * ArduinoProMini发射器
 * A0~3 -> 模拟输入
 * 2~5  -> 通道正反开关
 * 
 * NRF24L01 | Arduino
 * CE    -> 7
 * CSN   -> 8
 * MOSI  -> 11
 * MISO  -> 12
 * SCK   -> 13
 * IRQ   -> 无连接
 * VCC   -> 小于3.6V
 * GND   -> GND
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
const uint64_t pipeOut = 0x141450196; //与接收器中相同的地址进行通信
RF24 radio(7, 8);                     // SPI通信，引脚对应关系：CE ->7,CSN ->8
struct Signal
{
  byte joystick1_x;
  byte joystick1_y;
  byte joystick2_x;
  byte joystick2_y;
};
Signal data;

void ResetData()
{
  data.joystick1_x = 127;
  data.joystick1_y = 127;
  data.joystick2_x = 127;
  data.joystick2_y = 127;
}

void setup()
{
  radio.begin();
  //radio.setAutoAck(false);
  //radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  //radio.setChannel(108);
  radio.openWritingPipe(pipeOut); //pipeOut通信地址
  radio.stopListening();          //发射模式
  ResetData();                    //初始化6个通道值
  Serial.begin(115200);

  pinMode(2, INPUT); //正反通道开关为数字输入
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
}

// 将ADC获取的0~1023转换到0~255
int chValue(int val, int lower, int middle, int upper, bool reverse)
{
  val = constrain(val, lower, upper); //将val限制在lower~upper范围内
  if (val < middle)
    val = map(val, lower, middle, 0, 128);
  else
    val = map(val, middle, upper, 128, 255);
  //return (reverse ? 255 - val : val);
  return (0 ? 255 - val : val);
}

void loop()
{
  //将数据通过串口输出
  /*Serial.print("\t");
  Serial.print(analogRead(A0));
  Serial.print("\t");
  Serial.print(analogRead(A1));
  Serial.print("\t");
  Serial.print(analogRead(A2));
  Serial.print("\t");
  Serial.println(analogRead(A3));*/

  // 需要对摇杆的最值、中值进行设置
  data.joystick1_x = chValue(analogRead(A0), 4, 510, 1009, digitalRead(2));
  data.joystick1_y = chValue(analogRead(A1), 0, 528, 1022, digitalRead(3));
  data.joystick2_x = chValue(analogRead(A2), 0, 508, 1022, digitalRead(4));
  data.joystick2_y = chValue(analogRead(A3), 0, 500, 1022, digitalRead(5));

  //将数据发送出去
  radio.write(&data, sizeof(Signal));

  /*Serial.print("\t");
  Serial.print(data.joystick1_x);
  Serial.print("\t");
  Serial.print(data.joystick1_y);
  Serial.print("\t");
  Serial.print(data.joystick2_x);
  Serial.print("\t");
  Serial.println(data.joystick2_y);*/
}
