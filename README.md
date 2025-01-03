# Simple_Shell
This is a raw copy of the Simple Shell project

## How to Compile
1. Download the repository and unzip the file
2. Open the terminal
3. Navigate to the downloaded file
4. In the directory, create a new directory called "build": `mkdir build`
5. Navigate into the "build" directory and call command: `cmake ../`
6. Finally, call command: `cmake --build .`

## How to Run - Simple Shell
1. Navigatve to newely created "build' file
2. Call command `./shell`

## In this project...
- Constructed a simple shell that executes user command input using Linux systems programming functions in C
- Followed the signal safety to avoid abnormalities during execution by using signal-safe functions
- Used fork function to execute commands in separate processes to support foreground and background command

## About:
- This shell can take simple commands such as,
  - ls, cd, pwd, mkdir, and more
- Multiple arguments to the commands are supported. For example,
  - ls -la
  - cd /home/Simple-shell
- History command displays the last 10 most recent successful commands
