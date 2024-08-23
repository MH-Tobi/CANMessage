#include <Arduino.h>
#include <CANMessage.h>
#include <MCP2515.h>

// Create Instances of the CAN-Controller and 2 Messages
MCP2515 MCP2515Module;
CANMessage TimeCounter;
CANMessage MessageCounter;

// Definition of Chip-Select-Pin for the SPI-Communication
uint8_t CS_Pin = 53;

// Definition of Interrupt-Pin for Interrupt-Handling
uint8_t IntPin = 3;

// Definition of Values they are received
uint32_t time_ms;
uint32_t time_s;
uint16_t counter_up;
uint16_t counter_up_overflow;
uint16_t counter_down;
uint16_t counter_down_overflow;


// Interrupt Routine (not really mature at the moment)
void onReceive(){

  // It is recommend that Interrupt service routines should generally be as short and fast as possible.
  // However, timing has not been given much importance so far.
  // So when you receive Messages in short time cycles it is possible that some Messages are dropped or the Serial print is lagging.
  // Actually, when you use the default setting in the CanMessageSend-Example it is also possible that TimeCounter-Messages are not recognized.
  // Especially when both Messages are send simultaniously.
  //
  // So keep in mind. THIS Interrupt-Routine is kept really simple.
  TimeCounter.checkReceive();
  MessageCounter.checkReceive();

  // Only nessecary when other Messages are also provided on the CAN-Bus they doesn't match with the defined Messages.
  MCP2515Module.releaseReceiveBuffer(0);
  MCP2515Module.releaseReceiveBuffer(1);
}


void setup() {
  // Initialize Serial for Debug
  // Attention!!! When you started the Serial-Connection once you have to keep it open.
  // Otherwise on each try to print a message it will stuck till the timeout (default 1s) is expired.
  // This disrupts the process.
  // In this Example not really important because we used an Interrupt to accept the Messages.
  // Otherwise you will generate Overflow-Errors in the RX-Buffer and Sending-Errors on the Transmission-Side.
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

  // To got Interrupts on the Interrupt-Pin you have to enable the Interrupts for Receive-Buffer.
  while (!MCP2515Module.changeInterruptSetting(true, 0) || !MCP2515Module.changeInterruptSetting(true, 1))
  {
    Serial.print("ChangeInterruptSetting-Error: 0x");
    Serial.println(MCP2515Module.getLastMCPError(), HEX);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);                     
    digitalWrite(LED_BUILTIN, LOW); 
    delay(250);                     
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);                     
    digitalWrite(LED_BUILTIN, LOW); 
    delay(250);
  }
  
  // Create CAN-Messages
  TimeCounter.init((uint32_t) 0xA74BF55, 8, false, CANMESSAGE_FRAME_EXTENDED, CANMESSAGE_DIRECTION_RECEIVE, MCP2515Module);
  MessageCounter.init((uint32_t) 0x1AB, 8, false, CANMESSAGE_FRAME_STANDARD, CANMESSAGE_DIRECTION_RECEIVE, MCP2515Module);

  // Prepare SPI-Communication for Interrupts
  SPI.usingInterrupt(digitalPinToInterrupt(IntPin));

  // Define the Interrupt
  attachInterrupt(digitalPinToInterrupt(IntPin), onReceive, LOW);
}

void loop() {
  
  // Check if Data is in the Message-Buffer of the Message TimeCounter
  if (TimeCounter.dataAvailable())
  {
    time_s = 0x00000000;
    time_ms = 0x00000000;

    // Get the Values from the Message-Buffer
    time_s = time_s << 8 | TimeCounter.getDataByte();
    time_s = time_s << 8 | TimeCounter.getDataByte();
    time_s = time_s << 8 | TimeCounter.getDataByte();
    time_s = time_s << 8 | TimeCounter.getDataByte();

    time_ms = time_ms << 8 | TimeCounter.getDataByte();
    time_ms = time_ms << 8 | TimeCounter.getDataByte();
    time_ms = time_ms << 8 | TimeCounter.getDataByte();
    time_ms = time_ms << 8 | TimeCounter.getDataByte();

    // and print it.
    Serial.print("Time [s]\t");
    Serial.print(time_s, DEC);
    Serial.print("\tTime [ms]\t");
    Serial.println(time_ms, DEC);
  }

  // Check if Data is in the Message-Buffer of the Message MessageCounter
  if (MessageCounter.dataAvailable())
  {
    counter_up = 0x0000;
    counter_up_overflow = 0x0000;
    counter_down = 0x0000;
    counter_down_overflow = 0x0000;

    // Get the Values from the Message-Buffer
    counter_up = counter_up << 8 | MessageCounter.getDataByte();
    counter_up = counter_up << 8 | MessageCounter.getDataByte();

    counter_up_overflow = counter_up_overflow << 8 | MessageCounter.getDataByte();
    counter_up_overflow = counter_up_overflow << 8 | MessageCounter.getDataByte();

    counter_down = counter_down << 8 | MessageCounter.getDataByte();
    counter_down = counter_down << 8 | MessageCounter.getDataByte();

    counter_down_overflow = counter_down_overflow << 8 | MessageCounter.getDataByte();
    counter_down_overflow = counter_down_overflow << 8 | MessageCounter.getDataByte();

    // and print it.
    Serial.print("Counter Up\t");
    Serial.print(counter_up, DEC);
    Serial.print("\tCounter Up Overflow\t");
    Serial.print(counter_up_overflow, DEC);
    Serial.print("\tCounter Down\t");
    Serial.print(counter_down, DEC);
    Serial.print("\tCounter Down Overflow\t");
    Serial.println(counter_down_overflow, DEC);
  }
}