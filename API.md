# API

## Constructor

```c++
CANMessage Message;
```


## Initialisation

```c++
Message.init(uint32_t id, uint8_t dlc, bool rtr, uint8_t frame, uint8_t direction, MCP2515 controller);
```
- `id` - Message-ID
    - for Standard-Frame max. 11 Bit
    - for Extended-Frame max. 29 Bit
- `dlc` - Data Length Code of the Message (max. 8)
- `rtr` - True if the Message is a Remote-Transmission-Request, False when not
- `frame` - Kind of Frame
    - 0 = Standard-Frame
    - 1 = Extended-Frame
- `direction` - Direction of the Message
    - 0 = Receive
    - 1 = Transmit
- `controller` - CAN-Controller Instance of the Class MCP2515
- Returns on success `true`, on any failure `false`


## Error-Handling

See also [Error.md](Error.md).

### Get the last CAN-Error

```c++
Message.getLastCanError();
```
- Returns the last CAN-Error.
- The last CAN-Error will always been reset at the beginning of a Method.
- When no Error occured it will show `0x0000`



## For Transmission

### Add DataByte

Adding Data to the given Buffer of the Message.

```c++
Message.addDataByte(uint8_t Data, uint8_t BufferNumber)
```
- `Data` - Data to be add to the given Buffer
- `BufferNumber` - Number of the Buffer to be filled with Data
- Returns on success `true`, on any failure `false`


### Release Buffer

Release the given Buffer, so new Data can be inserted to it.

```c++
Message.releaseBuffer(uint8_t BufferNumber = 0);
```
- `BufferNumber` - Number of the Buffer to be released
- Returns on success `true`, on any failure `false`


### Check for Remote-Transmission-Request

It is possible that for the Transmission-Message a Remote-Transmission-Request was received.
With this function you can get these Information.

```c++
Message.checkForRTR();
```
- `true` if a Remote-Transmission-Request was received, `false` when not


### Check if Ready to send

Checks if the Message is already filled with Data so a Transmission could be initiated.

```c++
Message.messageSendReady();
```
- `true` if the Message is Ready, `false` when not


### Send Message

Initiates the Message-Transmission.

```c++
Message.send();
```
- `true` if Transmission was successfull, `false` when not (Check getLastCanError() for further Information)



## For Reception

### Check for Receive

Checks if the Message was received. If the Message was received the Databuffer of the Message would be automatically filled with the Data.

```c++
Message.checkReceive();
```
- `true` if Message was received, `false` when not


### Check if Data is available

Check if Data is available in the Databuffer of the Message.

```c++
Message.dataAvailable();
```
- `true` if Data is available, `false` when not


### Get Data

Returns the Data of  the highest available DataByte from the Buffer.
Check if Data is available with the dataAvailable()-Method.

```c++
Message.getDataByte();
```
- Returns the Data of the Buffer



## Message Properties

### Get Message-ID

```c++
Message.getID();
```
- Returns the Message-ID


### Get Message-Data-Length-Code
```c++
Message.getDLC();
```
- Returns the Message-DLC


### Get Remote-Transmission-Request property
```c++
Message.getRTR();
```
- Returns `true` if Message is a Remote-Transmission-Request


### Get kind of Frame
```c++
Message.getFrame();
```
- Returns the kind of Frame
    - 0 = Standard Frame
    - 1 = Extended Frame


### Get Direction of the Message
```c++
Message.getDirection();
```
- Returns the Direction of the Message
    - 0 = Reception-Message
    - 1 = Transmission-Message
