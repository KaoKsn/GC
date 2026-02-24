# Simple Prototypes of Mark and Sweep, Refcount based GCs

## Project Structure
<pre>
    .
    ├── LICENSE
    ├── Mark_N_Sweep
    │   ├── include
    │   │   ├── objects.h
    │   │   ├── stack.h
    │   │   └── vm.h
    │   ├── makefile
    │   └── src
    │       ├── main.c
    │       ├── objects.c
    │       ├── stack.c
    │       └── vm.c
    ├── README.md
    └── Refcount
        ├── caveats
        ├── objects.c
        ├── objects.h
        ├── stack.c
        └── stack.h

5 directories, 15 files
</pre>

## Compiling the Running.
```bash
    make
    ./main
    make clean
```
