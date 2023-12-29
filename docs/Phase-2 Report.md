# IMPLEMENTATION OF EXTERNAL SORTING

## Introduction

External sorting is a crucial technique used in scenarios where the data to be sorted does not fit entirely into the computer's main memory. It is commonly employed in tasks like sorting large database files or other data sets that are too large to be processed in memory.

## External Sorting Algorithm

The external sorting algorithm employs a "sort-merge" strategy, breaking the task into two main phases: the sorting phase and the merging phase.

### Sorting Phase

In the sorting phase, the algorithm works on smaller portions of the file, referred to as "runs." These runs are small enough to fit into the available buffer space in the computer's main memory. The primary steps in this phase are as follows:

1. Read runs from the original file that can fit within the available buffer space.
2. Sort the data within these runs using an internal sorting algorithm.
3. Write the sorted runs back to the disk as temporary sorted subfiles (runs).

The size of each run and the number of initial runs (nR) are determined by the number of file blocks (b) and the available buffer space (nB).

### Merging Phase

In the merging phase, the sorted runs created during the sorting phase are combined to produce larger sorted subfiles. These subfiles are merged iteratively during one or more merge passes, and each merge pass can consist of multiple merge steps. Key aspects of the merging phase include:

- The degree of merging (dM), which is the number of sorted subfiles merged in each step. It's calculated as the smaller of (nB - 1) and nR.
- The number of merge passes is determined by ⎡logdM(nR)⎤.

## Implementation Specifics

In our project, the implementation of external sorting follows the algorithm described above. We use buffer space in the computer's main memory, which is managed as part of the DBMS cache. The buffer space is divided into individual buffers, each of which is the same size in bytes as one disk block.

### Example

For a better understanding, let's consider an example:

- Available main memory buffers (nB) = 5 disk blocks.
- Size of the file (b) = 1,024 disk blocks.

In this case, the number of initial runs (nR) is calculated as ⎡(b/nB)⎤, which results in 205 initial runs. Each run consists of 5 blocks, except for the last run, which has only 4 blocks.

During the sorting phase, 205 sorted runs (or temporary subfiles) are stored on the disk.

For the merging phase, with nB = 5 and dM = 4 (four-way merging), we merge the initial 205 sorted runs four at a time in each step, resulting in 52 larger sorted subfiles at the end of the first merge pass. These 52 files are then merged, and the process continues until we finally obtain one fully sorted file, which indicates that four merge passes are needed.

This implementation ensures efficient sorting and merging of large data sets that do not fit in the computer's main memory, making it suitable for various applications, including database systems.

# JOIN OPERATION

## Overview

The JOIN operation in a database system is a fundamental operation for combining data from multiple tables based on a specified condition. An efficient JOIN operation is critical for optimizing the performance of database queries. In this document, we will explore the concept of JOIN and discuss various implementation strategies.

## Implementing JOIN

The implementation of JOIN typically involves the following steps:

1. **Sorting (if necessary)**: If the tables are not already sorted by the join attribute, they may need to be sorted using techniques such as external sorting.

2. **Looping through Tables**: The core of the JOIN operation involves looping through both tables and comparing records based on the specified condition.

3. **Match and Combine**: When records from both tables match the JOIN condition, they are combined to create a result row.

## Sort-Merge Join Algorithm

If the records of tables R and S are physically sorted by the values of their join attributes A and B, respectively, the Sort-Merge Join algorithm is a highly efficient approach. It works as follows:

1. Sort the tuples in R on attribute A.
2. Sort the tuples in S on attribute B.
3. Initialize pointers, e.g., set i ← 1, j ← 1.

4. While (i ≤ n) and (j ≤ m):
   - Compare the values of R(i)[A] and S(j)[B].
   - If R(i)[A] > S(j)[B], increment j; if R(i)[A] < S(j)[B], increment i.
   - If R(i)[A] = S(j)[B], output the combined tuple <R(i), S(j)> to the result table T.
   - Output other tuples that match R(i) or S(j) as needed.

5. Update pointers and continue the loop.

## Handling Secondary Indexes

When secondary indexes exist on both join attributes, a variation of the Sort-Merge Join algorithm is required. These indexes allow ordered access but may scatter records across disk blocks, which can impact efficiency.

## Conclusion

Efficiently implementing the JOIN operation is crucial for optimizing the performance of a database system. The Sort-Merge Join algorithm, especially when records are sorted, provides an optimal solution. However, for situations with secondary indexes, special considerations must be made to balance the advantages of index-based access with potential disk block access inefficiencies.

By understanding the types of JOIN and implementing the appropriate strategy, database systems can efficiently handle complex queries and extract meaningful insights from data.

# ORDER BY Operation

The **ORDER BY** operation in a database system is used to sort the rows of a table based on a specified column in ascending (ASC) or descending (DESC) order. This operation can be efficiently implemented using external sorting, as previously explained. Here's a description of the **ORDER BY** operation and how it is executed:

## Syntax

The syntax of the **ORDER BY** statement is as follows:

```sql
<new_table> <- ORDER BY <attribute> ASC|DESC ON <table_name>
```

- `<new_table>`: Represents the name of the new table that will store the sorted results.
- `<attribute>`: Specifies the column based on which the table should be sorted.
- `ASC|DESC`: Indicates whether the sorting should be in ascending (ASC) or descending (DESC) order.
- `<table_name>`: Refers to the name of the table on which the query is performed.

## Implementation

The **ORDER BY** operation is implemented using the external sorting method, as explained previously in the documentation. The process involves the following steps:

1. **Sorting the Table**: The table is sorted based on the specified column in the specified order (ASC or DESC). This sorting can be done using external sorting, especially when the table is too large to fit entirely in memory.

2. **External Sorting**: The external sorting algorithm is employed to efficiently sort the table. This involves dividing the table into smaller portions (runs) that can fit in available memory buffers and then merging these runs into larger sorted subfiles using a merge pass approach.

3. **Result Stored in New Table**: The sorted results are stored in a new table. This new table contains the same columns as the original table, but the rows are ordered based on the specified column in the desired order (ASC or DESC).

## Conclusion

The **ORDER BY** operation is a fundamental query operation in a database system, allowing data to be presented in a specific order. Implementing **ORDER BY** efficiently is crucial, especially for large datasets that require external sorting to manage the sorting process. The result is saved in a new table, preserving the original data while presenting it in the desired order. This approach ensures that the database system can provide sorted results in a performant and organized manner.

# GROUP BY OPERATION

The **GROUP BY** Aggregates Operation with a **HAVING** clause is a crucial operation in a database system that allows rows to be grouped by a specific attribute and apply aggregate functions, such as MAX, MIN, SUM, COUNT, and AVG, to these groups. This operation is efficiently implemented using external sorting, as previously described. Here's an explanation of how this operation is performed:

## Syntax

The syntax of the **GROUP BY** statement is as follows:

```sql
<new_table> <- GROUP BY <grouping_attribute> FROM <table_name> HAVING <aggregate(attribute)> <bin_op> <attribute_value> RETURN <aggregate_func(attribute)>
```

- `<new_table>`: Represents the final table that will store the result of the **GROUP BY** operation.
- `<grouping_attribute>`: Specifies the column in the table based on which the table is grouped.
- `<table_name>`: Refers to the name of the table on which the operation is performed.
- `<aggregate(attribute)>`: Denotes the column used for comparison.
- `<bin_op>`: Represents the comparison operator (e.g., greater than, less than) used for filtering groups.
- `<attribute_value>`: Refers to the value used in the comparison.
- `<aggregate_func(attribute)>`: Indicates the aggregate function to be applied to the grouped data.

## Implementation

The **GROUP BY** Aggregates Operation with a **HAVING** clause is implemented as follows:

1. **Sorting Based on Grouping Attribute**: The table specified in `<table_name>` is sorted based on the `<grouping_attribute>`. This sorting is done using external sorting, especially when the table is too large to fit entirely in memory.

2. **External Sorting**: The external sorting algorithm is employed to efficiently sort the table. This involves dividing the table into smaller portions (runs) that can fit in available memory buffers and then merging these runs into larger sorted subfiles using a merge pass approach.

3. **Grouping and Aggregation**: After sorting, the rows with the same values in the `<grouping_attribute>` are grouped together. Then, the specified `<aggregate_func(attribute)>` is applied to each group to compute the aggregate values.

4. **Filtering with HAVING Clause**: The **HAVING** clause is used to filter groups based on the specified `<aggregate(attribute)>`, `<bin_op>`, and `<attribute_value>`. Groups that meet the criteria are retained in the result, while others are filtered out.

5. **Creating the New Table**: The result of the **GROUP BY** operation is saved in a new table, denoted by `<new_table>`. This new table contains the aggregated values and the `<grouping_attribute>`.

## Conclusion

The **GROUP BY** Aggregates Operation with a **HAVING** clause is a fundamental query operation in a database system, allowing data to be grouped, aggregated, and filtered based on specific criteria. Implementing this operation efficiently is crucial, especially for large datasets that require external sorting to manage the grouping and aggregation process. The result is saved in a new table, preserving the aggregated data while adhering to the specified criteria. This approach ensures that the database system can provide meaningful insights and organized results to users.

## Contribution
Pothuri Praneeth Varma, Pothugunta Venkat -> Sort
Pothuri Praneeth Varma, Gangam Supreeth -> Join
Gangam Supreeth , Pothugunta Venkat -> Order BY
Pothuri Praneeth, Gangam Supreeth -> Group BY
