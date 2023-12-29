# Project Phase-1 Report

## Assumptions

**Data Format Assumptions**:
- The input matrices are assumed to be in CSV format.
- Matrices are assumed to contain only integer values.

**Matrix Size Assumptions**:
- The project assumes that matrices provided as input will be square matrices (n x n), where n <= 10^4.

**Memory Assumptions**:
- The project assumes that the system has a limited amount of main memory available for in-place operations (e.g., 2 blocks).
- It assumes that the available memory is sufficient to perform in-place transpositions and other operations.

**File Assumptions**:
- The project assumes that the data files (CSV files) for matrices to be loaded or exported will exist in a specific data folder.
- It assumes that the data folder structure adheres to a certain format.

**Naming Assumptions**:
- The project assumes that matrix and table names do not contain special characters or spaces.
- Renaming operations assume that the new name does not conflict with existing matrices or tables.

**Question Assumptions**:
- We assumed that the compute operation is done and placed in the same matrix. We did not create a new matrix for the result.

## Learnings

- **Database Management**: We have learned about the fundamentals of database management systems, including data storage, retrieval, and manipulation.
- **File Handling**: Understanding how to read and write files is a fundamental skill, and this project likely enhanced your proficiency in file handling.
- **Memory Management**: Dealing with limited memory resources and optimizing memory usage for in-place operations, such as transposing matrices, has likely taught us valuable memory management techniques.
- **Block Access Optimization**: We learned how to optimize block access to minimize disk I/O and improve the efficiency of your database system.
- **Algorithmic Thinking**: Implementing matrix operations and commands often requires algorithmic thinking to design efficient and correct algorithms for tasks like transposition and symmetry checking.
- **Team Coordination**: Working as a team on a complex project teaches collaboration, communication, and effective coordination among team members.

## Contribution

- **G. Supreeth Reddy**: Completed `TRANSPOSE`, `CHECKSYMMETRY` and `COMPUTE`
- **Praneeth Varma**: Completed `LOAD`, `PRINT` and `DOCUMENTATION`
- **P. Venkat**: Completed `EXPORT`, `RENAME` and `DOCUMENTATION`

---

### Command: LOAD MATRIX

**Logic**: The LOAD MATRIX command reads the contents of a CSV file from the data folder and loads it into memory as a matrix. The logic for loading a matrix involves the following steps:

1. Read the CSV file: The command reads the CSV file named `<matrix_name>.csv` from the data folder, where `<matrix_name>` is the specified matrix name.

2. Matrix Initialization: Initialize an empty matrix structure in memory to store the loaded data. The matrix structure may include information such as dimensions (n x n) and memory allocation.

3. Read Values Row-wise: We are reading the csv file only once but writing in each page multiple times. Read the first `M` elements of the file and write it to the first page.Read the next `M` elements of the file and write it to the next page. Continue this until we reach the end of the first row of the file. Then we move to the next row of the file and we start **appending** back to the first page. After we write `M` rows in a page, then we skip approximately the next `N/M` pages and start writing to this new page. We do this until we reach the end of the file. This will write the whole matrix in the form of smaller submatrices.

4. **Page Design**: We split the `N` x `N` matrix into smaller `M` x `M` submatrices and each submatrix is written to a disk block. `M = sqrt((BLOCK\_SIZE * 1024) / sizeof(int))` which comes out to be `45`.  We use this technique because this makes the transpose much more efficient. Storing in the row major order is very easy but transpose will take a lot of time, which is undesirable. We also take care of the fact that `N` may not be divisible by `M`.

5. **Block Access**: Keep track of the block access to monitor the number of blocks used during the loading process.

6. **Error Handling**: Check whether the given command is right or not i.e. `LOAD MATRIX < matrix_name >`.

---

### Command: PRINT MATRIX

**Logic**: The PRINT MATRIX command is used to print the contents of a matrix onto the terminal. The logic for printing a matrix involves the following steps:

1. Matrix Identification: Identify the matrix to be printed based on the specified `<matrix_name>`.

2. Matrix Dimensions: Determine the dimensions of the matrix, specifically its size 'n' (n x n).

3. Print Matrix Values: Depending on the size of the matrix 'n':
   - If 'n' is less than or equal to 20, print the entire matrix.
   - If 'n' is greater than 20, print only the first 20 rows and columns of the matrix. This limitation is applied to avoid overwhelming the terminal with large matrices.

4. Implementation: Write the first row of the first page. Get to the next page using the cursor. Write the first row of the next page and move the cursor to the next page. After we write the entire first row of the matrix, we move the cursor back to the first page, but instead to the second row instead of the fist row.After `M` row writes, the cursor reaches to the end of a page, we move the cursor to the block which is immediate to its down i.e. skipping approximately `N / M` pages. We do this until we reach the end of the last page. This will write the complete matrix.

5. Formatting: Format the matrix values for display, ensuring proper alignment and spacing.

6. Display Matrix: Display the matrix on the terminal, row by row.

7. **Error Handling**: Check whether the given command is right or not i.e. `PRINT MATRIX < matrix_name >`.

---

### Command: TRANSPOSE MATRIX

**Logic**: The TRANSPOSE MATRIX command is used to compute the transpose of a matrix in-place. The logic for in-place transposition involves the following steps:

1. Matrix Identification: Identify the matrix to be transposed based on the specified `<matrix_name>`.

2. Matrix Dimensions: Determine the dimensions of the matrix, specifically its size 'n' (n x n).

3. In-Place Transposition:
   - For every disk block `(i, j)`, `i < j`:
      - Get it in memory
      - Load the disk block `(j, i)` in the memory.
      - Transpose both the matrices in `(i, j)` and `(j, i)` internally (inplace).
      - Write the page `(j, i)` where the block `(i, j)` was stored and write the page `(i, j)` where the block `(j, i)` was stored.

   - For every disk block `(i, i)`:
      - Get it in memory
      - Transpose it internally by swapping each element `(m, n)` with `(n, m)`
      - Write the page back to it's original location

   This is an inplace operation because we are not using any additional disk blocks nor are we using any additional main memory.

4. **Page Design**: Ensure that the transposition is performed in-place without using additional disk blocks. You are allowed to use a limited amount of main memory (e.g., 2 blocks) for this operation.

5. **Block Access**: Keep track of the block access to monitor the number of blocks used during the in-place transposition process.

6. **Error Handling**: Check whether the given command is right or not i.e. `TRANSPOSE MATRIX < matrix_name >`

---

### Command: EXPORT MATRIX

**Logic**: The EXPORT MATRIX command is used to export the contents of a matrix to a CSV file in the data folder. The logic for exporting a matrix is similar to the process of displaying it using cout and involves the following steps:

1. Export Matrix Values: Iterate through the matrix elements, row by row, and prepare the matrix data for export in CSV format. This step is similar to the process of displaying the matrix using cout.

2. File Creation: Create a new CSV file named `<matrix_name>.csv` in the data folder for exporting the matrix data. Ensure that the file is created or overwritten as needed.

3. Write Matrix Data: Write the prepared matrix data to the CSV file, adhering to CSV formatting standards. This step involves writing rows of values to the file.

4. **Error Handling**: Check whether the given command is right or not i.e. `EXPORT MATRIX < matrix_name >`

---

### Command: RENAME MATRIX

**Logic**: The RENAME MATRIX command is used to rename an existing matrix in memory. The logic for renaming a matrix involves the following steps:

1. Update in Memory: Rename the matrix in memory by updating its name from `<matrix_currentname>` to `<matrix_newname>`.

2. Update in Temp Files: If the matrix was originally loaded from a CSV file in the data folder, delete the original CSV file named `<matrix_currentname>.csv`.

3. Create New CSV File: Create a new CSV file named `<matrix_newname>.csv` in the data folder if the matrix is to be exported in the future. This step is necessary to ensure that the matrix data remains accessible under the new name.

4. **Error Handling**: Check whether the given command is right or not i.e. `RENAME MATRIX < matrix_currentname > < matrix_newname >`

---

### Command: CHECKSYMMETRY

**Logic**: 

1. Symmetry Check:
   - For every disk block `(i, j)`, `i < j`:
      - Get it in memory
      - Load the disk block `(j, i)` in the memory.
      - Check if matrix in `(i, j)` and transpose of matrix in `(j, i)` are equal. If not equal, the matrix cannot be symmetric, and the process is halted.
      - If both the matrices are equal, continue checking the remaining elements.

   - For every disk block `(i, i)`:
      - Get it in memory
      - Check if the matrix is symmetric internally by comparing each element `(m, n)` with `(n, m)`. If not equal, the matrix cannot be symmetric, and the process is halted.

   This is an inplace operation because we are not using any additional disk blocks nor are we using any additional main memory.

2. **Error Handling**: Check whether the given command is right or not i.e. `CHECKSYMMETRY < matrix_name >`

---

### Command: COMPUTE

**Logic**: Explain how the COMPUTE command computes A - A' and stores the result in A itself.

1. Transpose Calculation: Calculate the transpose of matrix 'A' by following the logic similar to the TRANSPOSE MATRIX command. However, in this case, there is no need to swap elements.

2. Element-wise Subtraction:
   - For every disk block `(i, j)`, `i < j`:
      - Get it in memory
      - Load the disk block `(j, i)` in the memory.
      - For matrix in `(i, j)` and matrix in `(j, i)`, subtract each element `(m, n)` with `(n, m)` and store the result in `(m, n)`, and vice versa.
      - Write the page `(j, i)` where the block `(i, j)` was stored and write the page `(i, j)` where the block `(j, i)` was stored.

   - For every disk block `(i, i)`:
      - Get it in memory
      - Subtract each element `(m, n)` with `(n, m)` and store the result in `(m, n)`, and vice versa.
      - Write the page back to it's original location
   

3. **Error Handling**: Check whether the given command is right or not i.e. `COMPUTE < matrix_name >`

---
