# Project 1 Report
    
## Introduction
The most important implementation for my project 1 are parse_cmd, built-in cmds, two data structure, and the main body of processes running. 

## parse_cmd
Step 1 : Taking in the usr input\
Step 2 : Using strtok() function to separate the usr input by "|" character. Here I learned online that if I put NULL as the first argument of strtok(), it will automatically use the lastest non-null token generated before. I think there is a built-in buffer in it. And I can find all token by using it in a loop with NULL arg. But when the targeted char appears on the first index, it will  mess up my error detecting method. So I hard coded this situation.\
Step 3 : Putting each token into the parse_cmd function and let it manipulate the array of data structure called job. It basically go through every character in the token and act differently detecting '\n', '<', '>', ' '(whitespace), otherwise. I used two ints as switch that represented redirect_enabled and redirect_detected. I think there is better way to tell if there is redirect and has it been done.\
Step4 : Using a for loop to parse all cmd separated by pipeline and put them into a array of job.
## processes running
Step 1 : Generating n-1(n = # of jobs) pipes. I used varied sized 2d array to contain pipes, but when there is only one job and I actully do not need pipes, the pipe[0][0 and 1] will be initalized as {0, 0}. And this will mess up the single process pipe arrange. So I add a if condition to hard code this situation.\
Step 2 : Using for loop to generte processes and return in child process so it won't grow exponentially. Inside ezch child process, check if it's needed to arrange pipes and file redirection. And hard coded the two error situation of mislocated redirection.\
Ster 3 : Collecting the exit status and waiting all of the child processes out of the generating loop so that they are not running one by one.
## built-in cmd
For cd and pwd, I just use the chdir and getcwd functions. For the directory stack, I referenced online to review how to create stack data structure in C.
## Referenced material
scaler.com/topics/stack-in-c/\
educative.io/answers/splitting-a-string-using-strtok-in-c
