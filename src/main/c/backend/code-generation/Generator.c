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
static void _generatePrologue(void);
static char * _indentation(const unsigned int indentationLevel);
static void _output(const unsigned int indentationLevel, const char * const format, ...);
static void _generateSequence(Sequence *seq);
static void _generateRepeat(Repeat *repeat);
static void _generateTurn(Turn *turn); 

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
        "dot_x = 50 - 10\n"
        "dot_y = height - 27\n"
        "dot_radius = 3\n"
        "draw.ellipse((dot_x - dot_radius, dot_y - dot_radius, dot_x + dot_radius, dot_y + dot_radius),fill='black')\n"
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
