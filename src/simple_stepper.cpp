/*
 * 简化版ULN2003步进电机控制
 * 适合初学者理解基本原理
 *
 * 注意：这是备份文件，不会被编译
 * 如果要使用此版本，请将main.cpp重命名，然后将此文件重命名为main.cpp
 */

/*
#include <Arduino.h>

// ULN2003 步进电机控制引脚
#define IN1 1
#define IN2 12
#define IN3 18
#define IN4 19

// 4步全步模式序列 (更简单但扭矩更大)
int steps[4][4] = {
  {1, 0, 0, 1},
  {1, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 1}
};

void setup() {
  Serial.begin(115200);

  // 设置引脚为输出
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.println("简化版步进电机控制器启动");
}

void loop() {
  // 正转10步
  Serial.println("正转10步");
  for(int i = 0; i < 10; i++) {
    for(int step = 0; step < 4; step++) {
      digitalWrite(IN1, steps[step][0]);
      digitalWrite(IN2, steps[step][1]);
      digitalWrite(IN3, steps[step][2]);
      digitalWrite(IN4, steps[step][3]);
      delay(5); // 5毫秒延时
    }
  }

  delay(1000); // 暂停1秒

  // 反转10步
  Serial.println("反转10步");
  for(int i = 0; i < 10; i++) {
    for(int step = 3; step >= 0; step--) {
      digitalWrite(IN1, steps[step][0]);
      digitalWrite(IN2, steps[step][1]);
      digitalWrite(IN3, steps[step][2]);
      digitalWrite(IN4, steps[step][3]);
      delay(5); // 5毫秒延时
    }
  }

  // 停止电机
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  delay(2000); // 暂停2秒后重复
}
*/
