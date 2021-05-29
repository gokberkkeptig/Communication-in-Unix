# Communication-in-Unix
Communication in Unix using fork, wait, pipe,kill,signal

# Part-1
  Use files to pass information from children processes to the parent process. Your parent process is executed via part1nto use the first ninput filesin the file folder, each named input<i>.txt. Parent process creates nchildren processes and each child process sorts the numbers in the input fileand writes the result to an intermediate output file. Input file format:<m>number_1 number_2 number_3 ... number_mOutput file is a 4-liner with the following format:<m>sortednumber_1sortednumber_2 sortednumber_3 ... sortednumber_m<execution time in seconds><name of the signal received, e.g., SIGUSR2, and receive time, e.g., 11:37:44>Child  process  uses SelectionSortif  its  process  id  is  odd  and InsertionSortotherwise.  It  also sleeps xseconds  after the sorting as it getstired. xis a random number between 1 and 7.  Its execution time is sorting + sleeping, so it will be at least 1 second.Parent reads the intermediate files when all children processes finish.It also sends a signalto each active child.Send SIGUSR1 if child id odd, SIGUSR2 if it’s even.Once all intermediate files are read, parent process creates a single output file called output.txt which willbe sorted w.r.t. the <execution time>s. Here is its format:<execution tof proc_i> <sorted numbers for proc_i> <name & t of receive for signal by proc_i><execution t of proc_j> <sorted numbers for proc_j> <name & t of receive for signal by proc_j>and so on.
# Part-2 
  Implement the same program using pipes instead of files. 
# Part-3
  Signal support in both parts, i.e., do the <signal by proc>.
