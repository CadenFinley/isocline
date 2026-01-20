# Isocline (CJSH Fork)

Isocline is a pure C line-editing and terminal-formatting library designed as a modern drop-in replacement for GNU readline. This fork keeps the upstream MIT license and zero-dependency ethos while expanding the feature surface for CJ's Shell (cjsh), language REPLs, and custom CLI hosts that want richer UX without pulling in external runtimes.

The code lives in `src/isocline/` and is distributed together with `include/isocline.h` so it can be vendored directly or built as a static library.

## What changed compared to upstream?

- **Multiline editor upgrades** – Relative/absolute line numbers, prompt-gutter replacement, configurable preallocated rows (`ic_set_multiline_start_line_count`), current-line highlighting, and visible whitespace markers (`ic_enable_visible_whitespace`).
- **Prompt cleanup pipeline** – `ic_enable_prompt_cleanup*` lets embedders rewrite or clear prompts after accepting a line, collapse multiline submissions, and add spacer rows for recording-friendly shells.
- **Abbreviations & widgets** – Fish-style abbreviations (`ic_add_abbreviation`, `ic_remove_abbreviation`, `ic_clear_abbreviations`) expand inline without breaking undo history. `ic_read_heredoc`, buffer accessors, and widget helpers (`ic_set_buffer`, `ic_get_buffer`, `ic_push_key_event`, `ic_set_unhandled_key_handler`) unlock advanced key-driven workflows.
- **Completion diagnostics** – Extended completion API supports previews, inline hints, help text, and source attribution (`ic_add_completion_ex_with_source`). Spell correction, auto-tab expansion, preview suppression, and menu pagination are all toggleable at runtime.
- **Key binding profiles** – Emacs and Vi profiles ship side-by-side, backed by inspectable bindings (`ic_list_key_bindings`, `ic_set_key_binding_profile`, `ic_bind_key_named`).
- **Structured output** – BBCode-inspired markup (`ic_print`, `ic_printf`, `ic_style_def`, custom palettes) now mirrors cjsh prompt styling, complete with named UI tokens such as `ic-hint`, `ic-error`, `ic-linenumbers`, and `ic-bracematch`.
- **Portability polish** – Smarter terminal detection, better Windows VT compatibility, configurable escape-sequence delays, and improved ANSI/color fallbacks keep the library usable in "dumb" terminals, tmux/screen splits, and Windows Terminal.
- **Async friendliness** – Typeahead buffers, status message callbacks (`ic_set_status_message_callback`), and cooperative interruption (`ic_async_stop`) make embedding in event loops straightforward.

All of these additions are backward compatible: leave the new toggles untouched and the editor behaves like classic isocline.

## Quick start

Add the header to your compilation units and call `ic_readline`:

```c
#include "isocline.h"

static void configure_editor(void) {
  ic_enable_multiline(true);
  ic_enable_multiline_indent(true);
  ic_enable_line_numbers(true);
  ic_enable_current_line_number_highlight(true);
  ic_enable_completion_preview(true);
  ic_enable_hint(true);
  ic_set_hint_delay(60);               // ms
  ic_enable_visible_whitespace(false); // opt-in markers
  ic_add_abbreviation("abbr", "abbreviate");
}

int main(void) {
  configure_editor();

  char* line = NULL;
  while ((line = ic_readline("cjsh> ", NULL, NULL)) != NULL) {
    if (strcmp(line, "exit") == 0) {
      free(line);
      break;
    }
    ic_println("[ic-info]you typed:[/]");
    ic_println(line);
    free(line);
  }
  return 0;
}
```

### Single translation unit build

Compile the amalgamated source when vendoring:

```bash
gcc -std=c99 -Iinclude -c src/isocline.c
```

Define `IC_SEPARATE_OBJS` to build translation units individually (recommended for faster incremental builds inside cjsh).

### CMake

The CJSH root `CMakeLists.txt` already exposes the library. To build it standalone:

```bash
git clone https://github.com/CadenFinley/CJsShell
cd CJsShell
cmake -S . -B build/isocline -DISOCLINE_ONLY=ON   # optional helper preset
cmake --build build/isocline -j$(nproc)
```

The build outputs `libisocline.a` (or `.lib` on Windows) plus the sample program in `test/example.c`.

## Editor capabilities

### Multiline & navigation
- Automatic continuation detection (quotes, braces, heredocs, trailing `\`).
- Absolute or relative line numbers with current-line highlighting.
- Line-number gutter can replace the final prompt line for banner-heavy PS1 setups.
- Configurable preallocated rows so multi-line prompts can render headers before editing begins.
- Optional visible whitespace markers and gutter styling via `ic_style_def`.
- Jump-to-match (`Alt+M`), history browsing, undo/redo stack, and familiar GNU readline bindings.

### Completion, hints, and history
- Context objects allow layered completers (filesystem + keywords + domain-specific commands).
- Completion menu highlights, source labels, numeric shortcuts, and preview pane toggles.
- Inline hints respect hint delays and respond to right-arrow acceptance.
- Spell correction and fuzzy matching improve typo handling; auto-tab grows unique prefixes automatically.
- Persistent history tracks timestamps and exit codes, with incremental search and duplicate suppression.

### Abbreviations, widgets, and automation
- Register fish-style abbreviations at runtime; they expand when followed by whitespace or submission.
- `ic_read_heredoc` reuses the full editor stack for heredoc bodies.
- Widget helpers (`ic_set_buffer`, `ic_get_buffer`, `ic_get_cursor_pos`, `ic_request_submit`) power custom bindings exposed through cjsh's `cjsh-widget` builtin.
- Typeahead APIs (`ic_push_key_event`, `ic_push_key_sequence`, `ic_push_raw_input`) queue keystrokes captured while external commands run, enabling seamless buffered input once the prompt reappears.

### Prompt cleanup & structured output
- Prompt cleanup toggles make REPL transcripts tidy: delete previous prompts, insert spacer lines, or collapse multiline submissions before printing output.
- BBCode markup works for prompts and general output: `[b]`, `[warning]`, `[color=#ff79c6]`, `width=`, `maxwidth=`, and named styles (`ic-info`, `ic-hint`, `ic-error`, etc.).
- Custom style definitions let embedders keep markup semantic while swapping themes at runtime.

### Terminal & portability
- Minimal ANSI subset for broad compatibility, with automated 24-bit, 256-color, or 16-color fallbacks.
- Windows console support via either native Console APIs or virtual terminal sequences when available.
- Configurable escape-sequence delays ensure lone `ESC` handling still works over slow SSH or serial links.

## Configuration surface (selected APIs)

| Function | Purpose |
| --- | --- |
| `ic_enable_multiline`, `ic_enable_multiline_indent` | Toggle multiline editing and smart indentation |
| `ic_enable_line_numbers`, `ic_enable_relative_line_numbers` | Show absolute/relative gutters |
| `ic_enable_line_numbers_with_continuation_prompt`, `ic_enable_line_number_prompt_replacement` | Keep gutters visible alongside PS2 or swap them with the final prompt line |
| `ic_enable_current_line_number_highlight` | Style the active line number differently |
| `ic_enable_visible_whitespace`, `ic_set_whitespace_marker` | Display custom whitespace markers |
| `ic_enable_prompt_cleanup`, `ic_enable_prompt_cleanup_empty_line`, `ic_enable_prompt_cleanup_truncate_multiline` | Control how prompts are rewritten after submission |
| `ic_enable_completion_preview`, `ic_enable_auto_tab`, `ic_enable_hint`, `ic_set_hint_delay`, `ic_enable_spell_correct` | Tune completions and inline hints |
| `ic_set_default_completer`, `ic_add_completion*` | Register layered completers with display/help/source metadata |
| `ic_set_default_highlighter`, `ic_highlight` | Provide syntax highlighting callbacks |
| `ic_bind_key`, `ic_set_key_binding_profile`, `ic_list_key_binding_profiles` | Inspect or override keymaps |
| `ic_add_abbreviation`, `ic_remove_abbreviation`, `ic_clear_abbreviations` | Manage fish-style abbreviations |
| `ic_set_status_message_callback` | Surface transient status banners under the prompt |
| `ic_async_stop` | Interrupt a running `ic_readline` from another thread |

Consult `include/isocline.h` for the full API, including completion transformers, history helpers, and terminal utilities.

## Documentation

CJSH's docs double as reference material for this fork:

- `docs/reference/editing.md` – exhaustive walkthrough of editing, hints, abbreviations, and key binding controls.
- `docs/reference/completions.md` – authoring guide for completion sources and cache formats.
- `docs/reference/non-posix-features.md` – high-level overview of interactive extras powered by isocline.

## Release timeline

- **2025 (current `main`)** – Extensive rewrite by Caden Finley for CJ's Shell: new completion pipeline, diagnostics, prompt cleanup, fish-style abbreviations, enhanced keymaps, and ~15k LOC refactor.
- **2022-01-15 (v1.0.9)** – Upstream: completion arg fixes, `ic_print` safety, `/dev/null` crash guard.
- **2021-09-05 (v1.0.5)** – Upstream: custom `wcwidth` for consistent Unicode widths.
- **2021-08-28 (v1.0.4)** – Upstream: color query fixes on Ubuntu/GNOME.
- **2021-08-27 (v1.0.3)** – Upstream: duplicate completions fix.
- **2021-08-23 (v1.0.2)** – Upstream: Windows EOL wrapping fix.
- **2021-08-21 (v1.0.1)** – Upstream: line buffering fix.
- **2021-08-20 (v1.0.0)** – Initial public release by Daan Leijen.

## Credits

- **Daan Leijen** – Original author and design of isocline.
- **Caden Finley** – Maintainer of the CJSH fork and 2025 rewrite.
- **CJSH contributors** – Bug fixes, completion sources, and documentation.

Isocline remains MIT-licensed. Vendoring and contributions are welcome; submit pull requests through the CJ's Shell repository or open issues describing desired editor features.
