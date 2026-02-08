/*
 * Geared Hand Servo Controller
 *
 * Copyright (c) 2025 Karl Kwon
 * All rights reserved.
 *
 * Controls 8 servo motors (5 finger + 3 center/wrist) for a robotic hand
 * via serial command protocol.
 *
 * Author: Karl Kwon (mrthinks@gmail.com)
 */

#include <Servo.h>
#include "hand_comm.h"

Servo center_servo_4;  // create servo object to control a servo
Servo center_servo_5;  // create servo object to control a servo
Servo center_servo_6;  // create servo object to control a servo

Servo *servos[5];
int servoDeg[5] = {0, 0, 0, 0, 0};
int serboPinNum[5] = {7, 8, 9, 10, 11};

int pos = 0;    // variable to store the servo position
unsigned char receivedData = 0;
unsigned long pasttime = 0;


void servo_init() {
  for(int i=0; i<5; ++i) {
    servos[i] = new Servo();
    servos[i]->attach(serboPinNum[i], 544, 2500);
    servos[i]->write(0);
  }
}

void PacketCB(PACKET_TYPE type, int id) {
  if(type == PACKET_SINGLE) {
    Serial.print("Single Packet: ID=");
    Serial.print(id + 1);
    Serial.print(", DEG=");
    Serial.println(degs[id]);
  } else if(type == PACKET_ALL) {
    Serial.print("All Packet: ");
    for(int i=0; i<5; i++) {
      Serial.print(",");
      Serial.print(i + 1);
      Serial.print("=");
      Serial.print(degs[i]);
      
      // servos[i]->write(degs[i]);
    }
    Serial.println("");
  }
}

void setup() {
  Serial.begin(115200);

  center_servo_4.attach(4);
  center_servo_5.attach(5);
  center_servo_6.attach(6);

  center_servo_4.write(0);              // tell servo to go to position in variable 'pos'
  center_servo_5.write(0);              // tell servo to go to position in variable 'pos'
  center_servo_6.write(0);              // tell servo to go to position in variable 'pos'

  pasttime = millis();

  packet_mgr_init(PacketCB);
  servo_init();
}

void loop() {
  unsigned long curtime = millis();

  if(Serial.available() > 0) {
    receivedData = Serial.read();
    Serial.write(receivedData);

    packet_mgr_parse(receivedData);
  }

  {
    int cnt = 0;
    int move_servo_cnt = 0;

    for(int i=0; i<5; ++i)
      if(degs[i] != degs_past[i])
        cnt++;

    if(cnt != 0 && curtime > (pasttime + 20)) {
      for(int i=0; i<5; ++i) {
        if(degs[i] >= (degs_past[i] + 10) ) {
          degs_past[i] += 10;
          servos[i]->write(degs_past[i]);
          move_servo_cnt++;
        } else if(degs[i] <= (degs_past[i] - 10) ) {
          degs_past[i] -= 10;
          servos[i]->write(degs_past[i]);
          move_servo_cnt++;
        }

        if(move_servo_cnt >= 3)
          break;
      }
      pasttime = curtime;
    }
  }

}
