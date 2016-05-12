#include "../../sdk/dexsdk.h"
#include "helpers.h"

#define TERMINAL_WIDTH 80
#define BODY_HEIGHT 20
#define TILDE_WIDTH 2
#define DEFAULT_LINE_NUMBER_SPACE 4
#define CURSOR_X_MIN 4
#define CURSOR_X_MAX 4
#define CURSOR_Y_MIN 2
#define COMMAND_LINE_NUMBER 22

#define KEY_LEFT  -106
#define KEY_DOWN -104
#define KEY_UP  -105
#define KEY_RIGHT -103
#define KEY_BACKSPACE 8
#define KEY_ENTER 10

typedef enum { NORMAL_MODE, COMMAND_MODE } mode;

void print_top_bar() {
    int i;
    
    textbackground(LIGHTGRAY);
    for(i = 0; i < TERMINAL_WIDTH; i++) {
        printf(" ");
    }
}

void print_line(int line_number, char *text, int cursor_x, int cursor_y) {
    int i;
        
    textbackground(BLACK);
    // render line number
    for(i = 0; i < DEFAULT_LINE_NUMBER_SPACE - digit_count(line_number); i++) {
        printf(" ");
    }
    
    textcolor(YELLOW);
    printf("%i ", line_number);
    
    textbackground(BLACK);
    textcolor(WHITE);
    for(i = 0; i < strlen(text); i++) {
        // render cursor if applicable
        if(line_number == cursor_y && (i + 5) == cursor_x) {
            textbackground(WHITE);
            textcolor(BLACK);
            printf("%c", text[i]);
            textbackground(BLACK);
            textcolor(WHITE);
        }
        else {
            printf("%c", text[i]);
        }
    }
    for(i = 0; i < TERMINAL_WIDTH - DEFAULT_LINE_NUMBER_SPACE - digit_count(line_number) + digit_count(line_number) - strlen(text) - 1; i++) {
        // render cursor if applicable
        if(line_number == cursor_y && (i + 5 + strlen(text)) == cursor_x) {
            textbackground(WHITE);
            textcolor(BLACK);
            printf(" ");
            textbackground(BLACK);
            textcolor(WHITE);
        }
        else {
            printf(" ");
        }
    }
}

void print_empty_line(int line_number, int cursor_x, int cursor_y) {
    int i;
    
    textbackground(BLACK);
    textcolor(BLUE);
    printf(" ~");
    
    textbackground(BLACK);
    textcolor(WHITE);
    for(i = 0; i < TERMINAL_WIDTH - TILDE_WIDTH; i++) {
        printf(" ");
    }
}

void print_status_bar(char *filename, int current_line_number) {
    int i;
    
    char status_message[80];
    char current_line_number_buffer[30];
    
    textbackground(WHITE);
    
    // print status message
    textcolor(BLACK);
    strcpy(status_message, " [");
    strcat(status_message, filename);
    strcat(status_message, "]  Line: ");
    itoa(current_line_number, current_line_number_buffer);
    strcat(status_message, current_line_number_buffer);
    printf("%s", status_message);
    
    for(i = 0; i < TERMINAL_WIDTH - strlen(status_message); i++) {
        printf(" ");
    }
    textbackground(BLACK);
    printf(" ");
}

int get_number_of_lines(char *file_buffer) {
    char file_buffer_cpy[1024];
    char *ptr;
    int number_of_lines = 0;
    
    // get number of lines    
    strcpy(file_buffer_cpy, file_buffer);
    
    ptr = strtok(file_buffer_cpy, "\n");
    
    while(ptr != NULL) {
        number_of_lines++;
        
        ptr = strtok(NULL, "\n");
    }
    
    return number_of_lines;
}

void render(char char_buffer[][75], int number_of_lines, int cursor_x, int cursor_y, int state, char *command_buffer, char *status_buffer) {
    int i, j;
    
    clrscr();
    
    // render top bar
    print_top_bar();
    
    // render valid lines
    for(i = 0; i < number_of_lines; i++) {
        print_line(i + 1, char_buffer[i], cursor_x, cursor_y);
    }
    
    // render empty lines
    for(i = 0; i < BODY_HEIGHT - number_of_lines; i++) {
        print_empty_line(i + 1, cursor_x, cursor_y);
    }
    
    char filename[] = "dummyfile.txt"; // hardcoded
    
    // render lower bar
    print_status_bar(filename, cursor_y);
    
    // render command line
    textcolor(WHITE);
    if(state == COMMAND_MODE) {
        char command_text[256] = ":";
        
        strcat(command_text, command_buffer);
        
        printf("%s", command_text);
    }
    else if(state == NORMAL_MODE) {
        printf("%s", status_buffer);
    }
    
    // empty space
}

void cursor_up(int *cursor_y, char char_buffer[][75], int *cursor_x) {
    if(*cursor_y > 1) {
        (*cursor_y)--;
        
        // handle case when there's no text directly above
        if(*cursor_x > strlen(char_buffer[*cursor_y - 1])) {
            *cursor_x = strlen(char_buffer[*cursor_y - 1]) + DEFAULT_LINE_NUMBER_SPACE + 1;
        }
    }
}

void cursor_down(int *cursor_y, int number_of_lines, char char_buffer[][75], int *cursor_x) {
    if(*cursor_y < number_of_lines) {
        (*cursor_y)++;
        
        // handle case when there's no text directly below
        if(*cursor_x > strlen(char_buffer[*cursor_y - 1])) {
            *cursor_x = strlen(char_buffer[*cursor_y - 1]) + DEFAULT_LINE_NUMBER_SPACE + 1;
        }
    }
}

void cursor_left(int *cursor_x) {
    if(*cursor_x > 5) {
        (*cursor_x)--;
    }
}

void cursor_right(int *cursor_x, int max) {
    if(*cursor_x < max + 1) {
        (*cursor_x)++;
    }
}

void set_state(mode *old_state, mode new_state) {
    *old_state = new_state;
}

void add_to_command_buffer(char *command_buffer, char input, int command_cursor) {
    int len = strlen(command_buffer);
    int i;
    
    for(i = len - 1; i >= command_cursor; i--) {
        command_buffer[i + 1] = command_buffer[i];
    }
    
    command_buffer[command_cursor] = input;
    command_buffer[len + 1] = '\0';
}

void backspace_command_buffer(char *command_buffer, int command_cursor) {
    int len = strlen(command_buffer);
    int i;
    
    if(command_cursor > 0) {
        for(i = command_cursor - 1; i < strlen(command_buffer); i++) {
            command_buffer[i] = command_buffer[i + 1];
        }
        
        command_buffer[len - 1] = '\0';
    }
}

void command_cursor_left(int *command_cursor) {
    if(*command_cursor > 0) {
        (*command_cursor)--;
    }
}

void command_cursor_right(int *command_cursor, char *command_buffer) {
    if(*command_cursor < strlen(command_buffer)) {
        (*command_cursor)++;
    }
}

void set_status(char *status_buffer, char *new_status) {
    strcpy(status_buffer, new_status);
}

int main() {
    int cursor_x = 5, cursor_y = 1;
    int user_input;
    int number_of_lines;
    char file_buffer[] = "Ignorance is your new bestfriend.\nParamore\nSome testing andddddd\nNew line!";   
    char file_buffer_cpy[1024];
    char *ptr;
    char command_buffer[256];
    int command_cursor;
    char status_buffer[256] = "";
    
    mode state = NORMAL_MODE;
    
    while(1) {
        // @TODO: functionalize
        number_of_lines = get_number_of_lines(file_buffer);
    
        char char_buffer[number_of_lines][TERMINAL_WIDTH - DEFAULT_LINE_NUMBER_SPACE - 1];
        
        // save to char buffer
        strcpy(file_buffer_cpy, file_buffer);
        
        int line = 0;
        
        ptr = strtok(file_buffer_cpy, "\n");
        
        while(ptr != NULL) {
            strcpy(char_buffer[line], ptr);
            
            line++;
            
            ptr = strtok(NULL, "\n");
        }
        // @END
        
        render(char_buffer, number_of_lines, cursor_x, cursor_y, state, command_buffer, status_buffer);
        
        textcolor(WHITE);
        
        // set cursor 
        if(state == NORMAL_MODE) {
            update_cursor(cursor_y, cursor_x);
        }
        else if(state == COMMAND_MODE) {
            update_cursor(COMMAND_LINE_NUMBER, command_cursor + 2);
        }
        
        user_input = getch();
        
        // handle user input
        if(state == NORMAL_MODE) {
            if(user_input == KEY_UP) {
                cursor_up(&cursor_y, char_buffer, &cursor_x);
            }
            if(user_input == KEY_DOWN) {
                cursor_down(&cursor_y, number_of_lines, char_buffer, &cursor_x);
            }
            if(user_input == KEY_RIGHT) {
                cursor_right(&cursor_x, strlen(char_buffer[cursor_y - 1]) + DEFAULT_LINE_NUMBER_SPACE);
            }
            if(user_input == KEY_LEFT) {
                cursor_left(&cursor_x);
            }
            
            if(user_input == ':') {
                set_state(&state, COMMAND_MODE);
                command_cursor = 0;
                
                // clear status buffer
                strcpy(status_buffer, "");
            }
        }
        else if(state == COMMAND_MODE) {
            // if printable character
            if(user_input >= 32 && user_input <= 126) {
                add_to_command_buffer(command_buffer, user_input, command_cursor);
                command_cursor_right(&command_cursor, command_buffer);
            }
            else if(user_input == KEY_BACKSPACE) {
                backspace_command_buffer(command_buffer, command_cursor);
                command_cursor_left(&command_cursor);
            }
            else if(user_input == KEY_LEFT) {
                command_cursor_left(&command_cursor);
            }
            else if(user_input == KEY_RIGHT) {
                command_cursor_right(&command_cursor, command_buffer);
            }
            else if(user_input == KEY_ENTER) {
                // process command buffer
                if(strcmp(command_buffer, "q") == 0) {
                    clrscr();
                    exit(0);
                }
                else {
                    set_state(&state, NORMAL_MODE);
                    set_status(status_buffer, "Not an editor command.");
                }
                
                // clear command buffer
                strcpy(command_buffer, "");
             }
        }
    }
    
    return 0;
}