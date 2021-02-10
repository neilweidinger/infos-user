#include <infos.h>

class Completion
{
    struct completion_candidate
    {
        char name[64];
        int start_of_completion;
    };

    public:
        Completion();
        int find_completions(char *cmd_buf, size_t *cmd_buf_n, const size_t cmd_buf_size);
        void print_completions(char *cmd_buf, size_t *cmd_buf_n, const size_t cmd_buf_size);

    private:
        const char* PATH = "/usr";
        completion_candidate candidates[20]; // length 49 or higher causes page fault, page fault still occasionally occurs at lower values too
        size_t candidates_index = 0;
};

namespace
{
    const char* find_filename(const char *cmd_buf, const size_t cmd_buf_n);
    int prefix_matches(const char *actual_fn, const char *potential_fn, const size_t potential_fn_len);
}
