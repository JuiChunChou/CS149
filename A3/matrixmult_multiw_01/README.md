use the exec system call (or one of its variants). You can use execvp, which takes a fixed-length vector of arguments,
or execlp, which takes a list. Some template code for using exec is provided under folder code/a3. When you call exec make sure to give a relative path,
not absolute path to the executable, since the graders may have different absolute paths from you.

