

## Specificities on Windows

### MinGW and the lovely backslashes

To generate on Windows, as it is not a simple UNIX environment, GNU is strangely ported on it (on my opinion), I installed GNU with MinGW and makefiles have to be made for MinGW:

```bash
$ cmake .. -G "MinGW Makefiles" 
```

To make the project, you then have to run 
```bash
$ mingw32-make
```
It may surely changes if you selected another make alternative for Windows (ninja, Cygwin, ...)

