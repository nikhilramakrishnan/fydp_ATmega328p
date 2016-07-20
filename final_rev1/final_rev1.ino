
#define LED_COUNT 60

const int FSR_PIN_LEFT = A0; // Pin connected to FSR/resistor divider
const int FSR_PIN_RIGHT = A1;
const float VCC = 4.98; // Measured voltage of Ardunio 5V line
const float R_DIV = 2970.0; // Measured resistance of 3.3k resistor

const int fslpSenseLine = A2;
const int fslpDriveLine1 = 8;
const int fslpDriveLine2 = A3;
const int fslpBotR0 = 9;
const String UP = "UP";
const String DOWN = "DOWN";
const String ERR = "ERR";
const String SQUEEZE = "SQUEEZE";

int position1;
int position2;
int position3; 
int positionCounter;
int maxDelay;

void setup()
{
  Serial.begin(9600);
  pinMode(FSR_PIN_LEFT, INPUT);
  pinMode(FSR_PIN_RIGHT, INPUT);
  delay(250);
  position1 = 0;
  position2 = 0;
  position3 = 0;
  positionCounter = 0;
  maxDelay = 0;
}

void loop()
{
  // Part 1: loop to spot force
  int fsrLeftADC = analogRead(FSR_PIN_LEFT);
  int fsrRightADC = 0;
  //int fsrRightADC = analogRead(FSR_PIN_RIGHT);
  
  int leftForce = calculateForce(fsrLeftADC);
  if (leftForce > 0.5) // If the right FSR has 0.5g reading
  {
    Serial.println(SQUEEZE);
    Serial.println("---");
    delay(60);
  }
  else
  {
    // No pressure detected
  }

  // Part 2: loop to spot position
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
    }
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

int calculateForce(int fsrADC) {
  // Use ADC reading to calculate voltage:
    float fsrV = fsrADC * VCC / 1023.0;
    // Use voltage and static resistor value to 
    // calculate FSR resistance:
    float fsrR = R_DIV * (VCC / fsrV - 1.0);
    // Guesstimate force based on slopes in figure 3 of
    // FSR datasheet:
    float force;
    float fsrG = 1.0 / fsrR; // Calculate conductance
    // Break parabolic curve down into two linear slopes:
    if (fsrR <= 600) 
      force = (fsrG - 0.00075) / 0.00000032639;
    else
      force =  fsrG / 0.000000642857;
    return force;
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

