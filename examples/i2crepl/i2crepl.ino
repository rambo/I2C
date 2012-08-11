// Get this from https://github.com/rambo/I2C
#define I2C_DEVICE_DEBUG
#include <I2C.h> // For some weird reason including this in the relevant .h file does not work

void setup()
{
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    Serial.begin(115200);
    // Initialize I2C library manually
    I2c.begin();
    I2c.timeOut(500);
    I2c.pullup(true);

    // Scan the bus
    I2c.scan();
    Serial.println("Booted");
    digitalWrite(13, LOW);
}

byte device_address = 0x4;
byte devdata[] = { 0x10, 0x20, 0x20 };
void loop()
{
    // TODO: Write the serial parser and REPL
    /*
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(500);
    */
    byte result = I2c.write(device_address, devdata[0], &devdata[1], 2);
    if (result > 0)
    {
        Serial.print("Write failed, I2c.write returned: ");
        Serial.println(result, DEC);
    }
    else
    {
        Serial.print("wrote to 0x");
        Serial.print(device_address, HEX);
        Serial.print(": { ");
        for (byte i = 0; i < sizeof(devdata); i++)
        {
            Serial.print(" 0x");
            Serial.print(devdata[i], HEX);
        }
        Serial.println(" }");
    }
    delay(2000);
    devdata[1] = devdata[1] + 50;
    devdata[2] = devdata[2] + 50;
}
