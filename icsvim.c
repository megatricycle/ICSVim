#include "../../sdk/dexsdk.h"
#include "helpers.h"

#define TERMINAL_WIDTH 80
#define BODY_HEIGHT 20
#define TILDE_WIDTH 2
#define DEFAULT_LINE_NUMBER_SPACE 4
#define CURSOR_X_MIN 4
#define CURSOR_X_MAX 4

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

void print_line(int line_number, char *text) {
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
    printf("%s", text);
    for(i = 0; i < TERMINAL_WIDTH - DEFAULT_LINE_NUMBER_SPACE - digit_count(line_number) + digit_count(line_number) - strlen(text) - 1; i++) {
        printf(" ");
    }
}

void print_empty_line() {
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

void render(int current_line_number, char *file_buffer) {
    int i, j;
    int number_of_lines;
    char *ptr;
    char file_buffer_cpy[1024];
    
    clrscr();
    
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
    
    // render top bar
    print_top_bar();
    
    // render valid lines
    for(i = 0; i < number_of_lines; i++) {
        print_line(i + 1, char_buffer[i]);
    }
    
    // render empty lines
    for(i = 0; i < BODY_HEIGHT - number_of_lines; i++) {
        print_empty_line();
    }
    
    char filename[] = "dummyfile.txt"; // hardcoded
    
    // render lower bar
    print_status_bar(filename, current_line_number);
    
    // render command line
    textcolor(WHITE);
    printf("This is currently a test mode. No features are available.");
    
    // empty space
}

int main() {
    int cursor_x = 5, cursor_y = 1;
    int user_input;
    char file_buffer[] = "Ignorance is your new bestfriend.\nParamore\nSome testing andddddd\nNew line!";
    
    render(1, file_buffer);
    
    while(1) {
        update_cursor(cursor_y, cursor_x);
        
        user_input = getch();
        
        // handle user input
        if(user_input == KEY_UP) {
            cursor_y--;
        }
        if(user_input == KEY_DOWN) {
            cursor_y++;
        }
        if(user_input == KEY_RIGHT) {
            cursor_x++;
        }
        if(user_input == KEY_LEFT) {
            cursor_x--;
        }
        
        textcolor(WHITE);
    }
    
    return 0;
}