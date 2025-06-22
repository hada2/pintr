
## pintr
Pintr is an execution tracer based on Intel Pin for reverse-engineering.

This is a rough and experimental tool developed for CTF and malware analysis. A common tracer is very slow because it saves all executed instructions in the log and makes huge log files, and is not easy to use for reverse-engineering. To solve this problem, this excludes uninteresting logs such as the instructions within API and records only essential information for analysis. 


## Requirement

* OS: Windows 10, Windows 8.1, Windows 7, Linux (Ubuntu 20.04)
* Arch: x64
* Intel Pin 3.21
* PE, ELF


## Install
### Linux

* Download and expand [Intel Pin](https://www.intel.com/content/www/us/en/developer/articles/tool/pin-a-binary-instrumentation-tool-downloads.html).
* Set the path.
    * `$ export PATH="$PATH;[PATH_TO_PIN_ROOT]"`
    * `$ export PIN_ROOT="[PATH_TO_PIN_ROOT]"`
* Run make.
    * `$ cd pintr`
    * `$ make`
* Output `obj-intel64/pintr.so`.


### Windows

* Install [Visual Studio](https://visualstudio.microsoft.com/) and [Cygwin](https://www.cygwin.com/).
* Download and expand [Intel Pin](https://www.intel.com/content/www/us/en/developer/articles/tool/pin-a-binary-instrumentation-tool-downloads.html).
* Launch the Visual Studio Developer Command Prompt. 
* Set the path.
    * `$ SET PATH=%PATH%;[PATH_TO_CYGWIN_BIN]`
        * e.g. `$ SET PATH=%PATH%;C:\cygwin64\bin`
    * `$ SET PATH=%PATH%;[PATH_TO_PIN_ROOT]`
    * `$ SET PIN_ROOT=[PATH_TO_PIN_ROOT]`
        * e.g. `SET PIN_ROOT=C:/pin-3.31` (Note: not backslash but slash)
* Run make.
    * `$ cd pintr`
    * `$ make`
* Output `obj-intel64\pintr.dll`.


## Synopsis
### Linux

* Run program.
    * `pin -t pintr.so [OPTIONS]... -- [TARGET_APP]`
* Attach to running program.
    * `pin -pid [PID] -t pintr.so [OPTIONS]...`


### Windows

* Run program.
    * `pin -t pintr.dll [OPTIONS]... -- [TARGET_APP]`
* Attach to running program.
    * `pin -pid [PID] -t pintr.dll [OPTIONS]...`


### Options
#### Select log columns

* -Ss
    * select log count column
* -St
    * select time of execution column
* -Sh
    * select hex of instruction binary column
* -Sb
    * select branch mark column
* -Si
    * select image name column
* -Sr
    * select routine name column (e.g. API)
* -Sf
    * select all eflags registers column
* -Sg
    * select all general-purpose registers column
* -Sx
    * select all xmm registers column
* -Sy
    * select all ymm registers column
* -Sall
    * select all column


#### Skip

* -Ki *IMAGE*
    * Image name to skip
    * This option can be used multiple times.

* -Kr *ROUTINE*
    * Routine name to skip
    * This option can be used multiple times.

* -Ka *ADDRESS*-*ADDRESS*
    * Address range to skip (e.g. 400000-408000)
    * This option can be used multiple times.


#### Hook

* -Hi *IMAGE*
    * Image name to hook
    * This option can be used multiple times.

* -Hr *ROUTINE*
    * Routine name to hook
    * This option can be used multiple times.

* -Ha *ADDRESS*-*ADDRESS*
    * Address range to hook (e.g. 400000-408000)
    * This option can be used multiple times.


#### Filter

* -Fi *IMAGE*
    * Image name to filter
    * This option can be used multiple times.

* -Fr *ROUTINE*
    * Routine name to filter
    * This option can be used multiple times.

* -Fa *ADDRESS*-*ADDRESS*
    * Address range to filter (e.g. 400000-408000)
    * This option can be used multiple times.

* -Fe
    * Filter before entry point (Start from entry point)

* -Fm
    * Filter before main image (Start from main image)

* -Fs
    * Filter before defined address (Start from defined address)

* -Fd
    * Filter after defined address (End in defined address)

* -Fx
    * Filter all instructions (Log only API call)


#### Trace

* -Ti *IMAGE*
    * Image name to trace
    * This option can be used multiple times.

* -Tr *ROUTINE*
    * Routine name to trace
    * This option can be used multiple times.

* -Ta *ADDRESS*-*ADDRESS*
    * Address range to trace (e.g. 400000-408000)
    * This option can be used multiple times.

* -Td
    * Trace inside routine

* -a
    * Trace all instruction


#### Exit

* -Ei *COUNT*
    * Instructions count limit

* -Et *MILLISECOND*
    * Time limit


#### Detection

* -Dc
    * Detect crypt magic numbers

* -Da
    * Detect crypt APIs (for Windows)

* -Dr *ROUTINE*
    * Detect defined routine
    * This option can be used multiple times.

* -Dm *MAGIC*
    * Detect defined magic number
    * This option can be used multiple times.

* -Dall
    * -Dc and -Da


#### Dump

* -Om *INS_ADDRESS*:*MEM_ADDRESS*:*SIZE*
    * Dump memory (e.g. 401000:7F000000:100)
    * This option can be used multiple times.

* -Or *INS_ADDRESS*
    * Dump registers (e.g. 401000) 
    * This option can be used multiple times.


#### Other

* -q
    * Quiet

* -v
    * Verbose

* -vv
    * Debug

* -mt
    * Trace only main thread

* -o *FILENAME*
    * Trace log filename


## Demo
### Trace all instructions
#### Command

The `-Td` option enables to output all logs including inside APIs.

```
$ pin -t pintr.so -Td -- ls
```

You can also use `-a`. The difference is that it makes all filtering rules clear.

```
$ pin -t pintr.so -a -- ls
```

#### Console log

```
 [+] Image info
Name:    /usr/bin/ls
Address: 555555554000-555555574b4f (22b40)
Entry:   55555555a7d0

 [+] Image info
Name:    /lib64/ld-linux-x86-64.so.2
Address: 7ffff7fcf000-7ffff7ffaccb (2ebf0)
Entry:   7ffff7fd0100

 [+] Image info
Name:    [vdso]
Address: 7ffff7fcd000-7ffff7fcdfde (2000)
Entry:   7ffff7fcd6d0

 [+] Tracing started ...
 [+] Settings
 [Columns]
                        Default 
 [Trigger]
   Start                -
   End                  -
 [Rules]
   Skip                 -
   Hook                 -
   Filter               -
   Trace                -
 [Mode]
   Log file (base)      trace-44352
   Verbose level        1
   Only API             No
   Only main thread     No
   Detect crypt magic   No
   Detect crypt API     No
 [Dump]
   Memory Dump          -
   Registry Dump        -
 [Others]
   Time Limit           -
   Ins Limit            -

 [+] Image info
Name:    /lib/x86_64-linux-gnu/libselinux.so.1
Address: 7fffe32ae000-7fffe32d8a07 (27d80)
Entry:   7fffe32b5040

 [+] Image info
Name:    /lib/x86_64-linux-gnu/libc.so.6
Address: 7fffe2fe8000-7fffe31d94d7 (1ef6a8)
Entry:   7fffe300f1f0

 [+] Image info
Name:    /lib/x86_64-linux-gnu/libpcre2-8.so.0
Address: 7fffe2f0d000-7fffe2f9c327 (8eac8)
Entry:   7fffe2f0f2e0

 [+] Image info
Name:    /lib/x86_64-linux-gnu/libdl.so.2
Address: 7fffe2f06000-7fffe2f0b10f (4980)
Entry:   7fffe2f07220

 [+] Image info
Name:    /lib/x86_64-linux-gnu/libpthread.so.0
Address: 7fffe2eb4000-7fffe2ed6477 (26628)
Entry:   7fffe2ebc190
```

#### Trace log

The log is saved in the following format.

```
(snip)
|55555555a7d4|xor ebp, ebp|{ebp_0}<={ebp_0,ebp_0}
|55555555a7d6|mov r9, rdx|{r9_7ffff7fe0d50}<={rdx_7ffff7fe0d50}
|55555555a7d9|pop rsi|{rsi_1,rsp_7fffffffdc98}<={rsp_7fffffffdc90,7fffffffdc90_8_1}
|55555555a7da|mov rdx, rsp|{rdx_7fffffffdc98}<={rsp_7fffffffdc98}
|55555555a7dd|and rsp, 0xfffffffffffffff0|{rsp_7fffffffdc90}<={rsp_7fffffffdc98}
|55555555a7e1|push rax|{rsp_7fffffffdc88,7fffffffdc88_8_1c}<={rax_1c,rsp_7fffffffdc90}
(snip)
```

The value read/written by registers/memory is described in the following formats.

`{written value}<={read value}`

The value includes that

* `reg_val`: Read or written `val` by register `reg`.
* `addr_size_val`: Read or written `val` of size `size` by memory address `addr`.

For example, `push rbp` is logged as follows.

```
{rsp_7fffffffd790,7fffffffd790_8_7fffe2dab298}<={rbp_7fffe2dab298,rsp_7fffffffd798}
```


### Select additional columns
#### Command

```
$ pin -t pintr.so -Td -Si -Sr -Sb -- ls
```

#### Trace log

```
(snip)
|ls|.text|55555555a7d4|xor ebp, ebp|{ebp_0}<={ebp_0,ebp_0}
|ls|.text|55555555a7d6|mov r9, rdx|{r9_7ffff7fe0d50}<={rdx_7ffff7fe0d50}
|ls|.text|55555555a7d9|pop rsi|{rsi_1,rsp_7fffffffdc98}<={rsp_7fffffffdc90,7fffffffdc90_8_1}
|ls|.text|55555555a7da|mov rdx, rsp|{rdx_7fffffffdc98}<={rsp_7fffffffdc98}
|ls|.text|55555555a7dd|and rsp, 0xfffffffffffffff0|{rsp_7fffffffdc90}<={rsp_7fffffffdc98}
|ls|.text|55555555a7e1|push rax|{rsp_7fffffffdc88,7fffffffdc88_8_1c}<={rax_1c,rsp_7fffffffdc90}
|ls|.text|55555555a7e2|push rsp|{rsp_7fffffffdc80,7fffffffdc80_8_7fffffffdc88}<={rsp_7fffffffdc88,rsp_7fffffffdc88}
(snip)
```


### Start tracing from an entry point

Tracing starts from the initial loader program and outputs long unnecessary logs before the main program starts. There are some techniques to filter uninteresting parts using options. 

Using `-Fe` enables to filter initial logs until the entry point.

```
$ pin -t pintr.so -Fe -- ls
```

Similarly, you can use `-Fm`. This is used for filtering initial logs until reaching the main image.

```
$ pin -t pintr.so -Fm -- ls
```

Using `-Fi` enables to exclude logs of the specified image. This is usually used with `-Td` option.

```
$ pin -t pintr.so -Td -Fi libc.so.6 -Ti ld-linux-x86-64.so.2 -- ls
```

`-Ki` enables to skip hooking all instructions of the specified image. This is similar but the difference from `Fi` is if hook or not. Filtering is slower than skipping.

```
$ pin -t pintr.so -Td -Ki libc.so.6 -Ki ld-linux-x86-64.so.2 -- ls
```

Using `-Ti` enables to specify the image to trace.

```
$ pin -t pintr.so -Td -Ti ls -- ls
```


## License

MIT License

## Author

* Copyright 2022, Hiroki Hada
* All rights reserved.




