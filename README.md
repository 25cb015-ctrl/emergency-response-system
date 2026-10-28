# Emergency Response System (C)

A console application that triages emergency patients and dispatches the
nearest ambulance along the shortest route - built entirely on classic
data structures and algorithms in C.

## Problem statement
An emergency control room receives calls continuously. It must:
1. Always serve the **most critical** patient first (ties: first come, first served).
2. Look up any patient's record **instantly** by ID.
3. Find the **nearest ambulance base** and the **shortest route** to the patient and then to the hospital.
4. Keep an ordered **history** of dispatches.

## DSA used
| Need | Data structure / algorithm | Complexity |
|---|---|---|
| Triage queue | Binary heap (priority queue) | push/pop O(log n) |
| Patient records by ID | Hash table (separate chaining) | avg O(1) |
| City road map | Weighted graph (adjacency matrix) | - |
| Nearest ambulance + route | Dijkstra's algorithm | O(V^2) |
| Dispatch history | Singly linked list (tail pointer) | append O(1) |

## Project structure
```
emergency-response-system/
├── include/            header files
├── src/                heap.c hashtable.c graph.c linkedlist.c main.c
├── tests/test_dsa.c    unit tests (assert based)
├── .vscode/            build task + debugger config
├── .github/workflows/  CI (builds and tests on every push)
├── Makefile
└── README.md
```

## Run in VS Code
**Prerequisites:** a C compiler (GCC) and the VS Code *C/C++* extension.
- Windows: install MinGW-w64 / MSYS2 and add `gcc` to PATH
- macOS: `xcode-select --install`
- Linux: `sudo apt install build-essential`

**Steps**
1. `File > Open Folder...` -> select `emergency-response-system`
2. Build: press `Ctrl+Shift+B`
3. Run in the terminal:
   - Linux/macOS: `./build/emergency`
   - Windows: `.\build\emergency.exe`
4. Debug: press `F5` (breakpoints work)

**With make (Linux/macOS/MSYS2):** `make run`, `make test`, `make clean`

**Run tests manually**
```
gcc -std=c11 -Iinclude tests/test_dsa.c src/heap.c src/hashtable.c src/graph.c src/linkedlist.c -o build/test_dsa
./build/test_dsa
```

## Try it
Choose `7` (load demo patients), then `3` to see the queue, then `2` repeatedly to
dispatch. Critical patients go first, and each dispatch prints the ambulance base,
route, distance and estimated time.

## Push to GitHub
```bash
cd emergency-response-system
git init
git add .
git commit -m "Emergency Response System using DSA in C"
git branch -M main
git remote add origin https://github.com/<your-username>/emergency-response-system.git
git push -u origin main
```
Create the empty repository on github.com first (do not add a README there).

## Ideas to extend
- Replace the adjacency matrix with an adjacency list + min-heap Dijkstra (O(E log V))
- Load the city map and patients from a file
- Add multiple ambulances with availability tracking
- Save/restore state to disk
