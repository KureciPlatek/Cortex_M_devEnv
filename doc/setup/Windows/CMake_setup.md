
>[!INFO]
>How `CMakeLists.txt` and its sub `.cmake` files are organized on Windows is the same as for Linux so I only explained the differences, which is how to generate `Makefiles` and compile on Windows.

## Specificity on Windows

### `MinGW` and the lovely backslashes

To generate on Windows, as it is not a simple UNIX environment, GNU is strangely ported on it (on my opinion), I installed GNU with `MinGW` and `Makefiles` have to be made for `MinGW`:

```bash
$ cmake .. -G "MinGW Makefiles" 
```

To make the project, you then have to run 
```bash
$ mingw32-make
```
It may surely changes if you selected another make alternative for Windows (`ninja`, `Cygwin`, ...)

