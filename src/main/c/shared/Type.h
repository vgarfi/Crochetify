#ifndef TYPE_HEADER
#define TYPE_HEADER

typedef enum {
	false = 0,
	true = 1
} boolean;

typedef enum {
    STITCH_CH,
    STITCH_SC,
    STITCH_DC,
	STITCH_INVALID = -1
} StitchType;

typedef enum {
	COLOR_TYPE,
	STITCH_TYPE,
	PATTERN_TYPE,
	STITCH_LIST_TYPE
} VariableType;

typedef struct RowData {
	char color[8];
    StitchType* stitches;
	int stitchCount;
} RowData;

typedef int Token;

#endif