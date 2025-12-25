# Isocline: a portable readline alternative

Isocline is a pure C line-editing and terminal-formatting library that you can drop into CLI programs as a modern replacement for [GNU readline]. It speaks a minimal subset of ANSI escape sequences, works out of the box on Linux, macOS, and Windows, and keeps dependencies at zero so you can vendor it anywhere.

Originally authored by Daan Leijen for the [Koka] compiler, this fork keeps the MIT license while modernizing the internals for contemporary shells, REPLs, and scripting hosts.

> **2025 rewrite.** Caden Finley rewrote the majority of Isocline for CJ's Shell, expanding the codebase to roughly 15k lines of production C while keeping the public API stable. Expect faster redraws, richer completion hooks, better diagnostics, and more color/TUI helpers than the 2022 v1.0.9 release.

## Highlights

- **Feature-rich line editing:** multi-line mode (`Shift+Tab`), undo/redo, brace matching, inline hints, syntax highlighting, completion menus, and incremental history search are built in.
- **Battle-tested portability:** runs on Unix, Windows Console, and Windows Terminal via either ANSI sequences or the console API, with graceful fallbacks on "dumb" terminals and custom allocator hooks.
- **Single translation unit friendly:** the entire ~15k LOC library can still be compiled via the amalgamated `src/isocline.c`, or as separate objects by defining `IC_SEPARATE_OBJS`.
- **High fidelity colors & formatting:** `ic_print*` exposes [bbcode]-style markup with 24-bit color, ANSI palette fallback, style definitions, and automatic tag balancing inspired by [Rich].
- **Language-friendly:** stays pure ISO C so it links cleanly from C and C++ (bindings for other languages are welcome).

## Project Snapshot

- **Language:** ISO C99 (GNU extensions guarded), no C++ runtime required.
- **Size:** ~15k lines across 20+ translation units or one amalgamated file.
- **Platforms:** Linux, macOS, Windows (legacy console & Windows Terminal).
- **License:** MIT.
- **Bindings:** C and C++ ready; additional community bindings encouraged.
- **Use cases:** REPLs, language shells, build/test consoles, and diagnostics tools that need readline-class UX.

## Demo

A showcase terminal session runs through unicode input, syntax highlighting, brace matching, jump-to-match, auto indent, multi-line editing, 24-bit colors, inline hinting, filename completion, and incremental history search (previous captures were produced with [termtosvg]). A refreshed recording will land in this repository once the new UI stabilizes.

## Quick Start

Include the public header:

```c
#include "include/isocline.h"
```

and call `ic_readline` to obtain user input with rich editing:

```c
char* input;
while ((input = ic_readline("prompt> ")) != NULL) {  // ctrl+d/c return tokens
  printf("you typed:\n%s\n", input);
  free(input);
}
```

See the full [example] for completions, history, hints, syntax highlighting, and custom allocators.

### Run the bundled example

```
$ gcc -o example -Iinclude test/example.c src/isocline.c
$ ./example
```

## Build Options

### Vendoring or single translation unit

Copy `include/` and `src/` into your project or add Isocline as a [submodule]. Compile the amalgamated file directly (no configuration needed):

```
$ gcc -c -std=c99 -Iinclude src/isocline.c
```

Define `IC_SEPARATE_OBJS` if you prefer building each translation unit separately.

### Build with CMake

```
$ git clone https://github.com/daanx/isocline
$ cd isocline
$ mkdir -p build/release
$ cd build/release
$ cmake ../..
$ cmake --build .
$ ./example
```

This produces `libisocline.a`/`isocline.lib` alongside the sample binary.

## Editing Experience

Isocline mirrors familiar [GNU readline] key bindings while adding multi-line editing, brace tools, inline hints, syntax highlighting, and advanced completion menus. Press `F1` during a prompt to display the built-in cheat sheet.

### Key bindings at a glance

| Navigation | Action |
|------------|--------|
| `left`, `Ctrl+B` | Move one character left |
| `right`, `Ctrl+F` | Move one character right |
| `up` | Previous history entry or visual row up |
| `down` | Next history entry or visual row down |
| `Ctrl+Left` | Jump to start of previous word |
| `Ctrl+Right` | Jump to end of current word |
| `Home`, `Ctrl+A` | Start of the current line |
| `End`, `Ctrl+E` | End of the current line |
| `PgUp`, `Ctrl+Home` | Top of the current input |
| `PgDn`, `Ctrl+End` | Bottom of the current input |
| `Alt+M` | Jump to matching brace |
| `Ctrl+P` / `Ctrl+N` | Back / forward in history |
| `Ctrl+R` / `Ctrl+S` | Incremental history search |

| Deletion | Action |
|----------|--------|
| `Del`, `Ctrl+D` | Delete the character under the cursor |
| `Backspace`, `Ctrl+H` | Delete the character before the cursor |
| `Ctrl+W` | Delete to preceding whitespace |
| `Alt+Backspace` | Delete to the start of the word |
| `Alt+D` | Delete to the end of the word |
| `Ctrl+U` | Delete to the start of the line |
| `Ctrl+K` | Delete to the end of the line |
| `Esc` | Clear the current buffer / exit when empty |

| Editing | Action |
|---------|--------|
| `Enter` | Accept input |
| `Ctrl+Enter`, `Ctrl+J`, `Shift+Tab` | Insert a newline (multi-line mode) |
| `Ctrl+L` | Clear the screen |
| `Ctrl+T` | Swap with the previous character |
| `Ctrl+Z`, `Ctrl+_` | Undo |
| `Ctrl+Y` | Redo |
| `Tab` | Trigger completion |

| Completion menu | Action |
|-----------------|--------|
| `Enter`, `Left` | Accept the highlighted completion |
| `1`–`9` | Pick completion N directly |
| `Tab`, `Down` | Next completion |
| `Shift+Tab`, `Up` | Previous completion |
| `PgDn`, `Ctrl+Enter`, `Ctrl+J` | Show all completions |
| `Esc` | Exit the menu |

| Incremental history search | Action |
|----------------------------|--------|
| `Enter` | Use the highlighted entry |
| `Backspace`, `Ctrl+Z` | Step back (undo) |
| `Tab`, `Ctrl+R`, `Up` | Next match |
| `Shift+Tab`, `Ctrl+S`, `Down` | Previous match |
| `Esc` | Exit search |

> On macOS, enable "Use Option as Meta key" in Terminal/iTerm2 preferences to access `Alt+` bindings.

## Completion, hints, and highlighting

The completion API accepts context objects so you can register multiple completers (filesystem, keywords, custom commands) and combine them dynamically. Inline hints, right-aligned prompts, and syntax highlighting hooks share the same tokenizer so completions stay in sync with what the user sees. Undo/redo, brace matching, and incremental search operate across multiple visual lines for complex REPL inputs. History persists across sessions with pluggable storage.

## Structured terminal output

Beyond `ic_readline`, Isocline exposes `ic_print`, `ic_println`, and `ic_printf` for rich terminal output. Inspired by [Rich] and [RichBBcode], you can style messages with nested [bbcode]-style tags:

```c
ic_println("[b]bold [red]and red[/red][/b]");
ic_println("[warning]this is a warning![/]");
ic_style_open("warning");
ic_println("[b]crimson underlined and bold[/]");
ic_style_close();
```

Custom styles are trivial:

```c
ic_style_def("warning", "crimson u");
ic_println("[warning]watch out![/]");
```

The `[!tag]...[/tag]` syntax preserves literal text without interpreting markup.

## Advanced Topics

### BBCode format

Open tags accept whitespace-separated entries that are either style names or primitive `property=value` pairs. Built-in styles include `b`, `u`, `i`, `r`, plus syntax-highlighting shorthands such as `keyword`, `string`, `comment`, `number`, `type`, `constant`, and UI styles like `ic-prompt`, `ic-info`, `ic-diminish`, `ic-emphasis`, `ic-hint`, `ic-error`, and `ic-bracematch`.

Boolean properties (`bold`, `italic`, `underline`, `reverse`) default to `on`. Color properties accept HTML [color names][htmlcolors], ANSI [color names][ansicolors], hex codes (`#rrggbb` / `#rgb`), or entries from the ANSI 256 [palette][ansicolor256] via `ansi-color=`_idx_ / `ansi-bgcolor=`_idx_. Use `color=`, `bgcolor=`, `on color`, or the shorthand `color` token to set foreground/background.

Width helpers:

- `width=WIDTH[;align[;fill]]` pads text to at least `WIDTH` columns with alignment `left|center|right`.
- `maxwidth=WIDTH[;align]` constrains text to at most `WIDTH`, inserting ellipses on the trimmed side.

### Environment variables

- `NO_COLOR`: disable colors entirely.
- `CLICOLOR=1`: enable automatic filename coloring via `LSCOLORS` / `LS_COLORS`.
- `COLORTERM=truecolor|256color|16color|8color|monochrome`: force a specific palette.
- `TERM`: consulted on some platforms to detect capabilities.

### Colors

Isocline detects 24-bit color support automatically and remaps to 256/16/8-color palettes when needed. Test your terminal with `test/test_colors.c`:

```
$ gcc -o test_colors -Iinclude test/test_colors.c src/isocline.c
$ ./test_colors
$ COLORTERM=truecolor ./test_colors
$ COLORTERM=16color ./test_colors
```

### ANSI escape sequences

Only widely supported sequences are used:

- Cursor movement: `ESC[nA`, `ESC[nB`, `ESC[nC`, `ESC[nD`.
- Clearing: `ESC[K`.
- Colors: `ESC[nm` (0 reset, 1/22 bold, 3/23 italic, 4/24 underline, 7/27 reverse, 30–37/40–47/90–97/100–107 colors, 39/49 default).
- Extended colors: `ESC[38;5;nm`, `ESC[48;5;nm`, `ESC[38;2;r;g;bm`, `ESC[48;2;r;g;bm`.

Windows builds use the Console API when ANSI sequences are unavailable.

### Async and threads

Isocline is not thread-safe; call `ic_readline*` and `ic_print*` from a single thread. To integrate with async loops, run `ic_readline` inside a dedicated blocking thread and relay results. Use:

```c
bool ic_async_stop(void);
```

from other threads to interrupt an active `ic_readline`, simulating `Ctrl+C`.

### Color mapping

When mapping RGB colors to ANSI palettes, Isocline minimizes perceptual color distance using a red-mean metric (with gray correction) instead of naive sRGB or CIEDE2000, striking a balance between accuracy and predictability:

(Older documentation contained a color-space comparison chart; updated captures will ship with the refreshed documentation.)

The top row in that chart displayed the target 24-bit color; lower rows showed the approximations across multiple strategies.

## API Reference

- Browse the generated [C API reference][docapi] and the local [example](test/example.c) for history, completion, highlighting, and printing patterns.

## Motivation & Related Work

Isocline was created for the [Koka] interactive compiler: requirements included pure C, zero external dependencies, portable unicode support, BSD/MIT licensing, and capable multi-line completion. Other excellent libraries include [GNU readline], [editline](https://github.com/troglobit/editline), [linenoise](https://github.com/antirez/linenoise), [replxx](https://github.com/AmokHuginnsson/replxx), and [Haskeline](https://github.com/judah/haskeline).

## Roadmap

- Vi-style key bindings.
- Shared kill/yank buffer across prompts.
- Thread-safe `ic_print*`.
- Extended low-level terminal helpers.
- Status/progress bars and prompt variants (confirmations, choices, etc.).

Reach out if you want to help with any of these items.

## Releases

- `2025`: trunk rewrite by Caden Finley — new completion pipeline, diagnostics, and ~15k LOC core (current `main`).
- `2022-01-15`: v1.0.9 — fix missing `ic_completion_arg` (issue #6), null-pointer check in `ic_print` (issue #7), crash when `/dev/null` is both input and output.
- `2021-09-05`: v1.0.5 — custom `wcwidth` for consistency; thanks to Hans-Georg Breunig for NetBSD testing.
- `2021-08-28`: v1.0.4 — fix color query on Ubuntu/GNOME.
- `2021-08-27`: v1.0.3 — fix duplicates in completions.
- `2021-08-23`: v1.0.2 — fix Windows EOL wrapping.
- `2021-08-21`: v1.0.1 — fix line buffering.
- `2021-08-20`: v1.0.0 — initial release.

## Credits

- **Daan Leijen** — original author and design.
- **Caden Finley** — 2025 rewrite/maintenance for CJ's Shell and community adopters.

[GNU readline]: https://tiswww.case.edu/php/chet/readline/rltop.html
[Koka]: http://www.koka-lang.org
[submodule]: https://git-scm.com/book/en/v2/Git-Tools-Submodules
[example]: test/example.c
[termtosvg]: https://github.com/nbedos/termtosvg
[Rich]: https://github.com/willmcgugan/rich
[RichBBcode]: https://rich.readthedocs.io/en/latest/markup.html
[bbcode]: https://en.wikipedia.org/wiki/BBCode
[htmlcolors]: https://en.wikipedia.org/wiki/Web_colors#HTML_color_names
[ansicolors]: https://en.wikipedia.org/wiki/Web_colors#Basic_colors
[ansicolor256]: https://en.wikipedia.org/wiki/ANSI_escape_code#8-bit
[docapi]: https://daanx.github.io/isocline
