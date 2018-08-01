# Arduino I2C master library

A better I2C master library, originally by Wayne Truchsess. See http://dsscircuits.com/index.php/articles/66-arduino-i2c-master-library

On most Arduino boards, SDA (data line) is on analog input pin 4, and SCL (clock line) is on analog input pin 5. On the Arduino Mega, SDA is digital pin 20 and SCL is 21. (source: https://www.arduino.cc/en/Reference/Wire)

## Pro tip

For devices that don't use de-facto standard register scheme you can use the low-level methods directly

    I2c.start();
    I2c.sendAddress(slave_addr);
    I2c.sendByte(my_byte);
    I2c.stop();
    
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
<dd> Allows the user to program a time out limit to prevent and recover from I2C bus lockups.  I2C bus lockups have a tendency to freeze a program which typically requires a power cycle to restart your progrm. This allows the user to define a time out in which the I2C will release itself and reinitialize and continue on with the next function.  Setting the value to zero will disable the function. On a side note, be careful with setting too low a value because some devices support clock stretching which can increase the time before an acknowledgement is sent which could be misconstrued as a lockup.
    
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
<dd>Initate an I2C write operation with no data sent. Typically used to set the "pointer" to a register address</dd>
    
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
<dd>Initate an I2C write operation,sending a single data byte. Typically used to send a single byte of data to a register address</dd>
    
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
<dd>Initate an I2C write operation, array of char. Typically used to send an array of char starting at registerAddress location.  As a side note there is no restriction on how many bytes may be sent unlike the Wire library which has a 32 byte restriction</dd>
    
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
<dd>Initate an I2C write operation, array of bytes. Typically used to send an array of bytes starting at registerAddress location. As a side note there is no restriction on how many bytes may be sent unlike the Wire library which has a 32 byte restriction</dd>
    
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
<dd>Initiate a write operation to set the pointer to the registerAddress, then sending a repeated start (not a stop then start) and store the number of bytes in an internal buffer.  The 32 byte size restriction is imposed for this function.  Data can be read out of the buffer using I2c.receive().</dd>
    
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
<dd>Initiate a write operation to set the pointer to the registerAddress, then sending a repeated start (not a stop then sta  and store the number of bytes in the dataBuffer. As a side note there is no restriction on how many bytes may be received unlike the Wire library which has a 32 byte restriction</dd>
    
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
An array to stire the read data</dd>

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
