# CANMessage
Library to manage CAN-Messages.

Not really tested yet.

## Dependency

This Library was build for the [MCP2515-Library](https://github.com/MH-Tobi/MCP2515).
So it is nessecary to use this Library.


## Setup

1. Import Librarys

```c++
#include <Arduino.h>
#include <MCP2515.h>
#include <CANMessage.h>
```

2. Create an Instance:

```c++
CANMessage Message;
```

3. Initialize the Message

```c++
Message.init((uint32_t) 0x123, 4, false, CANMESSAGE_FRAME_STANDARD, CANMESSAGE_DIRECTION_TRANSMIT, MCP2515Module);
```
CAN-Message with:
- ID = 0x123
- DLC = 4
- RTR-Message = false
- Frame = Standard
- Direction = Transmit
- CAN-Controller = MCP2515-Instance

## Usage

After initialization of a transmition Messages like above you can fill the Message with Data like:

```c++
Message.addDataByte(0x2F, 0);
Message.addDataByte(0xFF, 1);
Message.addDataByte(0x11, 2);
Message.addDataByte(0x43, 3);
```

After you filled the Message with 4 Bytes of Data (like defined during the Initialisation of the Message), you can send the Message like:
```c++
if (Message.messageSendReady())
{
    Message.send();
}
```


For receiving Messages you have also initialise a Message similar to the Transmit-Message:

```c++
Message.init((uint32_t) 0x123, 4, false, CANMESSAGE_FRAME_STANDARD, CANMESSAGE_DIRECTION_RECEIVE, MCP2515Module);
```

To check if the Message is received and also get the Data you can use:
```c++
Message.checkReceive();
```

If now Data is available you can collect the Data with:
```c++
if (TimeCounter.dataAvailable())
{
    uint8_t Data1;
    uint8_t Data2;
    uint8_t Data3;
    uint8_t Data4;

    Data1 = Message.getDataByte();
    Data2 = Message.getDataByte();
    Data3 = Message.getDataByte();
    Data4 = Message.getDataByte();

}
```

## Examples
See [examples](examples) folder.

## API
See [API.md](API.md).
