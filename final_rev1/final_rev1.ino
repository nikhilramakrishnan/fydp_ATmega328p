
#define LED_COUNT 60

const int fslpSenseLine = A2;
const int fslpDriveLine1 = 8;
const int fslpDriveLine2 = A3;
const int fslpBotR0 = 9;
const String UP = "UP";
const String DOWN = "DOWN";
const String ERR = "ERR";

int position1;
int position2;
int position3; 
int positionCounter;
int maxDelay;

void setup()
{
  Serial.begin(9600);
  delay(250);
  position1 = 0;
  position2 = 0;
  position3 = 0;
  positionCounter = 0;
  maxDelay = 0;
}

void loop()
{
  int pressure, position;

  pressure = fslpGetPressure();

  if (pressure == 0)
  {
    // There is no detectable pressure, so measuring
    // the position does not make sense.
    position1 = 0;
    position2 = 0;
    position3 = 0;
    positionCounter = 0;
    maxDelay = 0;
  }
  else
  {
    position = fslpGetPosition();  // Raw reading, from 0 to 1023.
    positionCounter++;

    // The resolution is too high for fat fingers to interpret. 
    // Reduce the resolution and notice the pattern. 
    if (positionCounter == 1) {
      position1 = position/5;
    } else if (positionCounter == 2) {
      position2 = position/5;
    } else if (positionCounter == 3) {
      position3 = position/5;
      positionCounter = 0;
      Serial.println(isUp());
      Serial.println("---");
//      Serial.println(position1);
//      Serial.println(position2);
//      Serial.println(position3);
//      Serial.println("---");
    }
  }

  char report[80];
  if (pressure != 0) {
  //sprintf(report, "pressure: %5d   position: %5d\n",
  //  pressure, position);
  //Serial.print(report);
  }

  delay(20);
  maxDelay = maxDelay + 20;
  if (maxDelay == 60) {
    position1 = 0;
    position2 = 0;
    position3 = 0;
    positionCounter = 0;
  }
}

// This function follows the steps described in the FSLP
// integration guide to measure the position of a force on the
// sensor.  The return value of this function is proportional to
// the physical distance from drive line 2, and it is between
// 0 and 1023.  This function does not give meaningful results
// if fslpGetPressure is returning 0.
int fslpGetPosition()
{
  // Step 1 - Clear the charge on the sensor.
  pinMode(fslpSenseLine, OUTPUT);
  digitalWrite(fslpSenseLine, LOW);

  pinMode(fslpDriveLine1, OUTPUT);
  digitalWrite(fslpDriveLine1, LOW);

  pinMode(fslpDriveLine2, OUTPUT);
  digitalWrite(fslpDriveLine2, LOW);

  pinMode(fslpBotR0, OUTPUT);
  digitalWrite(fslpBotR0, LOW);

  // Step 2 - Set up appropriate drive line voltages.
  digitalWrite(fslpDriveLine1, HIGH);
  pinMode(fslpBotR0, INPUT);
  pinMode(fslpSenseLine, INPUT);

  // Step 3 - Wait for the voltage to stabilize.
  delayMicroseconds(10);

  // Step 4 - Take the measurement.
  analogReset();
  return analogRead(fslpSenseLine);
}

// This function follows the steps described in the FSLP
// integration guide to measure the pressure on the sensor.
// The value returned is usually between 0 (no pressure)
// and 500 (very high pressure), but could be as high as
// 32736.
int fslpGetPressure()
{
  // Step 1 - Set up the appropriate drive line voltages.
  pinMode(fslpDriveLine1, OUTPUT);
  digitalWrite(fslpDriveLine1, HIGH);

  pinMode(fslpBotR0, OUTPUT);
  digitalWrite(fslpBotR0, LOW);

  pinMode(fslpSenseLine, INPUT);

  pinMode(fslpDriveLine2, INPUT);

  // Step 2 - Wait for the voltage to stabilize.
  delayMicroseconds(10);

  // Step 3 - Take two measurements.
  analogReset();
  int v1 = analogRead(fslpDriveLine2);
  analogReset();
  int v2 = analogRead(fslpSenseLine);

  // Step 4 - Calculate the pressure.
  // Detailed information about this formula can be found in the
  // FSLP Integration Guide.
  if (v1 == v2)
  {
    // Avoid dividing by zero, and return maximum reading.
    return 32 * 1023;
  }
  return 32 * v2 / (v1 - v2);
}

String isUp() {
  int firstDifference = position2 - position1;
  int secondDifference = position3 - position2;
  int totalDifference = position3 - position1;

  if (firstDifference > 2) {
    return UP;
  } else if (secondDifference > 2){
    return UP;
  } else if (totalDifference > 0) {
    return UP;
  } else if (firstDifference < 2) {
    return DOWN;
  } else if (secondDifference < 2) {
    return DOWN;
  } else if (totalDifference < 0) {
    return DOWN;
  } else {
    return ERR;
  }
}

// Performs an ADC reading on the internal GND channel in order
// to clear any voltage that might be leftover on the ADC.
// Only works on AVR boards and silently fails on others.
void analogReset()
{
#if defined(ADMUX)
#if defined(ADCSRB) && defined(MUX5)
    // Code for the ATmega2560 and ATmega32U4
    ADCSRB |= (1 << MUX5);
#endif
    ADMUX = 0x1F;

    // Start the conversion and wait for it to finish.
    ADCSRA |= (1 << ADSC);
    loop_until_bit_is_clear(ADCSRA, ADSC);
#endif
}

