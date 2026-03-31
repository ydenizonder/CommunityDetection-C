# Community Detection Algorithms Project

This project implements and compares two different algorithms for detecting community structure in graphs:
- Louvain Algorithm
- Fast Greedy Algorithm

The goal of the assignment is to understand modularity-based community detection, implement algorithms from scratch, and evaluate their behavior on Zachary’s Karate Club.

In this project there are 2 C files one for each algorthim.

## Project Overview

Community detection aims to divide a graph into groups (communities) such that nodes inside the same group are more densely connected than nodes in different groups.
This project explores three approaches:

1. Louvain Algorithm

A multi-phase hierarchical method that:
Starts with each node in its own community
Moves nodes to neighboring communities if it increases modularity (Phase 1)
Collapses the graph based on communities (Phase 2)
Repeats until no improvement is possible
The final output is a hierarchical community structure.

2. Fast Greedy Algorithm

A bottom-up agglomerative algorithm:
Each node starts as its own community
Communities are merged iteratively
At each step, the merge that gives the largest modularity gain is chosen
Stops when no merge improves modularity
This algorithm is simpler but slower than Louvain for large graphs.

## How to run and compile: 

- To compile and run Louvain algorithm:

Compile: 
gcc -o project ./Louvain.c

Run: 
Windows: ./project.exe
Mac/Linux: ./project

- To compile and run FastGreedy algorithm:

Compile: 
gcc -o project ./FastGreedy.c

Run:
Windows: ./project.exe
Mac/Linux: ./project

## Example Output

Louvain Algorithm: 
Community count: 4
Community 1: 1, 2, 3, 4, 8, 10, 12, 13, 14, 18, 20, 22
Community 2: 5, 6, 7, 11, 17
Community 3: 9, 15, 16, 19, 21, 23, 27, 30, 31, 33, 34
Community 4: 24, 25, 26, 28, 29, 32
Modularity: 0.418803