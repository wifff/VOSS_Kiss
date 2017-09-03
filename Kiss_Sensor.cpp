#include "Arduino.h"
#include "Kiss_Sensor.h"

/* 
Temperature [C]:       30
Voltage [V/100]:      984
Ampere [A/100]:         0
Consumption [mA/h]:     0
E-RPM [krpm]:           0
Timing [deg.]:          0
Throttle [%]:           0
Startuptime [ms]:       
*/

Power_Sensor::Power_Sensor() {}

int Power_Sensor::GetData() {
  int data;
  char *ret;
  static int count = 0;
  memset(buffer,0,sizeof(buffer));

  while (true) {  
    while (KISS_PORT.available()) {         
      int c = KISS_PORT.read();
      if (c == '\n') {           // received complete line
        buffer[count] = '\0';    // terminate the string
        ret = strstr(buffer, keyword); 
        if (ret) {
          data = atoi(ret + strlen(keyword));
          // Serial.print(keyword); 
          // Serial.println(data); 
          return(data);
        }
        count = 0;
      }
      else if (count < int (sizeof buffer) - 1) {
        buffer[count++] = c;
      }
    }
  }
}

float Power_Sensor::GetEsctemp() {
  if( ( timer_esctemp + read_delay ) <= millis() )
  {
    strcpy (keyword, "Temperature [C]:");
    esctemp_raw = GetData();     
    esctemp = (float) esctemp_raw; 
    timer_esctemp = millis();
     
    if (led_on)  digitalWrite(LED, HIGH); else digitalWrite(LED, LOW); // blink = active sensor
    led_on = ! led_on; 
  }
  return esctemp;
}


float Power_Sensor::GetVoltage() {
  if( ( timer_volt + read_delay ) <= millis() )
  {
    strcpy (keyword, "Voltage [V/100]:");
    volt_raw = GetData();     
    volt = (float) volt_raw / 100.0; 
    timer_volt = millis();
  }
  return volt;
}


float Power_Sensor::GetCurrent() {
  if( ( timer_amp + read_delay ) <= millis() )
  {
    strcpy (keyword, "Ampere [A/100]:");
    amp_raw = GetData();
    amp = (float) amp_raw / 100;
    timer_amp = millis();     
  }
  return amp;
}


float Power_Sensor::GetPower() {
  if( ( timer_watt + read_delay ) <= millis() )
  {
    watt = volt * amp;    
    timer_watt = millis();     
  }
  return watt;
}


float Power_Sensor::GetConsumption() {
  if((timer_consumption + read_delay ) <= millis() )
  {   
    strcpy (keyword, "Consumption [mA/h]:");
    consumption_raw = GetData();
    consumption = (float) consumption_raw;     
    timer_consumption = millis();     
  }
  return consumption;
}


float Power_Sensor::GetRpm() {
  if((timer_rpm + read_delay ) <= millis() )
  {   
    strcpy (keyword, "E-RPM [krpm]:");
    rpm_raw = GetData();
    rpm = (float) rpm_raw * 100;     
    timer_rpm = millis();     
  }
  return rpm;
}

float Power_Sensor::GetTiming() {
  if((timer_timing + read_delay ) <= millis() )
  {   
    strcpy (keyword, "Timing [deg.]:");
    timing_raw = GetData();
    timing = (float) timing_raw;     
    timer_timing = millis();     
  }
  return timing;
}

float Power_Sensor::GetThrottle() {
  if((timer_throttle + read_delay ) <= millis() )
  {   
    strcpy (keyword, "Throttle [%]:");
    throttle_raw = GetData();
    throttle = (float) throttle_raw;     
    timer_throttle = millis();     
  }
  return throttle;
}

void Power_Sensor::Start() {
//wait until Kiss Controller starts transmitting telemetry data on serial port 
  
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);   
  delay(5500);               // wait for a 5.5 seconds
  digitalWrite(LED, LOW);      

  KISS_PORT.println("tlm");
  delay(500);
 
  read_delay = 0;
  led_on = true;  
}

