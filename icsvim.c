#include "../../sdk/dexsdk.h"
#include "helpers.h"

#define TERMINAL_WIDTH 80
#define BODY_HEIGHT 20
#define TILDE_WIDTH 2

void render() {
    int i, j;
    int current_line_number = 1; // hardcoded
    int max_line_number = 1; // hardcoded as of now
    
    clrscr();
    
    // render top bar
    for(i = 0; i < TERMINAL_WIDTH; i++) {
        textbackground(LIGHTGRAY);
        printf(" ");
    }
    
    // render valid lines
    for(i = 0; i < max_line_number; i++) {
        // @TODO: Make width dynamic
        
        textbackground(BLACK);
        // render line number
        for(j = 0; j < 3; j++) {
            textbackground(BLACK);
            printf(" ");
        }
        
        textcolor(YELLOW);
        printf("%i", i + 1);
        
        for(j = 0; j < TERMINAL_WIDTH - 4; j++) {
            textbackground(BLACK);
            textcolor(WHITE);
            printf(" ");
        }
    }
    
    // render empty lines
    for(i = 0; i < BODY_HEIGHT - max_line_number; i++) {
        textbackground(BLACK);
        textcolor(BLUE);
        printf(" ~");
        
        for(j = 0; j < TERMINAL_WIDTH - TILDE_WIDTH; j++) {
            textbackground(BLACK);
            textcolor(WHITE);
            printf(" ");
        }
    }
    
    // render lower bar
    textbackground(WHITE);
    
    char filename[] = "dummyfile.txt";
    char status_message[80];
    char current_line_number_buffer[30];
    
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