/*
 * Routines related to interfacing with the sequencer
 */

void InitSequencer() {
  Serial1.begin(57600);
  
  InitSequencerMap();
  
  // Reset all channels on power-up
  for (int i=0; i<NUM_CH; i++) {
    SendToSequencer(sequencerMapArray[i], false);
  }
}

// InitSequencerMap is where you assoicate the specific channel on a particular sequencer with
// a color.  The sequencerMapArray values can range from 0 to 31 as follows:
//   Sequencer 0: 0 - 7 (for outputs A - G)
//   Sequencer 1: 8 - 15 (for outputs A - G)
//   Sequencer 2: 16 - 23 (for outputs A - G)
//   Sequencer 3: 24 - 31 (for outputs A - G)
//
void InitSequencerMap() {
  sequencerMapArray[cR1] = MapToChannel(0, 0);
  sequencerMapArray[cR2] = MapToChannel(1, 0);
  sequencerMapArray[cR3] = MapToChannel(1, 1);
  sequencerMapArray[cG1] = MapToChannel(0, 1);
  sequencerMapArray[cG2] = MapToChannel(1, 2);
  sequencerMapArray[cG3] = MapToChannel(1, 3);
  sequencerMapArray[cB1] = MapToChannel(1, 4);
  sequencerMapArray[cB2] = MapToChannel(3, 3);
  sequencerMapArray[cB3] = MapToChannel(0, 2);
  sequencerMapArray[cW1] = MapToChannel(2, 0);
  sequencerMapArray[cW2] = MapToChannel(2, 1);
  sequencerMapArray[cW3] = MapToChannel(3, 0);
  sequencerMapArray[cE1] = MapToChannel(3, 1);
  sequencerMapArray[cE2] = MapToChannel(3, 2);
}


//
// seqNum - Sequencer Board number (0 - 3)
// chNum - Channel on sequencer (0 - 7)
//
uint8_t MapToChannel(uint8_t seqNum, uint8_t chNum) {
  return (seqNum * 8) + chNum;
}


void LoadSequencerCmd(uint8_t color, boolean en) {
  uint8_t ch = sequencerMapArray[color];
  
  SendToSequencer(ch, en);
}


void SendToSequencer(uint8_t ch, boolean en) {
  Serial1.print("=");
  Serial1.print(ch >> 3, DEC);
  Serial1.print(",");
  Serial1.print(ch & 0x07, DEC);
  Serial1.print(",");
  if (en) {
    Serial1.print("1");
  } else {
    Serial1.print("0");
  }
  Serial1.print(",");
  Serial1.print((char) 0x0D);
}
