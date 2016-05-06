/*
    helpers.h
    
    Contains common functions from standard C libraries that are not included in ICSOS.
*/


// https://en.wikibooks.org/wiki/C_Programming/C_Reference/stdlib.h/itoa
/* reverse:  reverse string s in place */
void reverse(char s[]) {
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

// https://en.wikibooks.org/wiki/C_Programming/C_Reference/stdlib.h/itoa
/* itoa:  convert n to characters in s */
void itoa(int n, char s[]) {
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
       n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

int digit_count(int x) {
    int count = 1;
    
    while(x > 9) {
        x /= 10;
        count++;
    }
    
    return count;
}