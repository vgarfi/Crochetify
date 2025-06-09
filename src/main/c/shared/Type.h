#ifndef TYPE_HEADER
#define TYPE_HEADER

typedef enum {
	false = 0,
	true = 1
} boolean;

typedef enum {
    STITCH_CH,
    STITCH_SC,
    STITCH_DC
} StitchType;

typedef enum {
	COLOR,
	STITCH,
	PATTERN
} VariableType;

typedef struct RowData {
	char color[7];
    StitchType* stitches;
} RowData;

typedef int Token;

#endif