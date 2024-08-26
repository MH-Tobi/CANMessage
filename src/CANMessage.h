/**
 * @file CANMessage.h
 * @author MH-Tobi
 * @brief blub
 * @version 0.0.1
 * @date 2024-07-20
 *
 * @copyright -
 *
 */

#ifndef CANMESSAGE_H
#define CANMESSAGE_H

#include <Arduino.h>
#include <MCP2515.h>
#include "CANMessageError.h"


#define CANMESSAGE_DIRECTION_RECEIVE	0
#define CANMESSAGE_DIRECTION_TRANSMIT	1

#define CANMESSAGE_FRAME_STANDARD   	0
#define CANMESSAGE_FRAME_EXTENDED   	1


class CANMessage
{
	private:
		uint32_t _ID;               // Message-ID (11 bit for Standard-Frame; 29 bit for Extended Frame)
        uint8_t _DLC;               // Datallength 0-8
		bool _RTR;                  // Remote Transmission Request
		uint8_t _Frame;             // Standard-Frame = 0; Extended-Frame = 1
        uint8_t _Direction;         // Message-Direction (0 = Receive; 1 = Transmit)
        MCP2515 _Controller;        // MCP2515-Controller Instance
		uint8_t _DataByte[8];       // Buffer for the sending or receiving Data
        int8_t _DataBufferIndex;    // Index of the actual readed Buffer
        bool _BufferFilled[8];      // Shows if a Buffer is filled with Data
        bool _isInitialized;
        uint16_t _lastCanError;

	public:

		CANMessage();
		~CANMessage();

        uint16_t getLastCanError();

        bool init(uint32_t id, uint8_t dlc, bool rtr, uint8_t frame, uint8_t direction, MCP2515 controller);

        // For Transmit-Messages

        bool addDataByte(uint8_t Data, uint8_t BufferNumber = 0);
        bool releaseBuffer(uint8_t BufferNumber = 0);
        bool checkForRTR();
        bool messageSendReady();
        bool send();

        // for Receive-Messages

        bool checkReceive();
        uint8_t getDataByte();
        bool dataAvailable();

        // Getter Message properties

        uint32_t getID();
        uint8_t getDLC();
        bool getRTR();
        uint8_t getFrame();
        uint8_t getDirection();

};

#endif
