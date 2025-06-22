#include "Crochet.h"


static RowData cloneRowData(const RowData *src);

static Logger * _logger = NULL;

static RowNodeListADT _rowNodeList = NULL;

static Sequence * definitions = NULL;

static boolean lastPatternStartTurn = false;

static int processingPattern = 0;

static boolean validateStitch(StitchType stitchType);
static boolean validateRowLength(int firstRowLength, int currentRowLength, StitchType lastRowStitch);

typedef struct {
    StitchType lastRowStitch;
    int lastRowLength;
    int currentRowLength;
    int currentHeight;
    boolean currentWasTurn;
} CrochetBuildingState;

static CrochetBuildingState crochetBuildingState = { STITCH_INVALID, 0, 0, true };

static int findParamIndex(const char *name, const char **paramNames, int paramCount) {
    for (int i = 0; i < paramCount; i++) {
        if (strcmp(name, paramNames[i]) == 0) return i;
    }
    return -1;
}

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
    strncpy(node.color, color, sizeof(node.color));
    node.color[sizeof(node.color) - 1] = '\0';
    return node;
}

static Pattern * searchPatternDef(char* name){
    for(int i = 0; i < definitions->count; i++) {
        ItemType itemType = definitions->itemTypes[i];
        if (itemType == ITEM_PATTERN ) {
           Pattern * pattern = (Pattern *)definitions->items[i];
           if(strcmp(name,pattern->name) == 0){
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
            if(crochetBuildingState.currentWasTurn && row->isTurn){
                logError(_logger, "Two consecutive turn rows in row number %d\n", i+1);
                return crochetResult;
            }
            
            if (row->isTurn) {
                crochetBuildingState.currentHeight = 0;
                crochetBuildingState.lastRowLength = crochetBuildingState.currentRowLength;
            }
            
            crochetBuildingState.currentWasTurn = row->isTurn;
                        
            CrochetResult result = computeRow(row, scopeList, _rowNodeList);
            
            if (!result.succeed) {
                logError(_logger, "Failed to compute row at index %d", i+1);
                return crochetResult;
            }

            if (crochetBuildingState.currentWasTurn && crochetBuildingState.currentHeight != 1 && crochetBuildingState.currentHeight != 3) {
                logError(_logger, "Invalid TURN row height at index %d", i+1);
                return crochetResult;
            }
        } else {
            logError(_logger, "A program row inside the crochet body contains an invalid item type: %d", itemType);
            return crochetResult;
        }
    }

    // Verify row lengths:
    beginIteration(_rowNodeList);
    int firstRowLength = -1;
    int iteration = 0;
    StitchType lastRowStitch = STITCH_INVALID; 
    RowData nextNode = {0};
    while(hasNext(_rowNodeList)){
        nextNode = next(_rowNodeList);
        if(iteration % 2 == 1){
            iteration++;
            continue;
        }
        if(firstRowLength == -1){
            firstRowLength = nextNode.stitchCount;
            lastRowStitch = nextNode.stitches[nextNode.stitchCount - 1];
        } else {
            if(!validateRowLength(firstRowLength, nextNode.stitchCount, lastRowStitch)){
                logError(_logger, "Invalid row length at row: must match previous row or be one less if last stitch was CH or DC.");
                return crochetResult;
            }
            lastRowStitch = nextNode.stitches[nextNode.stitchCount - 1];
        }
        iteration++;
    }

    
    crochetResult.stitchRows = _rowNodeList;
    crochetResult.succeed = true;
    return crochetResult;
}

CrochetResult computeRow(Row * row, ScopeListADT scopeList, RowNodeListADT rowNodeList) {
    RowData node = createRowData(row->color);
    createRowNode(rowNodeList, node);

    CrochetResult crochetResult = {
        .stitchRows = NULL,
        .succeed = false
    };
    
    for (int i = 0; i < row->elements->count; i++) {   
        ItemType itemType = row->elements->itemTypes[i];
        CrochetResult result;
        switch (itemType) {
            case ITEM_PATTERN_USE:
                result = computePatternUse((PatternUse *)row->elements->items[i], scopeList,rowNodeList);
                break;
            case ITEM_ARGUMENT:
                {
                char* value =  ((Argument *)(row->elements->items[i]))->value;
                result = computeIdentifier(value, scopeList,rowNodeList);
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
                logError(_logger, "Failed to compute item of type %d in row.", itemType);
                return crochetResult;
        }
        if (!result.succeed) {
            logError(_logger, "Result did not succeed at computing item of type %d in row: %s", itemType, result.errorMsg);
            return crochetResult;
        }
    }

    int realStitchCount = 0;
    StitchType lastRealStitch = STITCH_INVALID;
    for (int i = 0; i < row->elements->count; i++) {
        ItemType itemType = row->elements->itemTypes[i];
        if (itemType == ITEM_STITCH) {
            Stitch *stitch = (Stitch *)row->elements->items[i];
            realStitchCount++;
            lastRealStitch = stitch->stitchType;
        }
    }

    if (!row->isTurn) {
        crochetBuildingState.currentRowLength = realStitchCount;
        crochetBuildingState.lastRowStitch = lastRealStitch;
    }

    crochetResult.succeed = true;
    return crochetResult;
}

CrochetResult computePatternUse(PatternUse * patternUse, ScopeListADT scopeList, RowNodeListADT rowNodeList) {
    CrochetResult crochetResult = {
        .stitchRows = NULL,
        .succeed = false
    };
    
    if (patternUse->name == NULL) {
        return computeStitchList((Sequence*) patternUse->arguments, rowNodeList);
    }
    
    PatternData* original = (PatternData*)getValueByIdentifier(scopeList, patternUse->name, PATTERN_TYPE);
    if (!original) {
        crochetResult.errorMsg = "Pattern not found in scope";
        return crochetResult;
    }
    Pattern * pattern = searchPatternDef(patternUse->name);
    if (pattern == NULL) {
        crochetResult.errorMsg = "Pattern definition not found";
        return crochetResult;
    }
    
    PatternData patterData;
    patterData.paramCount = original->paramCount;
    patterData.rowNodeList = original->rowNodeList;
    if (patterData.paramCount > 0) {
        patterData.params = malloc(patterData.paramCount * sizeof(PatternParam));
        for (int i = 0; i < patterData.paramCount; i++) {
            patterData.params[i].paramType = original->params[i].paramType;
            patterData.params[i].paramValue = original->params[i].paramValue;
        }
    } else {
        patterData.params = NULL;
    }
    RowNodeListADT rowNodeListAux = newRowNodeList();

    int argCount = patternUse->arguments ? patternUse->arguments->count : 0;
    for (int i = 0; i < argCount; i++) {
        ItemType itemType = ((Argument*)(patternUse->arguments->items[i]))->argumentType;
        Argument * arg = ((Argument*)(patternUse->arguments->items[i]));
        switch (itemType) {
            case ITEM_IDENTIFIER: {
                char* value = (char*) arg->value;
                void* isColor = getValueByIdentifier(scopeList, value, COLOR_TYPE);
                void* isStitch = getValueByIdentifier(scopeList, value, STITCH_TYPE);
                if (isColor != NULL) {
                    patterData.params[i].paramValue = isColor;
                } else if (isStitch != NULL) {
                    patterData.params[i].paramValue = isStitch;
                } else {
                    logError(_logger, "Identifier '%s' not found in scope list.", value);
                    if (rowNodeListAux){
                        freeRowNodeList(rowNodeListAux);
                        rowNodeListAux = NULL;
                    } 
                    if (patterData.params){
                        free(patterData.params);
                        patterData.params = NULL;
                    } 
                    return crochetResult;
                }
                break;
            }
            case ITEM_COLOR_VALUE:
                patterData.params[i].paramValue = arg->value;
                break;
            case ITEM_STITCH:
                {
                Stitch *stitch = (Stitch *)arg->value;
                StitchType *typePtr = malloc(sizeof(StitchType));
                *typePtr = stitch->stitchType;
                patterData.params[i].paramValue = typePtr;
                }
                break;
            case ITEM_PATTERN_USE:
                processingPattern++;
                createRowNode(rowNodeListAux,createRowData("#000000"));
                computePatternUse((PatternUse *) arg->value, scopeList, rowNodeListAux);
                processingPattern--;
                patterData.params[i].paramValue = rowNodeListAux;
                patterData.startsWithTurn = lastPatternStartTurn;
                rowNodeListAux = newRowNodeList();
                break;
            case ITEM_SEQUENCE:
                computeStitchList((Sequence *) arg->value, rowNodeListAux);
                patterData.params[i].paramValue = rowNodeListAux;
                patterData.params[i].paramType = STITCH_LIST_TYPE;
                rowNodeListAux = newRowNodeList();
                break;
            default:
                crochetResult.errorMsg = "Default value";
                if (rowNodeListAux){
                    freeRowNodeList(rowNodeListAux);
                    rowNodeListAux = NULL;
                } 
                if (patterData.params){
                    free(patterData.params);
                    patterData.params = NULL;
                } 
                return crochetResult;
        }
    }

    crochetResult = computePattern(patterData, pattern, scopeList, rowNodeList);

    for (int i = 0; i < argCount; i++) {
        ItemType itemType = ((Argument*)(patternUse->arguments->items[i]))->argumentType;
        if ((itemType == ITEM_SEQUENCE ) &&
            patterData.params[i].paramValue != original->params[i].paramValue &&
            patterData.params[i].paramValue != crochetResult.stitchRows) {
            freeRowNodeList((RowNodeListADT)patterData.params[i].paramValue);
        } else if(itemType == ITEM_STITCH && patterData.params[i].paramValue != original->params[i].paramValue) {
            free(patterData.params[i].paramValue); 
        }
    }
    if (rowNodeListAux != NULL){
        freeRowNodeList(rowNodeListAux);
        rowNodeListAux = NULL;
    } 

    if (patterData.params){
        free(patterData.params);
        patterData.params = NULL;
    } 

    if (!crochetResult.succeed) {
        crochetResult.errorMsg = "ComputePattern Error";
    }

    return crochetResult;
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

    if (stitch == NULL || rowNodeList == NULL || !validateStitch(stitch->stitchType)) {
        return crochetResult;
    }
    if(getSize(rowNodeList) == 0){
        RowData new = {0};
        createRowNode(rowNodeList, new);
    }
    addStitchToLastnode(rowNodeList, stitch->stitchType);
    
    crochetResult.succeed = true;
    return crochetResult;
}

CrochetResult computePattern(PatternData patterData, Pattern * pattern , ScopeListADT scopeList, RowNodeListADT rowNodeList) {
    boolean lastDirection = false;
    CrochetResult crochetResult = {
        .stitchRows = NULL,
        .succeed = false
    };
    if (pattern == NULL || scopeList == NULL || rowNodeList == NULL) {
        logError(_logger, "Invalid pattern or scope list or row node list.");
        return crochetResult;
    }

    int paramCount = (pattern->parameters == NULL) ? 0 : pattern->parameters->count;
    boolean patternHasArgs = paramCount > 0;
    const char **paramNames = NULL;
    RowNodeListADT * cdtToFree;
    if (patternHasArgs) {
        insertNewScope(scopeList);
        
        cdtToFree = calloc(paramCount,sizeof(RowNodeListADT));
        paramNames = malloc(paramCount * sizeof(char*));
        for (int i = 0; i < paramCount; i++) {
            paramNames[i] = ((Parameter*)pattern->parameters->items[i])->name;
            if(patterData.params[i].paramType == PATTERN_TYPE){
                cdtToFree[i] = (RowNodeListADT)patterData.params[i].paramValue;
            }
        }
        for (int i = 0; i < paramCount; i++) {
            if (patterData.params[i].paramType != PATTERN_TYPE) {
                putSymbolInSymbolTable(scopeList, ((Parameter *)(pattern->parameters->items[i]))->name, patterData.params[i].paramType, patterData.params[i].paramValue);
            }
        }
    }
    for (int i = 0; i < pattern->body->count; i++) {
      
        ItemType itemType = pattern->body->itemTypes[i];
        if (itemType == ITEM_ROW) {
            Row * row = (Row *)pattern->body->items[i];
            if(processingPattern > 0 && i == 0){
                lastPatternStartTurn = row->isTurn;
            }
            if (lastDirection != row->isTurn) {
               
                RowData node = createRowData(row->color);
                createRowNode(rowNodeList, node);
            }
            if(crochetBuildingState.currentWasTurn && row->isTurn){
                logError(_logger, "Two consecutive turn rows in row number %d inside Pattern: %s \n", i+1,pattern->name == NULL? "Anonymous" : pattern->name);
                return crochetResult;
            }
             crochetBuildingState.currentWasTurn = row->isTurn;

            if (row->isTurn) {
                crochetBuildingState.currentHeight = 0;
            }
            lastDirection = row->isTurn;
            for (int j = 0; j < row->elements->count; j++) {
                ItemType elemType = row->elements->itemTypes[j];
                CrochetResult result;
                switch (elemType) {
                    case ITEM_PATTERN_USE:
                        result = computePatternUse((PatternUse *)row->elements->items[j], scopeList, rowNodeList);
                        break;
                    case ITEM_ARGUMENT: {
                        char* value = ((Argument *)(row->elements->items[j]))->value;
                        if (patternHasArgs) {
                            int argIdx = findParamIndex(value, paramNames, paramCount);
                            if (argIdx < 0) {
                                logError(_logger, "Argument '%s' not found in parameter list.", value);
                                if (paramNames) free(paramNames);
                                if (patternHasArgs) removeLastScope(scopeList);
                                return crochetResult;
                            }
                            if (patterData.params[argIdx].paramType == PATTERN_TYPE) {
                                RowNodeListADT patternValue = (RowNodeListADT) patterData.params[argIdx].paramValue;
                                RowNodeListADT patternValueCopy = cloneRowNodeList(patternValue);

                                if(lastPatternStartTurn == false){
                                    appendNodeandList(rowNodeList, patternValueCopy);

                                    deintegrateListAndFirst(patternValueCopy);
                                    free(patternValueCopy);
                                }
                                else{
                                    appendList(rowNodeList, patternValueCopy);
                                    deintegrateList(patternValueCopy);
                                    free(patternValueCopy);
                                }
                                
                                result.succeed = (patternValue != NULL);
                            }else if(patterData.params[argIdx].paramType == STITCH_LIST_TYPE){
                                RowNodeListADT patternValue = (RowNodeListADT) patterData.params[argIdx].paramValue;
                                appendNodeandList(rowNodeList,patternValue);

                            }else {
                                result = computeIdentifier(value, scopeList,rowNodeList);
                            }
                        } else {
                            result = computeIdentifier(value, scopeList,rowNodeList);
                        }
                        break;
                    }
                    case ITEM_MIRROR:
                        result = computeMirror((Mirror*)row->elements->items[j], scopeList, rowNodeList);
                        break;
                    case ITEM_REPEAT:
                        result = computeRepeat((Repeat*)row->elements->items[j], scopeList, rowNodeList);
                        break;
                    case ITEM_STITCH:
                        result = computeStitch((Stitch *)row->elements->items[j], rowNodeList);
                        break;
                    default:
                        if (paramNames) free(paramNames);
                        if (patternHasArgs) removeLastScope(scopeList);
                        logError(_logger, "Failed to compute item of type %d in row.", elemType);
                        return crochetResult;
                }
                if (!result.succeed) {
                    if (paramNames) free(paramNames);
                    if (patternHasArgs) removeLastScope(scopeList);
                    logError(_logger, "Failed to compute item of type %d in row.", elemType);
                    return crochetResult;
                }
            }
        } else {
            if (paramNames) free(paramNames);
            if (patternHasArgs) removeLastScope(scopeList);
            logError(_logger, "A program row inside the crochet body contains an invalid item type: %d", itemType);
            return crochetResult;
        }
    }

    if(pattern->body->count > 0){
            Row * row = (Row *)pattern->body->items[0];
            if(processingPattern > 0 ){
                lastPatternStartTurn = row->isTurn;
            }
    }
    if(paramCount){
        for(int i = 0; i < paramCount; i++){
            if(cdtToFree[i] != NULL){
                deintegrateListFull(cdtToFree[i]);
                free(cdtToFree[i]);
            }
        }
        free(cdtToFree);
    }
    if (paramNames) free(paramNames);
    if (patternHasArgs) removeLastScope(scopeList);
   
    crochetResult.succeed = true;
    return crochetResult;
}

CrochetResult computeMirror(Mirror* mirror, ScopeListADT scopeList, RowNodeListADT rowNodeList) {
    CrochetResult crochetResult = {
        .stitchRows = NULL,
        .succeed = false
    };

    if (!mirror || !scopeList || !rowNodeList) {
        return crochetResult;
    }

    RowNodeListADT auxList = newRowNodeList();

    CrochetResult innerResult = computePatternUse((PatternUse *)(mirror->pattern), scopeList, auxList);
    if (!innerResult.succeed) {
        freeRowNodeList(auxList);
        return crochetResult;
    }
    boolean isAnonPattern = mirror->pattern->name == NULL ? true : false;

    int rowCount = getSize(auxList);
    if (rowCount == 0) {
        freeRowNodeList(auxList);
        crochetResult.succeed = true;
        crochetResult.stitchRows = rowNodeList;
        return crochetResult;
    }

    RowData *rows = malloc(rowCount * sizeof(RowData));
    if (!rows) {
        freeRowNodeList(auxList);
        return crochetResult;
    }
    beginIteration(auxList);
    for (int i = 0; i < rowCount; i++) {
        RowData n = next(auxList);
        rows[i] = cloneRowData(&n);
    }

    for (int t = 0; t < mirror->times; t++) {
        if (t % 2 == 0) {
            for (int i = rowCount - 1; i >= 0; i--) {
                RowData orig = rows[i];
                RowData node = createRowData(orig.color);
                if(!(t == 0 && i == (rowCount - 1)) && !isAnonPattern){
                    createRowNode(rowNodeList, node);
                }
                for (int j = orig.stitchCount - 1; j >= 0; j--) {
                    addStitchToLastnode(rowNodeList, orig.stitches[j]);
                    if (!crochetBuildingState.currentWasTurn) {
                        crochetBuildingState.lastRowStitch = orig.stitches[j];
                        crochetBuildingState.currentRowLength++;
                    }
                }
            }
        } else {
            for (int i = 0; i < rowCount; i++) {
                RowData orig = rows[i];
                RowData node = createRowData(orig.color);
                if(!isAnonPattern){
                    createRowNode(rowNodeList, node);
                }
                for (int j = 0; j < orig.stitchCount; j++) {
                    addStitchToLastnode(rowNodeList, orig.stitches[j]);
                    if (!crochetBuildingState.currentWasTurn) {
                        crochetBuildingState.lastRowStitch = orig.stitches[j];
                        crochetBuildingState.currentRowLength++;
                    }
                }
            }
        }
    }

    for (int i = 0; i < rowCount; i++) {
        if (rows[i].stitches) free(rows[i].stitches);
    }
    free(rows);
    freeRowNodeList(auxList);

    crochetResult.succeed = true;
    crochetResult.stitchRows = rowNodeList;
    return crochetResult;
}

static RowData cloneRowData(const RowData *src) {
    RowData dst = *src;
    if (src->stitchCount > 0 && src->stitches != NULL) {
        dst.stitches = malloc(src->stitchCount * sizeof(StitchType));
        memcpy(dst.stitches, src->stitches, src->stitchCount * sizeof(StitchType));
    } else {
        dst.stitches = NULL;
    }
    return dst;
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
        if((repeat->pattern)->name != NULL && i+1 != repeat->times){
            RowData rd = {0};
            createRowNode(rowNodeList, rd);
        }
    }

    crochetResult.succeed = true;
    return crochetResult;
}

CrochetResult computeIdentifier(char * identifier, ScopeListADT scopeList, RowNodeListADT rowNodeList) {
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
        changeColorToLastNode(rowNodeList, (char *)isColor);
    } else if (isStitch != NULL) {
        if (!validateStitch(*(StitchType *)isStitch)) {
            return crochetResult;
        }
        
        addStitchToLastnode(rowNodeList, *(StitchType *)isStitch);
        if (!crochetBuildingState.currentWasTurn) {
            crochetBuildingState.lastRowStitch = (*(StitchType *)isStitch);
            crochetBuildingState.currentRowLength++;
        }
    } else {
        logError(_logger, "Identifier '%s' not found in scope list.", identifier);
        return crochetResult;
    }
    
    crochetResult.succeed = true;
    return crochetResult;
}

static boolean validateStitch(StitchType stitchType) {
    if (crochetBuildingState.currentWasTurn) {
        if (stitchType != STITCH_CH) {
            logError(_logger, "You can only make TURN columns with CH stitches");
            return false;
        }
        crochetBuildingState.currentHeight++;
        if (crochetBuildingState.currentHeight > 3) {
            logError(_logger, "TURN exceeds maximum height (3 CH stitches)");
            return false;
        }
        return true;
    }

    if (stitchType == STITCH_DC && crochetBuildingState.currentHeight != 3) {
        logError(_logger, "You must have a 3 height TURN before using DC stitches (current height %d)", crochetBuildingState.currentHeight);
        return false;
    }

    if (stitchType == STITCH_SC && crochetBuildingState.currentHeight != 1) {
        logError(_logger, "You must have a height of 1 TURN before using SC stitches (current height %d)", crochetBuildingState.currentHeight);
        return false;
    }

    return true;
}

static boolean validateRowLength(int firstRowLength, int currentRowLength, StitchType lastRowStitch) {
    if (lastRowStitch == STITCH_CH || lastRowStitch == STITCH_DC) {
        return currentRowLength == firstRowLength - 1;
    }
    return currentRowLength == firstRowLength;
}