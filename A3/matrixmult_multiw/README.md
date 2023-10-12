For simplicity, assume the same fixed array sizes of 8x8 to keep things simple; so you do not need to check the A matrix dimensions match the sizes of W.  Same as Assgt2, assume your input A and Wi matrices all have the same size 8x8. Assume you are dealing only with non-negative integers in the files. In case your input file has less lines or numbers per line you pad the array rows or columns with zeros.

 

This kind of parallel execution of many weight matrix multiplications resembles something called transformers and attention in machine learning.

 

The ith child should then print the entire input matrices A and Wi and result Ri, showing their filenames, in an output file named PID.out (you will replace PID with the child's getpid()). It is OK if you show the full absolute path to each file, e.g. /home/.../A3/test/A1.txt (it is actually preferred this way). Each child process i does the same thing for a different Wi matrix. Remember to flush standard I/O streams (fflush(stdout)).

 

matrixmult_multiw runs until there are no more processes running. If a parent process has no child process then wait(..) returns immediately "-1", thus you can continue until wait(..) returns -1.
