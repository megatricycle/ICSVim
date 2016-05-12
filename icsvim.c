#include "../../sdk/dexsdk.h"
#include "helpers.h"

#define TERMINAL_WIDTH 80
#define BODY_HEIGHT 20
#define TILDE_WIDTH 2
#define DEFAULT_LINE_NUMBER_SPACE 4
#define CURSOR_X_MIN 4
#define CURSOR_X_MAX 4
#define CURSOR_Y_MIN 2

#define KEY_LEFT  -106
#define KEY_DOWN -104
#define KEY_UP  -105
#define KEY_RIGHT -103

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

void render(char char_buffer[][75], int number_of_lines, int cursor_x, int cursor_y) {
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
    printf("This is currently a test mode. No features are available.");
    
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

int main() {
    int cursor_x = 5, cursor_y = 1;
    int user_input;
    int number_of_lines;
    char file_buffer[] = "Ignorance is your new bestfriend.\nParamore\nSome testing andddddd\nNew line!";
    char file_buffer_cpy[1024];
    char *ptr;
    
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
        
        render(char_buffer, number_of_lines, cursor_x, cursor_y);
        
        textcolor(WHITE);
        
        update_cursor(cursor_y, cursor_x);
        
        user_input = getch();
        
        // handle user input
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
    }
    
    return 0;
}