#include <completion.h>

Completion::Completion() {}

int Completion::find_completions(char *cmd_buf, size_t *cmd_buf_n, const size_t cmd_buf_size)
{
    HDIR dir = opendir(PATH, 0);
    if (is_error(dir)) {
        printf("Unable to open directory '%s' for reading.\n", PATH);
        return -1;
    }

    struct dirent de;
    const char* potential_filename = find_filename(cmd_buf, *cmd_buf_n);

    while (readdir(dir, &de)) {
        auto start_of_completion = prefix_matches(de.name, potential_filename, (cmd_buf + *cmd_buf_n) - potential_filename);

        if (start_of_completion != -1) {
            strlcpy(candidates[candidates_index].name, de.name, sizeof(candidates[0].name));
            candidates[candidates_index].start_of_completion = start_of_completion;
            candidates_index++;
        }
    }

    closedir(dir);
    return 0;
}

void Completion::print_completions(char *cmd_buf, size_t *cmd_buf_n, const size_t cmd_buf_size)
{
    if (candidates_index == 1) {
        auto only_candidate = candidates[candidates_index - 1];

        // Need to allocate a null terminated buffer since printf doesn't support limiting number of bytes to be written
        const size_t suffix_size = 64;
        char completion_suffix[suffix_size];
        auto cmd_buf_remaining = cmd_buf_size - *cmd_buf_n;
        strlcpy(completion_suffix, only_candidate.name + only_candidate.start_of_completion, min(cmd_buf_remaining, suffix_size));

        printf("%s", completion_suffix);
        *cmd_buf_n += strlcpy(cmd_buf + *cmd_buf_n, completion_suffix, cmd_buf_remaining);
    }
    else if (candidates_index > 1) {
        printf("More candidates!\n");
    }
}

namespace
{
    const char* find_filename(const char *cmd_buf, const size_t cmd_buf_n)
    {
        if (cmd_buf_n == 0) {
            return cmd_buf;
        }

        size_t i = cmd_buf_n;
        i--; // decrement because cmd_buf_n is index of *next* write, not most recent write

        while (i > 0 && cmd_buf[i - 1] != '/') {
            i--;
        }

        return cmd_buf + i;
    }

    int prefix_matches(const char *actual_fn, const char *potential_fn, const size_t potential_fn_len)
    {
        size_t i = 0;

        while (*actual_fn && i < potential_fn_len && *actual_fn == potential_fn[i]) {
            actual_fn++;
            i++;
        }

        return i == potential_fn_len ? static_cast<int>(i) : -1; // very unlikely to overflow
    }
}
