/*
 * enLIGHTNINGment slave code
 */
 
// Parsing state machine state
#define PARSE_IDLE    0
#define PARSE_ARG     1
#define PARSE_END     2

int ID_check1 = A2;
int ID_check2 = A3;
int BlinkLED = 13;

uint8_t myBoardNum;

uint8_t cmdArgs[3];

uint8_t parseState;
uint8_t argCount;

void setup() {
  Serial.begin(57600);
  
  pinMode(ID_check1, INPUT_PULLUP);
  pinMode(ID_check2, INPUT_PULLUP);
  pinMode(BlinkLED, OUTPUT);
  
  myBoardNum = 0;
  if (digitalRead(ID_check1) == 1) {
    myBoardNum |= 0x02;
  }
  if (digitalRead(ID_check2) == 1) {
    myBoardNum |= 0x01;
  }
  
  parseState = PARSE_IDLE;
  
  for (int i=2; i<=9; i++) {
    pinMode(i, OUTPUT);
  }
  
  Serial.print("enLIGHTNINGment Board ID: ");
  Serial.println(myBoardNum);
}

void loop() {
  char c;
  
  if (Serial.available()) {
    c = Serial.read();
    
    switch (parseState) {
      case PARSE_IDLE:
        // Look for the start-of-packet
        if (c == '=') {
          argCount= 0;
          for (int i=0; i<3; i++) {
            cmdArgs[i] = 0;
          }
          parseState = PARSE_ARG;
        }
        break;
          
      case PARSE_ARG:
        if ((c >= '0') && (c <= '9')) {
          if (argCount < 3) {
            cmdArgs[argCount] = cmdArgs[argCount]*10 + (c - '0');
          } else {
            parseState = PARSE_IDLE;
          }
        } else if (c == ',') {
          argCount++;
          if (argCount == 3) {
            parseState = PARSE_END;
          }
        } else {
          parseState = PARSE_IDLE;
        }
        break;
        
      case PARSE_END:
        if (c == 0x0D) {
          ProcessPacket();
          parseState = PARSE_IDLE;
        } else {
          parseState = PARSE_IDLE;
        }
        break;
          
      default:
        parseState = PARSE_IDLE;
    }
  }
}

void ProcessPacket() {
  if (myBoardNum == cmdArgs[0]) {
    digitalWrite(BlinkLED, HIGH);
    
    if (cmdArgs[1] < 8) {
      if (cmdArgs[2] == 1) {
        digitalWrite(cmdArgs[1] + 2, HIGH);
      } else {
        digitalWrite(cmdArgs[1] + 2, LOW);
      }
    }
  } else {
    digitalWrite(BlinkLED, LOW);
  }
}
