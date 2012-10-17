#define DEBUG

#define rs485_pin A5
#define ENGINE_COUNT 3
#define REVERSE_PAUSE_MSEC 1000

class Engine
{
public:
  byte reversePin;
  byte powerPin;
  byte currentPower;
  byte reverseState;
};

Engine ENGINES[ENGINE_COUNT];

void SetupEngine (int engineNum, int powerPin, int reversePin)
{
  ENGINES[engineNum].powerPin = powerPin;
  ENGINES[engineNum].reversePin = reversePin;
  ENGINES[engineNum].currentPower = 0;
  ENGINES[engineNum].reverseState = 0;

  pinMode(reversePin, OUTPUT);   
  analogWrite(ENGINES[engineNum].powerPin, 0); 
  digitalWrite(ENGINES[engineNum].reversePin, LOW);
}

void WriteRS485(char buf[], int count)
{
  digitalWrite(rs485_pin, HIGH);
  for (int i=0; i<count; i++)
    Serial.print(buf[i]);
  delay(50); //without delay arduino cut mes
  digitalWrite(rs485_pin, LOW);
}

void setup()  
{ 
  SetupEngine(0,6,7);
  SetupEngine(1,5,4);
  SetupEngine(2,3,2);

  pinMode(rs485_pin, OUTPUT);   
  digitalWrite(rs485_pin, LOW);
  Serial.begin(19200); 

  WriteRS485("SMTU ROV v 1.11 firmvare", 24); 
} 

char buffer[20];
char inChar = -1;
byte charIndx = 0;

void ParseReverseCmd(byte engineNum)
{
  byte reverseState = buffer[3] - '0';
 
  if (reverseState != 1 && reverseState != 0)
     return;

  if (reverseState != ENGINES[engineNum].reverseState)
  {
    analogWrite(ENGINES[engineNum].powerPin, 0); 
    delay(REVERSE_PAUSE_MSEC);
  }

  ENGINES[engineNum].reverseState = reverseState;

  if (reverseState == 1)
    digitalWrite(ENGINES[engineNum].reversePin, HIGH);
  else 
    digitalWrite(ENGINES[engineNum].reversePin, LOW);

    analogWrite(ENGINES[engineNum].powerPin, ENGINES[engineNum].currentPower);
#ifdef DEBUG
  WriteRS485("R: ",3);
  char buf[3];
  itoa(ENGINES[engineNum].reverseState,buf,10);
  WriteRS485(buf, 1);    
  WriteRS485(". \n", 3);
#endif
}

void ParsePowerCmd(byte engineNum)
{
  if (charIndx <= 6)
  {
    int val = 0;
    switch (charIndx) 
    {
      case 6 : val = (buffer[3]-'0')*100 + (buffer[4]-'0')*10 + (buffer[5]-'0'); break;   
      case 5 : val = (buffer[3]-'0')*10 + (buffer[4]-'0'); break;   
      case 4 : val = (buffer[3]-'0'); break;
      default: return;      
    }
      
    if (0 <= val && val <= 255)
    {
      analogWrite(ENGINES[engineNum].powerPin, val);
      ENGINES[engineNum].currentPower = val;         

#ifdef DEBUG
      WriteRS485("P: ", 13);
      char buf[3];
      itoa(val,buf,10);
      WriteRS485(buf, charIndx - 3);   
      WriteRS485(". \n", 3);
#endif
    }
  } 
}


void ParseBuffer()
{
  byte engineNum = buffer[2] - '0';
  if (engineNum < 0 || engineNum >= ENGINE_COUNT)
    return;  
#ifdef DEBUG
  WriteRS485("Engine num: ", 12);
  char buf[3];
  itoa(engineNum,buf,10);
  WriteRS485(buf,1);    
  WriteRS485(". \n", 3);
#endif       
  switch (buffer[1])
  {
    case 'r' : ParseReverseCmd(engineNum); break;
    case 'p' : ParsePowerCmd(engineNum); break; 
  }
}

void ReadCommand()
{
  while (Serial.available() > 0) 
  {
    if(charIndx < 19) // One less than the size of the array
    {
      inChar = Serial.read(); 

      if (inChar == '#')
        charIndx = 0;
        
      if (inChar == '!')
      {
        ParseBuffer();
        charIndx = 0;
      }
        
      buffer[charIndx] = inChar; 
      charIndx++; 
    }
    else
      charIndx = 0;
  }
}


void loop()  
{
  ReadCommand(); 
}

