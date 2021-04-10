#ifndef GLOBE_INDEX_H
#define GLOBE_INDEX_H

#define TRACE_FOCUS BADDR

#define WRECENT (1<<10)
#define WMEM (1<<11)
#define WPERM (1<<12)

#define GLOBE_PERM_IVAL (4 * HOURS)
#define GLOBE_MEM_IVAL (9 * HOURS)

#define GLOBE_INDEX_GRID 3
#define GLOBE_SPECIAL_INDEX 70
#define GLOBE_LAT_MULT (360 / GLOBE_INDEX_GRID + 1)
#define GLOBE_MIN_INDEX (1000)
#define GLOBE_MAX_INDEX (180 / GLOBE_INDEX_GRID * GLOBE_LAT_MULT + GLOBE_MIN_INDEX)

#define TDATE_FORMAT "%Y/%m/%d"

#define TRACE_STALE (15 * SECONDS)
#define TRACE_MIN_ELAPSED (1642) // milliseconds

#define TRACE_RECENT_POINTS (84)
#define TRACE_CACHE_EXTRA (16)
#define TRACE_CACHE_POINTS (TRACE_RECENT_POINTS + TRACE_CACHE_EXTRA)
#define TRACE_CACHE_LIFETIME (30 * MINUTES)
struct traceCacheEntry {
    int32_t stateIndex;
    int32_t offset;
    int32_t len;
    int32_t leg_marker;
} __attribute__ ((__packed__));

struct traceCache {
    int32_t entriesLen;
    uint64_t startStamp;
    struct traceCacheEntry entries[TRACE_CACHE_POINTS];
    char json[TRACE_CACHE_POINTS * 256];
};

struct tile {
    int south;
    int west;
    int north;
    int east;
};

void checkNewDay(uint64_t now);
void checkNewDayLocked(uint64_t now);
int globe_index(double lat_in, double lon_in);
int globe_index_index(int index);
void init_globe_index(struct tile *s_tiles);
//void *load_state(void *arg);
void *load_blobs(void *arg);
void *save_blobs(void *arg);
void save_blob(int blob);
void writeInternalState();
void *jsonTraceThreadEntryPoint(void *arg);
ssize_t stateBytes(int len);
ssize_t stateAllBytes(int len);
void traceRealloc(struct aircraft *a, int len);
void traceCleanup(struct aircraft *a);
int traceAdd(struct aircraft *a, uint64_t now);
int traceUsePosBuffered();
void traceMaintenance(struct aircraft *a, uint64_t now);

int handleHeatmap(uint64_t now);

struct craftArray {
    struct aircraft **list;
    int len; // index of highest entry + 1
    int alloc; // memory allocated for aircraft pointers
    pthread_mutex_t mutex;
};


void ca_init (struct craftArray *ca);
void ca_destroy (struct craftArray *ca);
void ca_remove (struct craftArray *ca, struct aircraft *a);
void ca_add (struct craftArray *ca, struct aircraft *a);
void set_globe_index (struct aircraft *a, int new_index);

// this format is fixed, don't change.
// if the latitude has bit 30 set (lat & (1<<30)), it's an info entry:
// the lowest 12 bits of the lat contain squawk digits as a decimal number
// lon and alt together contain the 8 byte callsign
struct heatEntry {
    int32_t hex;
    int32_t lat;
    int32_t lon;
    int16_t alt;
    int16_t gs;
} __attribute__ ((__packed__));

#endif
