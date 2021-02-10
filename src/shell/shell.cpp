/* SPDX-License-Identifier: MIT */

#include <infos.h>
#include <completion.h>

namespace
{
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

    int tab_completion(char *cmd_buf, size_t *cmd_buf_n, const size_t cmd_buf_size)
    {
        Completion comp;

        if (comp.find_completions(cmd_buf, cmd_buf_n, cmd_buf_size) == -1) {
            return -1;
        }
        comp.print_completions(cmd_buf, cmd_buf_n, cmd_buf_size);

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
		}

		printf("\n");
		if (n == 0) continue;
		
		command_buffer[n] = '\0';
		
		if (strcmp("exit", command_buffer) == 0) break;
		run_command(command_buffer);
	}
	
	return 0;
}
