/**********************************************************************************************************
* Relative force and position readings are output to a serial terminal.
**********************************************************************************************************/


/**********************************************************************************************************
* PIN DEFS / MACROS
**********************************************************************************************************/
#define PIN_RIGHT           A3 //2
#define PIN_WIPER           A2 //A0
#define PIN_REFERENCE       12  //3

#define SERIAL_BAUD_RATE    115200
#define PER_CYCLE_DELAY     25
#define TOUCH_THRESH        25 //prolly change


/**********************************************************************************************************
* GLOBALS
**********************************************************************************************************/



/**********************************************************************************************************
* setup()
**********************************************************************************************************/
void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);
  
  pinMode(PIN_RIGHT, OUTPUT);
  pinMode(PIN_REFERENCE, INPUT);
  pinMode(PIN_WIPER, INPUT);
}


/**********************************************************************************************************
* loop()
**********************************************************************************************************/
void loop() 
{
  /*** First read force ***/
  
  pinMode(PIN_REFERENCE, OUTPUT);
  digitalWrite(PIN_REFERENCE, LOW);  //Ground reference divider resistor
  digitalWrite(PIN_RIGHT, HIGH);     //Both ends of pot are at +5V
  delay(1);
  int force_reading = analogRead(PIN_WIPER);
 pinMode(PIN_REFERENCE, INPUT);     //Effectively disconnect reference divider resistor
  digitalWrite(PIN_RIGHT, LOW);      //Left end of pot is at +5V, Right end is at GND
  delay(1);
  int position_reading = 1023 - analogRead(PIN_WIPER);  //subtract to output position relative to Left end 
  position_reading -= 512;  
  
  /*** Now read position ***/
  if(force_reading >= 780 && position_reading > 0 ){
  pinMode(PIN_REFERENCE, INPUT);     //Effectively disconnect reference divider resistor
  digitalWrite(PIN_RIGHT, LOW);      //Left end of pot is at +5V, Right end is at GND
  delay(1);
  int position_reading = 1023 - analogRead(PIN_WIPER);  //subtract to output position relative to Left end 
  position_reading -= 512;           //subtract half-scale reading; sensor center is position 0, left is -512, right is +512
  
  
  /*** If readings are valid, output position and force ***/
  
  if(force_reading >= TOUCH_THRESH)  //With no force on the sensor the wiper is floating, position readings would be bogus
  {
    Serial.print("Position: ");
    printFixed(position_reading, 3, true);
    Serial.print("   Force: ");
    printFixed(force_reading, 3, false);
    Serial.println();
  }
  else
  {
    Serial.println("Position:        Force:   0");
  }
  
  delay(PER_CYCLE_DELAY);
}
}

/**********************************************************************************************************
* printFixed() - Serial Print with space padding for consistent positioning in terminal. Optional +/- sign
**********************************************************************************************************/
void printFixed(int value, int digit_places, boolean show_sign)
{
  if(value < 0)
  {
    Serial.print("-");
    value = abs(value);
  }
  else if(show_sign)
  {
    Serial.print("+");
  }
  int compare = 10;
  for(int i = 0; i < digit_places - 1; i ++)
  {
    if(value < compare)
    {
      Serial.print(" ");
    }
    compare *= 10;
  } 
  Serial.print(value);
}
