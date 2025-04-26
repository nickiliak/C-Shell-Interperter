# C Shell Implementation

An interactive Unix-style shell written in C.  
Supports command execution, piping, I/O redirection, and built-in `cd`/`exit`.

---

## 🔍 Features

- **Interactive prompt** with customizable display  
- **Command execution** via `fork()` + `execvp()`  
- **Pipes** (`|`) to chain multiple commands  
- **Redirection** of stdout/​stdin with `>` / `>>` and `<`  
- **Built-ins**:  
  - `cd` – change directory  
  - `exit` – quit the shell  

---

## 🛠️ Prerequisites

- POSIX-compatible system (Linux, macOS, WSL, etc.)  
- GCC (or any C99-compatible compiler)  
- `make` (optional, if you add a Makefile)  

---

## 🚀 Build & Run

1. **Clone the repo**  
   ```bash
   git clone https://github.com/nickiliak/c-shell.git
