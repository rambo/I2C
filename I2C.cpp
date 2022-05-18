/*
  I2C.cpp - I2C library
  Copyright (c) 2011-2012 Wayne Truchsess.  All right reserved.
  Rev 5.0 - January 24th, 2012
          - Removed the use of interrupts completely from the library
            so TWI state changes are now polled.
          - Added calls to lockup() function in most functions
            to combat arbitration problems
          - Fixed scan() procedure which left timeouts enabled
            and set to 80msec after exiting procedure
          - Changed scan() address range back to 0 - 0x7F
          - Removed all Wire legacy functions from library
          - A big thanks to Richard Baldwin for all the testing
            and feedback with debugging bus lockups!
  Rev 4.0 - January 14th, 2012
          - Updated to make compatible with 8MHz clock frequency
  Rev 3.0 - January 9th, 2012
          - Modified library to be compatible with Arduino 1.0
          - Changed argument type from boolean to uint8_t in pullUp(),
            setSpeed() and _receiveByte() functions for 1.0 compatability
          - Modified return values for timeout feature to report
            back where in the transmission the timeout occured.
          - added function scan() to perform a bus scan to find devices
            attached to the I2C bus.  Similar to work done by Todbot
            and Nick Gammon
  Rev 2.0 - September 19th, 2011
          - Added support for timeout function to prevent
            and recover from bus lockup (thanks to PaulS
            and CrossRoads on the Arduino forum)
          - Changed return type for _stop() from void to
            uint8_t to handle timeOut function
  Rev 1.0 - August 8th, 2011

  This is a modified version of the Arduino Wire/TWI
  library.  Functions were rewritten to provide more functionality
  and also the use of Repeated Start.  Some I2C devices will not
  function correctly without the use of a Repeated Start.  The
  initial version of this library only supports the Master.


  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <inttypes.h>
#include "I2C.h"

uint8_t I2C::bytesAvailable = 0;
uint8_t I2C::bufferIndex = 0;
uint8_t I2C::totalBytes = 0;
uint16_t I2C::timeOutDelay = 0;

I2C::I2C()
{
}

////////////// Public Methods ////////////////////////////////////////

/*
 *  Description:
 *      Enables the I2C hardware
 *  Parameters:
 *      none
 *  Returns:
 *      none
 */
void I2C::begin()
{
  pullup(1);

  // initialize twi prescaler and bit rate
  cbi(TWSR, TWPS0);
  cbi(TWSR, TWPS1);
  TWBR = ((F_CPU / 100000) - 16) / 2;
  // enable twi module and acks
  TWCR = _BV(TWEN) | _BV(TWEA);
}

/*
 *  Description:
 *      Disables the I2C hardware
 *  Parameters:
 *      none
 *  Returns:
 *      none
 */
void I2C::end()
{
  TWCR = 0;
}

/*
 *  Description:
 *      Allows the user to program a time out limit to prevent and recover from
 *      I2C bus lockups. I2C bus lockups have a tendency to freeze a program
 *      which typically requires a power cycle to restart your program. This
 *      allows the user to define a time out in which the I2C will release
 *      itself and reinitialize and continue on with the next function. Setting
 *      the value to zero will disable the function. On a side note, be careful
 *      with setting too low a value because some devices support clock
 *      stretching which can increase the time before an acknowledgment is sent
 *      which could be misconstrued as a lockup.
 *
 *      If a lock up occurs the returned parameters from Read and/or Writes will
 *      contain a 1.
 *
 *  Parameters:
 *      timeOut - uint16_t
 *          The amount of time to wait before timing out. Can range from
 *          0 - 65535 milliseconds. If it's set to 0 it will be disabled.
 *  Returns:
 *      none
 */
void I2C::timeOut(uint16_t _timeOut)
{
  timeOutDelay = _timeOut;
}

/*
 *  Description:
 *      Enables high speed mode (400kHz)
 *  Parameters:
 *      fast - Boolean
 *          True: High Speed
 *          False: Low Speed
 *  Returns:
 *      none
 */
void I2C::setSpeed(uint8_t _fast)
{
  if (!_fast)
  {
    TWBR = ((F_CPU / 100000) - 16) / 2;
  }
  else
  {
    TWBR = ((F_CPU / 400000) - 16) / 2;
  }
}

/*
 *  Description:
 *      Enables/disables internal pullup resistors
 *  Parameters:
 *      activate - Boolean
 *          True: Enable internal pullup resistors (default)
 *          False: Disable internal pullup resistors
 *  Returns:
 *      none
 */
void I2C::pullup(uint8_t activate)
{
  if (activate)
  {
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__)
    // activate internal pull-ups for twi
    // as per note from atmega8 manual pg167
    sbi(PORTC, 4);
    sbi(PORTC, 5);
#elif defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__)
    // activate internal pull-ups for twi
    // as per note from atmega644p manual pg108
    sbi(PORTC, 0);
    sbi(PORTC, 1);
#else
    // activate internal pull-ups for twi
    // as per note from atmega128 manual pg204
    sbi(PORTD, 0);
    sbi(PORTD, 1);
#endif
  }
  else
  {
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__)
    // deactivate internal pull-ups for twi
    // as per note from atmega8 manual pg167
    cbi(PORTC, 4);
    cbi(PORTC, 5);
#elif defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__)
    // deactivate internal pull-ups for twi
    // as per note from atmega644p manual pg108
    cbi(PORTC, 0);
    cbi(PORTC, 1);
#else
    // deactivate internal pull-ups for twi
    // as per note from atmega128 manual pg204
    cbi(PORTD, 0);
    cbi(PORTD, 1);
#endif
  }
}

/*
 *  Description:
 *      Scans the bus for I2C devices and reports back each 7 bit address to the
 *      Serial Monitor. The timeout feature was implemented so if there is a
 *      problem with the bus during the scan, it will display on Serial Monitor
 *      that there was a problem.
 *  Parameters:
 *      none
 *  Returns:
 *      none
 */
void I2C::scan()
{
  uint16_t tempTime = timeOutDelay;
  timeOut(80);
  uint8_t totalDevicesFound = 0;
  Serial.println(F("Scanning for devices...please wait"));
  Serial.println();
  for (uint8_t s = 0; s <= 0x7F; s++)
  {
    returnStatus = 0;
    returnStatus = _start();
    if (!returnStatus)
    {
      returnStatus = _sendAddress(SLA_W(s));
    }
    if (returnStatus)
    {
      if (returnStatus == 1)
      {
        Serial.println(F("There is a problem with the bus, could not complete scan"));
        timeOutDelay = tempTime;
        return;
      }
    }
    else
    {
      Serial.print(F("Found device at address - "));
      Serial.print(F(" 0x"));
      Serial.println(s, HEX);
      totalDevicesFound++;
    }
    _stop();
  }
  if (!totalDevicesFound)
  {
    Serial.println(F("No devices found"));
  }
  timeOutDelay = tempTime;
}

/*
 *  Description:
 *      Returns the number of unread bytes stored in the internal 32 byte buffer
 *  Parameters:
 *      none
 *  Returns:
 *      uint8_t
 *          The number of unread bytes
 */
uint8_t I2C::available()
{
  return (bytesAvailable);
}

/*
 *  Description:
 *      Returns the first unread byte of the internal buffer.
 *  Parameters:
 *      none
 *  Returns:
 *      uint8_t
 *          First unread byte of the internal buffer
 */
uint8_t I2C::receive()
{
  bufferIndex = totalBytes - bytesAvailable;
  if (!bytesAvailable)
  {
    bufferIndex = 0;
    return (0);
  }
  bytesAvailable--;
  return (data[bufferIndex]);
}

/*
 * TRANSMISSION TIMEOUT RETURN VALUES
 *
 * Return values for new functions that use the timeOut feature
 * will now return at what point in the transmission the timeout
 * occurred. Looking at a full communication sequence between a
 * master and slave (transmit data and then readback data) there
 * a total of 7 points in the sequence where a timeout can occur.
 * These are listed below and correspond to the returned value:
 * 1 - Waiting for successful completion of a Start bit
 * 2 - Waiting for ACK/NACK while addressing slave in transmit mode (MT)
 * 3 - Waiting for ACK/NACK while sending data to the slave
 * 4 - Waiting for successful completion of a Repeated Start
 * 5 - Waiting for ACK/NACK while addressing slave in receiver mode (MR)
 * 6 - Waiting for ACK/NACK while receiving data from the slave
 * 7 - Waiting for successful completion of the Stop bit
 *
 * All possible return values:
 * 0           Function executed with no errors
 * 1 - 7       Timeout occurred, see above list
 * 8 - 0xFF    See datasheet for exact meaning
 */

/*
 *  Description:
 *      Initiate an I2C write operation with no data sent. Typically used to set
 *      the "pointer" to a register address
 *
 *      NOTE: For devices with 16-bit register addresses use
 *      I2c.write16(address, registerAddress). It is identical except
 *      registerAddress is a uint16_t
 *  Parameters:
 *      address - uint8_t
 *          The 7 bit I2C slave address
 *      registerAddress - uint8_t
 *          Address of the register you wish to access (as per the datasheet)
 *  Returns:
 *      uint8_t
 *          See "TRANSMISSION TIMEOUT RETURN VALUES" for return value meaning
 */
uint8_t I2C::write(uint8_t address, uint8_t registerAddress)
{
  returnStatus = 0;
  returnStatus = _start();
  if (returnStatus)
  {
    return (returnStatus);
  }
  returnStatus = _sendAddress(SLA_W(address));
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (2);
    }
    return (returnStatus);
  }
  returnStatus = _sendByte(registerAddress);
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (3);
    }
    return (returnStatus);
  }
  returnStatus = _stop();
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (7);
    }
    return (returnStatus);
  }
  return (returnStatus);
}

uint8_t I2C::write(int address, int registerAddress)
{
  return (write((uint8_t)address, (uint8_t)registerAddress));
}

/*
 *  Description:
 *      Initiate an I2C write operation, sending a single data byte. Typically
 *      used to send a single byte of data to a register address
 *
 *      NOTE: For devices with 16-bit register addresses use
 *      I2c.write16(address, registerAddress, data). It is identical except
 *      registerAddress is a uint16_t
 *  Parameters:
 *      address - uint8_t
 *          The 7 bit I2C slave address
 *      registerAddress - uint8_t
 *          Address of the register you wish to access (as per the datasheet)
 *      data - uint8_t
 *          A single byte of data to send
 *  Returns:
 *      uint8_t
 *          See "TRANSMISSION TIMEOUT RETURN VALUES" for return value meaning
 */
uint8_t I2C::write(uint8_t address, uint8_t registerAddress, uint8_t data)
{
  returnStatus = 0;
  returnStatus = _start();
  if (returnStatus)
  {
    return (returnStatus);
  }
  returnStatus = _sendAddress(SLA_W(address));
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (2);
    }
    return (returnStatus);
  }
  returnStatus = _sendByte(registerAddress);
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (3);
    }
    return (returnStatus);
  }
  returnStatus = _sendByte(data);
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (3);
    }
    return (returnStatus);
  }
  returnStatus = _stop();
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (7);
    }
    return (returnStatus);
  }
  return (returnStatus);
}

uint8_t I2C::write(int address, int registerAddress, int data)
{
  return (write((uint8_t)address, (uint8_t)registerAddress, (uint8_t)data));
}

/*
 *  Description:
 *      Initiate an I2C write operation, array of char. Typically used to send
 *      an array of char starting at registerAddress location. As a side note
 *      there is no restriction on how many bytes may be sent unlike the Wire
 *      library which has a 32 byte restriction
 *
 *      NOTE: For devices with 16-bit register addresses use
 *      I2c.write16(address, registerAddress, *data). It is identical except
 *      registerAddress is a uint16_t
 *  Parameters:
 *      address - uint8_t
 *          The 7 bit I2C slave address
 *      registerAddress - uint8_t
 *          Address of the register you wish to access (as per the datasheet)
 *      data - const char*
 *          Array of characters
 *  Returns:
 *      uint8_t
 *          See "TRANSMISSION TIMEOUT RETURN VALUES" for return value meaning
 */
uint8_t I2C::write(uint8_t address, uint8_t registerAddress, const char *data)
{
  uint8_t bufferLength = strlen(data);
  returnStatus = 0;
  returnStatus = write(address, registerAddress, (const uint8_t *)data, bufferLength);
  return (returnStatus);
}

/*
 *  Same as I2c.write(address, registerAddress, uint8_t data), but writes 2
 *  bytes instead
 */
uint8_t I2C::write(uint8_t address, uint8_t registerAddress, uint16_t data)
{
  //Array to hold the 2 bytes that will be written to the register
  uint8_t writeBytes[2];
  returnStatus = 0;

  writeBytes[0] = (data >> 8) & 0xFF; //MSB
  writeBytes[1] = data & 0xFF;        //LSB

  returnStatus = write(address, registerAddress, writeBytes, 2);
  return (returnStatus);
}

/*
 *  Same as I2c.write(address, registerAddress, uint8_t data), but writes 4
 *  bytes instead
 */
uint8_t I2C::write(uint8_t address, uint8_t registerAddress, uint32_t data)
{
  //Array to hold the 4 bytes that will be written to the register
  uint8_t writeBytes[4];
  returnStatus = 0;

  writeBytes[0] = (data >> 24) & 0xFF; //MSB
  writeBytes[1] = (data >> 16) & 0xFF;
  writeBytes[2] = (data >> 8) & 0xFF;
  writeBytes[3] = data & 0xFF; //LSB

  returnStatus = write(address, registerAddress, writeBytes, 4);
  return (returnStatus);
}

/*
 *  Same as I2c.write(address, registerAddress, uint8_t data), but writes 8
 *  bytes instead
 */
uint8_t I2C::write(uint8_t address, uint8_t registerAddress, uint64_t data)
{
  //Array to hold the 8 bytes that will be written to the register
  uint8_t writeBytes[8];
  returnStatus = 0;

  writeBytes[0] = (data >> 56) & 0xFF; //MSB
  writeBytes[1] = (data >> 48) & 0xFF;
  writeBytes[2] = (data >> 40) & 0xFF;
  writeBytes[3] = (data >> 32) & 0xFF;
  writeBytes[4] = (data >> 24) & 0xFF;
  writeBytes[5] = (data >> 16) & 0xFF;
  writeBytes[6] = (data >> 8) & 0xFF;
  writeBytes[7] = data & 0xFF; //LSB

  returnStatus = write(address, registerAddress, writeBytes, 8);
  return (returnStatus);
}

/*
 *  Description:
 *      Initiate an I2C write operation, array of bytes. Typically used to send
 *      an array of bytes starting at registerAddress location. As a side note
 *      there is no restriction on how many bytes may be sent unlike the Wire
 *      library which has a 32 byte restriction
 *
 *      NOTE: For devices with 16-bit register addresses use
 *      I2c.write16(address, registerAddress, *data, numberBytes). It is
 *      identical except registerAddress is a uint16_t
 *  Parameters:
 *      address - uint8_t
 *          The 7 bit I2C slave address
 *      registerAddress - uint8_t
 *          Address of the register you wish to access (as per the datasheet)
 *      data - const uint8_t*
 *          Array of bytes
 *      numberBytes - uint8_t
 *          The number of bytes in the array to be sent
 *  Returns:
 *      uint8_t
 *          See "TRANSMISSION TIMEOUT RETURN VALUES" for return value meaning
 */
uint8_t I2C::write(uint8_t address, uint8_t registerAddress, const uint8_t *data, uint8_t numberBytes)
{
  returnStatus = 0;
  returnStatus = _start();
  if (returnStatus)
  {
    return (returnStatus);
  }
  returnStatus = _sendAddress(SLA_W(address));
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (2);
    }
    return (returnStatus);
  }
  returnStatus = _sendByte(registerAddress);
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (3);
    }
    return (returnStatus);
  }
  for (uint8_t i = 0; i < numberBytes; i++)
  {
    returnStatus = _sendByte(data[i]);
    if (returnStatus)
    {
      if (returnStatus == 1)
      {
        return (3);
      }
      return (returnStatus);
    }
  }
  returnStatus = _stop();
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (7);
    }
    return (returnStatus);
  }
  return (returnStatus);
}

/*
 *  Description:
 *      Initiate a read operation from the current position of slave register
 *      pointer. The bytes will be stored in an internal buffer and will have
 *      the 32 byte size restriction. Data can be read out of the buffer using
 *      I2c.receive().
 *  Parameters:
 *      address - uint8_t
 *          The 7 bit I2C slave address
 *      numberBytes - uint8_t
 *          The number of bytes to be read
 *  Returns:
 *      uint8_t
 *          See "TRANSMISSION TIMEOUT RETURN VALUES" for return value meaning
 *          NOTE: Unlike the Wire library the read operation will not return the
 *          number of bytes read, instead it will return the error code which
 *          can be used for debugging.
 */
uint8_t I2C::read(uint8_t address, uint8_t numberBytes)
{
  bytesAvailable = 0;
  bufferIndex = 0;
  numberBytes = min(numberBytes, MAX_BUFFER_SIZE);
  if (numberBytes == 0)
  {
    numberBytes++;
  }
  nack = numberBytes - 1;
  returnStatus = 0;
  returnStatus = _start();
  if (returnStatus)
  {
    return (returnStatus);
  }
  returnStatus = _sendAddress(SLA_R(address));
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (5);
    }
    return (returnStatus);
  }
  for (uint8_t i = 0; i < numberBytes; i++)
  {
    if (i == nack)
    {
      returnStatus = _receiveByte(0);
      if (returnStatus == 1)
      {
        return (6);
      }

      if (returnStatus != MR_DATA_NACK)
      {
        return (returnStatus);
      }
    }
    else
    {
      returnStatus = _receiveByte(1);
      if (returnStatus == 1)
      {
        return (6);
      }
      if (returnStatus != MR_DATA_ACK)
      {
        return (returnStatus);
      }
    }
    data[i] = TWDR;
    bytesAvailable = i + 1;
    totalBytes = i + 1;
  }
  returnStatus = _stop();
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (7);
    }
    return (returnStatus);
  }
  return (returnStatus);
}

uint8_t I2C::read(int address, int numberBytes)
{
  return (read((uint8_t)address, (uint8_t)numberBytes));
}

/*
 *  Description:
 *      Initiate a write operation to set the pointer to the registerAddress,
 *      then send a repeated start (not a stop then start) and store the number
 *      of bytes in an internal buffer. The 32 byte size restriction is imposed
 *      for this function. Data can be read out of the buffer using
 *      I2c.receive().
 *
 *      NOTE: For devices with 16-bit register addresses use
 *      I2c.read16(address, registerAddress, numberBytes). It is identical
 *      except registerAddress is a uint16_t
 *  Parameters:
 *      address - uint8_t
 *          The 7 bit I2C slave address
 *      registerAddress - uint8_t
 *          Starting register address to read data from
 *      numberBytes - uint8_t
 *          The number of bytes to be read
 *  Returns:
 *      uint8_t
 *          NOTE: Unlike the Wire library the read operation will not return the
 *          number of bytes read, instead it will return the error code which
 *          can be used for debugging.
 */
uint8_t I2C::read(uint8_t address, uint8_t registerAddress, uint8_t numberBytes)
{
  bytesAvailable = 0;
  bufferIndex = 0;
  numberBytes = min(numberBytes, MAX_BUFFER_SIZE);
  if (numberBytes == 0)
  {
    numberBytes++;
  }
  nack = numberBytes - 1;
  returnStatus = 0;
  returnStatus = _start();
  if (returnStatus)
  {
    return (returnStatus);
  }
  returnStatus = _sendAddress(SLA_W(address));
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (2);
    }
    return (returnStatus);
  }
  returnStatus = _sendByte(registerAddress);
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (3);
    }
    return (returnStatus);
  }
  returnStatus = _start();
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (4);
    }
    return (returnStatus);
  }
  returnStatus = _sendAddress(SLA_R(address));
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (5);
    }
    return (returnStatus);
  }
  for (uint8_t i = 0; i < numberBytes; i++)
  {
    if (i == nack)
    {
      returnStatus = _receiveByte(0);
      if (returnStatus == 1)
      {
        return (6);
      }
      if (returnStatus != MR_DATA_NACK)
      {
        return (returnStatus);
      }
    }
    else
    {
      returnStatus = _receiveByte(1);
      if (returnStatus == 1)
      {
        return (6);
      }
      if (returnStatus != MR_DATA_ACK)
      {
        return (returnStatus);
      }
    }
    data[i] = TWDR;
    bytesAvailable = i + 1;
    totalBytes = i + 1;
  }
  returnStatus = _stop();
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (7);
    }
    return (returnStatus);
  }
  return (returnStatus);
}

uint8_t I2C::read(int address, int registerAddress, int numberBytes)
{
  return (read((uint8_t)address, (uint8_t)registerAddress, (uint8_t)numberBytes));
}

/*
 *  Description:
 *      Initiate a read operation from the current position of slave register
 *      pointer. The bytes will be stored in the dataBuffer. As a side note
 *      there is a maximum of 255 bytes that may be received unlike the Wire
 *      library which has a 32 byte restriction.
 *
 *      NOTE: For reading more bytes (up to 65535) use
 *      I2c.readex(address, numberBytes, *dataBuffer). It is identical except
 *      numberBytes is a uint16_t
 *  Parameters:
 *      address - uint8_t
 *          The 7 bit I2C slave address
 *      numberBytes - uint8_t
 *          The number of bytes to be read
 *      dataBuffer - uint8_t*
 *          Array to store the read data
 *  Returns:
 *      uint8_t
 *          See "TRANSMISSION TIMEOUT RETURN VALUES" for return value meaning
 *          NOTE: Unlike the Wire library the read operation will not return the
 *          number of bytes read, instead it will return the error code which
 *          can be used for debugging.
 */
uint8_t I2C::read(uint8_t address, uint8_t numberBytes, uint8_t *dataBuffer)
{
  bytesAvailable = 0;
  bufferIndex = 0;
  if (numberBytes == 0)
  {
    numberBytes++;
  }
  nack = numberBytes - 1;
  returnStatus = 0;
  returnStatus = _start();
  if (returnStatus)
  {
    return (returnStatus);
  }
  returnStatus = _sendAddress(SLA_R(address));
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (5);
    }
    return (returnStatus);
  }
  for (uint8_t i = 0; i < numberBytes; i++)
  {
    if (i == nack)
    {
      returnStatus = _receiveByte(0);
      if (returnStatus == 1)
      {
        return (6);
      }
      if (returnStatus != MR_DATA_NACK)
      {
        return (returnStatus);
      }
    }
    else
    {
      returnStatus = _receiveByte(1);
      if (returnStatus == 1)
      {
        return (6);
      }
      if (returnStatus != MR_DATA_ACK)
      {
        return (returnStatus);
      }
    }
    dataBuffer[i] = TWDR;
    bytesAvailable = i + 1;
    totalBytes = i + 1;
  }
  returnStatus = _stop();
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (7);
    }
    return (returnStatus);
  }
  return (returnStatus);
}

/*
 *  Same as I2c.read(address, numberBytes, *dataBuffer), but can read more
 *  bytes (up to 65535)
 */
uint8_t I2C::readex(uint8_t address, uint16_t numberBytes, uint8_t *dataBuffer)
{
  bytesAvailable = 0;
  bufferIndex = 0;
  if (numberBytes == 0)
  {
    numberBytes++;
  }
  uint16_t nack = numberBytes - 1;
  returnStatus = 0;
  returnStatus = _start();
  if (returnStatus)
  {
    return (returnStatus);
  }
  returnStatus = _sendAddress(SLA_R(address));
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (5);
    }
    return (returnStatus);
  }
  for (uint16_t i = 0; i < numberBytes; i++)
  {
    if (i == nack)
    {
      returnStatus = _receiveByte(0);
      if (returnStatus == 1)
      {
        return (6);
      }
      if (returnStatus != MR_DATA_NACK)
      {
        return (returnStatus);
      }
    }
    else
    {
      returnStatus = _receiveByte(1);
      if (returnStatus == 1)
      {
        return (6);
      }
      if (returnStatus != MR_DATA_ACK)
      {
        return (returnStatus);
      }
    }
    dataBuffer[i] = TWDR;
    bytesAvailable = i + 1;
    totalBytes = i + 1;
  }
  returnStatus = _stop();
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (7);
    }
    return (returnStatus);
  }
  return (returnStatus);
}

/*
 *  Description:
 *      Initiate a write operation to set the pointer to the registerAddress,
 *      then sending a repeated start (not a stop then start) and store the
 *      number of bytes in the dataBuffer. As a side note there is a maximum of
 *      255 bytes that may be received unlike the Wire library which has a 32
 *      byte restriction.
 *
 *      NOTE: For devices with 16-bit register addresses use
 *      I2c.read16(address, registerAddress, numberBytes, *dataBuffer). It is
 *      identical except registerAddress is a uint16_t
 *
 *      NOTE: For reading more bytes (up to 65535) use
 *      I2c.readex(address, registerAddress, numberBytes, *dataBuffer). It is
 *      identical except numberBytes is a uint16_t
 *  Parameters:
 *      address - uint8_t
 *          The 7 bit I2C slave address
 *      registerAddress - uint8_t
 *          Starting register address to read data from
 *      numberBytes - uint8_t
 *          The number of bytes to be read
 *      dataBuffer - uint8_t*
 *          An array to store the read data
 *  Returns:
 *      uint8_t
 *          See "TRANSMISSION TIMEOUT RETURN VALUES" for return value meaning
 *          NOTE: Unlike the Wire library the read operation will not return the
 *          number of bytes read, instead it will return the error code which
 *          can be used for debugging.
 */
uint8_t I2C::read(uint8_t address, uint8_t registerAddress, uint8_t numberBytes, uint8_t *dataBuffer)
{
  bytesAvailable = 0;
  bufferIndex = 0;
  if (numberBytes == 0)
  {
    numberBytes++;
  }
  nack = numberBytes - 1;
  returnStatus = 0;
  returnStatus = _start();
  if (returnStatus)
  {
    return (returnStatus);
  }
  returnStatus = _sendAddress(SLA_W(address));
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (2);
    }
    return (returnStatus);
  }
  returnStatus = _sendByte(registerAddress);
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (3);
    }
    return (returnStatus);
  }
  returnStatus = _start();
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (4);
    }
    return (returnStatus);
  }
  returnStatus = _sendAddress(SLA_R(address));
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (5);
    }
    return (returnStatus);
  }
  for (uint8_t i = 0; i < numberBytes; i++)
  {
    if (i == nack)
    {
      returnStatus = _receiveByte(0);
      if (returnStatus == 1)
      {
        return (6);
      }
      if (returnStatus != MR_DATA_NACK)
      {
        return (returnStatus);
      }
    }
    else
    {
      returnStatus = _receiveByte(1);
      if (returnStatus == 1)
      {
        return (6);
      }
      if (returnStatus != MR_DATA_ACK)
      {
        return (returnStatus);
      }
    }
    dataBuffer[i] = TWDR;
    bytesAvailable = i + 1;
    totalBytes = i + 1;
  }
  returnStatus = _stop();
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (7);
    }
    return (returnStatus);
  }
  return (returnStatus);
}

/*
 *  Same as I2c.read(address, numberBytes, *dataBuffer), but can read more
 *  bytes (up to 65535)
 */
uint8_t I2C::readex(uint8_t address, uint8_t registerAddress, uint16_t numberBytes, uint8_t *dataBuffer)
{
  bytesAvailable = 0;
  bufferIndex = 0;
  if (numberBytes == 0)
  {
    numberBytes++;
  }
  uint16_t nack = numberBytes - 1;
  returnStatus = 0;
  returnStatus = _start();
  if (returnStatus)
  {
    return (returnStatus);
  }
  returnStatus = _sendAddress(SLA_W(address));
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (2);
    }
    return (returnStatus);
  }
  returnStatus = _sendByte(registerAddress);
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (3);
    }
    return (returnStatus);
  }
  returnStatus = _start();
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (4);
    }
    return (returnStatus);
  }
  returnStatus = _sendAddress(SLA_R(address));
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (5);
    }
    return (returnStatus);
  }
  for (uint16_t i = 0; i < numberBytes; i++)
  {
    if (i == nack)
    {
      returnStatus = _receiveByte(0);
      if (returnStatus == 1)
      {
        return (6);
      }
      if (returnStatus != MR_DATA_NACK)
      {
        return (returnStatus);
      }
    }
    else
    {
      returnStatus = _receiveByte(1);
      if (returnStatus == 1)
      {
        return (6);
      }
      if (returnStatus != MR_DATA_ACK)
      {
        return (returnStatus);
      }
    }
    dataBuffer[i] = TWDR;
    bytesAvailable = i + 1;
    totalBytes = i + 1;
  }
  returnStatus = _stop();
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (7);
    }
    return (returnStatus);
  }
  return (returnStatus);
}

////////// 16-Bit Methods ///////////

//These functions will be used to write to Slaves that take 16-bit
//register addresses

/*
 *  Same as I2c.write(address, registerAddress), but writes to a slave
 *  device that takes 16-bit register addresses
 */
uint8_t I2C::write16(uint8_t address, uint16_t registerAddress)
{
  returnStatus = 0;
  returnStatus = _start();
  if (returnStatus)
  {
    return (returnStatus);
  }
  returnStatus = _sendAddress(SLA_W(address));
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (2);
    }
    return (returnStatus);
  }
  //Send MSB of register address
  returnStatus = _sendByte(registerAddress >> 8);
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (3);
    }
    return (returnStatus);
  }
  //Send LSB of register address
  returnStatus = _sendByte(registerAddress & 0xFF);
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (3);
    }
    return (returnStatus);
  }
  returnStatus = _stop();
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (7);
    }
    return (returnStatus);
  }
  return (returnStatus);
}

/*
 *  Same as I2c.write(address, registerAddress, data), but writes to a slave
 *  device that takes 16-bit register addresses
 */
uint8_t I2C::write16(uint8_t address, uint16_t registerAddress, uint8_t data)
{
  returnStatus = 0;
  returnStatus = _start();
  if (returnStatus)
  {
    return (returnStatus);
  }
  returnStatus = _sendAddress(SLA_W(address));
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (2);
    }
    return (returnStatus);
  }
  //Send MSB of register address
  returnStatus = _sendByte(registerAddress >> 8);
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (3);
    }
    return (returnStatus);
  }
  //Send LSB of register address
  returnStatus = _sendByte(registerAddress & 0xFF);
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (3);
    }
    return (returnStatus);
  }
  returnStatus = _sendByte(data);
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (3);
    }
    return (returnStatus);
  }
  returnStatus = _stop();
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (7);
    }
    return (returnStatus);
  }
  return (returnStatus);
}

/*
 *  Same as I2c.write(address, registerAddress, *data), but writes to a slave
 *  device that takes 16-bit register addresses
 */
uint8_t I2C::write16(uint8_t address, uint16_t registerAddress, const char *data)
{
  uint8_t bufferLength = strlen(data);
  returnStatus = 0;
  returnStatus = write16(address, registerAddress, (const uint8_t *)data, bufferLength);
  return (returnStatus);
}

/*
 *  Same as I2c.write(address, registerAddress, *data, numberBytes), but writes
 *  to a slave device that takes 16-bit address
 */
uint8_t I2C::write16(uint8_t address, uint16_t registerAddress, const uint8_t *data, uint8_t numberBytes)
{
  returnStatus = 0;
  returnStatus = _start();
  if (returnStatus)
  {
    return (returnStatus);
  }
  returnStatus = _sendAddress(SLA_W(address));
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (2);
    }
    return (returnStatus);
  }
  //Send MSB of register address
  returnStatus = _sendByte(registerAddress >> 8);
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (3);
    }
    return (returnStatus);
  }
  //Send LSB of register address
  returnStatus = _sendByte(registerAddress & 0xFF);
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (3);
    }
    return (returnStatus);
  }
  for (uint8_t i = 0; i < numberBytes; i++)
  {
    returnStatus = _sendByte(data[i]);
    if (returnStatus)
    {
      if (returnStatus == 1)
      {
        return (3);
      }
      return (returnStatus);
    }
  }
  returnStatus = _stop();
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (7);
    }
    return (returnStatus);
  }
  return (returnStatus);
}

/*
 *  Same as I2c.write16(address, registerAddress, uint8_t data), but writes 2
 *  bytes instead
 */
uint8_t I2C::write16(uint8_t address, uint16_t registerAddress, uint16_t data)
{
  //Array to hold the 2 bytes that will be written to the register
  uint8_t writeBytes[2];
  returnStatus = 0;

  writeBytes[0] = (data >> 8) & 0xFF; //MSB
  writeBytes[1] = data & 0xFF;        //LSB

  returnStatus = write16(address, registerAddress, writeBytes, 2);
  return (returnStatus);
}

/*
 *  Same as I2c.write16(address, registerAddress, uint8_t data), but writes 4
 *  bytes instead
 */
uint8_t I2C::write16(uint8_t address, uint16_t registerAddress, uint32_t data)
{
  //Array to hold the 4 bytes that will be written to the register
  uint8_t writeBytes[4];
  returnStatus = 0;

  writeBytes[0] = (data >> 24) & 0xFF; //MSB
  writeBytes[1] = (data >> 16) & 0xFF;
  writeBytes[2] = (data >> 8) & 0xFF;
  writeBytes[3] = data & 0xFF; //LSB

  returnStatus = write16(address, registerAddress, writeBytes, 4);
  return (returnStatus);
}

/*
 *  Same as I2c.write16(address, registerAddress, uint8_t data), but writes 8
 *  bytes instead
 */
uint8_t I2C::write16(uint8_t address, uint16_t registerAddress, uint64_t data)
{
  //Array to hold the 8 bytes that will be written to the register
  uint8_t writeBytes[8];
  returnStatus = 0;

  writeBytes[0] = (data >> 56) & 0xFF; //MSB
  writeBytes[1] = (data >> 48) & 0xFF;
  writeBytes[2] = (data >> 40) & 0xFF;
  writeBytes[3] = (data >> 32) & 0xFF;
  writeBytes[4] = (data >> 24) & 0xFF;
  writeBytes[5] = (data >> 16) & 0xFF;
  writeBytes[6] = (data >> 8) & 0xFF;
  writeBytes[7] = data & 0xFF; //LSB

  returnStatus = write16(address, registerAddress, writeBytes, 8);
  return (returnStatus);
}

//These functions will be used to read from Slaves that take 16-bit addresses

/*
 *  Same as I2c.read(address, registerAddress, numberBytes), but reads from a
 *  slave device that takes 16-bit register addresses
 */
uint8_t I2C::read16(uint8_t address, uint16_t registerAddress, uint8_t numberBytes)
{
  bytesAvailable = 0;
  bufferIndex = 0;
  numberBytes = min(numberBytes, MAX_BUFFER_SIZE);
  if (numberBytes == 0)
  {
    numberBytes++;
  }
  nack = numberBytes - 1;
  returnStatus = 0;
  returnStatus = _start();
  if (returnStatus)
  {
    return (returnStatus);
  }
  returnStatus = _sendAddress(SLA_W(address));
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (2);
    }
    return (returnStatus);
  }
  //Send MSB of register address
  returnStatus = _sendByte(registerAddress >> 8);
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (3);
    }
    return (returnStatus);
  }
  //Send LSB of register address
  returnStatus = _sendByte(registerAddress & 0xFF);
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (3);
    }
    return (returnStatus);
  }
  returnStatus = _start();
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (4);
    }
    return (returnStatus);
  }
  returnStatus = _sendAddress(SLA_R(address));
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (5);
    }
    return (returnStatus);
  }
  for (uint8_t i = 0; i < numberBytes; i++)
  {
    if (i == nack)
    {
      returnStatus = _receiveByte(0);
      if (returnStatus == 1)
      {
        return (6);
      }
      if (returnStatus != MR_DATA_NACK)
      {
        return (returnStatus);
      }
    }
    else
    {
      returnStatus = _receiveByte(1);
      if (returnStatus == 1)
      {
        return (6);
      }
      if (returnStatus != MR_DATA_ACK)
      {
        return (returnStatus);
      }
    }
    data[i] = TWDR;
    bytesAvailable = i + 1;
    totalBytes = i + 1;
  }
  returnStatus = _stop();
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (7);
    }
    return (returnStatus);
  }
  return (returnStatus);
}

/*
 *  Same as I2c.read(address, registerAddress, numberBytes, *dataBuffer), but
 *  reads from a slave device that takes 16-bit register addresses
 */
uint8_t I2C::read16(uint8_t address, uint16_t registerAddress, uint8_t numberBytes, uint8_t *dataBuffer)
{
  bytesAvailable = 0;
  bufferIndex = 0;
  if (numberBytes == 0)
  {
    numberBytes++;
  }
  nack = numberBytes - 1;
  returnStatus = 0;
  returnStatus = _start();
  if (returnStatus)
  {
    return (returnStatus);
  }
  returnStatus = _sendAddress(SLA_W(address));
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (2);
    }
    return (returnStatus);
  }
  //Send MSB of register address
  returnStatus = _sendByte(registerAddress >> 8);
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (3);
    }
    return (returnStatus);
  }
  //Send LSB of register address
  returnStatus = _sendByte(registerAddress & 0xFF);
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (3);
    }
    return (returnStatus);
  }
  returnStatus = _start();
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (4);
    }
    return (returnStatus);
  }
  returnStatus = _sendAddress(SLA_R(address));
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (5);
    }
    return (returnStatus);
  }
  for (uint8_t i = 0; i < numberBytes; i++)
  {
    if (i == nack)
    {
      returnStatus = _receiveByte(0);
      if (returnStatus == 1)
      {
        return (6);
      }
      if (returnStatus != MR_DATA_NACK)
      {
        return (returnStatus);
      }
    }
    else
    {
      returnStatus = _receiveByte(1);
      if (returnStatus == 1)
      {
        return (6);
      }
      if (returnStatus != MR_DATA_ACK)
      {
        return (returnStatus);
      }
    }
    dataBuffer[i] = TWDR;
    bytesAvailable = i + 1;
    totalBytes = i + 1;
  }
  returnStatus = _stop();
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (7);
    }
    return (returnStatus);
  }
  return (returnStatus);
}

//////////// LOW-LEVEL METHODS
//////////// (No need to use them if the device uses normal register protocol)

/*
 *  Description:
 *      Sends out a Start Condition. This puts all slave devices on notice that
 *      a transmission is about to start. This function incorporates the timeOut
 *      function.
 *  Parameters:
 *      none
 *  Returns:
 *      uint8_t
 *          0: The start condition was successfully sent
 *          1: The function timed out
 *          2 - 0xFF: See the datasheet
 */
uint8_t I2C::_start()
{
  unsigned long startingTime = millis();
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
  {
    if (!timeOutDelay)
    {
      continue;
    }
    if ((millis() - startingTime) >= timeOutDelay)
    {
      lockUp();
      return (1);
    }
  }
  if ((TWI_STATUS == START) || (TWI_STATUS == REPEATED_START))
  {
    return (0);
  }
  if (TWI_STATUS == LOST_ARBTRTN)
  {
    uint8_t bufferedStatus = TWI_STATUS;
    lockUp();
    return (bufferedStatus);
  }
  return (TWI_STATUS);
}

/*
 *  Description:
 *      Sends out the address byte. The address byte's first 7 bits are the
 *      7-bit address of the Slave you wish to communicate with, the last bit
 *      specifies if you wish to write or read to that slave,
 *      where 0 = write & 1 = read.
 *  Parameters:
 *      i2cAddress - uint8_t
 *          The address byte you wish to send
 *  Returns:
 *      uint8_t
 *          0: The address byte was successfully sent
 *          1: The function timed out
 *          2 - 0xFF: See the datasheet
 */
uint8_t I2C::_sendAddress(uint8_t i2cAddress)
{
  TWDR = i2cAddress;
  unsigned long startingTime = millis();
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
  {
    if (!timeOutDelay)
    {
      continue;
    }
    if ((millis() - startingTime) >= timeOutDelay)
    {
      lockUp();
      return (1);
    }
  }
  if ((TWI_STATUS == MT_SLA_ACK) || (TWI_STATUS == MR_SLA_ACK))
  {
    return (0);
  }
  uint8_t bufferedStatus = TWI_STATUS;
  if ((TWI_STATUS == MT_SLA_NACK) || (TWI_STATUS == MR_SLA_NACK))
  {
    _stop();
    return (bufferedStatus);
  }
  else
  {
    lockUp();
    return (bufferedStatus);
  }
}

/*
 *  Description:
 *      Sends out a byte of data to the slave.
 *  Parameters:
 *      i2cData - uint8_t
 *          The data byte you wish to send
 *  Returns:
 *      uint8_t
 *          0: The data byte was successfully sent
 *          1: The function timed out
 *          2 - 0xFF: See the datasheet
 */
uint8_t I2C::_sendByte(uint8_t i2cData)
{
  TWDR = i2cData;
  unsigned long startingTime = millis();
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
  {
    if (!timeOutDelay)
    {
      continue;
    }
    if ((millis() - startingTime) >= timeOutDelay)
    {
      lockUp();
      return (1);
    }
  }
  if (TWI_STATUS == MT_DATA_ACK)
  {
    return (0);
  }
  uint8_t bufferedStatus = TWI_STATUS;
  if (TWI_STATUS == MT_DATA_NACK)
  {
    _stop();
    return (bufferedStatus);
  }
  else
  {
    lockUp();
    return (bufferedStatus);
  }
}

/*
 *  Description:
 *      Receives a byte from the slave. The ack parameter specifies whether or
 *      not to send an acknowledge signal after receiving it. If it is the last
 *      byte you want to receive it must be 0, so that the slave will stop
 *      transmitting and allow the master to send a stop.
 *  Parameters:
 *      ack - uint8_t
 *          Whether or not you want the master to acknowledge the receipt of the
 *          data byte. If this is the last byte being received, do not
 *          acknowledge.
 *          0: Do not send an ACK signal on receipt
 *          1 - 0xFF: Send an ACK signal on receipt
 *  Returns:
 *      uint8_t
 *          0: The data byte was successfully received
 *          1: The function timed out
 *          2 - 0xFF: See the datasheet
 */
uint8_t I2C::_receiveByte(uint8_t ack)
{
  unsigned long startingTime = millis();
  if (ack)
  {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
  }
  else
  {
    TWCR = (1 << TWINT) | (1 << TWEN);
  }
  while (!(TWCR & (1 << TWINT)))
  {
    if (!timeOutDelay)
    {
      continue;
    }
    if ((millis() - startingTime) >= timeOutDelay)
    {
      lockUp();
      return (1);
    }
  }
  if (TWI_STATUS == LOST_ARBTRTN)
  {
    uint8_t bufferedStatus = TWI_STATUS;
    lockUp();
    return (bufferedStatus);
  }
  return (TWI_STATUS);
}

/*
 *  Description:
 *      Receives a byte from the slave. The ack parameter specifies whether or
 *      not to send an acknowledge signal after receiving it. If it is the last
 *      byte you want to receive it must be 0, so that the slave will stop
 *      transmitting and allow the master to send a stop.
 *  Parameters:
 *      ack - uint8_t
 *          Whether or not you want the master to acknowledge the receipt of the
 *          data byte. If this is the last byte being received, do not
 *          acknowledge.
 *          0: Do not send an ACK signal on receipt
 *          1 - 0xFF: Send an ACK signal on receipt
 *      *target - uint8_t
 *          A byte to store the received data.
 *  Returns:
 *      uint8_t
 *          0: The data byte was successfully received
 *          6: The function timed out
 *          1 - 5 and 7 - 0xFF: See the datasheet
 */
uint8_t I2C::_receiveByte(uint8_t ack, uint8_t *target)
{
  uint8_t stat = I2C::_receiveByte(ack);
  if (stat == 1)
  {
    return (6);
  }
  if (ack)
  {
    if (stat != MR_DATA_ACK)
    {
      *target = 0x0;
      return (stat);
    }
  }
  else
  {
    if (stat != MR_DATA_NACK)
    {
      *target = 0x0;
      return (stat);
    }
  }
  *target = TWDR;
  // I suppose that if we get this far we're ok
  return 0;
}

/*
 *  Description:
 *      Send out a stop condition
 *  Parameters:
 *      none
 *  Returns:
 *      uint8_t
 *          0: The stop condition was successfully sent
 *          1: The function timed out
 *          2 - 0xFF: See the datasheet
 */
uint8_t I2C::_stop()
{
  unsigned long startingTime = millis();
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
  while ((TWCR & (1 << TWSTO)))
  {
    if (!timeOutDelay)
    {
      continue;
    }
    if ((millis() - startingTime) >= timeOutDelay)
    {
      lockUp();
      return (1);
    }
  }
  return (0);
}

/////////////// Private Methods ////////////////////////////////////////

void I2C::lockUp()
{
  TWCR = 0;                     //releases SDA and SCL lines to high impedance
  TWCR = _BV(TWEN) | _BV(TWEA); //reinitialize TWI
}

I2C I2c = I2C();
