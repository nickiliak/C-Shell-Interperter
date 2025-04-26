# C Shell Implementation

A simple Unix‐style shell written in C by **Nikos Iliakis** ([`@nickiliak`](https://github.com/nickiliak)).

## 🔍 Features

- **Interactive prompt** with current user & working directory  
- **Command execution** via `fork()` + `execvp()`  
- **Pipes** (`|`) to chain commands  
- **I/O redirection**: `>` / `>>` (stdout) and `<` (stdin)  
- **Built-ins**:  
  - `cd` – change directory  
  - `exit` – quit the shell  

## 🛠️ Prerequisites

- POSIX-compatible OS (Linux, macOS, WSL, etc.)  
- GCC (or any C99-compatible compiler)  
- `make` (provided via the included Makefile)  

## 🚀 Build & Run

```bash
# Clone your repo
git clone https://github.com/nickiliak/C-Shell-Interperter.git
cd c-shell

# Build using the Makefile
make

# Run your shell
./shell
