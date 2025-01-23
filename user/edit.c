#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define MAXLINE 100  // Maximum line length
#define MAXLINES 100 // Maximum number of lines
#define CTRL(k) ((k) & 0x1F)

// Editor buffer
char lines[MAXLINES][MAXLINE];
int num_lines = 0;
int cur_line = 0;
int cur_col = 0;

// Mode flags
int insert_mode = 0;

void clear_screen() {
    printf("\033[2J");   // Clear screen
    printf("\033[H");    // Move cursor to home position
}

void display() {
    clear_screen();
    printf("-- EDITOR -- %s --\n", insert_mode ? "INSERT" : "NORMAL");
    
    // Display lines
    for(int i = 0; i < num_lines; i++) {
        if(i == cur_line)
            printf("-> ");
        else
            printf("   ");
        printf("%s\n", lines[i]);
    }
    
    // Display cursor position
    printf("\nLine: %d Col: %d\n", cur_line + 1, cur_col + 1);
}


void insert_char(char c) {
    if (c == '\n') { // Handle Enter key (newline)
        if (num_lines < MAXLINES - 1) {
            // Shift lines down from the current position
            for (int i = num_lines; i > cur_line; i--) {
                strncpy(lines[i], lines[i - 1], MAXLINE);
            }

            // Split the current line at the cursor
            strncpy(lines[cur_line + 1], &lines[cur_line][cur_col], MAXLINE - cur_col - 1);
            lines[cur_line + 1][MAXLINE - cur_col - 1] = '\0';

            lines[cur_line][cur_col] = '\0'; // Terminate current line at cursor

            // Move to the next line
            cur_line++;
            num_lines++;
            cur_col = 0;

            // Initialize the new line to ensure no leftover data
            memset(lines[cur_line], 0, MAXLINE);
        }
    } else {
        int len = strlen(lines[cur_line]);
        if (len < MAXLINE - 1 && cur_col <= len) {
            // Shift characters right to make space for the new character
            for (int i = len; i >= cur_col; i--) {
                lines[cur_line][i + 1] = lines[cur_line][i];
            }

            // Insert the new character
            lines[cur_line][cur_col] = c;
            cur_col++;
        }
    }
}


void delete_char() {
    int len = strlen(lines[cur_line]);
    if (cur_col > 0 && len > 0) {
        // Shift characters left to delete the character at the cursor
        for (int i = cur_col - 1; i < len; i++) {
            lines[cur_line][i] = lines[cur_line][i + 1];
        }
        cur_col--;
    } else if (cur_col == 0 && cur_line > 0) {
        // Merge the current line with the previous one
        cur_col = strlen(lines[cur_line - 1]);
        if (cur_col + strlen(lines[cur_line]) < MAXLINE) {
            mystrncat(lines[cur_line - 1], lines[cur_line], MAXLINE - cur_col - 1);

            // Shift lines up to remove the current line
            for (int i = cur_line; i < num_lines - 1; i++) {
                strncpy(lines[i], lines[i + 1], MAXLINE);
            }

            memset(lines[num_lines - 1], 0, MAXLINE);
            num_lines--;
            cur_line--;
        }
    }
}


void save_file(char* filename) {
    int fd = open(filename, O_CREATE | O_WRONLY);
    if (fd < 0) {
        printf("Error: Cannot open file for writing\n");
        return;
    }
    
    for (int i = 0; i < num_lines; i++) {
        if (write(fd, lines[i], strlen(lines[i])) < 0) {
            printf("Error: Failed to write to file\n");
            close(fd);
            return;
        }
        write(fd, "\n", 1);
    }
    
    close(fd);
    printf("File saved: %s\n", filename);
}

void load_file(char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        // New file
        num_lines = 1;
        memset(lines[0], 0, MAXLINE);
        return;
    }
    
    char buf[1];
    int i = 0, j = 0;
    
    while (read(fd, buf, 1) > 0) {
        if (buf[0] == '\n') {
            lines[i][j] = '\0';
            i++;
            j = 0;
            if (i >= MAXLINES) break;
        } else {
            if (j < MAXLINE - 1)
                lines[i][j++] = buf[0];
        }
    }
    
    if (j > 0) {
        lines[i][j] = '\0';
        i++;
    }
    
    num_lines = (i > 0) ? i : 1;
    close(fd);
}


int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Usage: edit filename\n");
        exit(1);
    }
    
    char* filename = argv[1];
    load_file(filename);
    
    char buf[1];
    clear_screen();
    printf("Key: %d\n", buf[0]);  // Debug the key value for insert mode
    
    while(1) {
        display();
        
        read(0, buf, 1);
        if(insert_mode) {
            
            switch(buf[0]) {
                case CTRL('x'):
                    // Exit
                    exit(0);
                case CTRL('s'):
                    // Save
                    save_file(filename);
                    break;
                case CTRL('i'):
                    // Toggle insert mode
                    insert_mode = 0;
                    break;
                case 127:  // Backspace
                    delete_char();
                    break;
                case '\n':  // Handle Enter key (newline)
                    insert_char(buf[0]);
                    break;
                default:
                    if(buf[0] >= 32 && buf[0] <= 126)  // Printable characters
                        insert_char(buf[0]);
                    break;
            }
        } else {
            switch(buf[0]) {
                case 'x':
                    exit(0);
                case 'i':
                    insert_mode = 1;
                    break;
                case 'h':
                    if(cur_col > 0) cur_col--;
                    break;
                case 'l':
                    if(cur_col < strlen(lines[cur_line])) cur_col++;
                    break;
                case 'j':
                    if(cur_line < num_lines - 1) cur_line++;
                    break;
                case 'k':
                    if(cur_line > 0) cur_line--;
                    break;
                case 's':
                    save_file(filename);
                    break;
            }
        }
    }
}