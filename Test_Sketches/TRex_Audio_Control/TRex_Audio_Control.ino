#include <TimeLib.h>
#include <TimeAlarms.h>

AlarmId id;


void setup() {
  Serial.begin(9600);
  while (!Serial) ; // wait for Arduino Serial Monitor

  setTime(7,59,50,1,1,11); // set time to Saturday 8:29:00am Jan 1 2011

  // create the alarms, to trigger at specific times
  Alarm.alarmRepeat(8,00,0, MorningAudioOn);  // 8:00am every day
  Alarm.alarmRepeat(2,00,0,NightAudioOff);  // 2am every day
  

}

void loop() {
  digitalClockDisplay();
  Alarm.delay(1000); // wait one second between clock display
}

// functions to be called when an alarm triggers:
void MorningAudioOn() {
  Serial.println("Alarm: - turn audio on");
}

void NightAudioOff() {
  Serial.println("Alarm: - turn audio off");
}

void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println();
}

void printDigits(int digits) {
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

