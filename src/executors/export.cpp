#include "global.h"

/**
 * @brief 
 * SYNTAX: EXPORT <relation_name> 
 */

bool syntacticParseEXPORT()
{
    logger.log("syntacticParseEXPORT");
    if  (tokenizedQuery.size() == 3 && tokenizedQuery[1] == "MATRIX") 
    {
        parsedQuery.queryType = EXPORT;
        parsedQuery.exportRelationName = tokenizedQuery[2];
        return true;
    }
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = EXPORT;
    parsedQuery.exportRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParseEXPORT()
{
    logger.log("semanticParseEXPORT");

    if (tokenizedQuery.size() == 3 && tokenizedQuery[1] == "MATRIX") 
    {
        if (matrixCatalogue.isMatrix(parsedQuery.exportRelationName))
            return true;
        cout << "SEMANTIC ERROR: No such relation exists" << endl;
        return false;
    }

    //Table should exist
    if (tableCatalogue.isTable(parsedQuery.exportRelationName))
        return true;
    cout << "SEMANTIC ERROR: No such relation exists" << endl;
    return false;
}

void executeEXPORT()
{
    logger.log("executeEXPORT");
    if (tokenizedQuery.size() == 3 && tokenizedQuery[1] == "MATRIX") 
    {
        Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.exportRelationName);
        matrix->makePermanent();
        return;
    }
    Table* table = tableCatalogue.getTable(parsedQuery.exportRelationName);
    table->makePermanent();
    return;
}