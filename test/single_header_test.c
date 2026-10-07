#include "isocline.h"
#include "isocline.h"  /* Public declarations must also tolerate repeated includes. */

#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "line %d: %s failed\n", __LINE__, #condition); \
        return 1; \
    } \
} while (0)

int main(int argc, char** argv) {
    CHECK(argc == 2);
    CHECK(freopen(argv[1], "r", stdin) != NULL);

    const char* copy = ic_strdup("single header");
    CHECK(copy != NULL && strcmp(copy, "single header") == 0);
    ic_free((void*)copy);
    CHECK(ic_next_char("\xC3\xA9x", 0) == 2);
    CHECK(ic_prev_char("\xC3\xA9x", 2) == 0);
    CHECK(ic_char_is_digit("7", 1));

    ic_keycode_t key = 0;
    ic_key_action_t action = IC_KEY_ACTION_NONE;
    CHECK(ic_parse_key_spec("ctrl+a", &key));
    CHECK(ic_bind_key(key, IC_KEY_ACTION_UNDO));
    CHECK(ic_get_key_binding(key, &action) && action == IC_KEY_ACTION_UNDO);
    CHECK(ic_set_key_binding_profile("vim"));
    CHECK(strcmp(ic_get_key_binding_profile(), "vim") == 0);
    CHECK(ic_set_key_binding_profile("emacs"));

    CHECK(ic_add_abbreviation("abbr", "expanded"));
    CHECK(ic_remove_abbreviation("abbr"));
    CHECK(!ic_remove_abbreviation("abbr"));

    ic_enable_typeahead(true);
    CHECK(ic_typeahead_is_enabled());
    ic_typeahead_clear();
    ic_enable_typeahead(false);
    CHECK(!ic_typeahead_is_enabled());

    ic_set_history(NULL, 10);
    ic_history_add("single header");
    ic_history_remove_last();
    ic_history_clear();

    ic_readline_result_t result = ic_readline_with_status(NULL, NULL, NULL);
    CHECK(result.disposition == IC_READLINE_DISPOSITION_SUBMIT);
    CHECK(result.input != NULL && strcmp(result.input, "single header") == 0);
    CHECK(result.cursor_pos == strlen(result.input));
    ic_free(result.input);

    char* line = ic_readline(NULL, NULL, NULL);
    CHECK(line != NULL && strcmp(line, "another line") == 0);
    ic_free(line);

    result = ic_readline_with_status(NULL, NULL, NULL);
    CHECK(result.disposition == IC_READLINE_DISPOSITION_EOF);
    CHECK(result.input != NULL && strcmp(result.input, IC_READLINE_TOKEN_CTRL_D) == 0);
    ic_free(result.input);
    return 0;
}
