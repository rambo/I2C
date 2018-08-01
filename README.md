# Arduino I2C master library

A better I2C master library, originally by Wayne Truchsess. See http://dsscircuits.com/index.php/articles/66-arduino-i2c-master-library

On most Arduino boards, SDA (data line) is on analog input pin 4, and SCL (clock line) is on analog input pin 5. On the Arduino Mega, SDA is digital pin 20 and SCL is 21. (source: https://www.arduino.cc/en/Reference/Wire)

## Pro tip

For devices that don't use de-facto standard register scheme you can use the low-level methods directly

    I2c._start();
    I2c._sendAddress(slave_addr);
    I2c._sendByte(my_byte);
    I2c._stop();

For more details see the documentation below, section titled: Low-level Methods
## Documentation

### I2c.begin()
<dl>
<dt>Description:</dt>
<dd>Enables the I2C hardware</dd>
    
<dt>Parameters:</dt>
<dd>none</dd>

<dt>Returns:</dt>
<dd>none</dd>
</dl>

### I2c.end()
<dl>
<dt>Description:</dt>
<dd>Disables the I2C hardware</dd>
    
<dt>Parameters:</dt>
<dd>none</dd>

<dt>Returns:</dt>
<dd>none</dd>
</dl>
 

### I2c.setSpeed(fast)
<dl>
<dt>Description:</dt>
<dd>Enables high speed mode (400kHz)</dd>
    
<dt>Parameters:</dt>
<dd>
<b>fast - <i>Boolean</i></b><br/>
<i>True</i>: High Speed<br/>
<i>False</i>: Low Speed<br/>
</dd>

<dt>Returns:</dt>
<dd>none</dd>
</dl> 


### I2c.pullup(activate)
<dl>
<dt>Description:</dt>
<dd>Enables/disables internal pullup resistors</dd>
    
<dt>Parameters:</dt>
<dd>
<b>activate - <i>Boolean</i></b><br/>
<i>True</i>: Enable internal pullup resistors (default)<br/>
<i>False</i>: Disable internal pullup resistors<br/>
</dd>

<dt>Returns:</dt>
<dd>none</dd>
</dl> 

### I2c.timeOut(timeOut)
<dl>
<dt>Description:</dt>
<dd> Allows the user to program a time out limit to prevent and recover from I2C bus lockups.  I2C bus lockups have a tendency to freeze a program which typically requires a power cycle to restart your program. This allows the user to define a time out in which the I2C will release itself and reinitialize and continue on with the next function.  Setting the value to zero will disable the function. On a side note, be careful with setting too low a value because some devices support clock stretching which can increase the time before an acknowledgement is sent which could be misconstrued as a lockup.
    
If a lock up occurs the returned parameters from Read and/or Writes will contain a 1.</dd>
    
<dt>Parameters:</dt>
<dd>
<b>timeOut - <i>uint16_t</i></b><br/>
The amount of time to wait before timing out. Can range from 0 - 65535 milliseconds. If it's set to 0 it will be disabled.
</dd>

<dt>Returns:</dt>
<dd>none</dd>
</dl> 


### I2c.scan()
<dl>
<dt>Description:</dt>
<dd>Scans the bus for I2C devices and reports back each 7 bit address to the Serial Monitor.  The timeout feature was implemented so if there is a problem with the bus during the scan, it will display on Serial Monitor that there was a problem.</dd>
    
<dt>Parameters:</dt>
<dd>none
</dd>

<dt>Returns:</dt>
<dd>none</dd>
</dl> 

### I2c.write(address, registerAddress)
<dl>
<dt>Description:</dt>
<dd>
    Initiate an I2C write operation with no data sent. Typically used to set the "pointer" to a register address
    </br>
    </br>
    <i><b>NOTE:</b> For devices with 16-bit register addresses use <b>I2c.write16(address, registerAddress)</b>. It is identical except registerAddress is a uint16_t</i>
    </dd>
    
<dt>Parameters:</dt>
<dd>
<b>address - <i>uint8_t</i></b><br/>
The 7 bit I2C slave address</dd>
<dd>
<b>registerAddress - <i>uint8_t</i></b><br/>
Address of the register you wish to access (as per the datasheet)</dd>

<dt>Returns:</dt>
<dd>
<b><i>uint8_t</i></b></br>
<i>0:   </i>   Function executed with no errors</br>
<i>1:   </i>   Function timed out waiting for successful completion of a Start bit</br>
<i>2:   </i>   Function timed out waiting for ACK/NACK while addressing slave in transmit mode (MT)</br>
<i>3:   </i>   Function timed out waiting for ACK/NACK while sending data to the slave</br>
<i>4:   </i>   Function timed out waiting for successful completion of a Repeated Start</br>
<i>5:   </i>   Function timed out waiting for ACK/NACK while addressing slave in receiver mode (MR)</br>
<i>6:   </i>   Function timed out waiting for ACK/NACK while receiving data from the slave</br>
<i>7:   </i>   Function timed out waiting for successful completion of the Stop bit</br>
<i>8 - 0xFF:    </i> See datasheet for exact meaning</br>
</dd>
</dl> 

### I2c.write(address, registerAddress, data)
<dl>
<dt>Description:</dt>
<dd>Initiate an I2C write operation, sending a single data byte. Typically used to send a single byte of data to a register address
    </br>
    </br>
    <i><b>NOTE:</b> For devices with 16-bit register addresses use <b>I2c.write16(address, registerAddress, data)</b>. It is identical except registerAddress is a uint16_t</i></dd>
    
<dt>Parameters:</dt>
<dd>
<b>address - <i>uint8_t</i></b><br/>
The 7 bit I2C slave address</dd>
<dd>
<b>registerAddress - <i>uint8_t</i></b><br/>
Address of the register you wish to access (as per the datasheet)</dd>
<dd>
<b>data - <i>uint8_t</i></b><br/>
A single byte of data to send</dd>


<dt>Returns:</dt>
<dd>
<b><i>uint8_t</i></b></br>
<i>0:   </i>   Function executed with no errors</br>
<i>1:   </i>   Function timed out waiting for successful completion of a Start bit</br>
<i>2:   </i>   Function timed out waiting for ACK/NACK while addressing slave in transmit mode (MT)</br>
<i>3:   </i>   Function timed out waiting for ACK/NACK while sending data to the slave</br>
<i>4:   </i>   Function timed out waiting for successful completion of a Repeated Start</br>
<i>5:   </i>   Function timed out waiting for ACK/NACK while addressing slave in receiver mode (MR)</br>
<i>6:   </i>   Function timed out waiting for ACK/NACK while receiving data from the slave</br>
<i>7:   </i>   Function timed out waiting for successful completion of the Stop bit</br>
<i>8 - 0xFF:    </i> See datasheet for exact meaning</br>
</dd>
</dl> 


### I2c.write(address, registerAddress, \*data)
<dl>
<dt>Description:</dt>
<dd>Initiate an I2C write operation, array of char. Typically used to send an array of char starting at registerAddress location.  As a side note there is no restriction on how many bytes may be sent unlike the Wire library which has a 32 byte restriction
    </br>
    </br>
    <i><b>NOTE:</b> For devices with 16-bit register addresses use <b>I2c.write16(address, registerAddress, *data)</b>. It is identical except registerAddress is a uint16_t</i></dd>
    
<dt>Parameters:</dt>
<dd>
<b>address - <i>uint8_t</i></b><br/>
The 7 bit I2C slave address</dd>
<dd>
<b>registerAddress - <i>uint8_t</i></b><br/>
Address of the register you wish to access (as per the datasheet)</dd>
<dd>
<b>*data - <i>char</i></b><br/>
Array of characters</dd>


<dt>Returns:</dt>
<dd>
<b><i>uint8_t</i></b></br>
<i>0:   </i>   Function executed with no errors</br>
<i>1:   </i>   Function timed out waiting for successful completion of a Start bit</br>
<i>2:   </i>   Function timed out waiting for ACK/NACK while addressing slave in transmit mode (MT)</br>
<i>3:   </i>   Function timed out waiting for ACK/NACK while sending data to the slave</br>
<i>4:   </i>   Function timed out waiting for successful completion of a Repeated Start</br>
<i>5:   </i>   Function timed out waiting for ACK/NACK while addressing slave in receiver mode (MR)</br>
<i>6:   </i>   Function timed out waiting for ACK/NACK while receiving data from the slave</br>
<i>7:   </i>   Function timed out waiting for successful completion of the Stop bit</br>
<i>8 - 0xFF:    </i> See datasheet for exact meaning</br>
</dd>
</dl> 


### I2c.write(address, registerAddress, \*data, numberBytes)
<dl>
<dt>Description:</dt>
<dd>Initiate an I2C write operation, array of bytes. Typically used to send an array of bytes starting at registerAddress location. As a side note there is no restriction on how many bytes may be sent unlike the Wire library which has a 32 byte restriction
    </br>
    </br>
    <i><b>NOTE:</b> For devices with 16-bit register addresses use <b>I2c.write16(address, registerAddress, *data, numberBytes)</b>. It is identical except registerAddress is a uint16_t</i></dd>
    
<dt>Parameters:</dt>
<dd>
<b>address - <i>uint8_t</i></b><br/>
The 7 bit I2C slave address</dd>
<dd>
<b>registerAddress - <i>uint8_t</i></b><br/>
Address of the register you wish to access (as per the datasheet)</dd>
<dd>
<b>*data - <i>uint8_t</i></b><br/>
Array of bytes</dd>
<dd>
<b>numberBytes - <i>uint8_t</i></b><br/>
The number of bytes in the array to be sent</dd>

<dt>Returns:</dt>
<dd>
<b><i>uint8_t</i></b></br>
<i>0:   </i>   Function executed with no errors</br>
<i>1:   </i>   Function timed out waiting for successful completion of a Start bit</br>
<i>2:   </i>   Function timed out waiting for ACK/NACK while addressing slave in transmit mode (MT)</br>
<i>3:   </i>   Function timed out waiting for ACK/NACK while sending data to the slave</br>
<i>4:   </i>   Function timed out waiting for successful completion of a Repeated Start</br>
<i>5:   </i>   Function timed out waiting for ACK/NACK while addressing slave in receiver mode (MR)</br>
<i>6:   </i>   Function timed out waiting for ACK/NACK while receiving data from the slave</br>
<i>7:   </i>   Function timed out waiting for successful completion of the Stop bit</br>
<i>8 - 0xFF:    </i> See datasheet for exact meaning</br>
</dd>
</dl> 

### I2c.read(address, numberBytes)
<dl>
<dt>Description:</dt>
<dd>Initiate a read operation from the current position of slave register pointer. The bytes will be stored in an internal buffer and will have the 32 byte size restriction.  Data can be read out of the buffer using I2c.receive().</dd>
    
<dt>Parameters:</dt>
<dd>
<b>address - <i>uint8_t</i></b><br/>
The 7 bit I2C slave address</dd>
<dd>
<b>numberBytes - <i>uint8_t</i></b><br/>
The number of bytes to be read</dd>

<dt>Returns:</dt>
<dd>
<b><i>uint8_t</i></b></br>
<i>0:   </i>   Function executed with no errors</br>
<i>1:   </i>   Function timed out waiting for successful completion of a Start bit</br>
<i>2:   </i>   Function timed out waiting for ACK/NACK while addressing slave in transmit mode (MT)</br>
<i>3:   </i>   Function timed out waiting for ACK/NACK while sending data to the slave</br>
<i>4:   </i>   Function timed out waiting for successful completion of a Repeated Start</br>
<i>5:   </i>   Function timed out waiting for ACK/NACK while addressing slave in receiver mode (MR)</br>
<i>6:   </i>   Function timed out waiting for ACK/NACK while receiving data from the slave</br>
<i>7:   </i>   Function timed out waiting for successful completion of the Stop bit</br>
<i>8 - 0xFF:    </i> See datasheet for exact meaning</br></br>

Unlike the Wire library the read operation will not return the number of bytes read, instead it will return the error code which can be used for debugging.
</dd>
</dl> 

### I2c.read(address, numberBytes, \*dataBuffer)
<dl>
<dt>Description:</dt>
<dd>Initiate a read operation from the current position of slave register pointer. The bytes will be stored in the dataBuffer. As a side note there is no restriction on how many bytes may be received unlike the Wire library which has a 32 byte restriction</dd>
    
<dt>Parameters:</dt>
<dd>
<b>address - <i>uint8_t</i></b><br/>
The 7 bit I2C slave address</dd>
<dd>
<b>numberBytes - <i>uint8_t</i></b><br/>
The number of bytes to be read</dd>
<dd>
<b>*dataBuffer - <i>uint8_t</i></b><br/>
Array to store the read data</dd>

<dt>Returns:</dt>
<dd>
<b><i>uint8_t</i></b></br>
<i>0:   </i>   Function executed with no errors</br>
<i>1:   </i>   Function timed out waiting for successful completion of a Start bit</br>
<i>2:   </i>   Function timed out waiting for ACK/NACK while addressing slave in transmit mode (MT)</br>
<i>3:   </i>   Function timed out waiting for ACK/NACK while sending data to the slave</br>
<i>4:   </i>   Function timed out waiting for successful completion of a Repeated Start</br>
<i>5:   </i>   Function timed out waiting for ACK/NACK while addressing slave in receiver mode (MR)</br>
<i>6:   </i>   Function timed out waiting for ACK/NACK while receiving data from the slave</br>
<i>7:   </i>   Function timed out waiting for successful completion of the Stop bit</br>
<i>8 - 0xFF:    </i> See datasheet for exact meaning</br></br>

Unlike the Wire library the read operation will not return the number of bytes read, instead it will return the error code which can be used for debugging.
</dd>
</dl> 

### I2c.read(address, registerAddress, numberBytes)
<dl>
<dt>Description:</dt>
<dd>Initiate a write operation to set the pointer to the registerAddress, then sending a repeated start (not a stop then start) and store the number of bytes in an internal buffer.  The 32 byte size restriction is imposed for this function.  Data can be read out of the buffer using I2c.receive().
    </br>
    </br>
    <i><b>NOTE:</b> For devices with 16-bit register addresses use <b>I2c.read16(address, registerAddress, numberBytes)</b>. It is identical except registerAddress is a uint16_t</i></dd>
    
<dt>Parameters:</dt>
<dd>
<b>address - <i>uint8_t</i></b><br/>
The 7 bit I2C slave address</dd>
<dd>
<b>registerAddress - <i>uint8_t</i></b><br/>
Starting register address to read data from</dd>
<dd>
<b>numberBytes - <i>uint8_t</i></b><br/>
The number of bytes to be read</dd>


<dt>Returns:</dt>
<dd>
<b><i>uint8_t</i></b></br>
<i>0:   </i>   Function executed with no errors</br>
<i>1:   </i>   Function timed out waiting for successful completion of a Start bit</br>
<i>2:   </i>   Function timed out waiting for ACK/NACK while addressing slave in transmit mode (MT)</br>
<i>3:   </i>   Function timed out waiting for ACK/NACK while sending data to the slave</br>
<i>4:   </i>   Function timed out waiting for successful completion of a Repeated Start</br>
<i>5:   </i>   Function timed out waiting for ACK/NACK while addressing slave in receiver mode (MR)</br>
<i>6:   </i>   Function timed out waiting for ACK/NACK while receiving data from the slave</br>
<i>7:   </i>   Function timed out waiting for successful completion of the Stop bit</br>
<i>8 - 0xFF:    </i> See datasheet for exact meaning</br></br>

Unlike the Wire library the read operation will not return the number of bytes read, instead it will return the error code which can be used for debugging.
</dd>
</dl> 

### I2c.read(address, registerAddress, numberBytes, \*dataBuffer)
<dl>
<dt>Description:</dt>
<dd>Initiate a write operation to set the pointer to the registerAddress, then sending a repeated start (not a stop then start) and store the number of bytes in the dataBuffer. As a side note there is no restriction on how many bytes may be received unlike the Wire library which has a 32 byte restriction
    </br>
    </br>
    <i><b>NOTE:</b> For devices with 16-bit register addresses use <b>I2c.read16(address, registerAddress, numberBytes, \*dataBuffer)</b>. It is identical except registerAddress is a uint16_t</i></dd>
    
<dt>Parameters:</dt>
<dd>
<b>address - <i>uint8_t</i></b><br/>
The 7 bit I2C slave address</dd>
<dd>
<b>registerAddress - <i>uint8_t</i></b><br/>
Starting register address to read data from</dd>
<dd>
<b>numberBytes - <i>uint8_t</i></b><br/>
The number of bytes to be read</dd>
<dd>
<b>*dataBuffer - <i>uint8_t</i></b><br/>
An array to store the read data</dd>

<dt>Returns:</dt>
<dd>
<b><i>uint8_t</i></b></br>
<i>0:   </i>   Function executed with no errors</br>
<i>1:   </i>   Function timed out waiting for successful completion of a Start bit</br>
<i>2:   </i>   Function timed out waiting for ACK/NACK while addressing slave in transmit mode (MT)</br>
<i>3:   </i>   Function timed out waiting for ACK/NACK while sending data to the slave</br>
<i>4:   </i>   Function timed out waiting for successful completion of a Repeated Start</br>
<i>5:   </i>   Function timed out waiting for ACK/NACK while addressing slave in receiver mode (MR)</br>
<i>6:   </i>   Function timed out waiting for ACK/NACK while receiving data from the slave</br>
<i>7:   </i>   Function timed out waiting for successful completion of the Stop bit</br>
<i>8 - 0xFF:    </i> See datasheet for exact meaning</br></br>

Unlike the Wire library the read operation will not return the number of bytes read, instead it will return the error code which can be used for debugging.
</dd>
</dl> 

### I2c.available()
<dl>
<dt>Description:</dt>
<dd>Returns the number of unread bytes stored in the internal 32 byte buffer</dd>
    
<dt>Parameters:</dt>
<dd>none</dd>

<dt>Returns:</dt>
<dd>
<b><i>uint8_t</i></b></br>
The number of unread bytes
</dd>
</dl> 

### I2c.receive()
<dl>
<dt>Description:</dt>
<dd>Returns the first unread byte of the internal buffer.</dd>
    
<dt>Parameters:</dt>
<dd>none</dd>

<dt>Returns:</dt>
<dd>
<b><i>uint8_t</i></b></br>
First unread byte of the internal buffer
</dd>
</dl> 


## Low-level methods

### I2c.\_start()
<dl>
<dt>Description:</dt>
<dd>Sends out a Start Condition. This puts all slave devices on notice that a transmission is about to start. This function incorporates the timeOut function.</dd>
    
<dt>Parameters:</dt>
<dd>none</dd>

<dt>Returns:</dt>
<dd>
<b><i>uint8_t</i></b></br>
<i>0:</i> The start condition was successfully sent</br>
<i>1:</i> The function timed out</br>
<i>2 - 0xFF:</i> See the datasheet
</dd>
</dl> 

### I2c.\_sendAddress(i2cAddress)
<dl>
<dt>Description:</dt>
<dd>Sends out the address byte. The address byte's first 7 bits are the 7-bit address of the Slave you wish to communicate with, the last bit specifies if you wish to write or read to that slave, 0 = write & 1 = read.</dd>
    
<dt>Parameters:</dt>
<dd>
<b>i2cAddress - <i>uint8_t</i></b><br/>
The address byte you wish to send</dd>

<dt>Returns:</dt>
<dd>
<b><i>uint8_t</i></b></br>
<i>0:</i> The address byte was successfully sent</br>
<i>1:</i> The function timed out</br>
<i>2 - 0xFF:</i> See the datasheet
</dd>
</dl> 


### I2c.\_sendByte(i2cData)
<dl>
<dt>Description:</dt>
<dd>Sends out a byte of data to the slave.</dd>
    
<dt>Parameters:</dt>
<dd>
<b>i2cData - <i>uint8_t</i></b><br/>
The data byte you wish to send</dd>

<dt>Returns:</dt>
<dd>
<b><i>uint8_t</i></b></br>
<i>0:</i> The data byte was successfully sent</br>
<i>1:</i> The function timed out</br>
<i>2 - 0xFF:</i> See the datasheet
</dd>
</dl> 

### I2c.\_receiveByte(ack)
<dl>
<dt>Description:</dt>
<dd>Receives a byte from the slave. The ack parameter specifies whether or not to send an acknowledge signal after receiving it. If it is the last byte you want to receive it must be 0, so that the slave will stop transmitting and allow the master to send a stop.</dd>
    
<dt>Parameters:</dt>
<dd>
<b>ack - <i>uint8_t</i></b><br/>
    Whether or not you want the master to acknowledge the receipt of the data byte. If this is the last byte being received, do not acknowledge.<br/>
<i>0:</i> Do not send an ACK signal on receipt<br/>
<i>1 - 0xFF:</i> Send an ACK signal on receipt<br/>
</dd>

<dt>Returns:</dt>
<dd>
<b><i>uint8_t</i></b></br>
<i>0:</i> The data byte was successfully received</br>
<i>1:</i> The function timed out</br>
<i>2 - 0xFF:</i> See the datasheet
</dd>
</dl> 

### I2c.\_receiveByte(ack, \*target)
<dl>
<dt>Description:</dt>
<dd>Receives a byte from the slave. The ack parameter specifies whether or not to send an acknowledge signal after receiving it. If it is the last byte you want to receive it must be 0, so that the slave will stop transmitting and allow the master to send a stop.</dd>
    
<dt>Parameters:</dt>
<dd>
<b>ack - <i>uint8_t</i></b><br/>
    Whether or not you want the master to acknowledge the receipt of the data byte. If this is the last byte being received, do not acknowledge.<br/>
<i>0:</i> Do not send an ACK signal on receipt<br/>
<i>1 - 0xFF:</i> Send an ACK signal on receipt<br/>
</dd>
<dd>
<b>*target - <i>uint8_t</i></b><br/>
    A byte to store the received data.
</dd>

<dt>Returns:</dt>
<dd>
<b><i>uint8_t</i></b></br>
<i>0:</i> The data byte was successfully received</br>
<i>6:</i> The function timed out</br>
<i>1 - 5 and 7 - 0xFF:</i> See the datasheet
</dd>
</dl> 


### I2c.\_stop()
<dl>
<dt>Description:</dt>
<dd>Send out a stop condition</dd>
    
<dt>Parameters:</dt>
<dd>none</dd>

<dt>Returns:</dt>
<dd>
<b><i>uint8_t</i></b></br>
<i>0:</i> The stop condition was successfully sent</br>
<i>1:</i> The function timed out</br>
<i>2 - 0xFF:</i> See the datasheet
</dd>
</dl> 
