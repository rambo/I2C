# Arduino I2C master library

A better I2C master library, originally by Wayne Truchsess. See http://dsscircuits.com/index.php/articles/66-arduino-i2c-master-library

On most Arduino boards, SDA (data line) is on analog input pin 4, and SCL (clock line) is on analog input pin 5. On the Arduino Mega, SDA is digital pin 20 and SCL is 21. (source: http://arduino.cc/it/Reference/Wire)

## Pro tip

For devices that don't use de-facto standard register scheme you can use the low-level methods directly

    I2c.start();
    I2c.sendAddress(slave_addr);
    I2c.sendByte(my_byte);
    I2c.stop();
