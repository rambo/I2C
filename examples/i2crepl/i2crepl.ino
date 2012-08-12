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
 * Working:
 *  - start / stop
 *  - hex parsing (mostly)
 *
 * TODO:
 *  - fix hex parsing for two-character values starting with 0 (it seems to fail)
 *  - REPL so this can be used via plain serial port as well
 *  - Smarter number parsing (0x to signify hex, othewise suppose decimal)
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

inline boolean is_hex_char(byte current_char)
{
    if (   (   current_char >= 0x30
            && current_char <= 0x39) // 0-9
        || (   current_char >= 0x61
            && current_char <= 0x66) // a-f
        || (   current_char >= 0x41
            && current_char <= 0x46) // A-F
       )
    {
        return true;
    }
    return false;
}

/**
 * Parses ASCII [0-9A-F] hexadecimal to byte value
 */
inline byte ardubus_hex2byte(byte hexchar)
{
    if (   0x40 < hexchar
        && hexchar < 0x47) // A-F
    {
        return (hexchar - 0x41) + 10; 
    }
    if (   0x2f < hexchar
        && hexchar < 0x3a) // 0-9
    {
        return (hexchar - 0x30);
    }
    return 0x0; // Failure.
    
}

inline byte ardubus_hex2byte(byte hexchar0, byte hexchar1)
{
    return (ardubus_hex2byte(hexchar0) << 4) | ardubus_hex2byte(hexchar1);
}

inline int ardubus_hex2int(byte hexchar0, byte hexchar1, byte hexchar2, byte hexchar3)
{
    return ardubus_hex2byte(hexchar0, hexchar1) << 8 | ardubus_hex2byte(hexchar2, hexchar3);
}

inline byte parse_hex(char *parsebuffer)
{
    byte len = strlen(parsebuffer);
    if (len > 2)
    {
        return 0x0;
    }
    if (len == 2)
    {
        return ardubus_hex2byte(parsebuffer[0], parsebuffer[1]);
    }
    return ardubus_hex2byte(parsebuffer[0]);
}

inline void process_command()
{
    char hexparsebuffer[5];
    byte hexparsebuffer_i = 0;
    byte parser_state = p_idle;
    byte next_parser_state = p_idle; // might not be needed 
    byte prev_parser_state = p_idle; // might not be needed 
    byte maxsize = strlen(incoming_command);
    for(byte i=0; i < maxsize; i++)
    {
        byte current_char = incoming_command[i];
        switch (parser_state)
        {
            case start_seen:
            {
                prev_parser_state = parser_state;
                if (current_char == 0x20) // space
                {
                    parser_state = p_idle;
                }
                else if (is_hex_char(current_char)) // Allow also hex directly after start
                {
                    parser_state = in_hex;
                    hexparsebuffer[hexparsebuffer_i++] = current_char; 
                }
                else
                {
                    // Invalid char
                    Serial.print("Invalid character in position ");
                    Serial.print(i, DEC);
                    Serial.println(" when parsing command");
                    return;
                }
            }
                break;
            case stop_seen:
            {
                // after stop we only allow space to return to idle
                if (current_char == 0x20) // space
                {
                    prev_parser_state = parser_state;
                    parser_state = p_idle;
                }
                else
                {
                    // Invalid char
                    Serial.print("Invalid character in position ");
                    Serial.print(i, DEC);
                    Serial.println(" when parsing command");
                    return;
                }
            }
                break;
            case in_hex:
            {
                if (is_hex_char(current_char))
                {
                    hexparsebuffer[hexparsebuffer_i++] = current_char;
                    if (hexparsebuffer_i > 2)
                    {
                        Serial.println("Can only have byte wide hex strings");
                        return;
                    }
                }
                else if (current_char == 0x20) // space
                {
                    byte parsed_byte = parse_hex(&hexparsebuffer[0]);
                    byte stat;
                    if (prev_parser_state == start_seen)
                    {
                        stat = I2c.sendAddress(parsed_byte);
                        Serial.print("sendAddress");
                    }
                    else
                    {
                        stat = I2c.sendByte(parsed_byte);
                        Serial.print("sendByte");
                    }
                    Serial.print("(0x");
                    Serial.print(parsed_byte, HEX);
                    Serial.print(") returned: ");
                    Serial.println(stat, DEC);
                    // Clear buffer
                    memset(&hexparsebuffer, 0, sizeof(hexparsebuffer));
                    hexparsebuffer_i = 0;
                    // Return state to idle
                    prev_parser_state = parser_state;
                    parser_state = p_idle;
                }
                else
                {
                    // Invalid char
                    Serial.print("Invalid character in position ");
                    Serial.print(i, DEC);
                    Serial.println(" when parsing command");
                    return;
                }
            }
                break;
            case p_idle:
            {
                switch (current_char)
                {
                    case 0x5b: // ASCII "[", our start signifier
                    {
                        byte stat = I2c.start();
                        parser_state = start_seen;
                        Serial.print("START returned ");
                        Serial.println(stat, DEC);
                    }
                        break;
                    case 0x5d: // ASCII "]", our stop signifier
                    {
                        byte stat = I2c.stop();
                        Serial.print("STOP returned ");
                        Serial.println(stat, DEC);
                        parser_state = stop_seen;
                    }
                        break;
                }
                if (is_hex_char(current_char))
                {
                    parser_state = in_hex;
                    hexparsebuffer[hexparsebuffer_i++] = current_char; 
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
