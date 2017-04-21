/*
 * Routines related to FIFO access
 *
 * NOTE: A FIFO entry is 16-bits
 *       Bits 15:12 - Command value for this entry
 *       bits 11: 0 - Command argument (e.g. delay count)
 */

void InitFifo() {
  for (int i=0; i<NUM_CH; i++) {
    for (int j=0; j<FIFO_DEPTH; j++) {
      fifoArray[i][j] = 0;
    }
    fifoCount[i] = 0;
    fifoPushIndex[i] = 0;
    fifoPopIndex[i] = 0;
  }
}


void SetSequenceDelay(uint8_t color, uint16_t dly) {
  PushFifo(color, FIFO_CMD_DELAY | (dly & FIFO_ARG_MASK));
}


void SetSequenceEnable(uint8_t color) {
  PushFifo(color, FIFO_CMD_SET);
}


void SetSequenceDisable(uint8_t color) {
  PushFifo(color, FIFO_CMD_CLR);
}


void PushFifo(uint8_t color, uint16_t entry) {
  uint8_t index = fifoPushIndex[color];
  
  if (fifoCount[color] < FIFO_DEPTH) {
    // Push entry into the FIFO
    fifoArray[color][index] = entry;
    
    // Bump the count
    ++fifoCount[color];
    
    // Increment the push index to the next (empty) location
    if (++fifoPushIndex[color] == FIFO_DEPTH) {
      fifoPushIndex[color] = 0;
    }
  }
}


// Returns true if there is something in the FIFO, false if it is empty
boolean PeekFifo(uint8_t color) {
  return (fifoCount[color] > 0);
}


// Don't call PopFifo unless there is something in it (use PeekFifo)
uint16_t PopFifo(uint8_t color) {
  uint8_t index = fifoPopIndex[color];
  uint16_t retVal;
  
  // Get the entry from the end of the FIFO
  retVal = fifoArray[color][index];
  
  // Decrement the count now that we "popped" the data out of the FIFO
  --fifoCount[color];
  
  // Increment the pop index to the next location to pop
  if (++fifoPopIndex[color] == FIFO_DEPTH) {
    fifoPopIndex[color] = 0;
  }
  
  return retVal;
}


