# Implementation Report

## Findings

- `Pooler` lacked the spawn, despawn, clear, and active-enemy methods used by the game. Its raw `new Enemy()` had no matching ownership or cleanup path.
- `Tower` lacked distance calculation and closest-enemy selection. Firing with no active enemy would dereference a null pointer.
- `Logger` lacked `Log` and `Flush`. Its worker accessed the queue and exit flag without synchronization and wrote queued messages in reverse order.
- `Game::SetGameOver` assigned its parameter to itself. As a result, pressing Esc and an enemy reaching the tower did not set the game's end state.

## Changes

- `Pooler` preallocates the requested minimum, owns enemies with `std::unique_ptr`, and reuses inactive objects through a free list. It expands only when the free list is empty. `Clear` deactivates all enemies for reuse. Despawning a null, duplicate, or foreign pointer leaves the pool unchanged. `PoolSize` reports the total number of allocated enemies.
- `Tower` selects the nearest active enemy using Chebyshev distance, `max(|dx|, |dy|)`, and applies the same distance to automatic firing range. This matches movement of up to one cell on each axis per update. It handles empty and null entries safely and accepts enemy vectors by constant reference.
- `Logger` uses a mutex, FIFO queue, and condition variables to preserve enqueue order while writing asynchronously. `Flush` waits for pending messages to be written and then flushes the file. Destruction drains the queue and joins the worker. Timestamp conversion uses thread-safe platform functions; opening the log file now reports failure explicitly.
- `Game::SetGameOver` now assigns to the member variable: `this->isGameOver = isGameOver`.

## Verification

- The Release build of the three-class changes succeeded with CMake.
- Assertions passed for pool preallocation, reuse, growth, duplicate despawn, clear, empty target lists, closest-target selection, automatic range, and manual firing.
- Four threads wrote 100 log entries each. After `Flush`, all 400 lines were present and each thread's sequence remained ordered.
- The Esc fix passed a separate full CMake build and a state-transition test (`false` to `true` to `false`). The existing executable was locked by a running process, so this build used `build/esc-fix`.
- `git diff --check` passed. No performance profiling was performed.

## Recommendations

- Cache the active-enemy snapshot per game update to reduce repeated traversal and temporary allocations in update and collision handling.
- Consider aimed projectile movement if bullets should hit targets outside horizontal, vertical, or exact diagonal paths. Bullets currently choose a fixed direction when fired.

## Submission scope

The original assignment requests only the three class files and a report. The subsequent Esc fix requires `src/game.cpp`, so the updated ZIP also includes that file.
