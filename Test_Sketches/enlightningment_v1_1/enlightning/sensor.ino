/*
 * Routines related to the sensors
 */


void InitSensor() {
  sensorPorts[ULTRASONIC] = SNS1_PORT;
  sensorIncreasing[ULTRASONIC] = SNS1_ACTIVE_INCREASING;
  sensorThreshold[ULTRASONIC] = SNS1_THRESHOLD;
  sensorThreshCounts[ULTRASONIC] = SNS1_THRESH_COUNT;
  
  sensorPorts[SOUND] = SNS2_PORT;
  sensorIncreasing[SOUND] = SNS2_ACTIVE_INCREASING;
  sensorThreshold[SOUND] = SNS2_THRESHOLD;
  sensorThreshCounts[SOUND] = SNS2_THRESH_COUNT;
  
  sensorPorts[LIGHT] = SNS3_PORT;
  sensorIncreasing[LIGHT] = SNS3_ACTIVE_INCREASING;
  sensorThreshold[LIGHT] = SNS3_THRESHOLD;
  sensorThreshCounts[LIGHT] = SNS3_THRESH_COUNT;
  
  // Reset sensor state
  for (int i = 0; i<NUM_SENSORS; i++) {
    sensorTrigger[i] = false;
    sensorState[i] = false;
    sensorCurCount[i] = 0;
  }
  
  // Reset sound analyzer
  for (int i = 0; i<100; i++) {
    soundSampleArray[i] = 0;
  }
  soundSampleIndex = 0;
}


void EvalSensor() {
  uint16_t curAdcVal;
  
  for (int i=0; i<NUM_SENSORS; i++) {
    curAdcVal = analogRead(sensorPorts[i]);

    sensorTrigger[i] = false;  // Will be set for this eval if the sensor is first detected "on"
    
    if (sensorState[i]) {
      // Look for the sensor to be switched "off"
      if (sensorIncreasing[i]) {
        // Need to see consecutive readings below the threshold
        if (curAdcVal < sensorThreshold[i]) {
          if (++sensorCurCount[i] > sensorThreshCounts[i]) {
            sensorState[i] = false;
            sensorCurCount[i] = 0;
          }
        } else {
          sensorCurCount[i] = 0;
        }
      } else {
        // Need to see consecutive readings above the threshold
        if (curAdcVal > sensorThreshold[i]) {
          if (++sensorCurCount[i] > sensorThreshCounts[i]) {
            sensorState[i] = false;
            sensorCurCount[i] = 0;
          }
        } else {
          sensorCurCount[i] = 0;
        }
      }
    } else {
      // Look for the sensor to be switched "on"
      if (sensorIncreasing[i]) {
        // Need to see consecutive readings above the threshold
        if (curAdcVal > sensorThreshold[i]) {
          if (++sensorCurCount[i] > sensorThreshCounts[i]) {
            sensorState[i] = true;
            sensorTrigger[i] = true;
            sensorCurCount[i] = 0;
          }
        } else {
          sensorCurCount[i] = 0;
        }
      } else {
        // Need to see consecutive readings below the threshold
        if (curAdcVal < sensorThreshold[i]) {
          if (++sensorCurCount[i] > sensorThreshCounts[i]) {
            sensorState[i] = true;
            sensorTrigger[i] = true;
          }
        } else {
            sensorCurCount[i] = 0;
        }
      }
    } 
  }
}


boolean SensorTriggered(uint8_t sns) {
  return sensorTrigger[sns];
}


boolean SensorOn(uint8_t sns) {
  return sensorState[sns];
}
