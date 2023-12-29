#include "global.h"
/**
 * @brief 
 * SYNTAX: R <- ORDER BY column_name sorting_order ON relation_name
 * 
 * sorting_order = ASC | DESC 
 */
bool syntacticParseGROUP()
{
    logger.log("syntacticParseGROUP");
    if (tokenizedQuery.size() != 13 || tokenizedQuery[2] != "GROUP" || tokenizedQuery[3] != "BY" || tokenizedQuery[5] != "FROM" || tokenizedQuery[7] != "HAVING" || tokenizedQuery[11] != "RETURN")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = GROUP;
		parsedQuery.groupResultRelationName = tokenizedQuery[0];
    parsedQuery.sortRelationName = tokenizedQuery[6];
    parsedQuery.sortColumnList.push_back(tokenizedQuery[4]);
		parsedQuery.sortStartegyList.push_back("ASC");
		parsedQuery.groupThreshold = stoi(tokenizedQuery[10]);

		string name="", type="";
		int flag = 0;
		for(auto c : tokenizedQuery[8]){
			if(c == ')'){
				break;
			}
			if(c == '('){
				flag = 1;
				continue;
			}
			if(flag){
				name += c;
			}
			else{
				type += c;
			}
		}
		parsedQuery.groupCompareColumnName = name;
		parsedQuery.compareAggregateType = type;

		name="", type="";
		flag = 0;
		for(auto c : tokenizedQuery[12]){
			if(c == ')'){
				break;
			}
			if(c == '('){
				flag = 1;
				continue;
			}
			if(flag){
				name += c;
			}
			else{
				type += c;
			}
		}
		parsedQuery.groupReturnColumnName = name;
		parsedQuery.returnAggregateType = type;

		string binaryOperator = tokenizedQuery[9];
		if (binaryOperator == "<")
        parsedQuery.joinBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.joinBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=")
        parsedQuery.joinBinaryOperator = GEQ;
    else if (binaryOperator == "<=")
        parsedQuery.joinBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.joinBinaryOperator = EQUAL;
    else
    {
      cout << "SYNTAX ERROR" << endl;
      return false;
    }
    return true;
}

bool semanticParseGROUP()
{
    logger.log("semanticParseGROUP");

    if (tableCatalogue.isTable(parsedQuery.groupResultRelationName))
		{
				cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
				return false;
		}

    if (!tableCatalogue.isTable(parsedQuery.sortRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.sortColumnList[0], parsedQuery.sortRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.groupCompareColumnName, parsedQuery.sortRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.groupReturnColumnName, parsedQuery.sortRelationName))
		{
				cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
				return false;
		}
    return true;
}

float aggregateFunction(vector<int> arr, string type){
	if(type == "AVG"){
		float sum = 0;
		for(auto x : arr){
			sum += x;
		}
		return sum/arr.size();
	}
	else if(type == "SUM"){
		float sum = 0;
		for(auto x : arr){
			sum += x;
		}
		return sum;
	}
	else if(type == "MAX"){
		float max = INT_MIN;
		for(auto x : arr){
			if(x > max){
				max = x;
			}
		}
		return max;
	}
	else if(type == "MIN"){
		float min = INT_MAX;
		for(auto x : arr){
			if(x < min){
				min = x;
			}
		}
		return min;
	}
	else if(type == "COUNT"){
		return arr.size();
	}
	return 0;
}

void executeGROUP()
{
    logger.log("executeGROUP");
    Table *table = tableCatalogue.getTable(parsedQuery.sortRelationName);
    Table *result = table->sort();

		int sortColumnIndex = table->getColumnIndex(parsedQuery.sortColumnList[0]);
		int compareColumnIndex = table->getColumnIndex(parsedQuery.groupCompareColumnName);
		int returnColumnIndex = table->getColumnIndex(parsedQuery.groupReturnColumnName);
		string returnColumn = parsedQuery.returnAggregateType + parsedQuery.groupReturnColumnName;

		Table *new_table = new Table(parsedQuery.groupResultRelationName, vector<string>{parsedQuery.groupCompareColumnName, returnColumn});

		Cursor cursor(result->tableName, 0);
		vector<int> row;
		row = cursor.getNext();
		for(int rowCounter = 0; rowCounter < result->rowCount; rowCounter++){
			int x = row[sortColumnIndex];
			vector<int> aggregateArray = {};
			vector<int> returnArray = {};
			while(rowCounter < result->rowCount && row[sortColumnIndex] == x){
				rowCounter++;				
				aggregateArray.push_back(row[compareColumnIndex]);
				returnArray.push_back(row[returnColumnIndex]);
				row = cursor.getNext();
			}		
			rowCounter--;
			if(parsedQuery.joinBinaryOperator == LESS_THAN){
					if(aggregateFunction(aggregateArray, parsedQuery.compareAggregateType) < parsedQuery.groupThreshold){
						new_table->writeRow<int>(vector<int>{x, int(floor(aggregateFunction(returnArray, parsedQuery.returnAggregateType)))});
					}
				}
				else if(parsedQuery.joinBinaryOperator == GREATER_THAN){
					if(aggregateFunction(aggregateArray, parsedQuery.compareAggregateType) > parsedQuery.groupThreshold){
						new_table->writeRow<int>(vector<int>{x, int(floor(aggregateFunction(returnArray, parsedQuery.returnAggregateType)))});
					}
				}
				else if(parsedQuery.joinBinaryOperator == GEQ){
					if(aggregateFunction(aggregateArray, parsedQuery.compareAggregateType) >= parsedQuery.groupThreshold){
						new_table->writeRow<int>(vector<int>{x, int(floor(aggregateFunction(returnArray, parsedQuery.returnAggregateType)))});
					}
				}
				else if(parsedQuery.joinBinaryOperator == LEQ){
					if(aggregateFunction(aggregateArray, parsedQuery.compareAggregateType) <= parsedQuery.groupThreshold){
						new_table->writeRow<int>(vector<int>{x, int(floor(aggregateFunction(returnArray, parsedQuery.returnAggregateType)))});
					}
				}
				else if(parsedQuery.joinBinaryOperator == EQUAL){
					if(aggregateFunction(aggregateArray, parsedQuery.compareAggregateType) == parsedQuery.groupThreshold){
						new_table->writeRow<int>(vector<int>{x, int(floor(aggregateFunction(returnArray, parsedQuery.returnAggregateType)))});
					}
				}
		}
		result->unload();
		delete result;
		new_table->blockify();
		tableCatalogue.insertTable(new_table);
    return;
}

// T1 <- GROUP BY Department_ID FROM EMPLOYEE HAVING AVG(Salary) > 50000 RETURN MAX(Salary)

// T2 <- GROUP BY Department_ID FROM EMPLOYEE HAVING SUM(Salary) > 70000 RETURN SUM(Salary)

// T3 <- GROUP BY Department_ID FROM EMPLOYEE HAVING COUNT(Salary) > 0 RETURN AVG(Salary)

// T4 <- GROUP BY Department_ID FROM EMPLOYEE HAVING MIN(Salary) < 40000 RETURN SUM(Salary)
