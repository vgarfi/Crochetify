#include "Crochet.h"

/* MODULE INTERNAL STATE */

// TODO checkear tema turns altura 

static Logger * _logger = NULL;

static RowNodeListADT _rowNodeList = NULL;

static Sequence * definitions = NULL;

typedef struct {
    StitchType lastRowStitch;
    int lastRowLength;
    int currentHeight;
    boolean currentWasTurn;
} CrochetBuildingState;

static CrochetBuildingState crochetBuildingState = { STITCH_INVALID, 0, 0, true };

void initializeCrochetModule() {
	_logger = createLogger("Crochet");
}

void shutdownCrochetModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}


static RowData createRowData(char * color){
    RowData node = {
        .stitchCount = 0,
        .stitches = NULL
    };
    strncpy(node.color, color, sizeof(node.color) - 1);
    node.color[sizeof(node.color) - 1] = '\0';
    return node;
}

static Pattern * searchPatternDef(char* name){
    for(int i = 0; i < definitions->count; i++) {
        ItemType itemType = definitions->itemTypes[i];
        if (itemType == ITEM_PATTERN ) {
           Pattern * pattern = (Pattern *)definitions->items[i];
           if(strcmp(name,pattern->name)){
                return pattern;
           }
        } 
    }
    return NULL;
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
        // if(crochetBuildingState.currentWasTurn && row->isTurn){
        //     logError(_logger, "Two consecutive turn rows in row number %d\n", i+1);
        //     return crochetResult;
        // }
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
    RowData node = createRowData(row->color);
    createRowNode(rowNodeList, node);
    
    for (int i = 0; i < row->elements->count; i++) {   
        ItemType itemType = row->elements->itemTypes[i];
        CrochetResult result;
        switch (itemType) {
            case ITEM_PATTERN_USE:
                result = computePatternUse((PatternUse *)row->elements->items[i], scopeList,rowNodeList);
                break;
            case ITEM_ARGUMENT:
                char* value =  ((Argument *)(row->elements->items[i]))->value;
                result = computeIdentifier(value, scopeList);
                break;
             case ITEM_MIRROR:
                result = computeMirror((Mirror*)row->elements->items[i], scopeList, rowNodeList);
                break;
            case ITEM_REPEAT:
                result = computeRepeat((Repeat*)row->elements->items[i], scopeList, rowNodeList);
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
    Pattern * pattern = searchPatternDef(patternUse->name); 
    for (int i = 0; i < patternUse->arguments->count; i++){   
        ItemType itemType = ((Argument*)(patternUse->arguments->items[i]))->argumentType; // todos por def son tipo argumento
                                                                                        // entoces del item argumuento quiero su tipo argumento
        switch (itemType){
            case ITEM_ARGUMENT:
                char* value =  ((Argument *)(patternUse->arguments->items[i]))->value;
                void* isColor = getValueByIdentifier(scopeList, value, COLOR_TYPE);
                void* isStitch = getValueByIdentifier(scopeList, value, STITCH_TYPE);
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
                computePatternUse((PatternUse *)patternUse->arguments->items[i],scopeList,rowNodeListAux);
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
    crochetResult = computePattern(patterData,pattern,scopeList,rowNodeList);
    for(int i =0; patternUse->arguments->count;i++){
        ItemType itemType = ((Argument*)(patternUse->arguments->items[i]))->argumentType; 
        if(itemType == ITEM_SEQUENCE || itemType == ITEM_PATTERN_USE){
            freeRowNodeList((RowNodeListADT)patterData.params[i].paramValue);
        }
    }
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
    
    addStitchToLastnode(rowNodeList, stitch->stitchType);
    crochetResult.succeed = true;
    return crochetResult;
}

CrochetResult computePattern(PatternData patterData, Pattern * pattern , ScopeListADT scopeList, RowNodeListADT rowNodeList) {
    CrochetResult crochetResult = {
        .stitchRows = NULL,
        .succeed = false
    };
    boolean lastDirection = false;
    RowNodeListADT rowNodeListAux = rowNodeList;
    boolean patternHasArgs = patterData.paramCount > 0? 1:0;
    if(patternHasArgs){
        insertNewScope(scopeList);
    }
    for(int i=0; i < patterData.paramCount; i++){
        if(patterData.params->paramType != PATTERN_TYPE){
            putSymbolInSymbolTable(scopeList,((Parameter *)(pattern->parameters->items[i]))->name, patterData.params[i].paramType, patterData.params[i].paramValue);
        }
    }
   

    for(int i = 0; i < pattern->body->count; i++) {
        ItemType itemType = pattern->body->itemTypes[i];
       
        if (itemType == ITEM_ROW) {
            Row * row = (Row *)pattern->body->items[i];
            crochetBuildingState.currentWasTurn = row->isTurn;
            if (lastDirection =! row->isTurn) {
                createRowData(row->color);
            }
            lastDirection = row->isTurn;
            for (int j = 0; j < row->elements->count; j++) {   
                ItemType itemType = row->elements->itemTypes[j];
                CrochetResult result;
                switch (itemType) {
                    case ITEM_PATTERN_USE:
                        result = computePatternUse((PatternUse *)row->elements->items[i], scopeList, rowNodeList);
                        break;
                    case ITEM_ARGUMENT:
                        char* value =  ((Argument *)(row->elements->items[i]))->value;
                        result = computeIdentifier(value, scopeList);
                        if(!result.succeed){
                            RowNodeListADT patternValue =  (RowNodeListADT) patterData.params[j].paramValue;
                            appendList(rowNodeList,patternValue);
                        }
                        break;
                    case ITEM_MIRROR:
                        result = computeMirror((Mirror*)row->elements->items[i], scopeList, rowNodeList);
                        break;
                    case ITEM_REPEAT:
                        result = computeRepeat((Repeat*)row->elements->items[i], scopeList, rowNodeList);
                        break;
                    case ITEM_STITCH:
                        result = computeStitch((Stitch *)row->elements->items[i], rowNodeList);
                        break;
                    default:
                        if(patternHasArgs){
                            removeLastScope(scopeList);
                        }
                        logError(_logger, "Failed to compute item of type %d in row.", itemType);
                        return crochetResult;
                        break;
                }
            if (!result.succeed) {
                if(patternHasArgs){
                    removeLastScope(scopeList);
                }
                logError(_logger, "Failed to compute item of type %d in row.", itemType);
                return crochetResult;
            }
            }
        } else {
            if(patternHasArgs){
                removeLastScope(scopeList);
            }
            logError(_logger, "A program row inside the crochet body contains an invalid item type: %d", itemType);
            return crochetResult;
        }
    }
    if(patternHasArgs){
        removeLastScope(scopeList);
    }
}

CrochetResult computeMirror(Mirror* mirror, ScopeListADT scopeList, RowNodeListADT rowNodeList) {
    CrochetResult crochetResult = {
        .stitchRows = NULL,
        .succeed = false
    };
    
    if (mirror == NULL || scopeList == NULL || rowNodeList == NULL) {
        return crochetResult;
    }

    RowNodeListADT inner = newRowNodeList();
    computePatternUse(mirror->pattern, scopeList, inner);

    for (int i = 0; i < mirror->times; ++i) {
        if (i % 2 == 0) {
            beginIteration(inner);
            while (hasNext(inner)) {
                RowData rd = next(inner);
                RowData copy = rd;
                createRowNode(rowNodeList, copy);
                for (int k = 0; k < rd.stitchCount; ++k) {
                    addStitchToLastnode(rowNodeList, rd.stitches[k]);
                }
            }
        } else {
            beginReverseIteration(inner);
            while (hasNextReverse(inner)) {
                RowData rd = nextReverse(inner);
                RowData copy = rd;
                createRowNode(rowNodeList, copy);
                for (int k = rd.stitchCount - 1; k >= 0; --k) {
                    addStitchToLastnode(rowNodeList, rd.stitches[k]);
                }
            }
        }
    }
    freeRowNodeList(inner);

    crochetResult.succeed = true;
    crochetResult.stitchRows = rowNodeList; 
    return crochetResult;
}


CrochetResult computeRepeat(Repeat* repeat, ScopeListADT scopeList, RowNodeListADT rowNodeList) {
    CrochetResult crochetResult = {
        .stitchRows = NULL,
        .succeed = false
    };
    
    if (repeat == NULL || scopeList == NULL) {
        return crochetResult;
    }
    
    for (int i = 0; i < repeat->times; i++) {
        computePatternUse(repeat->pattern, scopeList, rowNodeList);
    }

    crochetResult.succeed = true;
    return crochetResult;
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



