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

## Build and Run.
```bash
    make
    ./main
    make clean
```


## Special Thanks to:
1) TJ Devries - https://github.com/tjdevries (TJ's github homepage)
2) Boot Dev's Memory Management & C course - https://www.youtube.com/watch?v=rJrd2QMVbGM ("Youtube page for the course")
