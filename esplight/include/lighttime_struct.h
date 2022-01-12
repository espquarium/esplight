
#ifndef MAX_CHANNELS
#define MAX_CHANNELS 4
#endif

#ifndef LIGHTTIMESTRUCT_H
#define LIGHTTIMESTRUCT_H

struct LightTime {
    int h;
    int m;
    int c[MAX_CHANNELS];  // 4 channels
};

#endif
