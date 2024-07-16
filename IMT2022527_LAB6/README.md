# Persistent Data Structures (PDS) Project

This repository contains implementations and utilities for managing parent and child records using PDS principles, along with Binary Search Tree (BST) operations.

## Files Overview

- **IMT2022527_TESTCASE.in**: Test cases with specific syntax for each command.
- **IMT2022527_pds.c**: Implementation of PDS-specific functions.
- **pds.h**: Header file defining interfaces for PDS operations.
- **bst.c** and **bst.h**: Binary Search Tree implementation.
- **child.c** and **child.h**: Implementation of child records.
- **parent.c** and **parent.h**: Implementation of parent records.
- **structs.c** and **structs.h**: Functions handling operations on parent and child records, including error handling for links.
- **linked_data_demo.c**: Code for interactive testing of implemented functionalities.
- **pds_tester.c**: Automation script for testing new PDS functions against test cases.

## Running Programs

### Interactive Testing (linked_data_demo.c)

To run the interactive testing program:

   ```bash
   gcc -o interactive_testing linked_data_demo.c IMT2022527_pds.c bst.c structs.c parent.c child.c
   ./interactive_testing


### Non Interactive Testing:
   ```bash
   gcc -o pds_tester pds_tester.c IMT2022527_pds.c bst.c structs.c parent.c child.c
   ./pds_tester IMT2022527_TESTCASE.in
