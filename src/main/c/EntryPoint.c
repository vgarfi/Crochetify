#include "backend/code-generation/Generator.h"
#include "backend/domain-specific/Calculator.h"
#include "frontend/lexical-analysis/FlexActions.h"
#include "frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "frontend/syntactic-analysis/BisonActions.h"
#include "frontend/syntactic-analysis/SyntacticAnalyzer.h"
#include "shared/CompilerState.h"
#include "shared/Environment.h"
#include "shared/Logger.h"
#include "shared/String.h"
#include "backend/domain-specific/Crochet.h"

/**
 * The main entry-point of the entire application. If you use "strtok" to
 * parse anything inside this project instead of using Flex and Bison, I will
 * find you, and I will kill you (Bryan Mills; "Taken", 2008).
 */
const int main(const int count, const char ** arguments) {
	Logger * logger = createLogger("EntryPoint");
	initializeFlexActionsModule();
	initializeBisonActionsModule();
	initializeSyntacticAnalyzerModule();
	initializeAbstractSyntaxTreeModule();

	initializeCrochetModule();
	initializeGeneratorModule();

	// Logs the arguments of the application.
	for (int k = 0; k < count; ++k) {
		logDebugging(logger, "Argument %d: \"%s\"", k, arguments[k]);
	}

	// Begin compilation process.
	CompilerState compilerState = {
		.abstractSyntaxtTree = NULL,
		.scopeList = NULL,
		.succeed = true,
		.value = 0
	};
	const SyntacticAnalysisStatus syntacticAnalysisStatus = parse(&compilerState);
	CompilationStatus compilationStatus = SUCCEED;
	Program * program = compilerState.abstractSyntaxtTree;
	
	/*
		Representation for program:
		BEGIN CROCHET
			CH CH CH CH CH CH CH;
			TURN CH CH CH;
			#FFA010 CH SC DC SC CH SC;
			#FF0000 TURN CH CH CH;
			 #00FF00 CH CH SC DC SC SC SC;
		END CROCHET
	*/
	StitchType firstLine[] = {STITCH_CH, STITCH_CH, STITCH_CH, STITCH_SC, STITCH_CH, STITCH_CH, STITCH_CH};
	StitchType secondLine[] = {STITCH_CH, STITCH_CH, STITCH_CH};
	StitchType thirdLine[] = {STITCH_CH, STITCH_SC, STITCH_DC, STITCH_SC, STITCH_CH, STITCH_SC};
	StitchType fourthLine[] = {STITCH_CH, STITCH_CH, STITCH_CH};
	StitchType fifthLine[] = {STITCH_CH, STITCH_CH, STITCH_SC, STITCH_DC, STITCH_SC, STITCH_SC, STITCH_SC};

	RowData rn1 = {
		.color = {0},
		.stitchCount = 7,
		.stitches = firstLine
	};

	RowData rn2 = {
		.color = {0},
		.stitchCount = 3,
		.stitches = secondLine
	};
	
	RowData rn3 = {
		.color = {'#', 'F', 'F', 'A', '0', '1', '0', 0},
		.stitchCount = 6,
		.stitches = thirdLine
	};

	RowData rn4 = {
		.color = {'#', 'F', 'F', '0', '0', '0', '0', 0},
		.stitchCount = 3,
		.stitches = fourthLine
	};

	RowData rn5 = {
		.color = {'#', '0', '0', 'F', 'F', '0', '0', 0},
		.stitchCount = 7,
		.stitches = fifthLine
	};
	
	CrochetResult newResult = {
		.succeed = true,
		.stitchRows = newRowNodeList()
	};

	createRowNode(newResult.stitchRows, rn1);
	createRowNode(newResult.stitchRows, rn2);
	createRowNode(newResult.stitchRows, rn3);
	createRowNode(newResult.stitchRows, rn4);
	createRowNode(newResult.stitchRows, rn5);

	if (syntacticAnalysisStatus == ACCEPT) {
		// ----------------------------------------------------------------------------------------
		// Beginning of the Backend... ------------------------------------------------------------

		// computeCrochet(program, compilerState.scopeList);
		/*

		Nuestro computeExpression, llamado computeCrochetAST, agarra el AST y coloca en computationResult.value del structu qeu devuelve algun valor de retorno

		logDebugging(logger, "Computing expression value...");
		ComputationResult computationResult = computeExpression(program->expression);
		if (computationResult.succeed) {
			compilerState.value = computationResult.value;
			generate(&compilerState);
		}
		else {
			logError(logger, "The computation phase rejects the input program.");
			compilationStatus = FAILED;
		}
		// ...end of the Backend. -----------------------------------------------------------------
		// ----------------------------------------------------------------------------------------
		*/
	        generate(&newResult);

		}
	else {
		logError(logger, "The syntactic-analysis phase rejects the input program.");
		compilationStatus = FAILED;
	}

	freeRowNodeList(newResult.stitchRows);

	logDebugging(logger, "Releasing AST resources...");
	releaseProgram(program);
	logDebugging(logger, "Releasing modules resources...");
	shutdownGeneratorModule();
	shutdownCalculatorModule();
	shutdownCrochetModule();
	shutdownAbstractSyntaxTreeModule();
	shutdownSyntacticAnalyzerModule();
	shutdownBisonActionsModule();
	shutdownFlexActionsModule();
	logDebugging(logger, "Compilation is done.");
	destroyLogger(logger);
	return compilationStatus;
}
