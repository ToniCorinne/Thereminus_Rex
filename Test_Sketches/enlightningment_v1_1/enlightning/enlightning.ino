/*
 * Enlightningment Master Controller
 *
 * Color Mapping
 *   Red 1
 *   Red 2
 *   Red 3
 *   Green 1
 *   Green 2
 *   Green 3
 *   Blue 1
 *   Blue 2
 *   Blue 3
 *   White 1
 *   White 2
 *   White 3
 *   Eggshell 1
 *   Eggshell 2
 *
 * Version 1.1
 */
#include "TimerOne.h"

//
// Color Constants
//
#define cR1  0
#define cR2  1
#define cR3  2
#define cG1  3
#define cG2  4
#define cG3  5
#define cB1  6
#define cB2  7
#define cB3  8
#define cW1  9
#define cW2  10
#define cW3  11
#define cE1  12
#define cE2  13

//
// Threads
//
#define NUM_PARALLEL_THREADS   3

//
// Sensors
//
#define NUM_SENSORS            3

//
// Sensor 1 (Ultrasonic distance sensor) constants
//  ULTRASONIC points to the entry in the array used for this sensor
//
#define ULTRASONIC             0
#define SNS1_PORT              0
#define SNS1_ACTIVE_INCREASING false
#define SNS1_THRESHOLD         200
#define SNS1_THRESH_COUNT      5

//
// Sensor 2 (Sound sensor)
//  SOUND points to the entry in the array used for this sensor
//
#define SOUND                  1
#define SNS2_PORT              1
#define SNS2_ACTIVE_INCREASING true
#define SNS2_THRESHOLD         512
#define SNS2_THRESH_COUNT      3

// Sound threshold used to detect when the environment is noisy
//   SOUND_THRESH is a percentage (0-100) that specifies the number
//   of samples that need to read triggered to determine we're in a noisy
//   environment.  Lower number means more sensitive, higher number less
//   sensitive.
#define SOUND_THRESH           50

//
// Sensor 3 (Light sensor)
//  LIGHT points to the entry in the array used for this sensor
//
#define LIGHT                  2
#define SNS3_PORT              2
#define SNS3_ACTIVE_INCREASING true
#define SNS3_THRESHOLD         600
#define SNS3_THRESH_COUNT      3

//
// Basic time tick in mSec
//
#define TICK_INTERVAL 5

//
// FIFO Engine constants
//
#define NUM_CH           14
#define FIFO_DEPTH       64
#define FIFO_ARG_MASK    0x0FFF
#define FIFO_CMD_MASK    0xF000
#define FIFO_CMD_DELAY   0x0000
#define FIFO_CMD_SET     0x1000
#define FIFO_CMD_CLR     0x2000

//
// FIFOs
//
uint16_t fifoArray[NUM_CH][FIFO_DEPTH];
uint8_t fifoCount[NUM_CH];
uint8_t fifoPushIndex[NUM_CH];
uint8_t fifoPopIndex[NUM_CH];

//
// Mapping to sequencer channel (0-31)
//
uint8_t sequencerMapArray[NUM_CH];

//
// Playback engine
//
uint16_t playbackTimer[NUM_CH];
boolean  playbackBusy[NUM_CH];

//
// Sensors
//
uint8_t sensorPorts[NUM_SENSORS];        // Analog port number (0-5)
boolean sensorIncreasing[NUM_SENSORS];   // Set to true if the sensor value increases as it is triggered
boolean sensorTrigger[NUM_SENSORS];      // Set true the first eval the sensor is determined triggered
boolean sensorState[NUM_SENSORS];        // The current state (set = true)
uint16_t sensorThreshold[NUM_SENSORS];   // The analog threshold (in ADC counts) to consider triggered
uint8_t sensorThreshCounts[NUM_SENSORS]; // The number of consecutive readings above or below the threshold to turn on or off
uint8_t sensorCurCount[NUM_SENSORS];     // The number of readings accumulated towards a change of state

uint8_t soundSampleArray[100];           // Used to accumulate sound sensor samples
uint8_t soundSampleIndex;
boolean noisyEnvironment;

//
// Application Logic
//
typedef void(*iFunc)(void);

iFunc threadFuncs[NUM_PARALLEL_THREADS];
uint16_t threadCounters[NUM_PARALLEL_THREADS];
uint8_t threadState[NUM_PARALLEL_THREADS];

boolean enDiag;

//
// Timer
//
volatile boolean timerOneTick;


// ========================== MAIN ROUTINES =============================
void setup() {
  InitDiag();
  InitFifo();
  InitTimerOne();
  InitPlayback();
  InitSensor();
  InitSequencer();
  InitApp();
}

void loop() {
  if (timerOneTick) {
    timerOneTick = false;
    EvalSensor();
    EvalApp();
    EvalPlayback();
  }
  
  EvalDiag();
}

// ========================== Timer and ISR =============================
void InitTimerOne() {
  Timer1.initialize(TICK_INTERVAL*1000);
  Timer1.attachInterrupt(TimerOneIsr);
  timerOneTick = false;
}

void TimerOneIsr() {
  timerOneTick = true;
}

// ========================== Diagnostic Support =============================
void InitDiag() {
  Serial.begin(57600);
  enDiag = false;
}

void EvalDiag() {
  char c;
  
  if (Serial.available()) {
    c = Serial.read();
    switch (c) {
      case '+': enDiag = true; break;
      case '-': enDiag = false; break;
    }
  }
}

void DiagPrintString(char* s) {
  if (enDiag) {
    Serial.print(s);
  }
}

void DiagPrintNum(int n) {
  if (enDiag) {
    Serial.print(n);
  }
}

void DiagPrintEnd() {
  if (enDiag) {
    Serial.println("");
  }
}

