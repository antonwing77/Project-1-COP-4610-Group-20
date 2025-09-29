# Shell

A comprehensive shell interface that enhances process control, user interaction, and error handling mechanisms.

## Group Members
- **Jordan Forthman**: jf24b@fsu.edu
- **Anton Wingeier**: abw21e@fsu.edu
- **Grace Hutchinson**: gah24b@fsu.edu

## Division of Labor

### Part 1: Prompt
- **Responsibilities**: Create the shell prompt to display the username, hostname and working directory
- **Assigned to**: Grace Hutchinson, Anton Wingeier, Jordan Forthman

### Part 2: Environment Variables
- **Responsibilities**: Create the mechanism to expand tokens into their corresponding enviornment values
- **Assigned to**: Grace Hutchinson, Anton Wingeier, Jordan Forthman

### Part 3: Tilde Expansion
- **Responsibilities**: Create functionality to expand ~ into the path of $HOME
- **Assigned to**: Anton Wingeier

### Part 4: $PATH Search
- **Responsibilities**: Create path search to locate files in directories specified by $PATH
- **Assigned to**: Anton Wingeier

### Part 5: External Command Execution
- **Responsibilities**: Design eexecution of external commands to create child processes and handle command args
- **Assigned to**: Anton Wingeir

### Part 6: I/O Redirection
- **Responsibilities**: Enable I/O redirection
- **Assigned to**: Jordan Forthman, Grace Hutchinson

### Part 7: Piping
- **Responsibilities**: Enable piping to connect output of one command to input of another
- **Assigned to**: Jordan Forthman, Grace Hutchinson

### Part 8: Background Processing
- **Responsibilities**: Enable support for background command execution and job tracking
- **Assigned to**: Jordan Forthman, Grace Hutchinson

### Part 9: Internal Command Execution
- **Responsibilities**: Create commands exit, cd and jobs to handle directory change, job listing and shell termination
- **Assigned to**: Jordan Forthman, Grace Hutchinsonn, Anton Wingeier

## File Listing
```
shell/
│
├── src/
│ |-main.c
│ |-shell.c
| |-execute.c
| |-lexer.c
| |-parse.c
│
├── include/
│ |-shell.h
| |-execute.h
| |-lexer.h
| |-parse.h
| |-shell.h
│
├── README.md
└── Makefile
```
## How to Compile & Execute

### Requirements
- **Compiler**: gcc
- **Environment**: Designed to run on a linux environment, tested in linprog

### Compilation
To compile the shell program, navigate to the project root directory and run:

```bash
make
```
This will build the executable in the bin/ directory, named shell. The Makefile compiles all source files from the src/ directory, places object files in the obj/ directory, and links them to produce the bin/shell executable.

### Execution
```bash
make run
```
This will run the program ...

## Development Log
### Jordan Forthman

| Date       | Work Completed / Notes |
|------------|------------------------|
| 2025-09-19 | Worked on and finished prompt
| 2025-09-20 | Worked on and finished env variables
| 2025-09-24 | Worked on and finished I/O redirection
| 2025-09-25 | Worked on and finished piping and background processing
| 2025-09-27 | Worked on and finished internal command execution



### Anton Wingeier

| Date       | Work Completed / Notes |
|------------|------------------------|
| 2025-09-19 | Worked on and finished prompt
| 2025-09-20 | Worked on and finished env variables
| 2025-09-22 | Worked on and finished tilde expansion and $PATH search
| 2025-09-24 | Worked on and finished external command execution
| 2025-09-27 | Worked on and finished internal command execution



### Grace Hutchinson

| Date       | Work Completed / Notes |
|------------|------------------------|
| 2025-09-19 | Worked on and finished prompt
| 2025-09-20 | Worked on and finished env variables
| 2025-09-24 | Worked on and finished I/O redirection
| 2025-09-25 | Worked on and finished piping and background processing
| 2025-09-27 | Worked on and finished internal command execution


## Meetings
Document in-person meetings, their purpose, and what was discussed.

| Date       | Attendees           | Topics Discussed       ...| Outcomes / Decisions |
|------------|---------------------|---------------------------------------------------------------------|
| 2025-09-19 | Grace, Anton, Jordan| Preplanning, responsibility assignments, personal deadline formation| Created the divisions of labor and set deadlines for each sunday|

| 2025-09-28 | Grace, Anton, Jordan| Bugs, submission, formatting, cohesion| Fixed minor bugs and formatting discrepensies between modules, got project in final submission state|
