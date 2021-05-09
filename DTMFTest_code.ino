/* Hamshield
 * Example: DTMF
 * This is a simple example to demonstrate how to use DTMF.
 *
 * Connect the HamShield to your Arduino. Screw the antenna 
 * into the HamShield RF jack. 
 * Connect the Arduino to wall power and then to your computer
 * via USB. After uploading this program to your Arduino, open
 * the Serial Monitor. Press the switch on the HamShield to 
 * begin setup. After setup is complete, type in a DTMF value
 * (0-9, A, B, C, D, *, #) and hit enter. The corresponding
 * DTMF tones will be transmitted. The sketch will also print
 * any received DTMF tones to the screen.
**/

#include <HamShield.h>

// create object for radio

#define LED_PIN 13
#include <Servo.h>

Servo myservo;

#define MIC_PIN 3
#define RESET_PIN A3
//#define SWITCH_PIN 2

HamShield radio(A0, A3, A1); // nCS, CLK, DAT

uint32_t freq;
int pos = 180;

void setup() {
  // NOTE: if not using PWM out, it should be held low to avoid tx noise
  pinMode(MIC_PIN, OUTPUT);
  digitalWrite(MIC_PIN, LOW);
  
  // prep the switch
//  pinMode(SWITCH_PIN, INPUT_PULLUP);
  
  // set up the reset control pin
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);
  

   myservo.attach(9); 
  
  // initialize serial communication
  Serial.begin(9600);
  Serial.println("press the switch to begin...");
  
//  while (digitalRead(SWITCH_PIN));
  
  // now we let the AU ot of reset
  digitalWrite(RESET_PIN, HIGH);
  delay(5); // wait for device to come up
  
  Serial.println("beginning radio setup");

  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(radio.testConnection() ? "HamShield connection successful" : "HamShield connection failed");

  // initialize device
  radio.initialize();

  Serial.println("setting default Radio configuration");

  Serial.println("setting squelch");

  radio.setSQHiThresh(-10);
  radio.setSQLoThresh(-30);
  Serial.print("sq hi: ");
  Serial.println(radio.getSQHiThresh());
  Serial.print("sq lo: ");
  Serial.println(radio.getSQLoThresh());
  radio.setSQOn();
  //radio.setSQOff();

  Serial.println("setting frequency to: ");
  freq = 432650;
  radio.frequency(freq);
  Serial.print(radio.getFrequency());
  Serial.println("kHz");
  
  // set RX volume to minimum to reduce false positives on DTMF rx
  radio.setVolume1(6);
  radio.setVolume2(0);
  
  // set to receive
  radio.setModeReceive();
  
  radio.setRfPower(0);
    
  // configure Arduino LED for
  pinMode(LED_PIN, OUTPUT);

  // set up DTMF
  radio.enableDTMFReceive();
  
  /* DTMF timing settings are optional.
   * These times are set to default values when the device is started.
   * You may want to change them if you're DTMF receiver isn't detecting
   * codes from the HamShield (or vice versa).
   */
  radio.setDTMFDetectTime(24); // time to detect a DTMF code, units are 2.5ms
  radio.setDTMFIdleTime(50); // time between transmitted DTMF codes, units are 2.5ms
  radio.setDTMFTxTime(0); // duration of transmitted DTMF codes, units are 2.5ms
  
  Serial.println("ready");
}

char rx_dtmf_buf[255];
int  rx_dtmf_idx = 0;
void loop() {
  
  // look for tone
  if (radio.getDTMFSample() != 0) {
    uint16_t code = radio.getDTMFCode();
    if (code == 1) {
      //digitalWrite(OUTPUT_PIN, HIGH);
      pos = pos-90;
      myservo.write(pos); 
      Serial.print("It works!");
      delay(1000);
      //digitalWrite(OUTPUT_PIN, LOW);
    }
      if (code == 2) {
      //digitalWrite(OUTPUT_PIN, HIGH);
      pos = pos+90;
      myservo.write(pos); 
      Serial.print("It works!");
      delay(1000);
      //digitalWrite(OUTPUT_PIN, LOW);
    }
    rx_dtmf_buf[rx_dtmf_idx++] = code2char(code);

    // reset after this tone
    int j = 0;
    while (j < 4) {
      if (radio.getDTMFSample() == 0) {
        j++;
      }
      delay(10);
    }
  } else if (rx_dtmf_idx > 0) {
  }
  
  // Is it time to send tone?
}

uint8_t char2code(char c) {
    uint8_t code;
    if (c == '#') {
      code = 0xF;
    } else if (c=='*') {
      code = 0xE;
    } else if (c >= 'A' && c <= 'D') {
      code = c - 'A' + 0xA;
    } else if (c >= '0' && c <= '9') {
      code = c - '0';
    } else {
      // invalid code, skip it
      code = 255;
    }

    return code;
}

char code2char(uint16_t code) {
  char c;
  if (code < 10) {
    c = '0' + code;
  } else if (code < 0xE) {
    c = 'A' + code - 10;
  } else if (code == 0xE) {
    c = '*';
  } else if (code == 0xF) {
    c = '#';
  } else {
    c = '?'; // invalid code
  }
 
  return c;
}
