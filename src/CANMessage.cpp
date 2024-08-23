#include "CANMessage.h"

/**
 * @brief ClassConstructor
 *
 */
CANMessage::CANMessage() :
    _ID(0),
    _DLC(0),
    _RTR(false),
    _Frame(0),
    _Direction(0),
    _Controller(),
    _DataBufferIndex(-1),
    _isInitialized(false),
    _lastCanError(EMPTY_VALUE_16_BIT)
{
}

/**
 * @brief Deconstructor
 *
 */
CANMessage::~CANMessage()
{
}

/**
 * @brief Returns the last CAN-Error.
 *
 * The last CAN-Error will always been reset at the beginning of a Method.
 * @return uint16_t CAN-Error
 *
 * 0x0000 = no Error
 */
uint16_t CANMessage::getLastCanError()
{
    return _lastCanError;
}

/**
 * @brief Initialisation of a Message.
 * @param id Message ID (11-Bit for Standard-Frames, 29-Bit for Extended-Frames)
 * @param dlc Data-Length-Code (0 - 8 bytes)
 * @param rtr Remote-Transmission-Request (only allowed when Message-Direction is a transmit)
 * @param frame CAN-Frame (0 = Standard-Frame; 1 = Extended-Frame)
 * @param direction Message-Direction (0 = Recieve; 1 = Transmit)
 * @param controller Object of a MCP2515 Instance
 * @return True when initialisation is successfull, False when not.
 */
bool CANMessage::init(uint32_t id, uint8_t dlc, bool rtr, uint8_t frame, uint8_t direction, MCP2515 controller){

    _isInitialized = true;
    _lastCanError = EMPTY_VALUE_16_BIT;

    if (frame != CANMESSAGE_FRAME_STANDARD && frame != CANMESSAGE_FRAME_EXTENDED)
    {
        _isInitialized = false;
        _lastCanError = ERROR_CAN_INIT_FRAME_NOT_PLAUSIBLE;
        return false;
    }

    if (direction != CANMESSAGE_DIRECTION_RECEIVE && direction != CANMESSAGE_DIRECTION_TRANSMIT)
    {
        _isInitialized = false;
        _lastCanError = ERROR_CAN_INIT_DIRECTION_NOT_PLAUSIBLE;
        return false;
    }

    if (id <= 0x1FFFFFFF)
    {
        if (frame != CANMESSAGE_FRAME_EXTENDED && id > 0x7FF)
        {
            _isInitialized = false;
            _lastCanError = ERROR_CAN_INIT_ID_NOT_PLAUSIBLE;
            return false;
        }
    } else {
        _isInitialized = false;
        _lastCanError = ERROR_CAN_INIT_ID_OUTA_RANGE;
        return false;
    }

    if (rtr && direction != CANMESSAGE_DIRECTION_TRANSMIT)
    {
        _isInitialized = false;
        _lastCanError = ERROR_CAN_INIT_RTR_NOT_ALLOWED;
        return false;
    }

    if (dlc > 8)
    {
        _isInitialized = false;
        _lastCanError = ERROR_CAN_INIT_DLC_NOT_VALID;
        return false;
    }

    _ID = id;
    _DLC = dlc;
    _RTR = rtr;
    _Frame = frame;
    _Direction = direction;
    _Controller = controller;
    return true;
}

/**
 * @brief Add Data to the defined DataBuffer.
 * @param Data Data to be filled in the Buffer
 * @param BufferNumber Number of the Buffer (0 - 7)
 * @return true when success, false on any error (Check _lastCanError)
 */
bool CANMessage::addDataByte(uint8_t Data, uint8_t BufferNumber)
{
    _lastCanError = EMPTY_VALUE_16_BIT;

    if (!_isInitialized)
    {
        _lastCanError = ERROR_CAN_NOT_INITIALIZED;
        return false;
    }

    if (_Direction != CANMESSAGE_DIRECTION_TRANSMIT)
    {
        _lastCanError = ERROR_CAN_METHOD_NOT_ALLOWED;
        return false;
    }

    if (BufferNumber > 7 || BufferNumber >= _DLC)
    {
        _lastCanError = ERROR_CAN_VALUE_OUTA_RANGE;
        return false;
    }

    if (_BufferFilled[BufferNumber] == true)
    {
        _lastCanError = ERROR_CAN_BUFFER_FILLED;
        return false;
    }

    _DataByte[BufferNumber] = Data;
    _BufferFilled[BufferNumber] = true;

    return true;
}

/**
 * @brief Initiates the transmitting-Request.
 *
 * Fails when Message is not complete (all relevant Buffer are filled with data).
 * @return true when success, false on any error (Check _lastCanError)
 */
bool CANMessage::send()
{
    _lastCanError = EMPTY_VALUE_16_BIT;

    if (!_isInitialized)
    {
        _lastCanError = ERROR_CAN_NOT_INITIALIZED;
        return false;
    }

    if (_Direction != CANMESSAGE_DIRECTION_TRANSMIT)
    {
        _lastCanError = ERROR_CAN_METHOD_NOT_ALLOWED;
        return false;
    }

    if (!_RTR)
    {
        for (size_t i = 0; i < _DLC; i++)
        {
            if (_BufferFilled[i] != true)
            {
                _lastCanError = ERROR_CAN_MESSAGE_NOT_COMPLETE;
                return false;
            }
        }
    }

    uint8_t Buffer = _Controller.check4FreeTransmitBuffer();

    if (Buffer >= 0xE0)
    {
        _lastCanError = ERROR_CAN_NO_FREE_TRANSMIT_BUFFER | Buffer;
        return false;
    }

    if (!_Controller.fillTransmitBuffer(Buffer, _ID, _Frame, _RTR, _DLC, _DataByte))
    {
        _lastCanError = ERROR_CAN_FILLING_TRANSMIT_BUFFER;
        return false;
    }

    if (!_Controller.sendMessage(Buffer, 0))
    {
        _lastCanError = _Controller.getLastMCPError() | ERROR_CAN_MESSAGE_NOT_SEND;
        return false;
    }

    for (size_t i = 0; i < 8; i++)
    {
        _BufferFilled[i] = false;
    }

    return true;
}

/**
 * @brief Release the defined Buffer.
 * @param BufferNumber Value 0 - 7
 * @return true when success, false on any error (Check _lastCanError)
 */
bool CANMessage::releaseBuffer(uint8_t BufferNumber)
{
    _lastCanError = EMPTY_VALUE_16_BIT;

    if (!_isInitialized)
    {
        _lastCanError = ERROR_CAN_NOT_INITIALIZED;
        return false;
    }

    if (_Direction != CANMESSAGE_DIRECTION_TRANSMIT)
    {
        _lastCanError = ERROR_CAN_METHOD_NOT_ALLOWED;
        return false;
    }

    if (BufferNumber > 7)
    {
        _lastCanError = ERROR_CAN_VALUE_OUTA_RANGE;
        return false;
    }

    _BufferFilled[BufferNumber] = false;

    return true;
}

/**
 * @brief Check if a RemoteTransmissionRequest for the Message was received.
 * @return True if a RemoteTransmissionRequest for the Message was received, False when not (or on Error check _lastCanError).
 */
bool CANMessage::checkForRTR()
{
    _lastCanError = EMPTY_VALUE_16_BIT;

    if (!_isInitialized)
    {
        _lastCanError = ERROR_CAN_NOT_INITIALIZED;
        return false;
    }

    if (_Direction != CANMESSAGE_DIRECTION_TRANSMIT)
    {
        _lastCanError = ERROR_CAN_METHOD_NOT_ALLOWED;
        return false;
    }

    return _Controller.check4Rtr(_ID, _Frame);
}

/**
 * @brief Check if a Message was received.
 *
 * If this Message is received the Data is send to the local Buffer, so the Buffer of the CAN-Module can be released for the next incomming messages.
 * @return true when success, false on any error (Check _lastCanError)
 */
bool CANMessage::checkReceive()
{
    if (_Direction != CANMESSAGE_DIRECTION_RECEIVE)
    {
        _lastCanError = ERROR_CAN_METHOD_NOT_ALLOWED;
        return false;
    }

    if (_DataBufferIndex != -1)
    {
        _lastCanError = ERROR_CAN_RECEIVED_DATA_IN_BUFFER;
        return false;
    }

    if (!_Controller.check4Receive(_ID, _Frame, _DLC, _DataByte))
    {
        _lastCanError = _Controller.getLastMCPError() | ERROR_CAN_MESSAGE_NOT_RECEIVED;
        return false;
    }

    _DataBufferIndex = 0;

    return true;
}

/**
 * @brief Get the highest available DataByte from the Buffer.
 *
 * Check if Data is available with the dataAvailable()-Method.
 * @return The highest available DataByte.
 */
uint8_t CANMessage::getDataByte()
{
    // Check if Message is a receiving Message.
    if (_Direction != CANMESSAGE_DIRECTION_RECEIVE)
    {
        return 0;
    }

    // Check if Message is checked
    if (_DataBufferIndex == -1)
    {
        return 0;
    }

    uint8_t Data = _DataByte[_DataBufferIndex];

    if (_DataBufferIndex == (_DLC - 1))
    {
        _DataBufferIndex = -1;
    } else {
        _DataBufferIndex++;
    }

    return Data;
}

/**
 * @brief Checks if Data is available in the Buffer.
 * @return True when Data is available, False when no Data is available.
 */
bool CANMessage::dataAvailable()
{
    if (_Direction != CANMESSAGE_DIRECTION_RECEIVE)
    {
        return false;
    }

    if (_DataBufferIndex == -1)
    {
        return false;
    }
    return true;
}

/**
 * @brief Returns the ID of the CAN-Message.
 * @return uint32_t CAN-Message-ID
 * @note If it is an extended Message you can get from getFrame().
 */
uint32_t CANMessage::getID()
{
    return _ID;
}

/**
 * @brief Returns the DLC of the CAN-Message.
 * @return uint8_t CAN-Message-DLC (max. 8 Byte)
 */
uint8_t CANMessage::getDLC()
{
    return _DLC;
}

/**
 * @brief Returns if Message is a Remote-Transmit-Request Message.
 * @return bool True if Message is a RTR-Message
 */
bool CANMessage::getRTR()
{
    return _RTR;
}

/**
 * @brief Returns the Kind of the Frame.
 * @return uint8_t 0 if Message is a Standard-Frame, 1 if Message is an Extended-Frame
 */
uint8_t CANMessage::getFrame()
{
    return _Frame;
}

/**
 * @brief Returns the Direction of the Message.
 * @return uint8_t 0 if Message is Received, 1 if Message is Transmit
 */
uint8_t CANMessage::getDirection()
{
    return _Direction;
}

/**
 * @brief Checks if Message is ready to Send.
 * @return bool True if Message is ready, false if Message is not ready
 */
bool CANMessage::messageSendReady()
{
    for (size_t i = 0; i < _DLC; i++)
    {
        if (_BufferFilled[i] != true)
        {
            return false;
        }
    }
    return true;
}