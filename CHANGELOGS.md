### Jul 9, 2026 (Kritarth Ranjan's fork)
- Fixed a startup crash: `CommandRegistry::commandMap` was a plain static member with undefined initialization order relative to the static Command instances that register into it at startup, causing an intermittent `Floating point exception` inside `std::unordered_map`. Switched to a function-local static (Meyer's singleton).
- Fixed `rm`/`rmf` printing a false "removed" success message even when `FileSystemObject::remove()` failed.
- Added a single-owner permission model to `FileSystemObject` (3-char `rwx`-style string), enforced across `remove()`, `FileObject::read/rewrite/addWrite`, `DirectoryObject::displayContent`, `touch`, `mkdir`, and `cd`.
- Added programs: `chmod`, `grep`, `find`, `tree`, `man`.
- Enhanced `ls` with `-l`/`-la` flags for a detailed (permissions/size/updated-time) listing.
- Enhanced `FileSystemObject::displayInfo()` (`info` command) to also print permissions and size.

### Sep 10, 2025
- Added 'apps' and 'alias' programs
  - custom name validator is needed for 'alias' program
- Added built-in error for command arguments
  - 'ls' program does not work without arguments.
  - This may look like a drawback, but this enhancement removed a lot of boilerplate from adding programs
- Separated FileSystemObject::nameValidator to a separate unit `filesystem/utils` for further use by other modules
- Needs documentation changes now
