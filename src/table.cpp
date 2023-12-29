#include "global.h"

/**
 * @brief Construct a new Table:: Table object
 *
 */
Table::Table()
{
    logger.log("Table::Table");
}

/**
 * @brief Construct a new Table:: Table object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tableName 
 */
Table::Table(string tableName)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/" + tableName + ".csv";
    this->tableName = tableName;
}

/**
 * @brief Construct a new Table:: Table object used when an assignment command
 * is encountered. To create the table object both the table name and the
 * columns the table holds should be specified.
 *
 * @param tableName 
 * @param columns 
 */
Table::Table(string tableName, vector<string> columns)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/temp/" + tableName + ".csv";
    this->tableName = tableName;
    this->columns = columns;
    this->columnCount = columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * columnCount));
    this->writeRow<string>(columns);
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates table
 * statistics.
 *
 * @return true if the table has been successfully loaded 
 * @return false if an error occurred 
 */
bool Table::load()
{
    logger.log("Table::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        fin.close();
        if (this->extractColumnNames(line))
            if (this->blockify())
                return true;
    }
    fin.close();
    return false;
}

/**
 * @brief Function extracts column names from the header line of the .csv data
 * file. 
 *
 * @param line 
 * @return true if column names successfully extracted (i.e. no column name
 * repeats)
 * @return false otherwise
 */
bool Table::extractColumnNames(string firstLine)
{
    logger.log("Table::extractColumnNames");
    unordered_set<string> columnNames;
    string word;
    stringstream s(firstLine);
    while (getline(s, word, ','))
    {
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        if (columnNames.count(word))
            return false;
        columnNames.insert(word);
        this->columns.emplace_back(word);
    }
    this->columnCount = this->columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * this->columnCount));
    return true;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Table::blockify()
{
    logger.log("Table::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<int> row(this->columnCount, 0);
    vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
    int pageCounter = 0;
    unordered_set<int> dummy;
    dummy.clear();
    this->distinctValuesInColumns.assign(this->columnCount, dummy);
    this->distinctValuesPerColumnCount.assign(this->columnCount, 0);
    getline(fin, line);
    while (getline(fin, line))
    {
        stringstream s(line);
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (!getline(s, word, ','))
                return false;
            row[columnCounter] = stoi(word);
            rowsInPage[pageCounter][columnCounter] = row[columnCounter];
        }
        pageCounter++;
        this->updateStatistics(row);
        if (pageCounter == this->maxRowsPerBlock)
        {
            bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
            this->blockCount++;
            this->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
    }
    if (pageCounter)
    {
        bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
        this->blockCount++;
        this->rowsPerBlockCount.emplace_back(pageCounter);
        pageCounter = 0;
    }

    if (this->rowCount == 0)
        return false;
    this->distinctValuesInColumns.clear();
    return true;
}

/**
 * @brief Given a row of values, this function will update the statistics it
 * stores i.e. it updates the number of rows that are present in the column and
 * the number of distinct values present in each column. These statistics are to
 * be used during optimisation.
 *
 * @param row 
 */
void Table::updateStatistics(vector<int> row)
{
    this->rowCount++;
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (!this->distinctValuesInColumns[columnCounter].count(row[columnCounter]))
        {
            this->distinctValuesInColumns[columnCounter].insert(row[columnCounter]);
            this->distinctValuesPerColumnCount[columnCounter]++;
        }
    }
}

/**
 * @brief Checks if the given column is present in this table.
 *
 * @param columnName 
 * @return true 
 * @return false 
 */
bool Table::isColumn(string columnName)
{
    logger.log("Table::isColumn");
    for (auto col : this->columns)
    {
        if (col == columnName)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Renames the column indicated by fromColumnName to toColumnName. It is
 * assumed that checks such as the existence of fromColumnName and the non prior
 * existence of toColumnName are done.
 *
 * @param fromColumnName 
 * @param toColumnName 
 */
void Table::renameColumn(string fromColumnName, string toColumnName)
{
    logger.log("Table::renameColumn");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (columns[columnCounter] == fromColumnName)
        {
            columns[columnCounter] = toColumnName;
            break;
        }
    }
    return;
}

/**
 * @brief Function prints the first few rows of the table. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Table::print()
{
    logger.log("Table::print");
    uint count = min((long long)PRINT_COUNT, this->rowCount);

    //print headings
    this->writeRow(this->columns, cout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < count; rowCounter++)
    {
        row = cursor.getNext();
        this->writeRow(row, cout);
    }
    printRowCount(this->rowCount);
}



/**
 * @brief This function returns one row of the table using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor 
 * @return vector<int> 
 */
void Table::getNextPage(Cursor *cursor)
{
    logger.log("Table::getNext");

        if (cursor->pageIndex < this->blockCount - 1)
        {
            cursor->nextPage(cursor->pageIndex+1);
        }
}



/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Table::makePermanent()
{
    logger.log("Table::makePermanent");
    if(!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->tableName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    //print headings
    this->writeRow(this->columns, fout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        row = cursor.getNext();
        this->writeRow(row, fout);
    }
    fout.close();
}

/**
 * @brief Function to check if table is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Table::isPermanent()
{
    logger.log("Table::isPermanent");
    if (this->sourceFileName == "../data/" + this->tableName + ".csv")
    return true;
    return false;
}

/**
 * @brief The unload function removes the table from the database by deleting
 * all temporary files created as part of this table
 *
 */
void Table::unload(){
    logger.log("Table::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->tableName, pageCounter);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}

/**
 * @brief Function that returns a cursor that reads rows from this table
 * 
 * @return Cursor 
 */
Cursor Table::getCursor()
{
    logger.log("Table::getCursor");
    Cursor cursor(this->tableName, 0);
    return cursor;
}
/**
 * @brief Function that returns the index of column indicated by columnName
 * 
 * @param columnName 
 * @return int 
 */
int Table::getColumnIndex(string columnName)
{
    logger.log("Table::getColumnIndex");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (this->columns[columnCounter] == columnName)
            return columnCounter;
    }
}

/**
 * @brief Sorts the table
 * 
 */
Table* Table::sort()
{
    logger.log("Table::sort");
    // sorting phase
    float j = this->blockCount;
    float k = BUFFER_SIZE;
    int m = ceil(j/k), d = 0, buffer_start = 0;
    Cursor tableCursor(this->tableName, 0);
    vector<int> cols;
    for(int i=0; i<parsedQuery.sortColumnList.size(); i++){
        cols.push_back(getColumnIndex(parsedQuery.sortColumnList[i]));
        // cout << parsedQuery.sortColumnList[i] << " " << (parsedQuery.sortStartegyList[i] == ASC) << endl;
    }
    vector<Table*> tables;
    while(d < m){
        Table *table = new Table("Sorted_Subpage" + to_string(d) + "_" + this->tableName, this->columns);
        vector<vector<int>> row_buffer;  
        for(int page = buffer_start; (page - buffer_start < k) && (page < j); page++){
            for(int row = 0; row < this->rowsPerBlockCount[page]; row++){                
                row_buffer.push_back(tableCursor.getNext());
            }
        }
        std::sort(row_buffer.begin(), row_buffer.end(), [&cols](const vector<int> &a, const vector<int> &b) {
            for(int i=0; i<cols.size(); i++){
                if(parsedQuery.sortStartegyList[i] == "ASC"){
                    if(a[cols[i]] < b[cols[i]]) return true;
                    else if(a[cols[i]] > b[cols[i]]) return false;
                    else continue;
                }
                else if(parsedQuery.sortStartegyList[i] == "DESC"){
                    if(a[cols[i]] > b[cols[i]]) return true;
                    else if(a[cols[i]] < b[cols[i]]) return false;
                    else continue;
                }
            }
            return false;
        });
        for (auto row : row_buffer){
            table->writeRow<int>(row);
        }
        if(table->blockify()){
            tableCatalogue.insertTable(table);
            tables.push_back(table);
        }
        else{
            cout<<"Empty Table"<<endl;
            table->unload();
            delete table;
        }
        buffer_start += k;
        d++;
    }
    
    // merging phase
    int count = d;
    d = 1;
    int p = ceil(log(m)/log(k-1));
    j = m;
    while(d <= p){
        int n = 0, q = ceil(j/(k-1));
        int buffer_start = 0;
        while(n < q){
            Table *table = new Table("Sorted_Subpage" + to_string(count) + "_" + this->tableName, this->columns);
            vector<vector<int>> row_buffer;  
            for(int subpage = buffer_start; (subpage - buffer_start < k - 1) && (subpage < tables.size()); subpage++){                
                Cursor tableCursor(tables[subpage]->tableName, 0);
                for(int row = 0; row < tables[subpage]->rowCount; row++){                
                    row_buffer.push_back(tableCursor.getNext());
                }
            }
            std::sort(row_buffer.begin(), row_buffer.end(), [cols](const vector<int> &a, const vector<int> &b) {
                for(int i=0; i<cols.size(); i++){
                    if(parsedQuery.sortStartegyList[i] == "ASC"){
                        if(a[cols[i]] < b[cols[i]]) return true;
                        else if(a[cols[i]] > b[cols[i]]) return false;
                        else continue;
                    }
                    else if(parsedQuery.sortStartegyList[i] == "DESC"){
                        if(a[cols[i]] > b[cols[i]]) return true;
                        else if(a[cols[i]] < b[cols[i]]) return false;
                        else continue;
                    }
                }
                return false;
            });
            for (auto row : row_buffer){
                table->writeRow<int>(row);
            }
            if(table->blockify()){
                tableCatalogue.insertTable(table);
                for(int i = 0; i < k - 1; i++){
                    tables[buffer_start + i]->unload();
                    delete tables[buffer_start + i];
                }
                tables.assign(tables.begin() + k - 1, tables.end());
                tables.push_back(table);
            }
            else{
                table->unload();
                delete table;
                break;
            }
            buffer_start += k-1;
            count++;
        }
        j = q;
        d++;
    }
    return tables[0];
}

bool comp(int x,int y,BinaryOperator comp){
    if(comp==LESS_THAN){
        return x<y ;
    }
    else if(comp==LEQ){
        return x<=y ;
    }
    else if(comp==GREATER_THAN){
        return x>y ;
    }
    else if(comp==GEQ){
        return x>=y ;
    }
    else if(comp==NOT_EQUAL){
        return x!=y ;
    }
}

Table* Table::join(Table* b){
    logger.log("Table::join") ;
    int p = this->getColumnIndex(parsedQuery.joinFirstColumnName) ;
    int q = b->getColumnIndex(parsedQuery.joinSecondColumnName) ;
    vector<string> all_columns(this->columns.begin(), this->columns.end());
    all_columns.insert(all_columns.end(), b->columns.begin(), b->columns.end());
    Table *res = new Table(parsedQuery.joinResultRelationName, all_columns);

    Cursor Cursor1 = Cursor(this->tableName,0) ;

    int n = this->rowCount ;
    int m = b->rowCount ;

    BinaryOperator op = parsedQuery.joinBinaryOperator;

    if(op==EQUAL){
        int i=0 ;
        int j=0 ;
        vector<int> row1 = Cursor1.getNext() ;
        Cursor Cursor2 = Cursor(b->tableName,0) ;
        vector<int> row2 = Cursor2.getNext() ;
        while(i<n and j<m){
            if(row1[p] < row2[q]){
                row1=Cursor1.getNext() ;
                i++ ;
            }
            else if(row1[p] > row2[q]){
                row2=Cursor2.getNext() ;
                j++ ;
            }
            else{
                vector<vector<int>> x ;
                vector<vector<int>> y ;
                vector<int> temp_row1 = row1 ;
                vector<int> temp_row2 = row2 ;
                int temp_i=i ;
                int temp_j=j ;
                while(temp_i < n and row1[p]==temp_row1[p]){
                    x.push_back(row1) ;
                    row1=Cursor1.getNext() ;
                    temp_i++ ;
                }

                while(temp_j < m and row2[q]==temp_row2[q]){
                    y.push_back(row2) ;
                    row2=Cursor2.getNext() ;
                    temp_j++ ;
                }

                for(int itr1=0;itr1<x.size();itr1++){
                    for(int itr2=0;itr2<y.size();itr2++){
                        vector<int> res_row ;
                        for(int pp=0;pp<x[itr1].size();pp++){
                            res_row.push_back(x[itr1][pp]) ;
                        }
                        for(int pp=0;pp<y[itr2].size();pp++){
                            res_row.push_back(y[itr2][pp]) ;
                        }
                        res->writeRow<int>(res_row) ;
                    }
                }
                i=temp_i ;
                j=temp_j ;
            }
        }
        return res ;
    }
    else{
        for(int i=0;i<n;i++){
            vector<int> row1 = Cursor1.getNext() ;
            Cursor Cursor2 = Cursor(b->tableName,0) ;
            vector<vector<int>> x ;
            for(int j=0;j<m;j++){
                vector<int> row2 = Cursor2.getNext() ;
                if(comp(row1[p],row2[q],op)){
                    x.push_back(row2) ;
                }
            }
            for(int j=0;j<x.size();j++){
                vector<int> res_row ;
                for(int k=0;k<row1.size();k++){
                    res_row.push_back(row1[k]) ;
                }
                for(int k =0;k<x[j].size();k++){
                    res_row.push_back(x[j][k]) ;
                }
                res->writeRow<int>(res_row) ;
            }
        }
        return res ;
    }

}

