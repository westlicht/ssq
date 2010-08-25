#ifndef __DEFINES_H__
#define __DEFINES_H__

/** default output latency in ms */
#define OUTPUT_LATENCY      100

/** number of sequences in a pattern */
#define NUM_SEQUENCES       4

/** number of lines in a sequence */
#define NUM_LINES           8

/** number of steps in a line */
#define NUM_STEPS           32

/** number of sources */
#define NUM_SOURCES         (NUM_LINES * 2)

/* line modes */
#define LINE_MODE_OFF       0
#define LINE_MODE_NOTE      1
#define LINE_MODE_VEL       2
#define LINE_MODE_GATE      3
#define LINE_MODE_LEN       4
#define LINE_MODE_MIDI      5
#define LINE_MODE_ADD       6
#define LINE_MODE_CTRL      7
#define LINE_MODE_MODE      8

/* play modes */
#define PLAY_MODE_FWD       0
#define PLAY_MODE_BWD       1
#define PLAY_MODE_PINGPONG  2
#define PLAY_MODE_FWD_BWD   3
#define PLAY_MODE_RANDOM    4

/* step modes */
#define STEP_MODE_ON        0
#define STEP_MODE_OFF       1
#define STEP_MODE_SKIP      2

/* sync modes */
#define SYNC_MODE_AUTO      0
#define SYNC_MODE_MANUAL    1

#endif /*__DEFINES_H__*/
