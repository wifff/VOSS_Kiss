#ifndef POWER_SENSOR_H
#define POWER_SENSOR_H

#define LED 13
#define KISS_PORT Serial2

class Power_Sensor 
{       
  public:   
    Power_Sensor();
    float GetEsctemp();
    float GetVoltage(); 
    float GetCurrent();
    float GetConsumption();
    float GetRpm();
    float GetTiming();
    float GetThrottle();
    float GetPower();    
                           
    void Start();
    
  protected:
    int GetData();   
    char buffer[80];
    char keyword[80];
    
    int esctemp_raw;   
    int amp_raw;
    int volt_raw;
    int consumption_raw;
    int rpm_raw;
    int timing_raw;
    int throttle_raw;
    int power_raw;
    
    float esctemp = 0;
    float volt = 0;
    float amp = 0;
    float consumption = 0;
    float rpm = 0;
    float timing = 0; 
    float throttle = 0;
    float watt = 0;
        
    unsigned long timer_esctemp;        
    unsigned long timer_volt;
    unsigned long timer_amp;
    unsigned long timer_consumption;        
    unsigned long timer_rpm;
    unsigned long timer_timing;
    unsigned long timer_throttle; 
    unsigned long timer_watt;       
    
    int read_delay;
    boolean led_on;   
}; 

#endif

