/*
 * Routines that implement the application logic go here
 */


//
// Thread 0 (Ultrasonic sensor thread) constants
//
#define THREAD_IDLE       0
#define T0_EMPTY_RUNNING  1
#define T0_EMPTY_WAITING  2
#define T0_FULL_RUNNING   3
#define T0_FULL_WAITING   4
#define T0_SOUND_RUNNING  5

//
// Thread 1 (Light sensor thread) constants
//
#define T1_NORMAL_RUNNING 1
#define T1_SOUND_RUNNING  2
#define T1_SOUND_WAITING  3

//
// Thread 2 (Sound sensor thread) constants
//
#define T2_WAITING        1



void InitApp() {
  // Load a pointer to each thread function so we can quickly index the array to execute the functions
  threadFuncs[0] = EvalThread0;
  threadFuncs[1] = EvalThread1;
  threadFuncs[2] = EvalThread2;
  
  // Reset the state for each thread
  for (int i=0; i<NUM_PARALLEL_THREADS; i++) {
    threadCounters[i] = 0;
    threadState[i] = THREAD_IDLE;
  }
  
  noisyEnvironment = false;
}


void EvalApp() {
  uint8_t prevState, newState;
  
  for (int i=0; i<NUM_PARALLEL_THREADS; i++) {
    // Evaluate each thread
    prevState = threadState[i];
    threadFuncs[i]();
    newState = threadState[i];
    
    if (prevState != newState) {
      DiagPrintString("T");
      DiagPrintNum(i);
      DiagPrintString(": ");
      DiagPrintNum(prevState);
      DiagPrintString(" -> ");
      DiagPrintNum(newState);
      if (noisyEnvironment) {
        DiagPrintString(" N");
      }
      DiagPrintEnd();
    }
  }
}


// =========================== THREADS =============================

// White Lightning thread associated with ultrasonic sensor
void EvalThread0() {
  switch (threadState[0]) {
    case THREAD_IDLE:
      if (SensorOn(ULTRASONIC)) {
        if (!noisyEnvironment) {
          FullWhiteSequence();
          threadState[0] = T0_FULL_RUNNING;
        } else {
          if (SensorTriggered(SOUND)) {
            FullWhiteSequence();
            threadState[0] = T0_SOUND_RUNNING;
          }
        }
      } else {
        EmptyWhiteSequence();
        threadState[0] = T0_EMPTY_RUNNING;
      }
      break;
      
    case T0_EMPTY_RUNNING:
      // Wait until the sequence has finished running then load a long wait (since we're empty)
      if (!WhiteSequenceBusy()) {
        threadCounters[0] = random(10000, 15000) / TICK_INTERVAL;
        threadState[0] = T0_EMPTY_WAITING;
      }
      break;
    
    case T0_EMPTY_WAITING:
      // Wait for our inactivity period to end or for another trigger
      if ((--threadCounters[0] == 0) || noisyEnvironment || SensorTriggered(ULTRASONIC)) {
        threadState[0] = THREAD_IDLE;
      }
      break;
    
    case T0_FULL_RUNNING:
      // Wait until the sequence has finished running then load a short wait (since we're full)
      if (!WhiteSequenceBusy()) {
        threadCounters[0] = random(1000, 2000) / TICK_INTERVAL;
        threadState[0] = T0_FULL_WAITING;
      }
      break;
    
    case T0_FULL_WAITING:
      // Wait for our inactivity period to end or for another trigger
      if ((--threadCounters[0] == 0) || noisyEnvironment) {
        threadState[0] = THREAD_IDLE;
      }
      break;
    
    case T0_SOUND_RUNNING:
      // Wait until the sequence has finished and then go back to look for another trigger immediately
      if (!WhiteSequenceBusy()) {
        threadState[0] = T0_EMPTY_WAITING;
      }
      break;
  }
}


// Red (Light Sensor) Lightning Thread
void EvalThread1() {
  switch (threadState[1]) {
    case THREAD_IDLE:
      if (!noisyEnvironment) {
        if (SensorTriggered(LIGHT)) {
          RedSequence();
          threadState[1] = T1_NORMAL_RUNNING;
        }
      } else {
        if (SensorTriggered(SOUND)) {
          RedSequence();
          threadState[0] = T1_SOUND_RUNNING;
        }
      }
      break;
      
    case T1_NORMAL_RUNNING:
      // Wait until the sequence has finished running
      if (!RedSequenceBusy()) {
        threadState[1] = THREAD_IDLE;
      }
      break;
    
    case T1_SOUND_RUNNING:
      // Wait until the sequence has finished running
      if (!RedSequenceBusy()) {
        threadState[1] = THREAD_IDLE;
      }
      break; 
  }
}


// Green/Blue (Sound) Lightning Thread
void EvalThread2() {
  uint8_t soundSum = 0;
  uint8_t subMask;
  
  // Evaluate the ambient noise detector
  if (SensorOn(SOUND)) {
    soundSampleArray[soundSampleIndex] = 1;
  } else {
    soundSampleArray[soundSampleIndex] = 0;
  }
  if (++soundSampleIndex == 100) soundSampleIndex = 0;
  for (int i=0; i<100; i++) {
    soundSum += soundSampleArray[i];
  }
  noisyEnvironment = (soundSum > SOUND_THRESH);
  
  // Evaluate our state
  switch (threadState[2]) {
    case THREAD_IDLE:
      if (SensorTriggered(SOUND)) {
        subMask = random(0,3) + 1;
        
        if ((subMask & 0x01) != 0) {
          GreenSequence();
        }
        
        if ((subMask & 0x02) != 0) {
          BlueSequence();
        }
        
        threadState[2] = T2_WAITING;
      }
      break;
    
    case T2_WAITING:
      // Wait until the sequence(s) have finished running
      if (!GreenSequenceBusy() && !BlueSequenceBusy()) {
        threadState[2] = THREAD_IDLE;
      }
      break;
  }
}


// =========================== SEQUENCE GENERATORS =============================

// ------------------ RED CHANNELS ----------------------
void RedSequence() {
  uint16_t shudders, onDur, offDur;
  uint8_t subMask;
  
  shudders = random(3, 8);
  for (int i=0; i<shudders; i++) {
    onDur = random(80, 300);
    offDur = random(40, 200);

    SetSequenceEnable(cR1);
    SetSequenceDelay(cR1, onDur);
    SetSequenceDisable(cR1);
    SetSequenceDelay(cR1, offDur);
    if (onDur > 160) {
      subMask = random(0,3) + 1;
      if (subMask & 0x01) {
        SetSequenceDelay(cR2, random(10, 30));
        SetSequenceEnable(cR2);
        SetSequenceDelay(cR2, random(10, 30));
        SetSequenceDisable(cR2);
      }
      if (subMask & 0x02) {
        SetSequenceDelay(cR3, random(10, 30));
        SetSequenceEnable(cR3);
        SetSequenceDelay(cR3, random(10, 30));
        SetSequenceDisable(cR3);
      }
    }
  }
}

boolean RedSequenceBusy() {
  return (PeekFifo(cR1) || PeekFifo(cR2) || PeekFifo(cR3));
}


// ------------------ GREEN CHANNELS ----------------------
void GreenSequence() {
  uint16_t shudders, onDur, offDur;
  uint8_t subMask;
  
  shudders = random(3, 8);
  for (int i=0; i<shudders; i++) {
    onDur = random(80, 300);
    offDur = random(40, 200);

    SetSequenceEnable(cG1);
    SetSequenceDelay(cG1, onDur);
    SetSequenceDisable(cG1);
    SetSequenceDelay(cG1, offDur);
    if (onDur > 160) {
      subMask = random(0,3) + 1;
      if (subMask & 0x01) {
        SetSequenceDelay(cG2, random(10, 30));
        SetSequenceEnable(cG2);
        SetSequenceDelay(cG2, random(10, 30));
        SetSequenceDisable(cG2);
      }
      if (subMask & 0x02) {
        SetSequenceDelay(cG3, random(10, 30));
        SetSequenceEnable(cG3);
        SetSequenceDelay(cG3, random(10, 30));
        SetSequenceDisable(cG3);
      }
    }
  }
}

boolean GreenSequenceBusy() {
  return (PeekFifo(cG1) || PeekFifo(cG2) || PeekFifo(cG3));
}

// ------------------ BLUE CHANNELS ----------------------
void BlueSequence() {
  uint16_t shudders, onDur, offDur;
  uint8_t subMask;
  
  shudders = random(3, 8);
  for (int i=0; i<shudders; i++) {
    onDur = random(80, 300);
    offDur = random(40, 200);

    SetSequenceEnable(cB1);
    SetSequenceDelay(cB1, onDur);
    SetSequenceDisable(cB1);
    SetSequenceDelay(cB1, offDur);
    if (onDur > 160) {
      subMask = random(0,3) + 1;
      if (subMask & 0x01) {
        SetSequenceDelay(cB2, random(10, 30));
        SetSequenceEnable(cB2);
        SetSequenceDelay(cB2, random(10, 30));
        SetSequenceDisable(cB2);
      }
      if (subMask & 0x02) {
        SetSequenceDelay(cB3, random(10, 30));
        SetSequenceEnable(cB3);
        SetSequenceDelay(cB3, random(10, 30));
        SetSequenceDisable(cB3);
      }
    }
  }
}

boolean BlueSequenceBusy() {
  return (PeekFifo(cB1) || PeekFifo(cB2) || PeekFifo(cB3));
}

// ------------------ WHITE CHANNELS ----------------------

void FullWhiteSequence() {
  uint16_t shudders, onDur, offDur;
  uint8_t subMask1, subMask2;
  
  // Determine if W1 and/or E1 will participate
  subMask1 = random(0,3) + 1;
  
  // White EL wires
  if ((subMask1 & 0x01) != 0) {
    shudders = random(3, 8);
    for (int i=0; i<shudders; i++) {
      onDur = random(80, 300);
      offDur = random(40, 200);

      SetSequenceEnable(cW1);
      SetSequenceDelay(cW1, onDur);
      SetSequenceDisable(cW1);
      SetSequenceDelay(cW1, offDur);
      if (onDur > 200) {
         subMask2 = random(0,3) + 1;
        if ((subMask2 & 0x01) != 0) {
          SetSequenceDelay(cW2, random(10, 30));
          SetSequenceEnable(cW2);
          SetSequenceDelay(cW2, random(10, 40));
          SetSequenceDisable(cW2);
        }
        if ((subMask2 & 0x02) != 0) {
          SetSequenceDelay(cW3, random(5, 25));
          SetSequenceEnable(cW3);
          SetSequenceDelay(cW3, random(10, 30));
          SetSequenceDisable(cW3);
        }
      }
    }
  }
  
  // Eggshell EL wires
  if ((subMask1 & 0x02) != 0) {
    shudders = random(4, 9);
    for (int i=0; i<shudders; i++) {
      onDur = random(70, 300);
      offDur = random(50, 200);

      SetSequenceEnable(cE1);
      SetSequenceDelay(cE1, onDur);
      SetSequenceDisable(cE1);
      SetSequenceDelay(cE1, offDur);
      if (onDur > 180) {
        SetSequenceDelay(cE2, random(10, 30));
        SetSequenceEnable(cE2);
        SetSequenceDelay(cE2, random(10, 40));
        SetSequenceDisable(cE2);
      }
    }
  }
}

boolean WhiteSequenceBusy() {
  return (PeekFifo(cW1) || PeekFifo(cW2) || PeekFifo(cW3) || PeekFifo(cE1) || PeekFifo(cE2));  
}

// Controls W2, W3, E2 separately or in pairs as an attractor
void EmptyWhiteSequence() {
  uint8_t color1, color2;
  uint16_t shudders, onDur, offDur;
  
  color1 = GetWhiteAttractor();
  shudders = random(3, 6);
  for (int i=0; i<shudders; i++) {
    onDur = random(80, 300);
    offDur = random(40, 200);

    // First lightening flash
    SetSequenceEnable(color1);
    SetSequenceDelay(color1, random(80, 300));
    SetSequenceDisable(color1);
    SetSequenceDelay(color1, random(40, 200));
  }
  
  // We pick a second color a percentage specified below
  if (random(0,101) > 40) {
    shudders = random(2, 5);
    for (int i=0; i<shudders; i++) {
      color2 = GetWhiteAttractor();
      SetSequenceEnable(color2);
      SetSequenceDelay(color2, random(80, 300));
      SetSequenceDisable(color2);
      SetSequenceDelay(color2, random(40, 200));
    }
  }
}

uint8_t GetWhiteAttractor() {
  switch(random(0,3)) {
    case 0: return cW2;
    case 1: return cW3;
    case 2: return cE2;
    default: return cE2;
  }
}


