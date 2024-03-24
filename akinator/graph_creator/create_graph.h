#ifndef GRAPH_CREATE
#define GRAPH_CREATE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../common/log.h"
#include "../binary_tree/BinaryTree.h"
#include "../akinator/data/dataBaseRead.h"
#include "../onegin/fileReader.h"

FILE *graph_err_file;

#ifdef DEBUG
#define LOG(...) fileLog(graph_err_file, __VA_ARGS__)
#define OPEN_LOG_FILE() graph_err_file = fopen("log.log", "wb")
#define CLOSE_LOG_FILE() fclose(graph_err_file)
#else
#define LOG(...) do {} while (0)
#define OPEN_LOG_FILE() do {} while (0)
#define CLOSE_LOG_FILE() do {} while (0)
#endif

//#define SYSTEM_CALL(codeName, pictureName)  "dot  "#codeName"-Tpng -o"#pictureName

#define GPRAPH_CODE_START   "digraph G\n\
{\n\
    graph [dpi = 1000];\n\
    bgcolor = \"#9F8C76\"\n\
\n\
    node[shape = \"box\", style = \"rounded, filled\",\n\
    height = 1, width = 2,\n\
    fillcolor = \"#F5F5DC\",\n\
    fontsize = 17,\n\
    fontname = \"Courier New\",\n\
    fontcolor = \"#1a1718\",\n\
    penwidth = 2, color = \"#1a1718\"\n\
    ]\n\n\
"

enum GRAPH_ERRORS
{
    ERR = 1,
    NO_ERR = 0,
};

#endif