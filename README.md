# User-Level Threads Library (uthreads)

This project implements a user-level threads library (uthreads), developed for the Operating Systems course at Hebrew University. It includes functionalities such as thread creation, termination, blocking, and scheduling, along with handling signals and thread quantums.

## Features

- **Thread Management**: Allows for creating, terminating, and managing the states of threads.
- **Concurrency**: Supports multiple threads running and scheduling based on a preemptive policy.
- **Signal Handling**: Manages timer signals to handle thread quantums and preemption.
- **Thread States**: Includes states like READY, RUNNING, BLOCKED, and TERMINATE, allowing complex flow controls.

## Getting Started

### Prerequisites
Ensure you have a C++ compiler that supports C++11 (like g++), and standard Unix libraries.

### Compilation
To compile the library, navigate to the project directory and run the following command:
```bash
g++ -std=c++11 -o uthreads uthreads.cpp tools.cpp Thread.cpp -lpthread
## Running the Library
You can integrate this library into your C++ applications by including `uthreads.h` and linking against the compiled library. This allows you to leverage user-level thread management in your own applications.

## API Reference
This library provides several functions to manage threads within C++ applications:

- `uthread_spawn(thread_entry_point entry_point)`: Spawns a new thread that starts execution at the specified entry point function.
- `uthread_terminate(int tid)`: Terminates the thread identified by `tid`.
- `uthread_block(int tid)`: Blocks the thread identified by `tid`, preventing it from being scheduled until resumed.
- `uthread_resume(int tid)`: Resumes the execution of a previously blocked thread identified by `tid`.
- `uthread_sleep(int num_quantums)`: Puts the currently RUNNING thread to sleep for `num_quantums` scheduling intervals.

For more detailed information on each API function, please refer to the function prototypes and comments in the `uthreads.h` file.

## Built With
- **C++**: The primary programming language used.
- **Unix Standard Libraries**: Utilized for handling low-level operating system interactions.

## Authors
- **Yinon Kedem**: Principal developer and maintainer of the project.

## License
This project is open-sourced under the MIT License which allows modification and distribution of the software under specific conditions. See the `LICENSE` file for more details.
