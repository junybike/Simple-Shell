# Simple_Shell
This is a raw copy of the Simple Shell project

## In this project...
- Constructed a simple shell that executes user command input using C in a Linux virtual machine
- Followed the signal safety to avoid abnormalities during execution by using signal-safe functions
- Used fork function to execute commands in separate processes to support foreground and background command

## About:
- This shell can take simple commands such as,
  - ls
  - cd
  - pwd
- Multiple arguments to the commands are supported. For example,
  - ls -la
  - cd /home/Simple-shell
- History command displays the last 10 most recent successful commands
