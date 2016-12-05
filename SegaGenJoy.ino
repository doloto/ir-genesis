/*
 * IRremote: IRrecvDemo - demonstrates receiving IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

#include <IRremote.h>

int RECV_PIN = 2;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  pinMode(13, OUTPUT);
}

#define SG_HEADER_LEN 38
#define SG_PAIRS_NUM 17

//in USECs
#define SG_HEADER_1 2250
#define SG_HEADER_2 700
#define SG_HIGH_1 300
#define SG_HIGH_2 250
#define SG_LOW_2  700

#define SG_TOLERANCE 100
#define SG_MATCH(x, y) (abs(y - x) <= SG_TOLERANCE)

#define SG_A 32639
#define SG_B 4294950847
#define SG_C 4294959071

#define SG_X 4294959101
#define SG_Y 4294950907
#define SG_Z 32759

#define SG_UP 30719
#define SG_DN 4294949887
#define SG_LF 4294958591
#define SG_RH 32511

#define SG_MN 28671
#define SG_ST 32751

unsigned long int decode_sega_gen(decode_results *results) {
  long raw_usec[SG_HEADER_LEN];
  unsigned long int scan_code = 0;
  for (int i = 1;  i < results->rawlen;  i++) {
      raw_usec[i-1] = results->rawbuf[i] * USECPERTICK;
  }

  // Check len
  if (results->rawlen != SG_HEADER_LEN) {
      Serial.println("Size");
      return 0;  
  }
  
  // Check header   
  if (!SG_MATCH(raw_usec[0], SG_HEADER_1)) {      
      Serial.print("H1 ");
      Serial.println(abs(SG_HEADER_1 - raw_usec[0]));
      return 0;
  }
  if (!SG_MATCH(raw_usec[1], SG_HEADER_2)) {      
      Serial.print("H2 ");
      Serial.println(abs(SG_HEADER_2 - raw_usec[1]));
      return 0;
  }
  Serial.print("scancode ");
  for (int i = 2;  i < SG_PAIRS_NUM*2;  i=i+2) {
      long p1 = raw_usec[i];
      long p2 = raw_usec[i+1];
      
      if (SG_MATCH(p1, SG_HIGH_1) && SG_MATCH(p2, SG_HIGH_2)) {
          scan_code |= 1<<((i/2)-1);
          Serial.print("1 ");
          continue;
      }
      if (SG_MATCH(p1, SG_HIGH_1) && SG_MATCH(p2, SG_LOW_2)) {
          Serial.print("0 ");
          continue;
      }   
      Serial.print("pair ");
      Serial.print(p1);
      Serial.print(" ");
      Serial.println(p2);
      return 0;
  }
  Serial.println(" OK");
  if (scan_code == 4294967295)
      return 0;
  
  return scan_code;
}

void loop() {
  unsigned long res = 0;
  if (irrecv.decode(&results)) {
    Serial.print("Scancode: ");
    res = decode_sega_gen(&results);        
    switch (res) {
        case SG_A:
            Serial.println("A");
            break;
        case SG_B:
            Serial.println("B");
            break;           
        case SG_C:
            Serial.println("C");
            break;             
        default:
            Serial.println(res);     
    }
    irrecv.resume(); // Receive the next value
  }
  delay(100);  
}
