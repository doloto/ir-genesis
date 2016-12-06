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
}

// Format of message
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

// Scancodes for P1
#define SG_A 0x7F7F
#define SG_B 0xFFFFBFBF
#define SG_C 0xFFFFDFDF

#define SG_X 0xFFFFDFFD
#define SG_Y 0xFFFFBFFB
#define SG_Z 0x7FF7

#define SG_UP 0x77FF
#define SG_DN 0xFFFFBBFF
#define SG_LF 0xFFFFDDFF
#define SG_RH 0x7EFF

#define SG_MN 0x6FFF
#define SG_ST 0x7FEF

#define SG_END 0xFFFFFFFF

#ifdef DEBUG_PRINT
#define DEB_PR(x)  (Serial.print(x))
#define DEB_PRL(x) (Serial.println(x))
#else
#define DEB_PR(x)
#define DEB_PRL(x)
#endif

// Returns scan code of key or 0, if not recognized
unsigned long int decode_sega_gen(decode_results *results) {
  long raw_usec[SG_HEADER_LEN];
  unsigned long int scan_code = 0;
  for (int i = 1;  i < results->rawlen;  i++) {
      raw_usec[i-1] = results->rawbuf[i] * USECPERTICK;
  }

  // Check len
  if (results->rawlen != SG_HEADER_LEN) {
      DEB_PRL("Size");
      return 0;  
  }
  
  // Check header   
  if (!SG_MATCH(raw_usec[0], SG_HEADER_1)) {      
      DEB_PR("H1 ");
      DEB_PRL(abs(SG_HEADER_1 - raw_usec[0]));
      return 0;
  }
  if (!SG_MATCH(raw_usec[1], SG_HEADER_2)) {      
      DEB_PR("H2 ");
      DEB_PRL(abs(SG_HEADER_2 - raw_usec[1]));
      return 0;
  }
  DEB_PR("scancode ");
  for (int i = 2;  i < SG_PAIRS_NUM*2;  i=i+2) {
      long p1 = raw_usec[i];
      long p2 = raw_usec[i+1];
      
      if (SG_MATCH(p1, SG_HIGH_1) && SG_MATCH(p2, SG_HIGH_2)) {
          scan_code |= 1<<((i/2)-1);
          DEB_PR("1 ");
          continue;
      }
      if (SG_MATCH(p1, SG_HIGH_1) && SG_MATCH(p2, SG_LOW_2)) {
          DEB_PR("0 ");
          continue;
      }   
      DEB_PR("pair ");
      DEB_PR(p1);
      DEB_PR(" ");
      DEB_PRL(p2);
      return 0;
  }
  DEB_PRL(" OK");

  return scan_code;
}

void print_scancode(unsigned long scancode) {
    switch (scancode) {
        case SG_A:
            Serial.print("A ");
            break;
        case SG_B:
            Serial.print("B ");
            break;           
        case SG_C:
            Serial.print("C ");
            break;
        case SG_X:
            Serial.print("X ");
            break;
        case SG_Y:
            Serial.print("Y ");
            break;           
        case SG_Z:
            Serial.print("Z ");
            break;               
        case SG_UP:
            Serial.print("UP ");
            break;
        case SG_DN:
            Serial.print("DOWN ");
            break;           
        case SG_LF:
            Serial.print("LEFT ");
            break;               
        case SG_RH:
            Serial.print("RIGHT ");
            break;               
        case SG_MN:
            Serial.print("MENU ");
            break;               
        case SG_ST:
            Serial.print("START ");
            break; 
        case SG_END:
            Serial.println("KEY RELEASE");            
            break;                              
        default:
            Serial.println(scancode);     
    }    
    return;
}

void loop() {
  unsigned long res = 0;
  if (irrecv.decode(&results)) {    
    res = decode_sega_gen(&results);
    print_scancode(res);           
    irrecv.resume(); // Receive the next value
  }
  delay(100);  
}
