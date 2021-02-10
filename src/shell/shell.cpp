/* SPDX-License-Identifier: MIT */

#include <infos.h>

namespace
{
    const char *PATH = "/usr";

    struct completion_candidate
    {
        char name[64];
        int start_of_completion;
    };

    void run_command(const char *cmd)
    {
        //printf("Running Command: %s\n", cmd);
        
        char prog[64];
        int n = 0;
        while (*cmd && *cmd != ' ' && n < 63) {
            prog[n++] = *cmd++;
        }
        
        prog[n] = 0;
        
        if (*cmd) cmd++;
        
        HPROC pcmd = exec(prog, cmd);
        if (is_error(pcmd)) {
            printf("error: unable to run command '%s'\n", cmd);
        } else {
            wait_proc(pcmd);
        }
    }

    // Returns the beginning of the potential filename in cmd_buf.
    // put examples, explain params, etc.
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

    // Assumes actual_fn is null terminated.
    // Returns -1 if potential_fn not a prefix of actual_fn, otherwise returns index of first non-matching
    // character in actual_fn (beginning of completion).
    int prefix_matches(const char *actual_fn, const char *potential_fn, const size_t potential_fn_len)
    {
        size_t i = 0;

        while (*actual_fn && i < potential_fn_len && *actual_fn == potential_fn[i]) {
            actual_fn++;
            i++;
        }

        return i == potential_fn_len ? static_cast<int>(i) : -1; // very unlikely to overflow
    }

    int tab_completion(char *cmd_buf, size_t *cmd_buf_n, const size_t cmd_buf_size)
    {
        HDIR dir = opendir(PATH, 0);
        if (is_error(dir)) {
            printf("Unable to open directory '%s' for reading.\n", PATH);
            return -1;
        }

        completion_candidate candidates[20]; // length 49 or higher causes page fault, page fault still occasionally occurs at lower values too
        size_t candidates_index = 0;
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

        if (candidates_index == 1) {
            auto only_candidate = candidates[candidates_index - 1];

            // Need to allocate a null terminated buffer since printf doesn't support limiting number of bytes to be written
            const size_t suffix_size = 64;
            char completion_suffix[suffix_size]; 
            auto cmd_buf_remaining = cmd_buf_size - *cmd_buf_n;
            /* printf("cmd_buf_remaining: %u\n", cmd_buf_remaining); */
            /* printf("only_candidate.name: %s\n", only_candidate.name); */
            /* printf("only_candidate.start_of_completion: %d\n", only_candidate.start_of_completion); */
            strlcpy(completion_suffix, only_candidate.name + only_candidate.start_of_completion, min(cmd_buf_remaining, suffix_size));

            printf("%s", completion_suffix);
            *cmd_buf_n += strlcpy(cmd_buf + *cmd_buf_n, completion_suffix, cmd_buf_remaining);
            printf("\ncmd_buf: %s\n", cmd_buf);
        }
        else if (candidates_index > 1) {
            printf("More candidates!\n");
        }

        return 0;
    }
}


// also implement ctrl-g and stuff


int main(const char *cmdline)
{
	printf("This is the InfOS shell.  Path resolution is not-yet-implemented, so you\n"
			"must type the command exactly, e.g. try typing: /usr/ls.\n\n");
	
	printf("Use the cat program to view the README: /usr/cat /usr/docs/README\n\n");
	
	while (true) {
		printf("> ");

        const size_t buf_size = 128;
		char command_buffer[buf_size];
		size_t n = 0;
        printf("\nn: %u\n", n);

		while (n < buf_size - 1) {
			char c = getch();

			if (c == 0) continue;
            else if (c == '\n') break;
            else if (c == '\b') {
				if (n > 0) {
					command_buffer[--n] = 0;
					printf("\b");
				}
			}
            else if (c == '\t') {
                tab_completion(command_buffer, &n, buf_size);
            }
            else {
				command_buffer[n++] = c;
				printf("%c", c);
			}

            printf("\nn: %u\n", n);
		}

		printf("\n");
		if (n == 0) continue;
		
		command_buffer[n] = '\0';
		
		if (strcmp("exit", command_buffer) == 0) break;
		run_command(command_buffer);
	}
	
	return 0;
}
