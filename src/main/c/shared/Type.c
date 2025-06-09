#include "Type.h"
#include <stdlib.h>

boolean isNullRowData(RowData rowData) {
    return rowData.stitches == NULL && rowData.color[0] == '\0';
}