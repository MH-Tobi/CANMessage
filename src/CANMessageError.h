#ifndef CANMESSAGEERROR_H
#define CANMESSAGEERROR_H

#define ERROR_CAN_NOT_INITIALIZED                       0x1000      // Occurs when a Method is called, but the CAN-Message is still not initialized.
#define ERROR_CAN_VALUE_OUTA_RANGE                      0x2000      // Occurs when given Value is not in the defined range.
#define ERROR_CAN_METHOD_NOT_ALLOWED                    0x3000      // Occurs when a Method is called which is not allowed for the specified Message-Direction.
#define ERROR_CAN_BUFFER_FILLED                         0x4000      // Occurs when the choosen Buffer is already filled.
#define ERROR_CAN_MESSAGE_NOT_COMPLETE                  0x5000      // Occurs when a Message transmission is requested but the Message is not complete.
#define ERROR_CAN_NO_FREE_TRANSMIT_BUFFER               0x6000      // Occurs when during message transmission preparation no free Transmit-Buffer could be found.
#define ERROR_CAN_FILLING_TRANSMIT_BUFFER               0x7000      // Occurs when filling the Transmit-Buffer is not successfull.
#define ERROR_CAN_RECEIVED_DATA_IN_BUFFER               0x8000      // Occurs when in the DataBuffer is still Data.

#define ERROR_CAN_INIT_FRAME_NOT_PLAUSIBLE              0xF100      // Occurs when during the initialisation a not defined Frame is given.
#define ERROR_CAN_INIT_DIRECTION_NOT_PLAUSIBLE          0xF200      // Occurs when during the initialisation a not defined Direction is given.
#define ERROR_CAN_INIT_ID_OUTA_RANGE                    0xF300      // Occurs when during the initialisation the given ID is not in a allowed Range.
#define ERROR_CAN_INIT_ID_NOT_PLAUSIBLE                 0xF400      // Occurs when during the initialisation the given ID does not match with the given Frame.
#define ERROR_CAN_INIT_RTR_NOT_ALLOWED                  0xF500      // Occurs when during the initialisation the RTR is set but the direction is not Transmit.
#define ERROR_CAN_INIT_DLC_NOT_VALID                    0xF600      // Occurs when during the initialisation the given DLC is not in the allowed Range.

#define ERROR_CAN_MESSAGE_NOT_SEND                      0x000E      // Occurs when sending Message wasn't successfull (the higher bits would be filled with the MCP-Error).
#define ERROR_CAN_MESSAGE_NOT_RECEIVED                  0x000F      // Occurs when receiving Message wasn't successfull (the higher bits would be filled with the MCP-Error).

#define ERROR_CAN_NOT_IMPLEMENTED                       0xFFFF      // Occurs when Method is not implemented yet.

#define EMPTY_VALUE_8_BIT                               0x00
#define EMPTY_VALUE_16_BIT                              0x0000
#define EMPTY_VALUE_32_BIT                              0x00000000
#define EMPTY_VALUE_64_BIT                              0x0000000000000000

#endif
