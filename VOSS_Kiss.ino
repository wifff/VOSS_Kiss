/* 
  VOSS - Versatile Open Source Sensor for Teensy (not for Arduino as we need two HW UARTs)
  Copyright (C) 2017 Wolfgang Korosec
  
  Based on the brilliant JetiExSensor library written by Bernd Wokoeck
  Extended notice on additional work and copyrights, see headers of included files and libraries
  
  Jeti EX Telemetry for Kiss 24A reads data from the Kiss serial port (RX, TX Pads):
  - Temperature (resolution 1°C)
  - Voltage (resolution 0.01V)
  - Current (resolution 0.01A)
  - Consumption (resolution m/Ah)
  - Electrical Rpm (resolution 100Rpm)
  - Timing (deg)
  - Throttle (%)
  - Startuptime (ms)
  
  Wiring: 
  
  Teensy 3.2                                              Kiss 24A      
                               +--------------------+                         
                               |        D12         |        MOTOR
                        |o o o o o o o|             |   | O   O   O |
                        |o     GND   o|             |   |           |
                        |o       TX2 o|----------+  |   |           |
                        |o       RX2 o|----------|--|---|o RX       |
                        |o       TX3 o|          +--|---|o TX       |
                        |o SDA0  RX3 o|             |   |           |
                        |o SCL0      o|             +---| O       O |
                        |o           o|                  GND      +
                        |o           o|                      BATT
                        |o           o|          
                        |o           o|          
                        |o       TX1 o|----+     
                        |o       RX1 o|    |     
                    +---|o Vin   GND o|----|-----+
                    |                      |     |
                    v                      v     v
  Jeti Receiver  EXT red           EXT orange  EXT black



  Prerequisites:
  Jeti Sensor EX Telemetry C++ Library, Copyright (C) 2015, 2016 Bernd Wokoeck
  https://sourceforge.net/projects/jetiexsensorcpplib/ 

  Version history:
  0.10   2016-08-19  created
  
  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  IN THE SOFTWARE.

**************************************************************/
#define VERSION "VOSS V0.1"
#define SENSORS "Kiss 24A"

#include<string>
#include "JetiExProtocol.h"
#include "Kiss_Sensor.h"

JetiExProtocol jetiEx;
Power_Sensor motor;

long esctemp_ex;
long voltage_ex;
long current_ex;
long consumption_ex;
long rpm_ex;
long timing_ex;
long throttle_ex;
long watt_ex;

char JetiBoxLine1[40] = {0};
char JetiBoxLine2[40] = {0};
char buffer1[40] = {0};
char buffer2[40] = {0};

// we will not use timing and throttle to get better performance
enum
{
  ID_ESCTEMP = 1,
  ID_VOLTAGE,
  ID_CURRENT,
  ID_CONSUMPTION,
  ID_RPM,
//  ID_TIMING,
//  ID_THROTTLE,  
  ID_WATT,
};
// id from 1..15
// name plus unit must be < 20 characters
// precision = 0 --> 0, precision = 1 --> 0.0, precision = 2 --> 0.00

JETISENSOR_CONST sensors[] PROGMEM = 
{
  // id               name         unit         data type             precision 
  {   ID_ESCTEMP,      "ESCTemp",   "\xB0\x43", JetiSensor::TYPE_14b, 0 },      
  {   ID_VOLTAGE,      "Voltage",   "V",        JetiSensor::TYPE_14b, 2 },
  {   ID_CURRENT,      "Current",   "A",        JetiSensor::TYPE_14b, 2 }, 
  {   ID_CONSUMPTION,  "Consumpt",  "mAh",      JetiSensor::TYPE_14b, 0 },
  {   ID_RPM,          "RPM",       "RPM",      JetiSensor::TYPE_14b, 0 }, 
//  {   ID_TIMING,       "Timing",    "\xB0",     JetiSensor::TYPE_14b, 0 },
//  {   ID_THROTTLE,     "Thrott",    "%",        JetiSensor::TYPE_14b, 0 },
  {   ID_WATT,         "Power",     "W",        JetiSensor::TYPE_14b, 0 },  
  { 0 } // end of array
};


void setup() {  
  Serial.begin(115200);
  KISS_PORT.begin(115200);
  KISS_PORT.clear();
  
  jetiEx.Start( "VOSS", sensors, JetiExProtocol::SERIAL1); 
  sprintf(JetiBoxLine1,"%16s", VERSION);
  sprintf(JetiBoxLine2,"%16s", SENSORS);
        
  jetiEx.SetJetiboxText( JetiExProtocol::LINE1, JetiBoxLine1);
  jetiEx.SetJetiboxText( JetiExProtocol::LINE2, JetiBoxLine2);

  motor.Start();

  jetiEx.DoJetiSend();
}

void loop() 
{  
  esctemp_ex = motor.GetEsctemp();  
  voltage_ex = motor.GetVoltage()*100;
  current_ex = motor.GetCurrent()*100;
  consumption_ex = motor.GetConsumption();
  rpm_ex = motor.GetRpm();
//  timing_ex = motor.GetTiming();
//  throttle_ex = motor.GetThrottle();      
  watt_ex = motor.GetPower();
  
  jetiEx.SetSensorValue( ID_ESCTEMP,    esctemp_ex);
  jetiEx.SetSensorValue( ID_VOLTAGE,    voltage_ex);
  jetiEx.SetSensorValue( ID_CURRENT,    current_ex);
  jetiEx.SetSensorValue( ID_CONSUMPTION,consumption_ex);
  jetiEx.SetSensorValue( ID_RPM,        rpm_ex);
  // jetiEx.SetSensorValue( ID_TIMING,     timing_ex);
  // jetiEx.SetSensorValue( ID_THROTTLE,   throttle_ex);
  jetiEx.SetSensorValue( ID_WATT,       watt_ex);   
  
  dtostrf(motor.GetVoltage(), 4, 2, buffer1); // sprintf for float not implemented for Arduino so we have to convert!!!
  dtostrf(motor.GetCurrent(), 4, 2, buffer2); // on Teensy sprintf for floats works - very strange ... 
  sprintf(JetiBoxLine1,"%5s V %5s A",buffer1, buffer2);

  dtostrf(motor.GetPower(), 1, 0, buffer1);
  dtostrf(motor.GetConsumption(), 1, 0, buffer2);  
  sprintf(JetiBoxLine2,"%5s W %5smAh",buffer1, buffer2);

  jetiEx.SetJetiboxText( JetiExProtocol::LINE1, JetiBoxLine1);
  jetiEx.SetJetiboxText( JetiExProtocol::LINE2, JetiBoxLine2);

  jetiEx.DoJetiSend();
}

