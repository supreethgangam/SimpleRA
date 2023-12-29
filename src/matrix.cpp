    #include "global.h"

    /**
     * @brief Construct a new Matrix:: Matrix object used in the case where the data
     * file is available and LOAD command has been called. This command should be
     * followed by calling the load function;
     *
     * @param matrixName
     */
    Matrix::Matrix(string matrixName)
    {
        logger.log("Matrix::Matrix");
        this->sourceFileName = "../data/" + matrixName + ".csv";
        this->matrixName = matrixName;
    }

    /**
     * @brief The load function is used when the LOAD command is encountered. It
     * reads data from the source file, splits it into blocks and updates matrix
     * statistics.
     *
     * @return true if the matrx has been successfully loaded
     * @return false if an error occurred
     */
    bool Matrix::load()
    {
        logger.log("Matrix::load");
        fstream fin(this->sourceFileName, ios::in);
        string line;
        if (getline(fin, line))
        {
            fin.close();
            if(this->blockify())
            {
                cout << "Number of blocks read: " << 0 << endl;
                cout << "Number of blocks written: " << this->blockCount << endl;
                cout << "Number of blocks accessed: " << this->blockCount << endl;
                return true;
            }
        }
        fin.close();
        cout << "Number of blocks read: " << 0 << endl;
        cout << "Number of blocks written: " << this->blockCount << endl;
        cout << "Number of blocks accessed: " << this->blockCount << endl;
        return false;
    }


    /**
     * @brief This function splits all the rows and stores them in multiple files of
     * one block size.
     *
     * @return true if successfully blockified
     * @return false otherwise
     */
    bool Matrix::blockify()
    {
        logger.log("Matrix::blockify");
        if (!this->setStatistics())
            return false;

        ifstream fin(this->sourceFileName, ios::in);

        for (int row = 0; row < this->rowCount; row++)
        {
            for (int block = 0; block < this->blocksPerRow; block++)
            {
                int numOfWords = this->maxRowsPerBlock;

                if (block == this->blocksPerRow - 1)
                {
                    numOfWords = this->columnCount - this->maxRowsPerBlock * (this->blocksPerRow - 1);
                }

                vector<int> rowSegment = this->readRowSegment(numOfWords, fin, block == (this->blocksPerRow - 1));
                int pageIndex = (row / this->maxRowsPerBlock) * this->blocksPerRow + block;
                this->writeRowSegment(rowSegment, pageIndex);

                this->dimPerBlockCount[pageIndex].first++;
                if (this->dimPerBlockCount[pageIndex].second == 0)
                {
                    this->dimPerBlockCount[pageIndex].second = numOfWords;
                }
            }
        }

        fin.close();


        if (this->rowCount == 0)
            return false;

        return true;
    }


    /**
     * @brief This function reads the segment of the row pointed by fin.
     *
     * @return vector<int>
     */

    vector<int> Matrix::readRowSegment(int numOfWords, ifstream &fin, bool isLastBlock)
    {
        logger.log("Matrix::readRowSegment");
        vector<int> row;
        string word;

        for (int wordCounter = 0; wordCounter < numOfWords - 1; wordCounter++)
        {
            if (getline(fin, word, ','))
            {
                word.erase(remove_if(word.begin(), word.end(), ::isspace), word.end());
                row.push_back(stoi(word));
            }
        }

        if (!isLastBlock)
        {
            if (getline(fin, word, ','))
            {
                word.erase(remove_if(word.begin(), word.end(), ::isspace), word.end());
                row.push_back(stoi(word));
            }
        }

        else
        {
            if (getline(fin, word, '\n'))
            {
                word.erase(remove_if(word.begin(), word.end(), ::isspace), word.end());
                row.push_back(stoi(word));
            }
        }

        return row;
    }

    /**
     * @brief This function appends a row segment to the page with given index.
     */

    void Matrix::writeRowSegment(vector<int> &rowSegment, int pageIndex)
    {
        logger.log("Matrix::writeRowSegment");

        string pageName = "../data/temp/" + this->matrixName + "_Page" + to_string(pageIndex);

        ofstream fout;
        fout.open(pageName, ios::out | ios::app);

        for (int ind = 0; ind < rowSegment.size(); ind++)
        {
            if (ind != 0)
                fout << " ";
            fout << rowSegment[ind];
        }
        fout << endl;
        fout.close();
    }

    /**
     * @brief Function finds total no. of columns (N), max rows per block
     * (M) and no. of blocks required per row (ceil(N / M)). NxN matrix is
     * split into smaller MxM matrix which can fit in a block.
     *
     */
    bool Matrix::setStatistics()
    {
        logger.log("Matrix::setStatistics");
        ifstream fin(this->sourceFileName, ios::in);
        string line, word;
        if (!getline(fin, line))
        {
            fin.close();
            return false;
        }
        fin.close();
        stringstream s(line);
        while (getline(s, word, ','))
            this->columnCount++;

        this->maxRowsPerBlock = (uint)sqrt((BLOCK_SIZE * 1024) / sizeof(int));
        this->blocksPerRow = this->columnCount / this->maxRowsPerBlock + (this->columnCount % this->maxRowsPerBlock != 0);
        this->blockCount = this->blocksPerRow * this->blocksPerRow;
        this->rowCount = this->columnCount;
        this->dimPerBlockCount.assign(this->blockCount, {0, 0});
        
        return true;
    }


    /**
     * @brief Transposes matrix by swaping block_ij and block_ji elements for each i, j.
     *
     */

    void Matrix::transpose()
    {
        logger.log("Matrix::transpose");
        
        int readCount = 0, writeCount = 0;
        for (int block_i = 0; block_i < this->blocksPerRow; block_i++)
        {
            for (int block_j = block_i; block_j < this->blocksPerRow; block_j++)
            {
                if (block_i != block_j)
                {
                    int block_ij = block_i * this->blocksPerRow + block_j, block_ji = block_j * this->blocksPerRow + block_i;
                    MatrixPage *page_ij = bufferManager.getMatrixPage(this->matrixName, block_ij);
                    MatrixPage *page_ji = bufferManager.getMatrixPage(this->matrixName, block_ji);
                    page_ij = bufferManager.getMatrixPage(this->matrixName, block_ij);

                    page_ij->transpose(page_ji);
                    page_ij->writePage();
                    page_ji->writePage();

                    readCount += 2;
                    writeCount += 2;
                }
                else
                {
                    int block_ij = block_i * this->blocksPerRow + block_j;
                    MatrixPage *page_ij = bufferManager.getMatrixPage(this->matrixName, block_ij);
                    page_ij->transpose();
                    page_ij->writePage();

                    readCount++;
                    writeCount++;
                }
            }
        }
        cout << "Number of blocks read: " << readCount << endl;
        cout << "Number of blocks written: " << writeCount << endl;
        cout << "Number of blocks accessed: " << readCount + writeCount << endl;
    }

    /**
     * @brief Function that checks if the matrix is symmetric or not
     *
     */
    void Matrix::checkSymmetry()
    {
        logger.log("Matrix::checkSymmetry");
        bool isSymmetric = true;

        int readCount = 0, writeCount = 0;

        for (int block_i = 0; block_i < this->blocksPerRow; block_i++)
        {
            for (int block_j = block_i; block_j < this->blocksPerRow; block_j++)
            {
                if (block_i != block_j)
                {
                    int block_ij = block_i * this->blocksPerRow + block_j, block_ji = block_j * this->blocksPerRow + block_i;
                    MatrixPage *page_ij = bufferManager.getMatrixPage(this->matrixName, block_ij);
                    MatrixPage *page_ji = bufferManager.getMatrixPage(this->matrixName, block_ji);
                    page_ij = bufferManager.getMatrixPage(this->matrixName, block_ij);

                    if(!page_ij->checksymmetry(page_ji))
                    {
                        isSymmetric = false;
                        break;
                    }

                    readCount += 2;
                }
                else
                {
                    int block_ij = block_i * this->blocksPerRow + block_j;
                    MatrixPage *page_ij = bufferManager.getMatrixPage(this->matrixName, block_ij);
                    if(!page_ij->checksymmetry())
                    {
                        isSymmetric = false;
                        break;
                    }

                    readCount++;
                }
            }
        }
        

        if (isSymmetric)
            cout << "TRUE" << endl;
        else
            cout << "FALSE" << endl;

        cout << "Number of blocks read: " << readCount << endl;
        cout << "Number of blocks written: " << writeCount << endl;
        cout << "Number of blocks accessed: " << readCount + writeCount << endl;

    }

    void Matrix::computeMatrix()
    {
        logger.log("Matrix::computeMatrix");

        int readCount = 0, writeCount = 0;

        for (int block_i = 0; block_i < this->blocksPerRow; block_i++)
        {
            for (int block_j = block_i; block_j < this->blocksPerRow; block_j++)
            {
                if (block_i != block_j)
                {
                    int block_ij = block_i * this->blocksPerRow + block_j, block_ji = block_j * this->blocksPerRow + block_i;
                    MatrixPage *page_ij = bufferManager.getMatrixPage(this->matrixName, block_ij);
                    MatrixPage *page_ji = bufferManager.getMatrixPage(this->matrixName, block_ji);
                    page_ij = bufferManager.getMatrixPage(this->matrixName, block_ij);
                    page_ij->compute(page_ji);
                    page_ij->writePage();
                    page_ji->writePage();

                    readCount += 2;
                    writeCount += 2;
                }
                else
                {
                    int block_ij = block_i * this->blocksPerRow + block_j;
                    MatrixPage *page_ij = bufferManager.getMatrixPage(this->matrixName, block_ij);
                    page_ij->compute();
                    page_ij->writePage();

                    readCount++;
                    writeCount++;
                }
            }
        }

        cout << "Number of blocks read: " << readCount << endl;
        cout << "Number of blocks written: " << writeCount << endl;
        cout << "Number of blocks accessed: " << readCount + writeCount << endl;
    }

    void Matrix::compute()
    {
        logger.log("Matrix::compute");

        string newMatrixName = this->matrixName+"_RESULT";  

        Matrix *temp = new Matrix(newMatrixName);
        temp->matrixName = newMatrixName;
        temp->blockCount = this->blockCount;
        temp->blocksPerRow = this->blocksPerRow;
        temp->columnCount = this->columnCount;
        temp->maxRowsPerBlock = this->maxRowsPerBlock;
        temp->rowCount = this->rowCount;
        temp->numOfZeros = this->numOfZeros;
        temp->dimPerBlockCount = this->dimPerBlockCount;
        

        for(int i = 0; i < this->blockCount; i++)
        {
            string oldName = "../data/temp/" + this->matrixName + "_Page" + to_string(i);
            string newName = "../data/temp/" + newMatrixName + "_Page" + to_string(i);
            std::filesystem::copy(oldName.c_str(),newName.c_str());
        }

        
        matrixCatalogue.insertMatrix(temp);

        temp->computeMatrix();

    }


    /**
     * @brief Function prints the first few rows of the matrix. If the matrix contains
     * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
     * the rows are printed.
     *
     */

    void Matrix::print()
    {
        logger.log("Matrix::print");

        uint count = min((long long)PRINT_COUNT, this->rowCount);

        CursorMatrix cursor = this->getCursor();
        for (int rowCounter = 0; rowCounter < count; rowCounter++)
        {
            int remaining = count;
            bool last = false;

            for (int seg = 0; remaining != 0 && seg < this->blocksPerRow; seg++)
            {
                vector<int> nextSegment = cursor.getNext();

                if (nextSegment.size() >= remaining)
                {
                    nextSegment.resize(remaining);
                    last = true;
                }

                remaining -= nextSegment.size();
                
                this->writeRow(nextSegment, cout, seg == 0, last);

                if (!remaining && seg != this->blocksPerRow - 1 && rowCounter != count - 1)
                {
                    int lastSegBlockNum = cursor.pageIndex - cursor.pageIndex % this->blocksPerRow + this->blocksPerRow - 1;
                    cursor.nextPage(lastSegBlockNum, cursor.pagePointer);
                    cursor.getNext();
                }
            }
        }
        cout << endl;
        cout << "Number of blocks read: " << this->blockCount << endl;
        cout << "Number of blocks written: " << 0 << endl;
        cout << "Number of blocks accessed: " << this->blockCount << endl;
    }

    /**
     * @brief This function moves cursor to next page if next page exists.
     *
     * @param cursor
     */
    void Matrix::getNextPage(CursorMatrix *cursor)
    {
        logger.log("Matrix::getNextPage");

        if (cursor->pageIndex < this->blockCount - 1)
        {
            cursor->nextPage(cursor->pageIndex + 1);
        }
    }

    /**
     * @brief This function moves cursor to point to the next segment
     * of the matrix's row or to the starting of the next row if exists.
     *
     * @param cursor
     */
    void Matrix::getNextPointer(CursorMatrix *cursor)
    {
        logger.log("Matrix::getNextPointer");

        // pageIndex = block number of the page, pagePointer = row number within the block

        if ((cursor->pageIndex + 1) % this->blocksPerRow == 0)
        {
            if (cursor->pagePointer == this->dimPerBlockCount[cursor->pageIndex].first - 1)
            {
                if (cursor->pageIndex < this->blockCount - 1)
                    cursor->nextPage(cursor->pageIndex + 1);
            }

            // nextPage (dubious variable name): given page index, return the page with that page index
            else
                cursor->nextPage(cursor->pageIndex - this->blocksPerRow + 1, cursor->pagePointer + 1);
        }

        else if (cursor->pageIndex < this->blockCount - 1)
            cursor->nextPage(cursor->pageIndex + 1, cursor->pagePointer);
    }

    /**
     * @brief called when EXPORT command is invoked to move source file to "data"
     * folder.
     *
     */
    void Matrix::makePermanent()
    {
        logger.log("Matrix::makePermanent");
        if (!this->isPermanent())
            bufferManager.deleteFile(this->sourceFileName);
        
        string newSourceFile = "../data/" + this->matrixName + ".csv";
        ofstream fout(newSourceFile, ios::out);

        CursorMatrix cursor = this->getCursor();
        for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
        {
            for (int seg = 0; seg < this->blocksPerRow; seg++)
                this->writeRow(cursor.getNext(), fout, seg == 0, seg == this->blocksPerRow - 1);
        }
        fout.close();
        cout << "Number of blocks read: " << this->blockCount << endl;
        cout << "Number of blocks written: " << 0 << endl;
        cout << "Number of blocks accessed: " << this->blockCount << endl;
    }


    /**
     * @brief Function to check if matrix is already exported
     *
     * @return true if exported
     * @return false otherwise
     */
    bool Matrix::isPermanent()
    {
        logger.log("Matrix::isPermanent");
        if (this->sourceFileName == "../data/" + this->matrixName + ".csv")
            return true;
        return false;
    }

    /**
     * @brief The unload function removes the matrix from the database by deleting
     * all temporary files created as part of this table
     *
     */
    void Matrix::unload()
    {
        logger.log("Matrix::~unload");
        for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
            bufferManager.deleteFile(this->matrixName, pageCounter);
        if (!isPermanent())
            bufferManager.deleteFile(this->sourceFileName);
    }

    /**
     * @brief Function that returns a cursor that reads rows from this matrix
     *
     * @return CursorMatrix
     */
    CursorMatrix Matrix::getCursor()
    {
        logger.log("Matrix::getCursor");
        CursorMatrix cursor(this->matrixName, 0);
        return cursor;
    }

    void Matrix::Rename(string newMatrixName)
    {
        logger.log("Matrix::Rename");
        for(int i = 0; i < this->blockCount; i++)
        {
            string oldName = "../data/temp/" + this->matrixName + "_Page" + to_string(i);
            string newName = "../data/temp/" + newMatrixName + "_Page" + to_string(i);
            rename(oldName.c_str(), newName.c_str());
        }
        Matrix *temp = this;
        temp->matrixName = newMatrixName;
        temp->sourceFileName = "../data/" + newMatrixName + ".csv";
        matrixCatalogue.insertMatrix(temp);
        matrixCatalogue.matrices.erase(parsedQuery.renameFromColumnName);
    }

