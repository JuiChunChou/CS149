Option 1:

You will extend matrixmult.c (from assignment #1) to compute array multiplication in a parallel fashion using multiple processes. For example, the ith child process will compute dot products of the ith row of Ai against W to return the vector Ai * W . Thus it will return the ith row of result R. In the world of graphics or AI people use GPUs for this kind of operation, but we can't assume everyone has a GPU, so we will use your CPU.

You will develop a program matrixmult_parallel that takes the input files A, W on the command line (B is skipped since it's not essential for the assignment's learning experience). You will need to multiply the files similar to assignment #1, but do it in parallel.

As in assignment #1 you will assume the matrices have fixed sizes but this time assume they're square: 8x8 for A, and 8x8 for W. You will compute A*W. Thus you can use 8 processes. Process i will multiply the Ai row vector by all columns of W (it's called dot product).

You should use fork() to start a process for each of the rows of A, such that we can compute a vector for each row of the result individually in parallel. To communicate between processes you may use pipe(). You need to wait for the processes to finish using wait().

In order to keep track of which row of the result a child process computed, you could either return i as an int in a pipe; or have an array that maps i to pid, since pid is returned from wait; or something else...

You can assume that the files contain only valid ASCII characters separated by newlines , which can be converted to integers. 

The code/ipc directory contains examples of using fork, pipe and wait, which you can consult for help.

For example, running on these test files will output:

$ ./matrixmult_parallel A.txt W.txt
which will output to stdout:

* Result of A*W = [
* 11  7 0 0 0 11 0 0 
* 21 13 0 0 0 21 0 0 
* 16 10 0 0 0 16 0 0 
* 16 10 0 0 0 16 0 0 
* 21 13 0 0 0 21 0 0 
* 26 16 0 0 0 26 0 0 
* 26 16 0 0 0 26 0 0 
* 16 10 0 0 0 16 0 0 
* ]
Runtime 0.01 seconds
(it is fine if you output additional details, like the absolute paths to A W. Actually it is better to output the full paths to your input files)
The error codes are the same as in assignment 1, e.g., if an input file was inaccessible you should also print to stderr messages:

error: cannot open file W.txt
Terminating, exit code 1.
