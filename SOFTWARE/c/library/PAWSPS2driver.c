// PAWS PS/2 KEYBOARD DRIVER CODE
// PS/2 KEYBOARD
unsigned char volatile *PS2_AVAILABLE = (unsigned char volatile *) 0xf100;
unsigned char volatile *PS2_MODE = (unsigned char volatile *) 0xf100;
unsigned char volatile *PS2_CAPSLOCK = (unsigned char volatile *) 0xf102;
unsigned char volatile *PS2_NUMLOCK = (unsigned char volatile *) 0xf104;
unsigned short volatile *PS2_DATA = (unsigned short volatile *) 0xf102;

// MODIFIER KEYS
unsigned char lshift, rshift, lalt, ralt, lctrl, rctrl, lwin, rwin, application, capslock, numlock = 1;

// RETURN IF A PS/2 KEYEVENT IS AVAILABLE
char ps2_event_available( void ) {
    return *PS2_AVAILABLE;
}

// RETURN THE PS/2 KEY EVENT { press/release, extended, keycode }
unsigned short ps2_event_get( void ) {
    while( !*PS2_AVAILABLE ) {}
    return *PS2_DATA;
}

// PS/2 KEYCODE TO ASCII TRANSLATOR
unsigned short availablecharacter = 0;
unsigned char ps2_character_available() {
    unsigned short keyevent;
    unsigned char caps, shift, ctrl;

    if( !availablecharacter ) {
        if( ps2_event_available() ) {
            keyevent = ps2_event_get();
            if( keyevent & 0x200 ) {
                // KEYDOWN
                caps = capslock ^ ( lshift | rshift ); shift = lshift | rshift; ctrl = lctrl | rctrl;

                switch( keyevent & 0x1ff ) {
                    // DEAL WITH MODIFIER KEYS
                    case 0x58:  capslock = !capslock; *PS2_CAPSLOCK = capslock; break;
                    case 0x77:  numlock = !numlock; *PS2_NUMLOCK = numlock; break;
                    case 0x12:  lshift = 1; break;
                    case 0x59:  rshift = 1; break;
                    case 0x14:  lctrl = 1; break;
                    case 0x114: rctrl = 1; break;
                    case 0x11:  lalt = 1; break;
                    case 0x111: ralt = 1; break;
                    case 0x11f: lwin = 1; break;
                    case 0x127: rwin = 1; break;
                    case 0x12f: application = 1; break;

                    // LETTER KEYS
                    case 0x1c:  availablecharacter = ctrl ? 0x01 : caps ? 'A' : 'a'; break;
                    case 0x32:  availablecharacter = ctrl ? 0x02 : caps ? 'B' : 'b'; break;
                    case 0x21:  availablecharacter = ctrl ? 0x03 : caps ? 'C' : 'c'; break;
                    case 0x23:  availablecharacter = ctrl ? 0x04 : caps ? 'D' : 'd'; break;
                    case 0x24:  availablecharacter = ctrl ? 0x05 : caps ? 'E' : 'e'; break;
                    case 0x2b:  availablecharacter = ctrl ? 0x06 : caps ? 'F' : 'f'; break;
                    case 0x34:  availablecharacter = ctrl ? 0x07 : caps ? 'G' : 'g'; break;
                    case 0x33:  availablecharacter = ctrl ? 0x08 : caps ? 'H' : 'h'; break;
                    case 0x43:  availablecharacter = ctrl ? 0x09 : caps ? 'I' : 'i'; break;
                    case 0x3b:  availablecharacter = ctrl ? 0x0a : caps ? 'J' : 'j'; break;
                    case 0x42:  availablecharacter = ctrl ? 0x0b : caps ? 'K' : 'k'; break;
                    case 0x4b:  availablecharacter = ctrl ? 0x0c : caps ? 'L' : 'l'; break;
                    case 0x3a:  availablecharacter = ctrl ? 0x0d : caps ? 'M' : 'm'; break;
                    case 0x31:  availablecharacter = ctrl ? 0x0e : caps ? 'N' : 'n'; break;
                    case 0x44:  availablecharacter = ctrl ? 0x0f : caps ? 'O' : 'o'; break;
                    case 0x4d:  availablecharacter = ctrl ? 0x10 : caps ? 'P' : 'p'; break;
                    case 0x15:  availablecharacter = ctrl ? 0x11 : caps ? 'Q' : 'q'; break;
                    case 0x2d:  availablecharacter = ctrl ? 0x12 : caps ? 'R' : 'r'; break;
                    case 0x1b:  availablecharacter = ctrl ? 0x13 : caps ? 'S' : 's'; break;
                    case 0x2c:  availablecharacter = ctrl ? 0x14 : caps ? 'T' : 't'; break;
                    case 0x3c:  availablecharacter = ctrl ? 0x15 : caps ? 'U' : 'u'; break;
                    case 0x2a:  availablecharacter = ctrl ? 0x16 : caps ? 'V' : 'v'; break;
                    case 0x1d:  availablecharacter = ctrl ? 0x17 : caps ? 'W' : 'w'; break;
                    case 0x22:  availablecharacter = ctrl ? 0x18 : caps ? 'X' : 'x'; break;
                    case 0x35:  availablecharacter = ctrl ? 0x19 : caps ? 'Y' : 'y'; break;
                    case 0x1a:  availablecharacter = ctrl ? 0x1a : caps ? 'Z' : 'z'; break;

                    // NUMBER KEYS
                    case 0x16:  availablecharacter = shift ? '!' : '1'; break;
                    case 0x1e:  availablecharacter = shift ? '\"' : '2'; break;
                    case 0x26:  availablecharacter = shift ? 0x9c : '3'; break; // £
                    case 0x25:  availablecharacter = shift ? '$' : '4'; break;  // € not available
                    case 0x2e:  availablecharacter = shift ? '%' : '5'; break;
                    case 0x36:  availablecharacter = shift ? '^' : '6'; break;
                    case 0x3d:  availablecharacter = shift ? '&' : '7'; break;
                    case 0x3e:  availablecharacter = shift ? '*' : '8'; break;
                    case 0x46:  availablecharacter = shift ? '(' : '9'; break;
                    case 0x45:  availablecharacter = shift ? ')' : '0'; break;

                    // PUNCTUATION
                    case 0x29:  availablecharacter = ' '; break; // space
                    case 0x0e:  availablecharacter = ralt ? '|' : shift ? 0xaa : 0x60; break; // top left key
                    case 0x4e:  availablecharacter = shift ? '_' : '-'; break;
                    case 0x55:  availablecharacter = shift ? '+' : '='; break;
                    case 0x54:  availablecharacter = shift ? '{' : '['; break;
                    case 0x5b:  availablecharacter = shift ? '}' : ']'; break;
                    case 0x4c:  availablecharacter = shift ? ':' : ';'; break;
                    case 0x52:  availablecharacter = shift ? '@' : 0x27; break; // '
                    case 0x5d:  availablecharacter = shift ? '~' : '#'; break;
                    case 0x61:  availablecharacter = shift ? '|' : 0x5c; break; // backslash
                    case 0x41:  availablecharacter = shift ? '<' : ','; break;
                    case 0x49:  availablecharacter = shift ? '>' : '.'; break;
                    case 0x4a:  availablecharacter = shift ? '?' : '/'; break;

                    // ESCAPE, TAB, BACKSPACE, ENTER
                    case 0x76:  availablecharacter = 0x1b; break;  // ESCAPE
                    case 0x0d:  availablecharacter = 0x09; break;  // TAB
                    case 0x66:  availablecharacter = 0x08; break;  // BACKSPACE
                    case 0x5a:  availablecharacter = 0x0d; break;  // ENTER

                    // INSERT, HOME, ETC + CURSOR KEYS
                    case 0x170: availablecharacter = 0x132; break; // INSERT
                    case 0x16c: availablecharacter = 0x131; break; // HOME
                    case 0x17d: availablecharacter = 0x135; break; // PGUP
                    case 0x171: availablecharacter = 0x133; break; // DELETE
                    case 0x169: availablecharacter = 0x134; break; // END
                    case 0x17a: availablecharacter = 0x136; break; // PGDN

                    // KEYPAD
                    case 0x70:  availablecharacter = numlock ? '0' : 0x132; break;
                    case 0x69:  availablecharacter = numlock ? '1' : 0x134; break;
                    case 0x72:  availablecharacter = numlock ? '2' : 0x142; break;
                    case 0x7a:  availablecharacter = numlock ? '3' : 0x136; break;
                    case 0x6b:  availablecharacter = numlock ? '4' : 0x144; break;
                    case 0x73:  availablecharacter = numlock ? '5' : 0x147; break;
                    case 0x74:  availablecharacter = numlock ? '6' : 0x143; break;
                    case 0x6c:  availablecharacter = numlock ? '7' : 0x131; break;
                    case 0x75:  availablecharacter = numlock ? '8' : 0x141; break;
                    case 0x7d:  availablecharacter = numlock ? '9' : 0x135; break;
                    case 0x14a: availablecharacter = '/'; break;
                    case 0x7c:  availablecharacter = '*'; break;
                    case 0x7b:  availablecharacter = '-'; break;
                    case 0x79:  availablecharacter = '+'; break;
                    case 0x71:  availablecharacter = numlock ? '.' : 0x133; break;
                    case 0x15a: availablecharacter = 0x0d; break;

                    // FUNCTION KEYS
                    case 0x05:  availablecharacter = shift ? 0x111 : 0x101; break;
                    case 0x06:  availablecharacter = shift ? 0x112 : 0x102; break;
                    case 0x04:  availablecharacter = shift ? 0x113 : 0x103; break;
                    case 0x0c:  availablecharacter = shift ? 0x114 : 0x104; break;
                    case 0x03:  availablecharacter = shift ? 0x115 : 0x105; break;
                    case 0x0b:  availablecharacter = shift ? 0x116 : 0x106; break;
                    case 0x83:  availablecharacter = shift ? 0x117 : 0x107; break;
                    case 0x0a:  availablecharacter = shift ? 0x118 : 0x108; break;
                    case 0x01:  availablecharacter = shift ? 0x119 : 0x109; break;
                    case 0x09:  availablecharacter = shift ? 0x11a : 0x10a; break;
                    case 0x78:  availablecharacter = shift ? 0x11b : 0x10b; break;
                    case 0x07:  availablecharacter = shift ? 0x11c : 0x10c; break;

                    default:
                }
            } else {
                // KEYUP
                switch( keyevent & 0x1ff ) {
                    // DEAL WITH MODIFIER KEYS, ALL OTHER KEYUP EVENTS NOT DECODED
                    case 0x12:  lshift = 0; break;
                    case 0x59:  rshift = 0; break;
                    case 0x14:  lctrl = 0; break;
                    case 0x114: rctrl = 0; break;
                    case 0x11:  lalt = 0; break;
                    case 0x111: ralt = 0; break;
                    case 0x11f: lwin = 0; break;
                    case 0x127: rwin = 0; break;
                    case 0x12f: application = 0; break;
                    default:
                }
            }
        }
    }
    return( availablecharacter != 0 );
}

unsigned short ps2_inputcharacter() {
    while( !ps2_character_available() );
    unsigned short temp = availablecharacter; availablecharacter = 0;
    return temp;
}

// SWITCH THE PS/2 KEYBOARD TO RETURNING/NOT RETURNING KEYCODES3
// EMPTIES THE KEYCODE BUFFER AND RESETS MODIFIER KEYS
void ps2_keyboardmode( unsigned char mode ) {
    while( *PS2_AVAILABLE ) { (void)*PS2_DATA; } *PS2_MODE = mode;
    lshift = 0; rshift = 0; lalt = 0; ralt = 0; lctrl = 0; rctrl = 0; lwin = 0; rwin = 0; application = 0; capslock = 0; numlock = 0;
    availablecharacter = 0;
}
