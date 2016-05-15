#include "../../sdk/dexsdk.h"
#include "helpers.h"

#define TERMINAL_WIDTH 80
#define BODY_HEIGHT 20
#define TILDE_WIDTH 2
#define DEFAULT_LINE_NUMBER_SPACE 4
#define CURSOR_X_MIN 5
#define CURSOR_Y_MIN 1
#define COMMAND_LINE_NUMBER 22

#define KEY_HOME -110
#define KEY_END -109
#define KEY_LEFT  -106
#define KEY_DOWN -104
#define KEY_UP  -105
#define KEY_RIGHT -103
#define KEY_BACKSPACE 8
#define KEY_ENTER 10
#define KEY_ESC 27

typedef enum { NORMAL_MODE, COMMAND_MODE, INSERT_MODE, VISUAL_MODE } mode;

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
    else if(state == INSERT_MODE) {
        textcolor(YELLOW);
        printf("-- INSERT --");
        textcolor(WHITE);
    }
    else if(state == VISUAL_MODE) {
        textcolor(YELLOW);
        printf("-- VISUAL --");
        textcolor(WHITE);
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
    if(*cursor_x > CURSOR_X_MIN) {
        (*cursor_x)--;
    }
}

void cursor_right(int *cursor_x, int max) {
    if(*cursor_x < max + 1) {
        (*cursor_x)++;
    }
}

void cursor_home(int *cursor_x) {
    *cursor_x = CURSOR_X_MIN;
}

void cursor_end(int *cursor_x, int max) {
    *cursor_x = max + 1;
}

void set_state(mode *old_state, mode new_state) {
    *old_state = new_state;
}

void add_to_buffer_cursor(char *buffer, char input, int cursor_pos) {
    int len = strlen(buffer);
    int i;
    
    for(i = len; i >= cursor_pos; i--) {
        buffer[i + 1] = buffer[i];
    }
    
    buffer[cursor_pos] = input;
}

void remove_from_buffer_cursor(char *buffer, int cursor_pos) {
    int len = strlen(buffer);
    int i;
    
    if(cursor_pos > 0 && cursor_pos < strlen(buffer) + 1) {
        for(i = cursor_pos - 1; i < strlen(buffer); i++) {
            buffer[i] = buffer[i + 1];
        }
        
        buffer[len - 1] = '\0';
    }
}

void clear_buffer(char *buffer) {
    strcpy(buffer, "");
}

void reconstruct_file_buffer(char *file_buffer, char char_buffer[][75], int number_of_lines) {
    int i;
    
    clear_buffer(file_buffer);
    
    for(i = 0; i < number_of_lines; i++) {
        strcat(file_buffer, char_buffer[i]);
        
        if(i < number_of_lines - 1) {
            strcat(file_buffer, "\n");
        }
    }
}

void backspace_insert_buffer(char *file_buffer, char char_buffer[][75], int number_of_lines, int cursor_x, int cursor_y) {
    remove_from_buffer_cursor(char_buffer[cursor_y - 1], cursor_x);
    
    reconstruct_file_buffer(file_buffer, char_buffer, number_of_lines);
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

void add_to_buffer(char *buffer, char input) {
    char temp[2];
    
    charToString(temp, input);
    
    strcat(buffer, temp);
}

void process_input_buffer(char *input_buffer, mode *state, int *cursor_x, int *cursor_y, int right_max, char *file_buffer, char char_buffer[][75], int number_of_lines) {
    int i;
    char modifier_buffer[256] = "";
    char command_buffer[256] = "";
    
    for(i = 0; i < strlen(input_buffer); i++) {
        // if number
        if(input_buffer[i] >= '0' && input_buffer[i] <= '9') {
            add_to_buffer(modifier_buffer, input_buffer[i]);
        }
        else if(input_buffer[i] >= 32 && input_buffer[i] <= 126) {
            add_to_buffer(command_buffer, input_buffer[i]);
            
            // process command buffer
            if(strcmp(command_buffer, "i") == 0) {
                *state = INSERT_MODE;
                clear_buffer(input_buffer);
            }
            else if(strcmp(command_buffer, "v") == 0) {
                *state = VISUAL_MODE;
                clear_buffer(input_buffer);
            }
            else if(strcmp(command_buffer, "a") == 0) {
                cursor_right(cursor_x, right_max);
                *state = INSERT_MODE;
                clear_buffer(input_buffer);
            }
            else if(strcmp(command_buffer, "x") == 0) {
                for(i = 0; i < atoi(modifier_buffer); i++) {
                    backspace_insert_buffer(file_buffer, char_buffer, number_of_lines, *cursor_x - DEFAULT_LINE_NUMBER_SPACE, *cursor_y);
                }
                clear_buffer(input_buffer);
            }
            else {
                clear_buffer(input_buffer);
            }
        }
    }
}

void add_to_file_buffer(char *file_buffer, char char_buffer[][75], int number_of_lines, int cursor_x, int cursor_y, char user_input) {
    int i;
    
    add_to_buffer_cursor(char_buffer[cursor_y - 1], user_input, cursor_x - DEFAULT_LINE_NUMBER_SPACE - 1);
    
    reconstruct_file_buffer(file_buffer, char_buffer, number_of_lines);
}

void construct_char_buffer(char *file_buffer, char char_buffer[][75]) {
    char file_buffer_cpy[1024];
    char *ptr;
    
    strcpy(file_buffer_cpy, file_buffer);
        
    int line = 0;
    
    ptr = strtok(file_buffer_cpy, "\n");
    
    while(ptr != NULL) {
        strcpy(char_buffer[line], ptr);
        
        line++;
        
        ptr = strtok(NULL, "\n");
    }
}

int main() {
    int cursor_x = 5, cursor_y = 1;
    int user_input;
    int number_of_lines;
    char file_buffer[1024] = "This is a sample file.\nConfused.";
    char input_buffer[256] = "";
    char command_buffer[256];
    int command_cursor;
    char status_buffer[256] = "";
    
    mode state = NORMAL_MODE;
    
    while(1) {
        // @TODO: functionalize
        number_of_lines = get_number_of_lines(file_buffer);
    
        char char_buffer[number_of_lines][TERMINAL_WIDTH - DEFAULT_LINE_NUMBER_SPACE - 1];
        
        construct_char_buffer(file_buffer, char_buffer);
        
        render(char_buffer, number_of_lines, cursor_x, cursor_y, state, command_buffer, status_buffer);
        
        textcolor(WHITE);
        
        // set cursor 
        if(state == NORMAL_MODE || state == INSERT_MODE || state == VISUAL_MODE) {
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
            else if(user_input == KEY_DOWN) {
                cursor_down(&cursor_y, number_of_lines, char_buffer, &cursor_x);
            }
            else if(user_input == KEY_RIGHT) {
                cursor_right(&cursor_x, strlen(char_buffer[cursor_y - 1]) + DEFAULT_LINE_NUMBER_SPACE);
            }
            else if(user_input == KEY_LEFT) {
                if(cursor_x > CURSOR_X_MIN) {
                    cursor_left(&cursor_x);
                }
                else if(cursor_y > CURSOR_Y_MIN){
                    cursor_up(&cursor_y, char_buffer, &cursor_x);
                    cursor_end(&cursor_x, strlen(char_buffer[cursor_y - 1]) + DEFAULT_LINE_NUMBER_SPACE);
                }
            }
            else if(user_input == KEY_HOME) {
                cursor_home(&cursor_x);
            }
            else if(user_input == KEY_END) {
                cursor_end(&cursor_x, strlen(char_buffer[cursor_y - 1]) + DEFAULT_LINE_NUMBER_SPACE);
            }
            else if(user_input == KEY_BACKSPACE) {
                if(cursor_x > CURSOR_X_MIN) {
                    cursor_left(&cursor_x);
                }
                else if(cursor_y > CURSOR_Y_MIN){
                    cursor_up(&cursor_y, char_buffer, &cursor_x);
                    cursor_end(&cursor_x, strlen(char_buffer[cursor_y - 1]) + DEFAULT_LINE_NUMBER_SPACE);
                }
            }
            else if(user_input == ':') {
                set_state(&state, COMMAND_MODE);
                command_cursor = 0;
                
                // clear status buffer
                clear_buffer(status_buffer);
            }
            else if(user_input >= 32 && user_input <= 126) {
                add_to_buffer(input_buffer, user_input);
                process_input_buffer(input_buffer, &state, &cursor_x, &cursor_y, strlen(char_buffer[cursor_y - 1]) + DEFAULT_LINE_NUMBER_SPACE, file_buffer, char_buffer, number_of_lines);
            }
            else if(user_input == KEY_ESC){
                clear_buffer(input_buffer);
            }
        }
        else if(state == COMMAND_MODE) {
            // if printable character
            if(user_input >= 32 && user_input <= 126) {
                add_to_buffer_cursor(command_buffer, user_input, command_cursor);
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
                clear_buffer(command_buffer);
            }
            else if(user_input == KEY_ESC) {
                clear_buffer(command_buffer);
                set_state(&state, NORMAL_MODE);
            }
        }
        else if(state == INSERT_MODE) {
            if(user_input == KEY_UP) {
                cursor_up(&cursor_y, char_buffer, &cursor_x);
            }
            else if(user_input == KEY_DOWN) {
                cursor_down(&cursor_y, number_of_lines, char_buffer, &cursor_x);
            }
            else if(user_input == KEY_RIGHT) {
                cursor_right(&cursor_x, strlen(char_buffer[cursor_y - 1]) + DEFAULT_LINE_NUMBER_SPACE);
            }
            else if(user_input == KEY_LEFT) {
                if(cursor_x > CURSOR_X_MIN) {
                    cursor_left(&cursor_x);
                }
                else if(cursor_y > CURSOR_Y_MIN){
                    cursor_up(&cursor_y, char_buffer, &cursor_x);
                    cursor_end(&cursor_x, strlen(char_buffer[cursor_y - 1]) + DEFAULT_LINE_NUMBER_SPACE);
                }
            }
            else if(user_input == KEY_HOME) {
                cursor_home(&cursor_x);
            }
            else if(user_input == KEY_END) {
                cursor_end(&cursor_x, strlen(char_buffer[cursor_y - 1]) + DEFAULT_LINE_NUMBER_SPACE);
            }
            else if(user_input == KEY_ESC) {
                set_state(&state, NORMAL_MODE);
            }
            else if(user_input == KEY_BACKSPACE) {
                backspace_insert_buffer(file_buffer, char_buffer, number_of_lines, cursor_x - DEFAULT_LINE_NUMBER_SPACE - 1, cursor_y);
                cursor_left(&cursor_x);
            }
            else if(user_input >= 32 && user_input <= 126) {
                add_to_file_buffer(file_buffer, char_buffer, number_of_lines, cursor_x, cursor_y, user_input);
                cursor_right(&cursor_x, strlen(char_buffer[cursor_y - 1]) + DEFAULT_LINE_NUMBER_SPACE);
            }
            else if(user_input == KEY_ENTER) {
                add_to_file_buffer(file_buffer, char_buffer, number_of_lines, cursor_x, cursor_y, '\n');
                cursor_down(&cursor_y, number_of_lines, char_buffer, &cursor_x);
            }
        }
        else if(state == VISUAL_MODE) {
            if(user_input == KEY_UP) {
                cursor_up(&cursor_y, char_buffer, &cursor_x);
            }
            else if(user_input == KEY_DOWN) {
                cursor_down(&cursor_y, number_of_lines, char_buffer, &cursor_x);
            }
            else if(user_input == KEY_RIGHT) {
                cursor_right(&cursor_x, strlen(char_buffer[cursor_y - 1]) + DEFAULT_LINE_NUMBER_SPACE);
            }
            else if(user_input == KEY_LEFT) {
                if(cursor_x > CURSOR_X_MIN) {
                    cursor_left(&cursor_x);
                }
                else if(cursor_y > CURSOR_Y_MIN){
                    cursor_up(&cursor_y, char_buffer, &cursor_x);
                    cursor_end(&cursor_x, strlen(char_buffer[cursor_y - 1]) + DEFAULT_LINE_NUMBER_SPACE);
                }
            }
            else if(user_input == KEY_HOME) {
                cursor_home(&cursor_x);
            }
            else if(user_input == KEY_END) {
                cursor_end(&cursor_x, strlen(char_buffer[cursor_y - 1]) + DEFAULT_LINE_NUMBER_SPACE);
            }
            else if(user_input == KEY_ESC) {
                set_state(&state, NORMAL_MODE);
            }
        }
    }
    
    return 0;
}