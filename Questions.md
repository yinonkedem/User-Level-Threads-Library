# Operating Systems Course Questions & Answers

## Overview
This document provides answers to frequently asked questions for the Operating Systems course. It covers topics like system calls, user-level threads, and process management in Google Chrome, among others.

## Contents
1. [siglongjmp and sigsetjmp](#siglongjmp-and-sigsetjmp)
2. [Use of User-Level Threads](#use-of-user-level-threads)
3. [Google Chrome Processes](#google-chrome-processes)
4. [Interrupts and Signals](#interrupts-and-signals)
5. [Real vs Virtual Time](#real-vs-virtual-time)

## 1) siglongjmp and sigsetjmp
### Description
- **sigsetjmp**: Saves the stack context and CPU state for later use by siglongjmp. If called directly, it returns 0. When returning from siglongjmp, it returns a non-zero value.
- **siglongjmp**: Restores the saved stack context and CPU state, effectively resuming execution from the point where sigsetjmp was called.

### Effect on Masking
- **sigsetjmp**: Optionally saves the current signal mask if its second argument is non-zero.
- **siglongjmp**: Restores the saved signal mask if it was saved by sigsetjmp.

## 2) Use of User-Level Threads
User-level threads, such as those used in video game development, offer faster context switches and more flexible scheduling than kernel-level threads. They allow for efficient multitasking within applications, enhancing responsiveness and performance.

## 3) Google Chrome Processes
### Advantages
- Isolation: Each tab is a separate process, so issues in one tab don't affect others.
- Security: Process isolation enhances security.
- Performance: Can utilize multi-core processors effectively.

### Disadvantages
- Resource Intensive: Uses more resources than using threads.
- Complex Communication: Inter-process communication is slower and more complex than inter-thread communication.

## 4) Interrupts and Signals
Commands like `kill pid` involve signals such as SIGTERM, which instructs a process to terminate. The OS handles the delivery of these signals, and the application must manage the signal to gracefully stop operations.

## 5) Real vs Virtual Time
- **Real Time**: Elapsed time measured by a clock, used in applications like timers and alarms.
- **Virtual Time**: CPU time used by a process, important for profiling and performance measurements.

## Authors
- **Yinon Kedem**: Principal developer and maintainer of this project.

---

For detailed usage of each API function and more in-depth information, please refer to the specific function prototypes and comments in the `uthreads.h` file.
