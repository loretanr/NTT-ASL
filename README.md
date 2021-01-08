# NTT-ASL
This project was part of the course *Advanced Systems Lab (Spring 2020)* in the computer science master at ETH Zurich.
### Task
Select a numerical problem and create a correct (tested) implementation in C/C++. Determine the arithmetic cost, measure the runtime and performance. Profile the implementation to find the parts in which most the runtime spent. Focusing on these, apply various optimization techniques from this class. Write a paper about the work and give a presentation.
### NTT
The Number Theoretic Transform (NTT) is a specialization of the DFT. But it's not as well known and optimized, therefore we basically started from scratch.
### This repo includes
* various NTT implementations (naive, iterative and recursive radix-2, recursive radix-4) 
* optimized versions that include various scalar tricks and use of Intel vector intrinsics
* validation infrastructure and script to generate large random test in/output vectors
* benchmark infrastructure to measure performance
* stuff for performance plotting and roofline analysis
* final paper and presentation
