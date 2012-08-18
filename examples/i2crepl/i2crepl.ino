// Get this from https://github.com/rambo/I2C
#define I2C_DEVICE_DEBUG
#include <I2C.h>
#define MAX_COMMAND_LENGTH 100 // null-terminated
char incoming_command[MAX_COMMAND_LENGTH+2]; //Reserve space for CRLF too.
byte incoming_position;

/**
 * Basic idea is to have "REPL" for low-level I2C operations
 *
 * [ -> start
 * ] -> stop
 * a1 -> hex numbers are bytes
 * r -> read one byte
 * = -> address calculator (for example "=4")
 * S -> scan I2C address space
 *
 * On newline the line is parsed and corresponding actions taken, we need to know if sending a byte right after start since
 * the slave address requires extra attention.
 *
 * Working:
 *  - start / stop
 *  - hex parsing (mostly) and sending bytes
 *  - reading
 *  - address scan
 *
 * TODO:
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
    //I2c.scan();
    Serial.println(F("Remember that you need to send the 8-bit address (with R/W-bit set) when addressing a device"));
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
            memset(incoming_command, 0, MAX_COMMAND_LENGTH+2);
            incoming_position = 0;
            return;
        }
        incoming_position++;

        // Sanity check buffer sizes
        if (incoming_position > MAX_COMMAND_LENGTH+2)
        {
            Serial.println(0x15); // NACK
            Serial.print(F("PANIC: No end-of-line seen and incoming_position="));
            Serial.print(incoming_position, DEC);
            Serial.println(F(" clearing buffers"));
            
            memset(incoming_command, 0, MAX_COMMAND_LENGTH+2);
            incoming_position = 0;
        }
    }
}


enum parser_states {
    start_seen,
    stop_seen,
    in_hex,
    p_idle,
    calc_seen,
    scan_seen,
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
    if (   0x60 < hexchar
        && hexchar < 0x67) // a-f
    {
        return (hexchar - 0x61) + 10; 
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
    /*
    Serial.print(F("DEBUG: hexbuffer len="));
    Serial.println(len, DEC);
    */
    if (len > 2)
    {
        //Serial.println(parsebuffer);
        return 0xff;
    }
    if (len == 2)
    {
        return ardubus_hex2byte(parsebuffer[0], parsebuffer[1]);
    }
    return ardubus_hex2byte(parsebuffer[0]);
}

void invalid_char(byte character, byte pos)
{
    Serial.print(F("Invalid character '"));
    Serial.write(character);
    Serial.print(F("' (0x"));
    Serial.print(character, HEX);
    Serial.print(F(") in position "));
    Serial.print(pos, DEC);
    Serial.println(F(" when parsing command"));
}


inline void process_command()
{
    char hexparsebuffer[5];
    // Clear buffer
    memset(hexparsebuffer, 0, sizeof(hexparsebuffer));
    volatile byte hexparsebuffer_i = 0;
    byte parser_state = p_idle;
    byte next_parser_state = p_idle; // might not be needed 
    byte prev_parser_state = p_idle; // might not be needed 
    byte maxsize = strlen(incoming_command);
    for(byte i=0; i < maxsize; i++)
    {
        byte current_char = incoming_command[i];
        switch (parser_state)
        {
            case calc_seen:
            {
                prev_parser_state = parser_state;
                if (is_hex_char(current_char))
                {
                    parser_state = in_hex;
                    i--; // muck i so we re-enter at some point to the hex parser
                }
                else
                {
                    Serial.print(F("calc_seen: "));
                    invalid_char(current_char, i);
                    return;
                }
            }
                break;
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
                    i--; // muck i so we re-enter at some point to the hex parser
                }
                else
                {
                    Serial.print(F("start_seen: "));
                    invalid_char(current_char, i);
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
                    Serial.print(F("stop_seen: "));
                    invalid_char(current_char, i);
                    return;
                }
            }
                break;
            case in_hex:
            {
                boolean is_valid_char = false;
                if (is_hex_char(current_char))
                {
                    is_valid_char = true;
                    hexparsebuffer[hexparsebuffer_i++] = current_char;
                    if (hexparsebuffer_i > 2)
                    {
                        Serial.println(F("Can only have byte wide hex strings"));
                        return;
                    }
                }
                if (   current_char == 0x20 // space
                    || (i == (maxsize-1)))  // end of string
                {
                    is_valid_char = true;
                    byte parsed_byte = parse_hex(hexparsebuffer);
                    // Clear buffer
                    memset(hexparsebuffer, 0, sizeof(hexparsebuffer));
                    hexparsebuffer_i = 0;
                    // I2C status code
                    byte stat;
                    boolean i2c_sent = true;
                    switch (prev_parser_state)
                    {
                        case calc_seen:
                            i2c_sent = false;
                            Serial.print(F("device 0x"));
                            Serial.print(parsed_byte, HEX);
                            Serial.print(F(": read 0x"));
                            Serial.print(((parsed_byte << 1) | 0x1), HEX);
                            Serial.print(F(" write 0x"));
                            Serial.println(((parsed_byte << 1) | 0x0), HEX);
                            break;
                        case start_seen:
                            stat = I2c.sendAddress(parsed_byte);
                            Serial.print(F("sendAddress"));
                            break;
                        default:
                            stat = I2c.sendByte(parsed_byte);
                            Serial.print(F("sendByte"));
                            break;
                    }
                    if (i2c_sent)
                    {
                        Serial.print(F("(0x"));
                        Serial.print(parsed_byte, HEX);
                        Serial.print(F("), stat=0x"));
                        Serial.println(stat, HEX);
                    }
                    // Return state to idle
                    prev_parser_state = parser_state;
                    parser_state = p_idle;
                }
                if (!is_valid_char)
                {
                    Serial.print(F("in_hex: "));
                    invalid_char(current_char, i);
                    return;
                }
            }
                break;
            case p_idle:
            {
                boolean is_valid_char = false;
                switch (current_char)
                {
                    case 0x3d: // ASCII "="
                    {
                        is_valid_char = true;
                        parser_state = calc_seen;
                    }
                        break;
                    case 0x53: // ASCII "S"
                    {
                        is_valid_char = true;
                        if (prev_parser_state != p_idle)
                        {
                            Serial.println(F("Address scan cannot be done in the middle of I2C transaction"));
                            return;
                        }
                        I2c.scan();
                        Serial.println(F(""));
                        Serial.println(F("Scan done."));
                    }
                        break;
                    case 0x20: // space
                        is_valid_char = true;
                        break;
                    case 0x5b: // ASCII "[", our start signifier
                    {
                        is_valid_char = true;
                        byte stat = I2c.start();
                        parser_state = start_seen;
                        Serial.print(F("START sent, stat=0x"));
                        Serial.println(stat, HEX);
                    }
                        break;
                    case 0x5d: // ASCII "]", our stop signifier
                    {
                        is_valid_char = true;
                        byte stat = I2c.stop();
                        Serial.print(F("STOP sent, stat=0x"));
                        Serial.println(stat, HEX);
                        parser_state = stop_seen;
                    }
                        break;
                    case 0x72: // ASCII "r", 
                    case 0x52: // ASCII "R", read byte
                    {
                        is_valid_char = true;
                        // peek ahead to see if this was last r -command
                        boolean is_last = true;
                        byte peek_i = i+1;
                        while (peek_i < maxsize)
                        {
                            switch (incoming_command[peek_i])
                            {
                                case 0x72: // ASCII "r", 
                                case 0x52: // ASCII "R", read byte
                                {
                                    is_last = false;
                                }
                                    break;
                                case 0x20: // space, command separator
                                    // no-op
                                    break;
                                case 0x5b: // ASCII "[", our start signifier
                                case 0x5d: // ASCII "]", our stop signifier
                                    peek_i = maxsize-1; // trigger break
                                    break;
                                default:
                                    // Any other command is in the wrong(est) place
                                    Serial.print(F("r lookahead: "));
                                    invalid_char(incoming_command[peek_i], peek_i);
                                    return;
                                
                            }
                            if (!is_last)
                            {
                                break;
                            }
                            peek_i++;
                        }
                        uint8_t tmpbuffer;
                        byte stat = I2c.receiveByte(!is_last, &tmpbuffer);
                        Serial.print(F("read 0x"));
                        Serial.print(tmpbuffer, HEX);
                        Serial.print(F(", stat=0x"));
                        Serial.println(stat, HEX);
                    }
                        break;
                }
                if (is_hex_char(current_char))
                {
                    is_valid_char = true;
                    parser_state = in_hex;
                    i--; // muck i so we re-enter at some point to the hex parser
                }
                if (!is_valid_char)
                {
                    Serial.print(F("p_idle: "));
                    invalid_char(current_char, i);
                    return;
                }
            }
                break;
        }
    }
}

void loop()
{
    read_command_bytes();
    digitalWrite(13, !digitalRead(13));
    delay(20);
}
