#include "global.h"
/**
 * @brief 
 * SYNTAX: PRINT relation_name
 */
bool syntacticParsePRINT()
{
    logger.log("syntacticParsePRINT");
    if (tokenizedQuery.size() == 3 && tokenizedQuery[1] == "MATRIX") 
    {
        parsedQuery.queryType = PRINT_MATRIX;
        parsedQuery.printRelationName = tokenizedQuery[2];
        return true;
    }
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = PRINT;
    parsedQuery.printRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParsePRINT()
{
    logger.log("semanticParsePRINT");
    if(parsedQuery.queryType == PRINT && !tableCatalogue.isTable(parsedQuery.printRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    if(parsedQuery.queryType == PRINT_MATRIX && !matrixCatalogue.isMatrix(parsedQuery.printRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}

void executePRINT()
{
    logger.log("executePRINT");
    if(parsedQuery.queryType == PRINT_MATRIX)
    {
        Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.printRelationName);
        matrix->print();
        return;
    }
    Table* table = tableCatalogue.getTable(parsedQuery.printRelationName);
    table->print();
    return;
}
