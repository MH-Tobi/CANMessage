#include <Arduino.h>
#include <CANMessage.h>
#include <MCP2515.h>

// Create Instances of the CAN-Controller and 2 Messages
MCP2515 MCP2515Module;
CANMessage TimeCounter;
CANMessage MessageCounter;

// Definition of Chip-Select-Pin for the SPI-Communication
uint8_t CS_Pin = 17;

// Initialize the Message-Counter
uint16_t counter_up=0;
uint16_t counter_up_overflow=0;
uint16_t counter_down=4095;
uint16_t counter_down_overflow=0;

// Initialize booleans for easyer Message-Handling
bool TimeCounterFilled=false;
bool MessageCounterFilled=false;

void setup() {
  // Initialize Serial for Debug
  // Attention!!! When you started the Serial-Connection once you have to keep it open.
  // Otherwise on each try to print a message it will stuck till the timeout (default 1s) is expired.
  // This disrupts the process.
  Serial.begin(115200);

  // Declaration of the Board-LED for Error display
  pinMode(LED_BUILTIN, OUTPUT);

  delay(5000);

  // Showing startup of the board
  digitalWrite(LED_BUILTIN, HIGH);
  delay(250);
  digitalWrite(LED_BUILTIN, LOW);
  delay(250);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(250);
  digitalWrite(LED_BUILTIN, LOW);
  delay(250);

  // Set the ChipSelect-Pin for the SPI-Communication
  MCP2515Module.setSpiPins(CS_Pin);

  // Start the CAN bus at 500 kbps
  while (!MCP2515Module.init(500E3)) {
    // When initialization of CAN-Bus failed (check MCP2515Error.h)
    Serial.print("Init-Error: 0x");
    Serial.println(MCP2515Module.getLastMCPError(), HEX);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }

  // Create CAN-Messages
  TimeCounter.init((uint32_t) 0xA74BF55, 8, false, CANMESSAGE_FRAME_EXTENDED, CANMESSAGE_DIRECTION_TRANSMIT, MCP2515Module);
  MessageCounter.init((uint32_t) 0x1AB, 8, false, CANMESSAGE_FRAME_STANDARD, CANMESSAGE_DIRECTION_TRANSMIT, MCP2515Module);
}

void loop() {
  uint32_t time_ms = millis() % 1000;
  uint32_t time_s = millis()/1000;
  
  // Each 100ms (when Buffer of Message TimeCounter is not filled)
  if ((millis() % 100) == 0 && TimeCounterFilled == false)
  {
    // fill the DataBuffer of the Message with the calculated Data.
    TimeCounter.addDataByte((uint8_t)((((time_s >> 8) >> 8) >> 8) & 0xFF), 0);
    TimeCounter.addDataByte((uint8_t)(((time_s >> 8) >> 8) & 0xFF), 1);
    TimeCounter.addDataByte((uint8_t)((time_s >> 8) & 0xFF), 2);
    TimeCounter.addDataByte((uint8_t)(time_s & 0xFF), 3);

    TimeCounter.addDataByte((uint8_t)((((time_ms >> 8) >> 8) >> 8) & 0xFF), 4);
    TimeCounter.addDataByte((uint8_t)(((time_ms >> 8) >> 8) & 0xFF), 5);
    TimeCounter.addDataByte((uint8_t)((time_ms >> 8) & 0xFF), 6);
    TimeCounter.addDataByte((uint8_t)(time_ms & 0xFF), 7);

  } else if ((millis() % 100) != 0 && TimeCounterFilled == true)
  {
    TimeCounterFilled = false;
  }

  // Each 1000ms (when Buffer of Message MessageCounter is not filled)
  if ((millis() % 1000) == 0 && MessageCounterFilled == false)
  {
    // Calculate the Data
    if (counter_up > 4094)
    {
      counter_up = 0;
      counter_up_overflow++;
    } else {
      counter_up++;
    }
    
    if (counter_down < 1)
    {
      counter_down = 4095;
      counter_down_overflow++;
    } else {
      counter_down--;
    }

    // and fill the DataBuffer of the Message with the calculated Data.
    MessageCounter.addDataByte((uint8_t)((counter_up >> 8) & 0xFF), 0);
    MessageCounter.addDataByte((uint8_t)(counter_up & 0xFF), 1);
    MessageCounter.addDataByte((uint8_t)((counter_up_overflow >> 8) & 0xFF), 2);
    MessageCounter.addDataByte((uint8_t)(counter_up_overflow & 0xFF), 3);
    MessageCounter.addDataByte((uint8_t)((counter_down >> 8) & 0xFF), 4);
    MessageCounter.addDataByte((uint8_t)(counter_down & 0xFF), 5);
    MessageCounter.addDataByte((uint8_t)((counter_down_overflow >> 8) & 0xFF), 6);
    MessageCounter.addDataByte((uint8_t)(counter_down_overflow & 0xFF), 7);

  } else if ((millis() % 1000) != 0 && MessageCounterFilled == true)
  {
    MessageCounterFilled = false;
  }

  // Check if Message is Ready to send (Checks if all DataBuffer are filled with Data)
  if (TimeCounter.messageSendReady())
  {
    if (!TimeCounter.send())
    {
      Serial.print("TimeCounter send-Error: 0x");
      Serial.println(TimeCounter.getLastCanError(), HEX);
    } else {
      Serial.println("TimeCounter send.");
    }
  }

  // Check if Message is Ready to send (Checks if all DataBuffer are filled with Data)
  if (MessageCounter.messageSendReady())
  {
    if (!MessageCounter.send())
    {
      Serial.print("MessageCounter send-Error: 0x");
      Serial.println(MessageCounter.getLastCanError(), HEX);
    } else {
      Serial.println("MessageCounter send.");
    }
  }
}