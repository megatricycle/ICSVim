#include "../../sdk/dexsdk.h"
#include "helpers.h"

#define TERMINAL_WIDTH 80
#define BODY_HEIGHT 20
#define TILDE_WIDTH 2

void print_top_bar() {
    int i;
    
    for(i = 0; i < TERMINAL_WIDTH; i++) {
        textbackground(LIGHTGRAY);
        printf(" ");
    }
}

void print_line(int line_number) {
    // @TODO: Make width dynamic
    int i;
        
    textbackground(BLACK);
    // render line number
    for(i = 0; i < 3; i++) {
        textbackground(BLACK);
        printf(" ");
    }
    
    textcolor(YELLOW);
    printf("%i", line_number);
    
    for(i = 0; i < TERMINAL_WIDTH - 4; i++) {
        textbackground(BLACK);
        textcolor(WHITE);
        printf(" ");
    }
}

void print_empty_line() {
    int i;
    
    textbackground(BLACK);
    textcolor(BLUE);
    printf(" ~");
    
    for(i = 0; i < TERMINAL_WIDTH - TILDE_WIDTH; i++) {
        textbackground(BLACK);
        textcolor(WHITE);
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

void render() {
    int i, j;
    int current_line_number = 1; // hardcoded
    int max_line_number = 1; // hardcoded as of now
    
    clrscr();
    
    // render top bar
    print_top_bar();
    
    // render valid lines
    for(i = 0; i < max_line_number; i++) {
        print_line(i + 1);
    }
    
    // render empty lines
    for(i = 0; i < BODY_HEIGHT - max_line_number; i++) {
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
    int i, j;
    
    while(1) {
        render();
    
        update_cursor(1, 5);
        
        getch();
        
        textcolor(WHITE);
    }
    
    return 0;
}