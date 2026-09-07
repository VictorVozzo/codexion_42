*This project has been created as part of the 42 curriculum by vivozzo-.*

# Codexion

Master the race for resources before the deadline masters you.

## Description

Codexion is a multithreaded simulation written in C, inspired by the classic
dining philosophers problem. A group of **coders** sit around a circular
co-working hub. Between each pair of coders there is exactly one shared
**USB dongle** (one dongle per coder), and compiling quantum code requires
holding **two dongles at once** — the one on the coder's left and the one on
their right.

Each coder repeats an infinite cycle:

1. **Take** the left and right dongles (one at a time, respecting the
   scheduling policy and the dongle cooldown).
2. **Compile** for `time_to_compile` ms while holding both dongles.
3. **Release** both dongles back on the table.
4. **Debug** for `time_to_debug` ms.
5. **Refactor** for `time_to_refactor` ms, then immediately try to compile
   again.

If a coder does not *start* compiling within `time_to_burnout` ms of the
beginning of the simulation (or of the start of their last compile), they
**burn out** and the whole simulation stops. The simulation also stops
successfully once every coder has compiled at least
`number_of_compiles_required` times.

Each coder is a POSIX thread, each dongle is protected by a mutex and a
condition variable, and dongle access is arbitrated by a custom priority
queue (min-heap) that implements either a **FIFO** or an **EDF**
(Earliest Deadline First) scheduling policy — built from scratch, without
any standard library priority queue.

A dedicated **monitor thread** continuously watches every coder's deadline
and total compile count, and is responsible for detecting burnout (within
10 ms of the actual event) and for stopping the simulation cleanly.

## Instructions

### Compilation

```sh
make          # builds the codexion binary
make clean    # removes object files
make fclean   # removes object files and the binary
make re       # fclean + all
```

The project compiles with `cc -Wall -Wextra -Werror -pthread` and has no
external dependencies besides the POSIX threads library.

### Usage

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument                      | Description                                                                                                   |
|--------------------------------|-----------------------------------------------------------------------------------------------------------------|
| `number_of_coders`             | Number of coders (and number of dongles).                                                                      |
| `time_to_burnout` (ms)         | A coder burns out if they don't start compiling within this delay since the simulation start or last compile.  |
| `time_to_compile` (ms)         | Time spent holding both dongles while compiling.                                                                |
| `time_to_debug` (ms)           | Time spent debugging after releasing the dongles.                                                               |
| `time_to_refactor` (ms)        | Time spent refactoring before trying to compile again.                                                          |
| `number_of_compiles_required`  | Simulation stops successfully once every coder reaches this number of compiles.                                 |
| `dongle_cooldown` (ms)         | Delay before a released dongle can be taken again.                                                              |
| `scheduler`                    | Arbitration policy for dongle access: `fifo` or `edf`.                                                          |

All arguments are mandatory and must be non-negative integers (except
`scheduler`, which must be exactly `fifo` or `edf`). Invalid input is
rejected with an explanatory error message and the program exits.

Example:

```sh
./codexion 4 800 200 200 200 5 50 edf
```

Each state change is printed as `timestamp_in_ms coder_number event`, e.g.:

```
0 1 has taken a dongle
2 1 has taken a dongle
2 1 is compiling
202 1 is debugging
402 1 is refactoring
```

## Resources

- [POSIX Threads Programming (LLNL tutorial)](https://hpc-tutorials.llnl.gov/posix/)
- [Linux man pages: pthread_mutex_lock(3), pthread_cond_wait(3), pthread_cond_timedwait(3), pthread_create(3)](https://man7.org/linux/man-pages/)
- YouTube videos about the philosophers problem, binary heap and threads in C
- 42 School's own subject and slides for this project

**How AI was used:** An AI assistant (Claude) was used as a learning and
review aid throughout the project, not as a code generator for the final
submission. It was also used to draft and structure this README.

All AI-assisted explanations were cross-checked against the subject and
verified through manual testing and peer discussion before being relied
upon.

## Blocking cases handled

- **Deadlock prevention (Coffman's conditions):** each coder always
  requests their two dongles in a fixed, deterministic order (an
  even-numbered coder takes the left dongle first, an odd-numbered coder
  takes the right dongle first). This breaks the *circular wait* condition
  that causes the classic dining-philosophers deadlock, since it's
  impossible for every coder in the ring to be simultaneously holding one
  dongle while waiting on the other in the same rotational direction.
- **Starvation prevention / liveness:** dongle access is arbitrated by a
  custom priority-queue scheduler instead of a plain lock. Under `fifo`,
  requests are served strictly in arrival order. Under `edf`, the coder
  with the earliest burnout deadline (`last_compile_start +
  time_to_burnout`) is served first, with arrival time as a deterministic
  tie-breaker, guaranteeing that the most urgent coder is never left
  waiting behind less urgent ones.
- **Cooldown handling:** a dongle that has just been released is not
  immediately reusable. Its `time_available` timestamp is recorded when it
  is released, and a coder is only allowed to actually take it once
  `dongle_cooldown` ms have elapsed, checked both when granting the dongle
  and via a timed wait (`pthread_cond_timedwait`) so the coder re-checks
  the condition instead of blocking forever.
- **Precise burnout detection:** a dedicated monitor thread polls every
  coder's deadline on a short cycle (1 ms), so a burnout is detected and
  logged within the required 10 ms tolerance, instead of relying on the
  coder threads themselves (which could be blocked or sleeping at the
  exact moment they burn out).
- **Log serialization:** every state-change message is printed while
  holding a dedicated `print_log_mutex`, guaranteeing that two messages
  from different threads are never interleaved on the same line, and that
  no message is printed after the simulation has already stopped.
- **Clean shutdown:** once burnout is detected or every coder has reached
  `number_of_compiles_required`, a shared `monitor_finished` flag is set
  and all dongle condition variables are broadcast, so every coder thread
  currently waiting for a dongle wakes up, notices the simulation has
  ended, and exits instead of blocking forever.

## Thread synchronization mechanisms

- **`pthread_mutex_t coder_mutex`** (one per coder): protects a coder's
  own state — `last_compile_start` and `total_compiles` — from concurrent
  reads/writes between the coder's own thread and the monitor thread. Any
  read of `last_compile_start` by the monitor, or update after a compile
  starts, is wrapped in a lock/unlock pair on this mutex, preventing the
  monitor from reading a torn or stale deadline.
- **`pthread_mutex_t mutex` + `pthread_cond_t cond`** (one pair per
  dongle): together they implement a monitor-style wait queue for each
  dongle. The mutex protects `available`, `time_available`, and the
  small internal priority queue (`coder_queue`) that tracks which coders
  are waiting for that specific dongle. A coder that cannot immediately
  take a dongle calls `pthread_cond_wait`/`pthread_cond_timedwait` while
  holding that dongle's mutex, releasing it atomically while sleeping, and
  re-acquiring it on wake-up — so no coder busy-waits, and no two coders
  can ever both believe they own the same dongle.
- **`pthread_cond_broadcast`**: used whenever a dongle becomes available
  again (after being released and, once its cooldown has passed) and
  whenever the simulation ends, so *every* coder currently waiting on that
  dongle's condition variable wakes up to re-check whether it is now their
  turn (based on the scheduler's priority queue), rather than signalling
  only one arbitrary waiter.
- **Custom priority queue (min-heap) per dongle**: implemented from
  scratch (`queue_push`/`queue_pop`/`heapify_down`) on top of each dongle's
  mutex. It orders waiting coders either by arrival time (`fifo`) or by
  burnout deadline with an arrival-time tie-breaker (`edf`), and is the
  data structure that actually decides who is allowed to take the dongle
  next once it becomes available.
- **`pthread_mutex_t monitor_mutex` + `monitor_finished` flag**: the
  single source of truth for whether the simulation is still running.
  Every coder thread checks this flag (under the mutex) both before
  attempting to acquire a dongle and while waiting on a dongle's condition
  variable, so a burnout or a completed run is propagated to all threads
  promptly and safely, without a race between the monitor declaring the
  end of the simulation and a coder thread starting a new compile.
- **`pthread_mutex_t print_log_mutex`**: serializes all `printf` calls
  used for state-change logging, guaranteeing that concurrent log lines
  from different coder threads (and the monitor) are never interleaved,
  and that logging happens consistently with the current value of
  `simulation_running`.

Race conditions are avoided throughout by the rule that **shared state is
never read or written outside of the mutex that protects it**: a dongle's
`available`/`time_available`/queue fields are only touched while holding
that dongle's mutex; a coder's `last_compile_start`/`total_compiles` are
only touched while holding that coder's mutex; and the simulation's
run/stop state is only touched while holding `monitor_mutex`. Condition
variables are always used together with their protecting mutex and are
re-checked in a loop after waking up, which avoids both busy-waiting and
lost-wakeup/spurious-wakeup bugs.
