#include"global.h"
/**
 * @brief File contains method to process SORT commands.
 * 
 * syntax:
 * R <- SORT relation_name BY column_name IN sorting_order
 * 
 * sorting_order = ASC | DESC 
 */
bool syntacticParseSORT(){
    logger.log("syntacticParseSORT");
     if(tokenizedQuery.size() < 6 || tokenizedQuery[2]!="BY"){
        cout<<"SYNTAX ERROR\n" ;
        return false ;
     }
     parsedQuery.sortRelationName = tokenizedQuery[1] ;
     parsedQuery.queryType = SORT ;
     vector<string> cols ;
     bool found = false ;
     int in_index=-1;
     for(int i=3;i<tokenizedQuery.size();i++){
        if(tokenizedQuery[i]=="IN"){
            found = true ;
            in_index=i ;
            break ;
        }
        else{
            parsedQuery.sortColumnList.push_back(tokenizedQuery[i]) ;
        }
     }
     if(!found){
        cout<<"SYNTAX ERROR\n" ;
        return false ;
     }
     else{
        for(int i=in_index+1;i<tokenizedQuery.size();i++){
            if(tokenizedQuery[i].compare("ASC")!=0 and tokenizedQuery[i].compare("DESC")!=0){
                // cout<<"server" ;
                cout<<tokenizedQuery[i]<<"\n" ;
                cout<<"SYNTAX ERROR\n" ;
                return false ;
            }
            else{
                parsedQuery.sortStartegyList.push_back(tokenizedQuery[i]) ;
            }
        }

        if(parsedQuery.sortColumnList.size()!=parsedQuery.sortStartegyList.size()){
            // cout<<"hello" ;
            cout<<"SYNTAX ERROR\n" ;
            return false ;
        }
    }
    return true ;
}

bool semanticParseSORT(){
    logger.log("semanticParseSORT");

    if(!tableCatalogue.isTable(parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Relation doesn't exist"<<endl;
        return false;
    }


    for(int i=0 ;i<parsedQuery.sortColumnList.size();i++){
        if(!tableCatalogue.isColumnFromTable(parsedQuery.sortColumnList[i], parsedQuery.sortRelationName)){
            cout<<"SEMANTIC ERROR: Column "<<parsedQuery.sortColumnList[i]<<" doesn't exist in relation"<<endl;
            return false;
        }        
    }

    return true;
}

void executeSORT(){
    logger.log("executeSORT");
    // cout<<this->blockCount<<"\n" ;
    Table* table = tableCatalogue.getTable(parsedQuery.sortRelationName);
    Table* result = table->sort() ;
    string name = table->tableName;
    vector<string> columns = table->columns;
    delete table;
    string newSourceFile = "../data/" + name + ".csv";
    ofstream fout(newSourceFile, ios::out);

    result->writeRow(result->columns, fout);

    Cursor cursor(result->tableName, 0);    
    vector<int> row;
    for (int rowCounter = 0; rowCounter < result->rowCount; rowCounter++)
    {
        row = cursor.getNext();
        result->writeRow(row, fout);
    }
    fout.close();
    result->unload();
    delete result;

    Table *final_table = new Table(name);
    if(final_table->load()){
        tableCatalogue.insertTable(final_table);
    }
    return;
}