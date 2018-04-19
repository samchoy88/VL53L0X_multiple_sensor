#include "Adafruit_VL53L0X.h"

//=================Pin define=================
#define lox1_XSHUT  4
#define lox2_XSHUT  5
#define LEFT_MOTOR  7
#define RIGHT_MOTOR 8


#define SENSOR_1 1
#define SENSOR_2 2
#define DISTANCE_ERROR  0xFFFF
#define DISTANCE_OUT_OF_RANGE  0
#define MOTOR_ON  1
#define MOTOR_OFF 0

//#define debug

const unsigned int numReadings_1 = 5;
unsigned int readings_1[numReadings_1];    // the readings from the analog input
unsigned int readIndex_1 = 0;              // the index of the current reading
unsigned int total_1 = 0;                  // the running total
unsigned int sensor_average_1 = 0;         // the average

const unsigned int numReadings_2 = 5;
unsigned int readings_2[numReadings_2];    // the readings from the analog input
unsigned int readIndex_2 = 0;              // the index of the current reading
unsigned int total_2 = 0;                  // the running total
unsigned int sensor_average_2 = 0;         // the average

Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();
VL53L0X_RangingMeasurementData_t measure1;
VL53L0X_RangingMeasurementData_t measure2;

void setup() {
  Serial.begin(115200);

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }

  for (int thisReading_1 = 0; thisReading_1 < numReadings_1; thisReading_1++) {
    readings_1[thisReading_1] = 0;
  }

  for (int thisReading_2 = 0; thisReading_2 < numReadings_2; thisReading_2++) {
    readings_2[thisReading_2] = 0;
  }
  pinMode(LEFT_MOTOR, OUTPUT);
  pinMode(RIGHT_MOTOR, OUTPUT);
  digitalWrite(LEFT_MOTOR, MOTOR_OFF);
  digitalWrite(RIGHT_MOTOR, MOTOR_OFF);
  
  pinMode(lox1_XSHUT, OUTPUT);
  pinMode(lox2_XSHUT, OUTPUT);
  digitalWrite(lox2_XSHUT, LOW);
  digitalWrite(lox1_XSHUT, LOW);
  delay(100);

  //=================Sensor 1=================
  digitalWrite(lox1_XSHUT, HIGH);
  delay(100);
  Serial.println("Adafruit VL53L0X test");
  if (!lox1.begin(0x39)) {
    Serial.println(F("Failed to boot VL53L0X A"));
    while(1);
  }
  
  //=================Sensor 2=================
  digitalWrite(lox2_XSHUT, HIGH);
  delay(100);
  if (!lox2.begin(0x3F)) {
    Serial.println(F("Failed to boot VL53L0X B"));
    while(1);
  }
  
  // power 
  Serial.println(F("VL53L0X API Simple Ranging example\n\n"));
  digitalWrite(lox1_XSHUT, HIGH);
  digitalWrite(lox2_XSHUT, HIGH);

  delay(100);
}

unsigned int read_sensor( unsigned char select_sensor )
{
  if(select_sensor == SENSOR_1)
  {
    lox1.rangingTest(&measure1, false); // pass in 'true' to get debug data printout!
    if (measure1.RangeStatus != 4)  // phase failures have incorrect data
      return measure1.RangeMilliMeter;
    else
      return DISTANCE_OUT_OF_RANGE;
  }
  else if (select_sensor == SENSOR_2)
  {
    lox2.rangingTest(&measure2, false); // pass in 'true' to get debug data printout!
    if (measure2.RangeStatus != 4)  // phase failures have incorrect data
      return measure2.RangeMilliMeter;
    else
      return DISTANCE_OUT_OF_RANGE;
  }
  else
    return DISTANCE_ERROR;
}

void collect_sensor_value( void )
{
  unsigned int val = 0;
  
  val = read_sensor(SENSOR_1);
  if(val > 2000)
    val = 0;

    total_1 = total_1 - readings_1[readIndex_1];        // subtract the last reading
    readings_1[readIndex_1] = val; // read from the sensor
    total_1 = total_1 + readings_1[readIndex_1];        // add the reading to the total
    readIndex_1++;                  // advance to the next position in the array
  
    // if we're at the end of the array...
    if (readIndex_1 >= numReadings_1)
    {
      readIndex_1 = 0;                            // ...wrap around to the beginning:
    }
    sensor_average_1 = total_1 / numReadings_1;              // calculate the average:
  
  val = read_sensor(SENSOR_2);
  if(val > 2000)
    val = 0;

    total_2 = total_2 - readings_2[readIndex_2];        // subtract the last reading
    readings_2[readIndex_2] = val; // read from the sensor
    total_2 = total_2 + readings_2[readIndex_2];        // add the reading to the total
    readIndex_2++;                  // advance to the next position in the array
  
    // if we're at the end of the array...
    if (readIndex_2 >= numReadings_2)
    {
      readIndex_2 = 0;                            // ...wrap around to the beginning:
    }
    sensor_average_2 = total_2 / numReadings_2;              // calculate the average:
}




void loop() {

  collect_sensor_value();
  if(sensor_average_1 < 100)
    digitalWrite(LEFT_MOTOR, MOTOR_ON);
  else
    digitalWrite(LEFT_MOTOR, MOTOR_OFF);

  
  if(sensor_average_2 < 100)
    digitalWrite(RIGHT_MOTOR, MOTOR_ON);
  else
    digitalWrite(RIGHT_MOTOR, MOTOR_OFF);

#ifdef debug
  Serial.print("Reading a measurement...  ");
  Serial.print(sensor_average_1);
  Serial.print("        ");
  Serial.println(sensor_average_2);
#endif

  delay(10);
}
