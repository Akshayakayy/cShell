# SH SHELL


## FILES
- main.c : main file which contains shell loop
- exec.c : contains execution loop, calls every function based on user command
- cd.c - changes directory
- ls.c - displays list of all files and directories 
- echo.c - echoes whatever the user typed
- pwd.c - displays present working directory
- pinfo.c - prints the process related info of your shell program or given process ID
- remindme.c - reminds user with a custom message after the given number of seconds elapse

## ADDITIONAL FUNCTIONS
- setenv var value : SHRILL sets the value of var to value, or to the empty string if value is omitted. Initially, SHRILL inherits environment variables from its parent. Your shell can modify the value of an existing environment variable or create a new environment variable via the setenv command.
- unsetenv var : SHRILL destroys the environment variable var.
- jobs : prints a list of all currently running jobs along with their pid, in particular, background jobs, in order of their creation along with their state – Running or Stopped.
- kjob <jobNumber> <signalNumber> : takes the job id of a running job and sends a signal value to that process.
- fg <jobNumber> : brings a running or a stopped background job with given job number to foreground.
- bg <jobNumber> : changes a stopped background job to a running background job.
- overkill : kills all background process at once.
- CTRL-Z : It changes the status of currently running job to stop, and pushes it in background process.
- CTRL-C : It should cause a SIGINT signal to be sent to the current foreground job of your shell. If there is no foreground job, then the signal doesn't have any effect.
