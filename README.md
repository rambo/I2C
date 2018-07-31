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
