#include "Generator.h"

/* MODULE INTERNAL STATE */

const char _indentationCharacter = ' ';
const char _indentationSize = 4;
static Logger * _logger = NULL;
static char * symbolNames[] = {
        "ch_symbol",
        "sc_symbol",
        "dc_symbol",
        "ch_rotated",
        NULL
    };

void initializeGeneratorModule() {
	_logger = createLogger("Generator");
}

void shutdownGeneratorModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** PRIVATE FUNCTIONS */
static void printPatternUse(PatternUse *use, int indent);
static const char _expressionTypeToCharacter(const ExpressionType type);
static void _generateConstant(const unsigned int indentationLevel, Constant * constant);
static void _generateEpilogue(void);
static void _generateExpression(const unsigned int indentationLevel, Expression * expression);
static void _generateFactor(const unsigned int indentationLevel, Factor * factor);
static void _generateProgram(Program * program);
static void _generatePrologue(void);
static char * _indentation(const unsigned int indentationLevel);
static void _output(const unsigned int indentationLevel, const char * const format, ...);
static void _generateSequence(Sequence *seq);
static void _generateRepeat(Repeat *repeat);
static void _generateTurn(Turn *turn); 
static void printSequence(Sequence *seq, int indent);

static const char * itemTypeToString(ItemType type) {
    switch (type) {
        case ITEM_STITCH: return "Stitch";
        case ITEM_ROW: return "Row";
        case ITEM_TURN: return "Turn";
        case ITEM_REPEAT: return "Repeat";
        case ITEM_MIRROR: return "Mirror";
        case ITEM_PATTERN: return "Pattern";
        case ITEM_PATTERN_USE: return "PatternUse";
        case ITEM_PARAMETER: return "Parameter";
        case ITEM_ARGUMENT: return "Argument";
        case ITEM_DECLARATION: return "Declaration";
        case ITEM_ASSIGNMENT: return "Assignment";
        case ITEM_IDENTIFIER: return "Identifier";
        case ITEM_COLOR_VALUE: return "ColorValue";
        case ITEM_SEQUENCE: return "Sequence";
        default: return "Unknown";
    }
}
static const char * parameterTypeToString(VariableType type) {
    switch (type) {
        case COLOR_TYPE: return "Color";
        case STITCH_TYPE: return "Stitch";
        case PATTERN_TYPE: return "Pattern";
        default: return "Unknown";
    }
}
/**
 * Converts an expression type to the proper character of the operation
 * involved, or returns '\0' if that's not possible.
 */
static const char _expressionTypeToCharacter(const ExpressionType type) {
	switch (type) {
		case ADDITION: return '+';
		case DIVISION: return '/';
		case MULTIPLICATION: return '*';
		case SUBTRACTION: return '-';
		default:
			logError(_logger, "The specified expression type cannot be converted into character: %d", type);
			return '\0';
	}
}

static void _generateConstant(const unsigned int indentationLevel, Constant * constant) {
	_output(indentationLevel, "%s", "[ $C$, circle, draw, black!20\n");
	_output(1 + indentationLevel, "%s%d%s", "[ $", constant->value, "$, circle, draw ]\n");
	_output(indentationLevel, "%s", "]\n");
}

static void _generateExpression(const unsigned int indentationLevel, Expression * expression) {
	_output(indentationLevel, "%s", "[ $E$, circle, draw, black!20\n");
	switch (expression->type) {
		case ADDITION:
		case DIVISION:
		case MULTIPLICATION:
		case SUBTRACTION:
			_generateExpression(1 + indentationLevel, expression->leftExpression);
			_output(1 + indentationLevel, "%s%c%s", "[ $", _expressionTypeToCharacter(expression->type), "$, circle, draw, purple ]\n");
			_generateExpression(1 + indentationLevel, expression->rightExpression);
			break;
		case FACTOR:
			_generateFactor(1 + indentationLevel, expression->factor);
			break;
		default:
			logError(_logger, "The specified expression type is unknown: %d", expression->type);
			break;
	}
	_output(indentationLevel, "%s", "]\n");
}


static void _generateFactor(const unsigned int indentationLevel, Factor * factor) {
	_output(indentationLevel, "%s", "[ $F$, circle, draw, black!20\n");
	switch (factor->type) {
		case CONSTANT:
			_generateConstant(1 + indentationLevel, factor->constant);
			break;
		case EXPRESSION:
			_output(1 + indentationLevel, "%s", "[ $(, circle, draw, purple ]\n");
			_generateExpression(1 + indentationLevel, factor->expression);
			_output(1 + indentationLevel, "%s", "[ $), circle, draw, purple ]\n");
			break;
		default:
			logError(_logger, "The specified factor type is unknown: %d", factor->type);
			break;
	}
	_output(indentationLevel, "%s", "]\n");
}

static void printIndent(int indent) {
    for (int i = 0; i < indent; ++i) putchar(' ');
}

static void printStitch(Stitch *stitch, int indent) {
    if (!stitch) return;
    const char *name = stitch->stitchType == STITCH_CH ? "CH" :
                       stitch->stitchType == STITCH_SC ? "SC" : "DC";
    printIndent(indent); printf("Stitch(%s)\n", name);
}

static void printParameter(Parameter *param, int indent) {
    if (!param) return;
    printIndent(indent);
    printf("Parameter(name=%s, type=%s)\n",
           param->name,
           parameterTypeToString(param->paramType));
}

static void printArgument(Argument *arg, int indent) {
    if (!arg) return;

    printIndent(indent);
    printf("Argument(type=%s, valueType=%s)\n",
           itemTypeToString(arg->type),
           itemTypeToString(arg->argumentType));

    if (arg->value) {
        printIndent(indent + 2);
        printf("value: ");
        switch (arg->argumentType) {
            case ITEM_IDENTIFIER:
                printf("%s\n", (char*)arg->value);
                break;
            case ITEM_COLOR_VALUE:
                printf("%s\n", (char*)arg->value);
                break;
            case ITEM_STITCH:
                printStitch((Stitch *)arg->value, 0);
                break;
            case ITEM_PATTERN_USE:
                printPatternUse((PatternUse *)arg->value, indent + 2);
                break;
            default:
                printf("0x%p\n", arg->value);
                break;
        }
    }
}

static void printRow(Row *row, int indent) {
    if (!row) return;
    printIndent(indent);
    printf("Row color=%s, isTurn=%d\n", row->color ? row->color : "#000000", row->isTurn);
    if (row->elements) printSequence(row->elements, indent + 2);
}

static void printPatternUse(PatternUse *use, int indent) {
    if (!use) return;
    printIndent(indent); printf("PatternUse name=%s\n", use->name);
    if (use->arguments) printSequence(use->arguments, indent + 2);
}

static void printPattern(Pattern *pattern, int indent) {
    if (!pattern) return;
    printIndent(indent); printf("Pattern name=%s\n", pattern->name);
    if (pattern->parameters) {
        printIndent(indent + 2); printf("parameters:\n");
        printSequence(pattern->parameters, indent + 4);
    }
    if (pattern->body) {
        printIndent(indent + 2); printf("body:\n");
        printSequence(pattern->body, indent + 4);
    }
}

static void printDeclaration(Declaration *decl, int indent) {
    if (!decl) return;
    printIndent(indent);
    printf("Declaration(type=%s, var=%s, value=%s)\n", decl->typeName, decl->varName, decl->value);
}

static void printAssignment(Assignment *assign, int indent) {
    if (!assign) return;
    printIndent(indent);
    printf("Assignment(var=%s, value=%s)\n", assign->varName, assign->value);
}

static void printTurn(Turn *turn, int indent) {
    if (!turn) return;
    printIndent(indent); printf("Turn");
    if (turn->chains) {
        printf(" chains:\n");
        printSequence(turn->chains, indent + 2);
    }
    if (turn->color) {
        printf(" color=%s", turn->color);
    }
    printf("\n");
}

static void printRepeat(Repeat *repeat, int indent) {
    if (!repeat) return;
    printIndent(indent); printf("Repeat count=%d\n", repeat->times);
    if (repeat->pattern) printPatternUse(repeat->pattern, indent + 2);
}


static void printSequence(Sequence *seq, int indent) {
    if (!seq) return;
    for (int i = 0; i < seq->count; ++i) {
        switch (seq->itemTypes[i]) {
            case ITEM_STITCH:
                printStitch((Stitch*)seq->items[i], indent);
                break;
            case ITEM_ROW:
                printRow((Row*)seq->items[i], indent);
                break;
            case ITEM_TURN:
                printTurn((Turn*)seq->items[i], indent);
                break;
            case ITEM_REPEAT:
                printRepeat((Repeat*)seq->items[i], indent);
                break;
            case ITEM_MIRROR:
                printIndent(indent); printf("Mirror\n");
                break;
            case ITEM_PATTERN:
                printPattern((Pattern*)seq->items[i], indent);
                break;
            case ITEM_PATTERN_USE:
                printPatternUse((PatternUse*)seq->items[i], indent);
                break;
            case ITEM_PARAMETER:
                printParameter((Parameter*)seq->items[i], indent);
                break;
            case ITEM_ARGUMENT:
                printArgument((Argument*)seq->items[i], indent);
                break;
            case ITEM_DECLARATION:
                printDeclaration((Declaration*)seq->items[i], indent);
                break;
            case ITEM_ASSIGNMENT:
                printAssignment((Assignment*)seq->items[i], indent);
                break;
            default:
                printIndent(indent); printf("UnknownItem\n");
                break;
        }
    }
}

static void printProgram(Program *program, int indent) {
    printIndent(indent); printf("Program\n");
    printIndent(indent + 2); printf("declarationsAndPatterns:\n");
    if (program->declarationsAndPatterns) printSequence(program->declarationsAndPatterns, indent + 4);
    printIndent(indent + 2); printf("body:\n");
    if (program->body) printSequence(program->body, indent + 4);
}

static void _generatePrologue(void) {
	_output(0, "%s",
		"from PIL import Image, ImageDraw, ImageFont, ImageOps\n\n"
		"def tint_symbol(image, color):\n"
		"   r, g, b = color\n"
		"   base = Image.new('RGBA', image.size, (r, g, b, 0))\n"
		"   alpha = image.getchannel('A')\n"
		"   base.putalpha(alpha)\n\n"
		"   return base\n"
		"def hex_to_rgb(hex_color):\n"
		"   hex_color = hex_color.lstrip('#')\n"
		"   if len(hex_color) == 3:\n"
        "       hex_color = ''.join([c * 2 for c in hex_color])\n"
        "   return tuple(int(hex_color[i:i+2], 16) for i in (0, 2, 4))\n\n"
	);
}

static void _generateCanvas(int width, int height){
    _output(0, "width, height = %d, %d\n", width, height);
    _output(0, "%s",
        "img = Image.new('RGB', (width, height), color='white')\n"
        "draw = ImageDraw.Draw(img)\n\n"
        "ch_symbol = Image.open('stitches/CH.png').convert('RGBA')\n"
        "sc_symbol = Image.open('stitches/SC.png').convert('RGBA')\n"
        "dc_symbol = Image.open('stitches/DC.png').convert('RGBA')\n\n"
        "ch_size, sc_size, dc_size = (35,25), (35,25), (35,80)\n"
        "ch_symbol, sc_symbol, dc_symbol = ch_symbol.resize(ch_size), sc_symbol.resize(sc_size), dc_symbol.resize(dc_size)\n"
        "rotated_size = (35, 25)\n"
        "ch_rotated = ch_symbol.rotate(90, expand=True)\n"
        "ch_rotated = ch_rotated.resize(rotated_size)\n"
        "x_offset, y_offset, small_y_offset = 35, 25, 12\n"
        "x, y = 50, height - 40\n\n"
        "# From this point onwards you start drawing your awesome crochet !\n"
    );
}

static void _generateEpilogue(void){
    _output(0, "%s",
        "\n# Ding! You heard that? Your crochet's just out of the oven!\n"
        "img = img.convert('RGB')\n"
        "img.save('awesome_crochet.png')\n"
    );
}

static void _clearColor(void){
    for(int i = 0; symbolNames[i] != NULL; i++){
        _output(0, "%s = tint_symbol(%s, hex_to_rgb('#000000'))\n",
            symbolNames[i],
            symbolNames[i]
        );
    }
    _output(0, "\n");
}

static void _generateColorChange(char * colorHex){
    for(int i = 0; symbolNames[i] != NULL; i++){
        _output(0, "%s = tint_symbol(%s, hex_to_rgb('%s'))\n",
            symbolNames[i],
            symbolNames[i],
            colorHex
        );
    }
    _output(0, "\n");
}

static void _beginTurn(boolean outsideTurn, boolean right){
    if(outsideTurn){
        return;
    }

    if (right) {
        _output(0, "x -= x_offset\n");
    } else {
        _output(0, "x += x_offset\n");
    }
}

static void _endTurn(boolean outsideTurn, boolean right){
    char * x_offset = right ? "x -= x_offset\n" : "x += x_offset\n";

    _output(0, "%s", x_offset);

    if(outsideTurn){
        _output(0, "y -= y_offset\n");
    }
}

// Only draws 'CH' stitches vertically !!
static void _generateVerticalStitch(void){
    _output(0, "%s", 
        "y -= y_offset\n"
        "img.paste(ch_rotated, (x, y), ch_rotated)\n"
    );
}

static void _generateHorizontalStitch(StitchType stitch, boolean right){
    switch(stitch){
        case STITCH_CH:
            _output(0, "%s", "img.paste(ch_symbol, (x, y), ch_symbol)\n");
            break;
        case STITCH_SC:
            _output(0, "%s", "img.paste(sc_symbol, (x, y), sc_symbol)\n");
            break;
        case STITCH_DC:
            _output(0, "%s", "img.paste(dc_symbol, (x, y), dc_symbol)\n");
    }
    if(right){
        _output(0, "%s", "x += x_offset\n");
    } else {
        _output(0, "%s", "x -= x_offset\n");
    }
}

static char * _indentation(const unsigned int level) {
	return indentation(_indentationCharacter, level, _indentationSize);
}

static void _output(const unsigned int indentationLevel, const char * const format, ...) {
	va_list arguments;
	va_start(arguments, format);
	char * indentation = _indentation(indentationLevel);
	char * effectiveFormat = concatenate(2, indentation, format);
	vfprintf(stdout, effectiveFormat, arguments);
	fflush(stdout);
	free(effectiveFormat);
	free(indentation);
	va_end(arguments);
}

void generate(CrochetResult * crochetResult) {
    logDebugging(_logger, "Generating final output...");
    RowNodeListADT rowNodeList = crochetResult->stitchRows;
    _generatePrologue();
    beginIteration(rowNodeList);
    int canvasWidth = BASE_CANVAS_SIZE;
    int canvasHeight = BASE_CANVAS_SIZE;
    int maxStitches;
    if(hasNext(rowNodeList)){
        maxStitches = next(rowNodeList).stitchCount;
        canvasWidth += maxStitches * STITCH_WIDTH;
        canvasHeight += getSize(rowNodeList) * STITCH_HEIGHT;
    }
    _generateCanvas(canvasWidth, canvasHeight);
    
    // Finally... Time to draw!
    beginIteration(rowNodeList);
    RowData current = {0};
    boolean turn = false;
    boolean outsideTurn = true;
    boolean right = true;
    boolean color = false;
    StitchType lastStitch;
    while(hasNext(rowNodeList)){
        current = next(rowNodeList);
        if(current.color[0] != '\0' && strcasecmp(current.color, "#000000") != 0){
            _generateColorChange(current.color);
            color = true;
        }
        if(turn){
            _beginTurn(outsideTurn, right);
            for(int i = 0; i < current.stitchCount; i++){
                _generateVerticalStitch();
            }
            _endTurn(outsideTurn, right);
            turn = false;
            right = !right;
        } else {
            for(int i = 0; i < current.stitchCount; i++){
                _generateHorizontalStitch(current.stitches[i], right);
                lastStitch = current.stitches[i];
            }
            outsideTurn = lastStitch == STITCH_SC ? true : false;
            turn = true;
        }

        if(color){
            color = false;
            _clearColor();
        }
    }

    _generateEpilogue();
    logDebugging(_logger, "Generation is done.");
}
