// UART SPEED
const unsigned long UART_SPEED = 19200;

// MESSAGE
const unsigned short MESSAGE_LENGTH = 32;
const unsigned short MESSAGE_LEN_WITHOUT_CHECKSUM = MESSAGE_LENGTH - (unsigned short)2;
const String MESSAGE_HEAD = ":FRONT";
const String MESSAGE_MID = "MIDDLE";
const String FAKE_MESSAGE = ":FRONT0MIDDLE8888888888888888858";

// DELAY
const unsigned short SEND_CYCLE_MS = 100;
const unsigned long REST_TIME = (unsigned long)1000 * (unsigned long)60 * (unsigned long)2 + (unsigned long)7;
unsigned long timeCounter = 0;

// MAXTIMES
const unsigned long MAX_SEND_TIMES = 1000;
const unsigned long MAX_SEND_TIMES_LIMIT = 9999999;
unsigned long sendCounter = 0;
unsigned long sentCounter = 0;

//LED
const unsigned char LED_RED = 8;
const unsigned char LED_GREEN = 6;

void onoffLedGreen() {
  static bool LEDONOFF_GREED = false;
  digitalWrite(LED_GREEN, LEDONOFF_GREED);
  LEDONOFF_GREED = !LEDONOFF_GREED;
}

void onoffLedRed() {
  static bool LEDONOFF_RED = true;
  digitalWrite(LED_RED, LEDONOFF_RED);
  LEDONOFF_RED = !LEDONOFF_RED;
}

void setup()
{
  Serial.begin(UART_SPEED);

  pinMode(LED_RED, OUTPUT);
  digitalWrite(LED_RED, false);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, true);

  delay(1000);
}

void doSendLoop() {
  sendCounter++;

  // MAKE MESSAGE
  String message = MESSAGE_HEAD;
  message  += String(sendCounter);
  message  += MESSAGE_MID;
  unsigned short message_length_now = message.length();

  // CALCULATE CHECKSUM (FORMER PART)
  unsigned char checksum = 0xFE;
  for (unsigned short counti = 0;  counti < message_length_now ; counti++) {
    checksum += message.charAt(counti);
  }

  // ADD RANDOM AND CALCULATE CHECKSUM (FOLLOWING PART)
  unsigned char rand_value = random(1, 9);
  String rand_value_str = String(rand_value);
  char rand_value_chr = rand_value_str.charAt(0);
  while (message_length_now < MESSAGE_LEN_WITHOUT_CHECKSUM) {
    message += rand_value_str;
    checksum += rand_value_chr;
    message_length_now++;
  }

  // ADD CHECKSUM TO THE MESSAGE
  if (checksum < 0x10) {
    message += String('0');
  }
  message += String(checksum, HEX);

  if (message.length() == MESSAGE_LENGTH) {
    // SEND METHOD USING WRITE
    {
      // unsigned short len = message.length();
      // for (unsigned short counti = 0; counti < len; counti++) {
      //   Serial.write(message.charAt(counti));
      //   Serial.flush();
      //   delayMicroseconds(0);
      // }
    }

    //  SEND METHOD USING PRINT
    {
      //    int a = message.length();
      //    int b = FAKE_MESSAGE.length();
      //    int c = a-b;
      //    String d = String(c);
      //    Serial.print(" "+ d +" ");
      //    Serial.print(FAKE_MESSAGE);
      Serial.print(message);
      Serial.flush();
    }

    sentCounter++;
  } else {
    sendCounter--;
  }
}

void doWhenTaskOver() {
  digitalWrite(LED_GREEN, true);
  digitalWrite(LED_RED, true);
  delay(2000);
  //Serial.print("[Sent>"+ String(sentCounter) +"/Send>"+ String(sendCounter) +"]");
  // dont use ":" mark
  digitalWrite(LED_GREEN, false);
  digitalWrite(LED_RED, false);
  delay(2000);
  //      Serial.print("[Sent>"+ String(sentCounter) +"/Send>"+ String(sendCounter) +"]");
}

void loop()
{
  if (sentCounter < MAX_SEND_TIMES) {
    unsigned long time_now = millis();
    if ((unsigned long)(time_now  - timeCounter) > SEND_CYCLE_MS) {
      timeCounter = time_now;
      onoffLedGreen();
      onoffLedRed();
      doSendLoop();
    }
  } else {
    // TASK OVER
    for (;;) {
      unsigned long time_now = millis();
      //      Serial.print("[" + String(time_now) + "/" + String(timeCounter) + "/" + String(REST_TIME) + "]");

      if (((unsigned long)(time_now  - timeCounter) > REST_TIME) && ((unsigned long)(sendCounter + sentCounter) < MAX_SEND_TIMES_LIMIT)) {
        timeCounter = time_now;
        sentCounter = 0;
        break;
      } else {
        doWhenTaskOver();
      }
    }
  }
}
