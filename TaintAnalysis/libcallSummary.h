
#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/CFG.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/CallSite.h"
#include "llvm/Support/raw_ostream.h"
#include <set>
#include <string>

using namespace std;

namespace llvm {


        class libcallSummary {
        public:
		
        };
        
		//
		// Structure which records which arguments should be marked tainted.
		//
		typedef struct CallTaintSummary {
		  static const unsigned NumArguments = 10;
		  bool TaintsReturnValue;
		  bool TaintsArgument[NumArguments];
		  bool TaintsVarargArguments;
		} CallTaintSummary;


		//
		// Holds an entry for a single function, summarizing which arguments are
		// tainted values and which point to tainted memory.
		//
		typedef struct CallTaintEntry {
		  const char *Name;
		  CallTaintSummary ValueSummary;
		  CallTaintSummary DirectPointerSummary;
	//	  CallTaintSummary RootPointerSummary;
		} CallTaintEntry;


		#define TAINTS_NOTHING \
		  { false, { }, false }
		#define TAINTS_ALL_ARGS { \
			false, \
			{ true, true, true, true, true, true, true, true, true, true }, \
			true \
		  }
		#define TAINTS_VARARGS \
		  { false, { }, true }
		#define TAINTS_RETURN_VAL \
		  { true, { }, false }

		#define TAINTS_ARG_1 \
		  { false, { true }, false }
		#define TAINTS_ARG_2 \
		  { false, { false, true }, false }
		#define TAINTS_ARG_3 \
		  { false, { false, false, true }, false }
		#define TAINTS_ARG_4 \
		  { false, { false, false, false, true }, false }

		#define TAINTS_ARG_1_2 \
		  { false, { true, true, false }, false }
		#define TAINTS_ARG_1_3 \
		  { false, { true, false, true }, false }
		#define TAINTS_ARG_1_4 \
		  { false, { true, false, false, true }, false }

		#define TAINTS_ARG_2_3 \
		  { false, { false, true, true, false }, false }
		#define TAINTS_ARG_3_4 \
		  { false, { false, false, true, true }, false }

		#define TAINTS_ARG_1_2_3 \
		  { false, { true, true, true }, false }

		#define TAINTS_ARG_1_AND_VARARGS \
		  { false, { true }, true }
		#define TAINTS_ARG_3_AND_RETURN_VAL \
		  { true, { false, false, true }, false }

		static const struct CallTaintEntry SourceTaintSummaries[] = {
		  // function  tainted values   tainted direct memory tainted root ptrs
		  { "fopen",   TAINTS_RETURN_VAL,  TAINTS_RETURN_VAL},
		  { "freopen", TAINTS_RETURN_VAL,  TAINTS_ARG_3_AND_RETURN_VAL},
		  { "fflush",  TAINTS_RETURN_VAL,  TAINTS_ARG_1},
		  { "fclose",  TAINTS_RETURN_VAL,  TAINTS_ARG_1},
		  { "setbuf",  TAINTS_NOTHING,     TAINTS_ARG_1_2},
		  { "setvbuf", TAINTS_RETURN_VAL,  TAINTS_ARG_1_2},
		  { "fread",   TAINTS_RETURN_VAL,  TAINTS_ARG_1_4},
		  { "fwrite",  TAINTS_RETURN_VAL,  TAINTS_ARG_4},
		  { "fgetc",   TAINTS_RETURN_VAL,  TAINTS_ARG_1},
		  { "getc",    TAINTS_RETURN_VAL,  TAINTS_ARG_1},
		  { "fgets",   TAINTS_RETURN_VAL,  TAINTS_ARG_1_3},
		  { "fputc",   TAINTS_RETURN_VAL,  TAINTS_ARG_2}, 
		  { "putc",    TAINTS_RETURN_VAL,  TAINTS_ARG_2},
		  { "fputs",   TAINTS_RETURN_VAL,  TAINTS_ARG_1_2},
		  { "getchar", TAINTS_RETURN_VAL,  TAINTS_NOTHING},
		  { "gets",    TAINTS_RETURN_VAL,  TAINTS_ARG_1},
		  { "putchar", TAINTS_RETURN_VAL,  TAINTS_NOTHING},
		  { "puts",    TAINTS_RETURN_VAL,  TAINTS_NOTHING},
		  { "ungetc",  TAINTS_RETURN_VAL,  TAINTS_ARG_2},
		  { "printf",  TAINTS_RETURN_VAL,  TAINTS_VARARGS},
		  { "fprintf", TAINTS_RETURN_VAL,  TAINTS_ARG_1_AND_VARARGS},
		  { "scanf",   TAINTS_RETURN_VAL,  TAINTS_VARARGS},
		  { "fscanf",  TAINTS_RETURN_VAL,  TAINTS_ARG_1_AND_VARARGS},
		  { "vscanf",  TAINTS_RETURN_VAL,  TAINTS_ARG_2},
		  { "vfscanf", TAINTS_RETURN_VAL,  TAINTS_ARG_1_3},
		  { "vprintf", TAINTS_RETURN_VAL,  TAINTS_ARG_2},
		  { "vfprintf",TAINTS_RETURN_VAL,  TAINTS_ARG_1_3},
		  { "ftell",   TAINTS_RETURN_VAL,  TAINTS_NOTHING},
		  { "feof",    TAINTS_RETURN_VAL,  TAINTS_NOTHING},
		  { "ferror",  TAINTS_RETURN_VAL,  TAINTS_NOTHING},
		  { "remove",  TAINTS_RETURN_VAL,  TAINTS_NOTHING},
		  { "fgetpos", TAINTS_RETURN_VAL,  TAINTS_ARG_1_2},
		  { "fseek",   TAINTS_RETURN_VAL,  TAINTS_ARG_1},
		  { "fsetpos", TAINTS_RETURN_VAL,  TAINTS_ARG_1},
		  { "rewind",  TAINTS_NOTHING,     TAINTS_ARG_1},
		  { "clearerr",TAINTS_NOTHING,     TAINTS_ARG_1},
		  { "perror",  TAINTS_NOTHING,     TAINTS_NOTHING},
		  { "rename",  TAINTS_RETURN_VAL,  TAINTS_NOTHING},
		  { "tmpfile", TAINTS_RETURN_VAL,  TAINTS_RETURN_VAL},
		  { "tmpnam",  TAINTS_RETURN_VAL,  TAINTS_ARG_1},
		  { "getenv",  TAINTS_RETURN_VAL,  TAINTS_RETURN_VAL},
		  { 0,         TAINTS_NOTHING,     TAINTS_NOTHING}
		};


}
