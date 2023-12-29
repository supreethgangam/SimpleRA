#include"global.h"

bool semanticParse(){
    logger.log("semanticParse");
    switch(parsedQuery.queryType){
        case CLEAR: return semanticParseCLEAR();
        case CROSS: return semanticParseCROSS();
        case DISTINCT: return semanticParseDISTINCT();
        case EXPORT: return semanticParseEXPORT();
        case INDEX: return semanticParseINDEX();
        case JOIN: return semanticParseJOIN();
        case LIST: return semanticParseLIST();
        case LOAD: return semanticParseLOAD();
        case LOAD_MATRIX: return semanticParseLOAD();
        case PRINT: return semanticParsePRINT();
        case PRINT_MATRIX: return semanticParsePRINT();
        case PROJECTION: return semanticParsePROJECTION();
        case RENAME: return semanticParseRENAME();
        case SELECTION: return semanticParseSELECTION();
        case SORT: return semanticParseSORT();
        case SOURCE: return semanticParseSOURCE();
        case TRANSPOSE: return semanticParseTRANSPOSE();
        case CHECKSYMMETRY: return semanticParseCHECKSYMMETRY();
        case COMPUTE: return semanticParseCOMPUTE();
        case GROUP: return semanticParseGROUP() ;
        case ORDER: return semanticParseORDER() ;
        default: cout<<"SEMANTIC ERRORRRR"<<endl;
    }

    return false;
}