// Get this from https://github.com/rambo/I2C
#define I2C_DEVICE_DEBUG
#include <I2C.h> // For some weird reason including this in the relevant .h file does not work
#define MAX_COMMAND_LENGTH 100 // null-terminated
char incoming_command[MAX_COMMAND_LENGTH+2]; //Reserve space for CRLF too.
byte incoming_position;

/**
 * Basic idea is to have "REPL" for low-level I2C operations
 *
 * [ = start
 * ] = stop
 * a1 (hex numbers) are bytes
 *
 * On newline the line is parsed and corresponding actions taken, we need to know if sending a byte right after start since
 * the slave address requires extra attention.
 *
 * At first simply usable with the arduino serial console, so no echo, later echo, backspace and maybe command history (RAM permitting)
 */

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


// Handle incoming Serial data, try to find a command in there
inline void read_command_bytes()
{
    for (byte d = Serial.available(); d > 0; d--)
    {
        incoming_command[incoming_position] = Serial.read();
        // Check for line end and in such case do special things
        if (   incoming_command[incoming_position] == 0xA // LF
            || incoming_command[incoming_position] == 0xD) // CR
        {
            incoming_command[incoming_position] = 0x0;
            if (   incoming_position > 0
                && (   incoming_command[incoming_position-1] == 0xD // CR
                    || incoming_command[incoming_position-1] == 0xA) // LF
               )
            {
                incoming_command[incoming_position-1] = 0x0;
            }
            process_command();
            // Clear the buffer and reset position to 0
            memset(&incoming_command, 0, MAX_COMMAND_LENGTH+2);
            incoming_position = 0;
            return;
        }
        incoming_position++;

        // Sanity check buffer sizes
        if (incoming_position > MAX_COMMAND_LENGTH+2)
        {
            Serial.println(0x15); // NACK
            Serial.print("PANIC: No end-of-line seen and incoming_position=");
            Serial.print(incoming_position, DEC);
            Serial.println(" clearing buffers");
            
            memset(&incoming_command, 0, MAX_COMMAND_LENGTH+2);
            incoming_position = 0;
        }
    }
}


enum parser_states {
    start_seen,
    stop_seen,
    in_hex,
    p_idle,
};
inline void process_command()
{
    byte parser_state = p_idle;
    byte next_parser_state = p_idle; // might not be needed 
    byte maxsize = strlen(incoming_command);
    for(byte i=0; i < maxsize; i++)
    {
        byte current_char = incoming_command[i];
        switch (parser_state)
        {
            case start_seen:
            {
            }
                break;
            case stop_seen:
            {
            }
                break;
            case in_hex:
            {
            }
                break;
            case p_idle:
            {
                switch (current_char)
                {
                    case 0x5b: // ASCII "[", our start signifier
                    {
                        I2c.start();
                        parser_state = start_seen;
                    }
                        break;
                    case 0x5d: // ASCII "]", our stop signifier
                    {
                        I2c.stop();
                        parser_state = stop_seen;
                    }
                        break;
                }
                if (   (   current_char >= 0x30
                        && current_char <= 0x39) // 0-9
                    || (   current_char >= 0x61
                        && current_char <= 0x66) // a-f
                    || (   current_char >= 0x41
                        && current_char <= 0x46) // A-F
                   )
                {
                    
                    parser_state = in_hex;
                }
            }
                break;
        }
    }
}

void loop()
{
    read_command_bytes();
}
