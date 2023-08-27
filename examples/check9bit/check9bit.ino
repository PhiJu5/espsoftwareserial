#include <Arduino.h>
#include <SoftwareSerial.h>

#define SPEED         9600
#define BUFFERED      64
#define XBIT9
#define DATA_PER_LINE 20

int check1[BUFFERED];
int check2[BUFFERED];

EspSoftwareSerial::UART ex1;
EspSoftwareSerial::UART ex2;

void setup() {
  Serial.begin(115200);
  delay(3000);

#ifdef XBIT9
  Serial.print("BIT9:");
#endif
  Serial.print("size:");
  Serial.println(BUFFERED);

#ifdef XBIT9
  ex1.begin(SPEED, SWSERIAL_9O2, 18, 18, false, BUFFERED);
  ex2.begin(SPEED, SWSERIAL_9O2, 19, 19, false, BUFFERED);
#else
  ex1.begin(SPEED, SWSERIAL_8O2, 18, 18, false, BUFFERED);
  ex2.begin(SPEED, SWSERIAL_8O2, 19, 19, false, BUFFERED);
#endif
  if (!ex1) {
    Serial.println("ex1 error");
    while (1);
  }
  if (!ex2) {
    Serial.println("ex2 error");
    while (1);
  }
  Serial.println("ExSerial test started");
}

void loop() {
  static int data1 = 0x40;
#ifdef XBIT9
  static int data2 = 0xff;
#else
  static int data2 = 0x80;
#endif
  static uint8_t reverse = 0;
  uint8_t cnt;
  uint8_t numb;
  uint8_t wr;
  int val;

  cnt = 0;
  if (reverse == 0) {
    ex2.enableTx(true);
    ex1.enableTx(false);
    Serial.println("Write SW TX2");
    numb = 0;
    for (uint8_t i = 0; i  < BUFFERED; i++) {
      data2++;
#ifdef XBIT9
      if (data2 > 0x1FF)
        data2 = 0;
#else
      if (data2 > 0xFF)
        data2 = 0;
#endif
      check2[i] = data2;
      Serial.print(":");
      Serial.print(data2, HEX);
      wr = (uint8_t)(data2);
      if (data2 & 0x100) {
        ex2.write9bit(wr);
      } else {
        ex2.write(wr);
      }
      if (++numb >= DATA_PER_LINE) {
        Serial.println("");
        numb = 0;
      }
    }
    ex2.flush();
    Serial.println("");
    Serial.println("Read  Sw TX1");
    numb = 0;
    cnt = 0;
    while (cnt < BUFFERED) {
      if (ex1.available()) {
        val = ex1.read();
        Serial.print(":");
        Serial.print(val, HEX);
        if (val != check2[cnt]) {
          Serial.print(":check2:");
          Serial.print(check2[cnt], HEX);
          Serial.println("Error");
          while (1);
        }
        cnt++;
        if (++numb >= DATA_PER_LINE) {
          Serial.println("");
          numb = 0;
        }
      }
    }
    Serial.print(":count=");
    Serial.println(cnt);
  } else {
    ex1.enableTx(true);
    ex2.enableTx(false);
    Serial.println("Write Sw TX1");
    numb = 0;
    for (uint8_t i = 0; i  < BUFFERED; i++) {
      ++data1;
#ifdef XBIT9
      if (data1 > 0x1FF)
        data1 = 0;
#else
      if (data1 > 0xFF)
        data1 = 0;
#endif
      check1[i] = data1;
      Serial.print(":");
      Serial.print(data1, HEX);
      wr = (uint8_t)(data1);
      if (data1 & 0x100) {
        ex1.write9bit(wr);
      } else {
        ex1.write(wr);
      }
      if (++numb >= DATA_PER_LINE) {
        Serial.println("");
        numb = 0;
      }
    }
    ex1.flush();
    Serial.println("");
    Serial.println("Read Sw  TX2");
    numb = 0;
    cnt = 0;
    while (cnt < BUFFERED) {
      if (ex2.available()) {
        val = ex2.read();
        if (val != check1[cnt]) {
          Serial.print(":check1:");
          Serial.print(check1[cnt], HEX);
          Serial.println("Error");
          while (1);
        }
        Serial.print(":");
        Serial.print(val, HEX);
        cnt++;
        if (++numb >= DATA_PER_LINE) {
          Serial.println("");
          numb = 0;
        }
      }
    }
    Serial.print(":count=");
    Serial.println(cnt);
  }
  reverse = ~reverse;
  Serial.println("Reverse");

}
