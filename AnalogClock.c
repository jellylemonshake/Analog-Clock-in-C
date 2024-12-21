#include <stdio.h>
#include <math.h>
#include <time.h>
#include <windows.h>
#include <string.h>

#define PI 3.14159265358979323846
#define CLOCK_RADIUS 10  // Reduced from 15
#define CLOCK_CENTER_X 40  // Adjusted center
#define CLOCK_CENTER_Y 12  // Reduced from 15
#define DISPLAY_WIDTH 82   // Fixed width for display
#define DISPLAY_HEIGHT 25  // Reduced height

// Previous helper functions remain the same
int validateTime(int hours, int minutes, int seconds) {
    if (hours < 0 || hours > 23) return 0;
    if (minutes < 0 || minutes > 59) return 0;
    if (seconds < 0 || seconds > 59) return 0;
    return 1;
}

void calculatePoint(int centerX, int centerY, double angle, double radius, int* x, int* y) {
    *x = centerX + (int)(radius * 2 * sin(angle));
    *y = centerY - (int)(radius * cos(angle));
}

char getHandChar(double angle, char handType) {
    while (angle < 0) angle += 2 * PI;
    while (angle > 2 * PI) angle -= 2 * PI;
    
    switch(handType) {
        case 'H': // Hour hand - thicker symbols
            if (angle < PI/4 || angle > 7*PI/4) return '┃';
            if (angle < 3*PI/4) return '┓';
            if (angle < 5*PI/4) return '━';
            if (angle < 7*PI/4) return '┏';
            return '┃';
            
        case 'M': // Minute hand - double lines
            if (angle < PI/4 || angle > 7*PI/4) return '║';
            if (angle < 3*PI/4) return '╗';
            if (angle < 5*PI/4) return '═';
            if (angle < 7*PI/4) return '╔';
            return '║';
            
        case 'S': // Second hand - thin lines
            if (angle < PI/4 || angle > 7*PI/4) return '│';
            if (angle < 3*PI/4) return '╱';
            if (angle < 5*PI/4) return '─';
            if (angle < 7*PI/4) return '╲';
            return '│';
    }
    return '│';
}

void drawHand(char display[DISPLAY_HEIGHT][DISPLAY_WIDTH], int centerX, int centerY, 
              double angle, double radius, char handType) {
    int endX, endY;
    calculatePoint(centerX, centerY, angle, radius, &endX, &endY);
    
    int dx = endX - centerX;
    int dy = endY - centerY;
    int steps = (int)(sqrt(dx*dx + dy*dy));
    
    for (int i = 0; i <= steps; i++) {
        int x = centerX + dx * i / steps;
        int y = centerY + dy * i / steps;
        if (x >= 0 && x < DISPLAY_WIDTH-1 && y >= 0 && y < DISPLAY_HEIGHT) {
            display[y][x] = getHandChar(angle, handType);
        }
    }
}

void clearScreen() {
    system("cls");
}

void placeNumber(char display[DISPLAY_HEIGHT][DISPLAY_WIDTH], int x, int y, int number) {
    if (x >= 0 && x < DISPLAY_WIDTH-1 && y >= 0 && y < DISPLAY_HEIGHT) {
        char numStr[3];
        sprintf(numStr, "%2d", number);
        display[y][x] = numStr[0];
        display[y][x + 1] = numStr[1];
    }
}

void drawClock(int hours, int minutes, int seconds) {
    char display[DISPLAY_HEIGHT][DISPLAY_WIDTH];
    
    // Initialize display with spaces
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            display[y][x] = ' ';
        }
        display[y][DISPLAY_WIDTH-1] = '\0';
    }
    
    // Draw clock circle
    for (double angle = 0; angle < 2*PI; angle += 0.1) {
        int x, y;
        calculatePoint(CLOCK_CENTER_X, CLOCK_CENTER_Y, angle, CLOCK_RADIUS, &x, &y);
        if (x >= 0 && x < DISPLAY_WIDTH-1 && y >= 0 && y < DISPLAY_HEIGHT) {
            display[y][x] = '░';
        }
    }
    
    // Place hour numbers
    for (int hour = 1; hour <= 12; hour++) {
        double angle = (hour % 12) * 30 * PI / 180;
        int x, y;
        calculatePoint(CLOCK_CENTER_X, CLOCK_CENTER_Y, angle, CLOCK_RADIUS + 1, &x, &y);
        placeNumber(display, x, y, hour);
    }

    // Calculate hand angles
    double hourAngle = ((hours % 12) + minutes / 60.0) * 30 * PI / 180;
    double minuteAngle = minutes * 6 * PI / 180;
    double secondAngle = seconds * 6 * PI / 180;
    
    // Draw hands with different lengths and styles
    // Hour hand (shortest)
    drawHand(display, CLOCK_CENTER_X, CLOCK_CENTER_Y, hourAngle, CLOCK_RADIUS * 0.4, 'H');
    
    // Minute hand (medium)
    drawHand(display, CLOCK_CENTER_X, CLOCK_CENTER_Y, minuteAngle, CLOCK_RADIUS * 0.7, 'M');
    
    // Second hand (longest)
    drawHand(display, CLOCK_CENTER_X, CLOCK_CENTER_Y, secondAngle, CLOCK_RADIUS * 0.9, 'S');
    
    // Draw center point
    display[CLOCK_CENTER_Y][CLOCK_CENTER_X] = '⊕';
    
    // Print the clock with fixed-width border
    printf("\033[H");
    
    // Top border
    printf("╔");
    for (int i = 0; i < DISPLAY_WIDTH-2; i++) printf("═");
    printf("╗\n");
    
    // Clock content with side borders
    for (int y = 0; y < DISPLAY_HEIGHT-1; y++) {
        printf("║");
        for (int x = 0; x < DISPLAY_WIDTH-2; x++) {
            putchar(display[y][x]);
        }
        printf("║\n");
    }
    
    // Bottom border
    printf("╚");
    for (int i = 0; i < DISPLAY_WIDTH-2; i++) printf("═");
    printf("╝\n");

    // Legend
    printf("\n  Digital time: %02d:%02d:%02d", hours, minutes, seconds);
    printf("\n  Press Ctrl+C to exit");
}

int main() {
    int hours, minutes, seconds;
    int validInput = 0;
    
    SetConsoleOutputCP(CP_UTF8);
    
    while (!validInput) {
        printf("╔");
        for (int i = 0; i < DISPLAY_WIDTH-2; i++) printf("═");
        printf("╗\n");
        
        printf("║%*s Time Input %*s║\n", 
               (DISPLAY_WIDTH-13)/2, "", 
               (DISPLAY_WIDTH-13)/2, "");
        
        printf("╚");
        for (int i = 0; i < DISPLAY_WIDTH-2; i++) printf("═");
        printf("╝\n");
        
        printf("Hours (0-23): ");
        scanf("%d", &hours);
        printf("Minutes (0-59): ");
        scanf("%d", &minutes);
        printf("Seconds (0-59): ");
        scanf("%d", &seconds);
        
        if (validateTime(hours, minutes, seconds)) {
            validInput = 1;
        } else {
            printf("\n Invalid time! Please enter valid values:\n");
            printf(" Hours: 0-23\n");
            printf(" Minutes: 0-59\n");
            printf(" Seconds: 0-59\n\n");
        }
    }
    
    printf("\e[?25l"); // Hide cursor
    
    while (1) {
        clearScreen();
        drawClock(hours, minutes, seconds);
        
        seconds++;
        if (seconds >= 60) {
            seconds = 0;
            minutes++;
            if (minutes >= 60) {
                minutes = 0;
                hours++;
                if (hours >= 24) {
                    hours = 0;
                }
            }
        }
        
        Sleep(1000);
    }
    
    printf("\e[?25h"); // Show cursor
    return 0;
}