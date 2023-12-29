#include "global.h"
/**
 * @brief 
 * SYNTAX: R <- ORDER BY column_name sorting_order ON relation_name
 * 
 * sorting_order = ASC | DESC 
 */
bool syntacticParseORDER()
{
    logger.log("syntacticParseORDER");
    if (tokenizedQuery.size() != 8 || tokenizedQuery[2] != "ORDER" || tokenizedQuery[3] != "BY" || tokenizedQuery[6] != "ON")
    {
        cout << "SYNTAC ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = ORDER;
		parsedQuery.orderResultRelationName = tokenizedQuery[0];
    parsedQuery.sortRelationName = tokenizedQuery[7];
    parsedQuery.sortColumnList.push_back(tokenizedQuery[4]);
		if(tokenizedQuery[5] == "ASC"){
			parsedQuery.sortStartegyList.push_back("ASC");
		} else {
			parsedQuery.sortStartegyList.push_back("DESC");
		}
    return true;
}

bool semanticParseORDER()
{
    logger.log("semanticParseORDER");

    if (tableCatalogue.isTable(parsedQuery.orderResultRelationName))
		{
				cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
				return false;
		}

    if (!tableCatalogue.isTable(parsedQuery.sortRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.sortColumnList[0], parsedQuery.sortRelationName))
		{
				cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
				return false;
		}
    return true;
}

void executeORDER()
{
    logger.log("executeORDER");
    Table *table = tableCatalogue.getTable(parsedQuery.sortRelationName);
    Table *result = table->sort();

    Table *new_table = new Table(parsedQuery.orderResultRelationName, table->columns);
    Cursor cursor(result->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < result->rowCount; rowCounter++)
    {
        row = cursor.getNext();
        new_table->writeRow<int>(row);
    }
		new_table->blockify();
		tableCatalogue.insertTable(new_table);
    result->unload();
    delete result;
    return;
}

// O <- ORDER BY Maths_marks ASC ON Marks