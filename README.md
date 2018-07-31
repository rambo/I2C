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
