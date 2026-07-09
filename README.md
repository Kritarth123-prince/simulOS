# simulOS

A simulated operating system written in C++ — an in-memory filesystem, an interactive shell, a command-dispatch layer, and a plug-in interface for adding new programs. It's not a wrapper around real OS calls; the filesystem, permissions, and every command are implemented from scratch in memory, and the "OS" boots into an interactive shell session exactly like a real terminal would.

<br>

## Overview

simulOS models four things a real OS needs, each as its own module:

- **FileSystem** — an in-memory tree of directories and files, with permissions, timestamps, and path resolution (including `.`, `..`, and absolute/relative paths)
- **Shell** — the interactive REPL a user types into; owns the current working directory and command history
- **Command unit** — parses raw input into a command + arguments and dispatches it to the matching registered program
- **Programs** — the actual commands (`ls`, `cd`, `mkdir`, `grep`, `chmod`, etc.), each self-registering into a global command registry so new ones can be dropped in without touching the core

Everything runs as a single process; there's no real disk I/O and no real users — it's a sandbox for exploring how these pieces fit together, and a base for experimenting with OS-level concepts (permissions, process-like commands, scripting) without needing a kernel.

<br>

## Features

**Filesystem**
- Directories and files, both dynamically allocated through factory methods (no direct construction — enforced at compile/runtime)
- Absolute and relative path resolution, including `.` and `..`
- Per-object creation and last-updated timestamps
- A single-owner permission model (see below)

**Shell**
- Persistent working directory with a live prompt (`~/current/path $`)
- Command history logged to a virtual file and replayable with `history`

**Commands** (26 total)

| Category | Commands |
|---|---|
| Navigation | `cd`, `ls`, `tree`, `info` |
| File/dir management | `touch`, `mkdir`, `rm`, `rmf` |
| Search | `grep`, `find` |
| Permissions | `chmod` |
| Shell utilities | `history`, `clear`, `alias`, `apps`, `help`, `man`, `date`, `shutdown`/`exit` |

Run `apps` inside the shell for the live list, and `man <command>` (or `help <command>`) for usage details on any one of them.

<br>

## Permission model

Every file and directory carries a 3-character permission string (default `rwx`) — a simplified, single-owner model, since simulOS only ever has one simulated user:

```
chmod rwx myfile     # full access (default)
chmod r-- myfile      # read-only
chmod rw- myfile      # no execute
chmod r-x mydir       # readable/enterable, but nothing can be added or removed
```

- **`r`** — required to view a file's contents (`info`, `grep`) or list a directory's contents (`ls`)
- **`w`** — required to modify or delete a file (`rm`/`rmf`), and required *on the parent directory* to create something inside it (`touch`/`mkdir`)
- **`x`** — required on a directory to `cd` into it

Permissions are checked directly inside the filesystem layer (not just in the commands), so any new program built against `FileSystemObject` automatically respects them.

<br>

## Example session

```bash
[simulsh@simulos] ~/ $ mkdir docs
Directory 'docs' created.
[simulsh@simulos] ~/ $ cd docs
[simulsh@simulos] ~/docs/ $ touch notes
FileSytem: File 'notes' created.
[simulsh@simulos] ~/docs/ $ ls -la .
total 1
-rwx  0    (empty)    notes
[simulsh@simulos] ~/docs/ $ chmod r-- notes
chmod: 'notes' permissions set to 'r--'.
[simulsh@simulos] ~/docs/ $ rm notes
FileSystemError: Permission denied: 'notes' is not writable (chmod to add 'w').
[simulsh@simulos] ~/docs/ $ cd /
[simulsh@simulos] ~/ $ tree /
/
├── sh_history
└── docs/
    └── notes

1 directory, 2 files
[simulsh@simulos] ~/ $ find notes /
/docs/notes
[simulsh@simulos] ~/ $ man chmod
```

<br>

## Architecture & workflow

```
User types a command
        │
        ▼
   Shell (owns FileSystem + current directory)
        │  passes raw input string
        ▼
CommandHandler::processCommand()
        │  parseCommand() → splits into [key, args...]
        ▼
CommandHandler::executeCommand()
        │  looks up key in CommandRegistry
        ▼
matching Command::operate(shell, args)
        │  program logic runs, touches FileSystem via Shell
        ▼
   result code returned to Shell (0 = success, -1 = error, -2 = shutdown)
```

`OS` is the top-level object: it constructs `FileSystem` and `Shell` and starts the session loop.

### Extending it: adding a new command

Every program is a class inheriting from the abstract `Command` interface:

```cpp
class YourCommand : public Command
{
  public:
    YourCommand() : Command("your_key", /* expected arg count, -1 = at least one */ 1, "description")
    {
        CommandRegistry::registerCommand("your_key", this);
    }

    int operate(Shell &shell, std::vector<std::string> cmd_args) override
    {
        // your logic here — use shell.getCurrentDir(), shell.getRootDir(),
        // and objectLocator() from FileSystem.h to resolve paths
        return 0;
    }
};

static YourCommand yourCommandInstance; // self-registers at startup
```

Drop the `.cpp` (and optional `.h`) into `programs/core/<yourprogram>/`, and the Makefile picks it up automatically — no other file needs to change. See `programs/core/grep/Grep.cpp` or `programs/core/chmod/Chmod.cpp` for a fuller example that recurses through the filesystem tree.

<br>

## Project structure

```
simulOS/
├── main.cpp                 # entry point — constructs and starts OS
├── os/                       # top-level OS class: owns FileSystem + Shell
├── shell/                    # interactive REPL, current dir, history
├── cmdunit/                  # command parsing, dispatch, CommandRegistry
├── filesystem/                # DirectoryObject, FileObject, path resolution,
│   └── utils/                 #   permissions; utils/ has name validation
├── programs/
│   └── core/                  # one directory per command (ls, cd, grep, chmod, ...)
└── tests/                     # test scaffolding
```

<br>

## Building and running

Requires a C++ compiler with `make` (or manually compile every `*.cpp` under the project, excluding `tests/`).

```bash
git clone <this-repo-url>
cd simulOS
make
./simulos
```

Inside the shell:
```bash
apps              # list every available command
man <command>      # or: help <command>
mkdir sandbox && cd sandbox
touch file1
ls -la .
shutdown           # or: exit
```

To rebuild from scratch:
```bash
make clean && make
```

<br>

## Design notes

- **Error handling** is integer-coded: `0` success, `-1` error, `-2` shutdown signal. Error messages that need to reach the user are printed directly to STDOUT/STDERR at the point of failure rather than bubbled up as return values.
- **Memory safety**: `DirectoryObject`/`FileObject` can only be created via their `create()` factory methods (private constructors), and self-removal goes through `remove()` rather than a bare `delete`, since directories recursively clean up their children on destruction.
- **Static registration**: commands register themselves into `CommandRegistry` via a static instance at file scope, evaluated once at program startup — this is what makes the plug-in system work without a central list of every command. The registry itself is implemented as a function-local static (Meyer's singleton) rather than a plain static class member, since a plain static member has no ordering guarantee relative to the static command objects registering into it across translation units, which caused a real, hard-to-diagnose startup crash during development.

<br>

## Known limitations

- No file extensions — names containing dots or slashes are rejected by the name validator.
- `touch`, `mkdir`, `rm`, and `rmf` only operate on a bare name in the *current* directory; they don't accept nested paths (`touch a/b` fails — use `cd a && touch b`). `chmod`, `info`, `cd`, `ls`, `tree`, `grep`, and `find` do accept full paths, since they route through path resolution.
- The permission model is single-owner only — no users or groups, since there's only ever one simulated user.
- Timestamps are stored as plain strings, which is fine for display but awkward to compare or sort by.
- Everything lives in memory for the process's lifetime; nothing persists across runs.

<br>

## Possible next steps

- Persist the filesystem to disk (serialize/deserialize on startup/shutdown)
- A tiny scripting layer to run `.simulos` script files with variables and loops
- Symbolic links
- A basic process/job model (`ps`, background jobs with `&`)
