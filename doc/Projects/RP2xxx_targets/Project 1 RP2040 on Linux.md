
Port `ThreadX` on `RP2040` was actually, like for stm23f4 project, not very complicated. Some difficulties were encountered, but with the help of other projects example, it became quite easy.

#### Initialize `ThreadX` low level:

I encountered some difficulties on official `ThreadX`'s `tx_initialize_low_level.S` and had to modify following lines:

Deactivate `.global  __RAM_segment_used_end__`:
```diff
-    .global  __RAM_segment_used_end__
+@    .global  __RAM_segment_used_end__
```

Modify vector table to be in ram:
```diff
-    .global  _vectors
+@    .global  _vectors
+    .global  ram_vector_table
```

And deactivate  `Set base of available memory to end of non-initialised RAM area.`:

```diff
-    LDR     r0, =_tx_initialize_unused_memory       @ Build address of unused memory pointer
-    LDR     r1, =__RAM_segment_used_end__           @ Build first free address
-    ADDS    r1, r1, #4                              @
-    STR     r1, [r0]                                @ Setup first unused memory pointer
+@    LDR     r0, =_tx_initialize_unused_memory       @ Build address of unused memory pointer
+@    LDR     r1, =__RAM_segment_used_end__           @ Build first free address
+@    ADDS    r1, r1, #4                              @
+@    STR     r1, [r0]                                @ Setup first unused memory pointer
```


#### Configure `openOCD` scripts

The `rp2040.cfg` configuration script from official `openOCD` repository seemed not to work so I took the configure script provided when installing pico's extension plugin on VSCode.

But maybe it was just because my pico board was not powered anymore and had to re-plug it. Who knows, now it works :)

#### Configure VSCode

I configured again the Marius's extension for Cortex-M debug. I slightly modified the one I explained in file [[About the Pico-SDK]] where I explain how to configure VSCode for debug

```json
   "configurations": [
         {
         "name": "prj_rp2040_debug_openOcd",
         "cwd": "${workspaceRoot}",
         "executable": "${workspaceRoot}/prj_rp2040/bin/prj_rp.elf",
         "request": "launch",
         "type": "cortex-debug",
         "servertype": "external",
         "gdbTarget": "localhost:3333",
         "svdFile": "${workspaceFolder}/../../pico-sdk/src/rp2350/hardware_regs/RP2040.svd",
         "runToEntryPoint": "main",
         "overrideLaunchCommands": [
             "monitor reset init"
         ]
      },
      {
	     ... /* other configs */
      }      
```


#### Modifications in source code and `CMake`:

In `app/app_thread.c` and `app/app_thread.h`, almost no modifications (actually no differences with projects `prj_h7` and `prj_f4`)

`system/tx_initialize_low_level.c` was as explained, modified to be for `cortex-m0`

In `CMakeLists.txt`, some modifications are important:
- Add `cmsis_core` in `target_link_library()`
- Set RTOS variable to `ThreadX` of course:
```CMake
# --- Selection of RTOS
set (RTOS_THREADX 1)   # Selection of RTOS, choose between RTOS_THREADX and RTOS_FREERTOS
```
- Add `ThreadX` sources manually as we are not using the integrated `CMakeLists.txt` of `ThreadX` but directly building and linking source to our project:
```CMake
set (PROJECT_SOURCES 
					 ...
                     ${CMAKE_SOURCE_DIR}/app/app_threadx.c
                     ${CMAKE_SOURCE_DIR}/system/tx_initialize_low_level.S
                     ${THREADX_PROJECT_SOURCES}
					 ...
)
```
- In `cortex_m_rtos.cmake`: `tx_misra.S` do not exists for port on Cortex-M0+ of `ThreadX`: So comment it out in `THREADX_PORT_SRC`:
```CMake
# ${THREADX_PATH}/ports/${MCPU_PATH}/gnu/src/tx_misra.S
```


## Result:

![[debug_example_VSCode_rp2040.png]]

All works fine, I can run the project and see the beautiful blink.