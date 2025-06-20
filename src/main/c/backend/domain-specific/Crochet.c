#include "Crochet.h"
/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

static RowNodeListADT _rowNodeList = NULL;

static Sequence * definitions = NULL;

typedef struct {
    StitchType lastRowStitch;
    int lastRowLength;
    int currentHeight;
} CrochetBuildingState;

void initializeCrochetModule() {
	_logger = createLogger("Crochet");
}

void shutdownCrochetModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}
static CrochetResult computeInnerPatternUse(PatternUse * patternUse, ScopeListADT scopeList, RowNodeListADT rowNodeList) {
    CrochetResult crochetResult = {
        .stitchRows = NULL,
        .succeed = false
    };
    
    if (patternUse == NULL || scopeList == NULL || rowNodeList == NULL) {
        logError(_logger, "Invalid pattern use or scope list.");
        return crochetResult;
    }

    // Process the pattern use and add it to the row node list
    // This is a placeholder for actual implementation logic
    // You would typically look up the pattern definition and apply it here

    crochetResult.succeed = true;
    return crochetResult;
}

CrochetResult computeCrochet(Program * program, ScopeListADT scopeList) {
    CrochetResult crochetResult = {
        .stitchRows = NULL,
        .succeed = false
    };
    
    if (program == NULL || program->body == NULL || program->body->count == 0) {
        logError(_logger, "The crochet program is empty or invalid.");
        return crochetResult;
    }

    _rowNodeList = newRowNodeList();
    definitions = program->declarationsAndPatterns;

    for(int i = 0; i < program->body->count; i++) {
        ItemType itemType = program->body->itemTypes[i];
        if (itemType == ITEM_ROW) {
            Row * row = (Row *)program->body->items[i];
            computeRow(row, scopeList, _rowNodeList);
        } else {
            logError(_logger, "A program row inside the crochet body contains an invalid item type: %d", itemType);
            return crochetResult;
        }
    }

    crochetResult.stitchRows = _rowNodeList;
    crochetResult.succeed = true;
    return crochetResult;
}

CrochetResult computeRow(Row * row, ScopeListADT scopeList, RowNodeListADT rowNodeList) {
    
    CrochetResult crochetResult = {
        .stitchRows = NULL,
        .succeed = false
    };
    
    RowData node = {
        .color = row->color, // TODO ver el tema de los negros
        .stitchCount = 0,
        .stitches = NULL
    };
    createRowNode(rowNodeList, node);
    
    for (int i = 0; i < row->elements->count; i++) {   
        ItemType itemType = row->elements->itemTypes[i];
        CrochetResult result;
        switch (itemType) {
            case ITEM_PATTERN_USE:
                result = computePatternUse((PatternUse *)row->elements->items[i], scopeList,rowNodeList);
                break;
            case ITEM_IDENTIFIER:
                result = computeIdentifier((char *)row->elements->items[i], scopeList);
                break;
            case ITEM_MIRROR:
                /* code */
                break;
            case ITEM_REPEAT:
                /* code */
                break;
            case ITEM_STITCH:
                result = computeStitch((Stitch *)row->elements->items[i], rowNodeList);
                break;
            default:
                logError(_logger, "Failed to compute item of type %d in row.", itemType);
                return crochetResult;
                break;
        }
        if (!result.succeed) {
            logError(_logger, "Failed to compute item of type %d in row.", itemType);
            return crochetResult;
        }
    }
    
}

CrochetResult computePatternUse(PatternUse * patternUse, ScopeListADT scopeList, RowNodeListADT rowNodeList) {
    CrochetResult crochetResult = {
        .stitchRows = NULL,
        .succeed = false
    };
    RowNodeListADT rowNodeListAux = newRowNodeList();
    // busco primero en la tabla el patternData
    PatternData patterData = *(PatternData*)getValueByIdentifier(scopeList,patternUse->name,PATTERN_TYPE);
    // Pattern * pattern = searchPatternDef(); // TODO
    for (int i = 0; i < patternUse->arguments->count; i++){   
        ItemType itemType = ((Argument*)(patternUse->arguments->items[i]))->argumentType; // todos por def son tipo argumento
                                                                                        // entoces del item argumuento quiero su tipo argumento
        switch (itemType){
            case ITEM_IDENTIFIER:
                   void* isColor = getValueByIdentifier(scopeList, (char*)(patternUse->arguments->items[i]), COLOR_TYPE);
                   void* isStitch = getValueByIdentifier(scopeList, (char*)(patternUse->arguments->items[i]), STITCH_TYPE);
                    if (isColor != NULL) {
                        patterData.params[i].paramValue = isColor;
                    } else if (isStitch != NULL) {
                        patterData.params[i].paramValue = isStitch;
                    } else {
                        logError(_logger, "Identifier '%s' not found in scope list.", (char*)(patternUse->arguments->items[i]));
                        return crochetResult;
                    }
                break;
            case ITEM_COLOR_VALUE:
                patterData.params[i].paramValue = patternUse->arguments->items[i];
                break;
            case ITEM_STITCH:
                patterData.params[i].paramValue = patternUse->arguments->items[i];
                break;
            case ITEM_PATTERN_USE:
                computeInnerPatternUse((PatternUse *)patternUse->arguments->items[i],scopeList,rowNodeListAux);
                patterData.params[i].paramValue = rowNodeListAux;
                rowNodeListAux = newRowNodeList();
                break;
            case ITEM_SEQUENCE:
                computeStitchList((Sequence *)(patternUse->arguments->items[i]), rowNodeListAux);
                patterData.params[i].paramValue = rowNodeListAux;
                rowNodeListAux = newRowNodeList();
                break;
            default:
                return crochetResult;
                break;
        }
    }
    // computePattern(patterData,pattern,scopeList,rowNodeList);
}



CrochetResult computeArgument(Argument * argument, ScopeListADT scopeList) {
    CrochetResult crochetResult = {
        .stitchRows = NULL,
        .succeed = false
    };
}


CrochetResult computeStitchList(Sequence * sequence, RowNodeListADT rowNodeList) {
    CrochetResult crochetResult = {
        .stitchRows = NULL,
        .succeed = false
    };

    if (sequence == NULL || sequence->count == 0) {
        return crochetResult;
    }

    for (int i = 0; i < sequence->count; i++) {
        ItemType itemType = sequence->itemTypes[i];
        if (itemType == ITEM_STITCH) {
            Stitch * stitch = (Stitch *)sequence->items[i];
            crochetResult = computeStitch(stitch, rowNodeList);
            if (!crochetResult.succeed) {
                return crochetResult;
            }
        } else {
            logError(_logger, "Invalid item type in stitch list: %d", itemType);
            return crochetResult;
        }
    }

    crochetResult.succeed = true;
    return crochetResult;
}

CrochetResult computeStitch(Stitch * stitch, RowNodeListADT rowNodeList) {
    CrochetResult crochetResult = {
        .stitchRows = NULL,
        .succeed = false
    };

    if (stitch == NULL || rowNodeList == NULL) {
        return crochetResult;
    }
    
    addStitchToLastnode(rowNodeList, stitch->stichType);
    crochetResult.succeed = true;
    return crochetResult;
}


CrochetResult computePattern(PatternData patterData, Pattern * pattern , ScopeListADT scopeList,RowNodeListADT rowNodeList) {
     CrochetResult crochetResult = {
        .stitchRows = NULL,
        .succeed = false
    };
}

CrochetResult computeIdentifier(char * identifier, ScopeListADT scopeList) {
    CrochetResult crochetResult = {
        .stitchRows = NULL,
        .succeed = false
    };
    
    if (identifier == NULL || scopeList == NULL) {
        return crochetResult;
    }

    void* isColor = getValueByIdentifier(scopeList, identifier, COLOR_TYPE);
    void* isStitch = getValueByIdentifier(scopeList, identifier, STITCH_TYPE);
    
    if (isColor != NULL) {
        changeColorToLastNode(_rowNodeList, (char *)isColor);
    } else if (isStitch != NULL) {
        addStitchToLastnode(_rowNodeList, *(StitchType *)isStitch);
    } else {
        logError(_logger, "Identifier '%s' not found in scope list.", identifier);
        return crochetResult;
    }
    
    crochetResult.succeed = true;
    return crochetResult;
}