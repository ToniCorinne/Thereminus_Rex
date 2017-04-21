/*
 * Routines related to the playback machine
 */

void InitPlayback() {
  for (int i=0; i<NUM_CH; i++) {
    playbackTimer[i] = 0;
    playbackBusy[i] = false;
  }
}

void EvalPlayback() {
  uint16_t cmd;
  
  for (int i=0; i<NUM_CH; i++) {
    if (playbackBusy[i]) {
      // Evaluate the timer for this color
      if (--playbackTimer[i] == 0) {
        playbackBusy[i] = false;
      }
    }
    
    // Look for a command to process
    if (PeekFifo(i) && !playbackBusy[i]) {
      cmd = PopFifo(i);
      
      switch (cmd & FIFO_CMD_MASK) {
        case FIFO_CMD_DELAY:
          // Load our timer
          playbackTimer[i] = (cmd & FIFO_ARG_MASK) / TICK_INTERVAL;
          playbackBusy[i] = true;
          break;
          
        case FIFO_CMD_SET:
          LoadSequencerCmd(i, true);
          break;
          
        case FIFO_CMD_CLR:
          LoadSequencerCmd(i, false);
          break;
      }
    }
  }
}
