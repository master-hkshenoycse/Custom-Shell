# MyShell - A Simple Linux Shell

MyShell is a custom-built Linux shell written in C. It supports command execution, background processes, piping, redirection, signal handling, and built-in commands like `cd`, `exit`, and `history`.

## **🚀 Features**
- **Execute Commands:** Run any system command using `execvp()`.
- **Built-in Commands:** Supports `cd`, `exit`, and `history`.
- **Background Execution (`&`)**: Run processes in the background.
- **Piping (`|`)**: Execute multiple commands using pipes.
- **I/O Redirection (`>`, `<`)**: Redirect input/output to files.
- **Signal Handling:** Handles `Ctrl+C` (`SIGINT`).
- **Command History:** Stores and retrieves previously executed commands.

## **💻 Installation, Compilation, and Usage**
Clone the repository, compile the shell using `make`, and run it:


## ** Running Tests & Cleanup**
Run test scripts to validate functionality and clean up compiled files:

make test
make clean