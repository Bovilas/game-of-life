// John Conway's Game Of Life 2023-01-08 version (or 1.1)
// Author: Povilas Daugėla
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h> // *need to do unit test
#include "conway_header.h" // defines save and load functions
//#include <assert.h>

#define VK_A 0x41
#define VK_S 0x53
#define VK_W 0x57
#define VK_D 0x44
#define VK_Q 0x51
#define VK_C 0x43
#define VK_E 0x45

struct xAndY // for storing cursor pos
{
    int x;
    int y;
};

int enterParameter(char xOrY) // not very foolproof way to enter input
{
    int number;
    char ch;

    printf("Enter new value for %c: ", xOrY);

    while (scanf("%d", &number) != 1)
    {
        while (((ch = getchar()) != '\n') && (ch != EOF));

        printf("Invalid input. Please enter a valid number: ");
    }

    while ((ch = getchar()) != '\n' && ch != EOF);

    return number;
}

int submenu(int *x, int *y)
{
    int choice;

    int tempX, tempY;

    while (1)
    {
        printf("Edit field size:\n");
        printf("x = %d\n", *x);
        printf("y = %d\n", *y);
        printf("1. Edit x\n");
        printf("2. Edit y\n");
        printf("3. Quit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            system("cls");
            do
            {
                tempX = enterParameter('x');

                if(tempX < 0)
                {
                    printf("Invalid input. Number cannot be less than 0. \n");
                }
            }
            while(tempX < 0);

            *x = tempX;
            system("cls");
            break;

        case 2:
            system("cls");
            tempY = enterParameter('y');
            *y = tempY;
            system("cls");
            break;

        case 3:
            system("cls");
            return 0;

        default:
            printf("Invalid choice.\n");
            break;
        }
    }

    system("cls");
}

int menu(int *x, int *y)
{
    int choice;

    while (1)
    {
        printf("        John Conway's Game Of Life\n");
        printf("------------------------------------------\n\n");
        printf("Main Menu\n");
        printf("1. Play \n");
        printf("2. Load save \n");
        printf("3. Set playing field size\n");
        printf("4. Quit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            return 1;
            break;

        case 2:
            return 2;
            break;

        case 3:
            system("cls");
            submenu(x, y);
            break;

        case 4:
            return 0;

        default:
            printf("Invalid choice.\n");
            break;
        }
    }
}

void printRules()
{
    printf("                  Rules\n");
    printf("------------------------------------------\n");
    printf("For a space that is populated:\n");
    printf("Each cell with one or no neighbors dies, as if by solitude.\n");
    printf("Each cell with four or more neighbors dies, as if by overpopulation.\n");
    printf("Each cell with two or three neighbors survives.\n\n");
    printf("For a space that is empty or unpopulated:\n");
    printf("Each cell with three neighbors becomes populated.\n");
}

void hidecursor()
{
   HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 100;
   info.bVisible = FALSE;
   SetConsoleCursorInfo(consoleHandle, &info);
}

int **init_field(int x, int y) // initializes field
{
    int** field = malloc(x * sizeof(int*));
    for (int i = 0; i < x; i++)
    {
        field[i] = malloc(y * sizeof(int));
    }

    if(field == NULL)
    {
        writeLog("Problem allocating memory.");
    }

    return field;
}

void printTop(int x, int rotation) // prints top of frame
{
    for(int i = 0; i < x + 2; ++i)
    {
        if(i == 0)
        {
            printf("%c", 201);
        }
        else if(i == x + 1)
        {
            printf("%c", 187);
            printf("      Generation number: %d", rotation);
        }
        else
        {
            printf("%c", 205);
            printf("%c", 205);
        }
    }
    printf("\n");
}

void printBot(int x) // prints bottom of frame
{
    for(int i = 0; i < x + 2; ++i)
    {
        if(i == 0)
        {
            printf("%c", 200);
        }
        else if(i == x + 1)
        {
            printf("%c", 188);
        }
        else
        {
            printf("%c", 205);
            printf("%c", 205);
        }
    }

    printf("\n");
    printf("Use wasd to navigate the grid\n");
    printf("Press c to populate or kill a cell\n");
    printf("Press q to advance one generation\n");
    printf("Press e to save and exit\n\n");
}

void writeLog(char *logMsg) // writes log message to log.txt file
{
    FILE *logp = fopen("log.txt", "a");

    time_t rawtime;
    struct tm *info;
    char buffer[80];

    time(&rawtime);

    info = localtime(&rawtime);

    strftime(buffer, 80, "%x - %I:%M%p", info);

    fprintf(logp, "|%s| ", buffer);

    fprintf(logp, "%s \n", logMsg);

    fclose(logp);
}

void writeExitLog()
{
    FILE *logp = fopen("log.txt", "a");

    time_t rawtime;
    struct tm *info;
    char buffer[80];

    time(&rawtime);
    info = localtime(&rawtime);

    clock_t endt = clock();
    int elapsed_secs = (int)(endt) / CLOCKS_PER_SEC;

    strftime(buffer, 80, "%x - %I:%M%p", info);

    fprintf(logp, "|%s| ", buffer);

    fprintf(logp, "%s ", "Exit log: ");

    fprintf(logp, "Game ran for %d minutes and %d seconds \n", elapsed_secs / 60, elapsed_secs - (elapsed_secs / 60));

    fclose(logp);
}

void zero2DArray(int **array, int x, int y)
{
    for (int i = 0; i < y; ++i)
    {
        for (int j = 0; j < x; ++j)
        {
            array[i][j] = 0;
        }
    }
}

int main()
{
    int fieldX = 10; // default field size
    int fieldY = 10;
    int **field;

    struct xAndY cursor;

    cursor.x = 0;
    cursor.y = 0;

    int runTwice = 0;

    int rotation = 1;

    int saveFlag = 0;

    int gameLoop = 1;
    int doGeneration = 1;
    int firstBoot = 1;

    int cellCount;

    //assert((field = readSave(&fieldX, &fieldY, &rotation)) != NULL); unit test for save reading func

    writeLog("Program is loaded.");

    gameLoop = menu(&fieldX, &fieldY);

    atexit(writeExitLog);

    if(gameLoop != 2)
    {
        field = init_field(fieldX, fieldY);

        zero2DArray(field, fieldX, fieldY);
    }

    int **tempField = init_field(fieldX, fieldY);

    zero2DArray(tempField, fieldX, fieldY);

    if(gameLoop == 2) // open save file
    {
        writeLog("Player has chosen to load save.");

        field = readSave(&fieldX, &fieldY, &rotation);

        if(field == NULL)
        {
            writeLog("Error reading field from save");
        }

        doGeneration = 0;

        --gameLoop;
    }

    while(gameLoop == 1)
    {
        if(firstBoot == 0)
        {
            system("cls");
        }

        hidecursor();

        printTop(fieldX, rotation);

        if(fieldY == 0)
        {
            printf("Congratulations!\n");
        }

        // prints middle

        for(int k = 0; k < fieldY; ++k)
        {
            printf("%c", 186);

            if(fieldX == 0)
            {
                printf("Congratulations!"); // no point, i could just not let it input that but i think it's funny
            }

            for(int j = 0; j < fieldX; ++j) // prints cell status
            {
                if(k == cursor.y && j == cursor.x)
                {
                    printf("X ");
                }
                else
                {
                    printf("%d ", field[k][j]);
                }

                cellCount = 0; // counts neighboring cells

                for(int i = k - 1; i <= k + 1; ++i)
                {
                    for(int h = j - 1; h <= j + 1; ++h)
                    {
                        if((i == k && h == j) || (i < 0 || h < 0) || (i >= fieldX || h >= fieldY))
                        {
                            continue;
                        }
                        if(field[i][h] == 1)
                        {
                            cellCount++;
                        }
                    }
                }

                if(doGeneration == 1) //logic
                {
                    if(field[k][j] == 1 && cellCount <= 1) // dies due to solitude :(
                    {
                        tempField[k][j] = 0;
                    }
                    else if(field[k][j] == 1 && cellCount >= 4) // dies due to overpopulation :/
                    {
                        tempField[k][j] = 0;
                    }
                    else if(field[k][j] == 0 && cellCount == 3) // a new cell is born :o
                    {
                        tempField[k][j] = 1;
                    }
                    else if(field[k][j] == 1 && cellCount == 3)
                    {
                        tempField[k][j] = 1;
                    }
                    else if(field[k][j] == 1 && cellCount == 2)
                    {
                        tempField[k][j] = 1;
                    }
                }
            }
            if(k <= fieldY - 1) //cosmetic
            {
                printf("%c", 186);
            }
            if(k != fieldY - 1)
            {
                printf("\n");
            }
        }

        printf("\n");

        printBot(fieldX);

        printRules();

        if(doGeneration == 1) // this is so that i don't change the cells while the next generation is being counted
        {
            for(int k = 0; k < fieldY; ++k)
            {
                for(int j = 0; j < fieldX; ++j)
                {
                    field[k][j] = tempField[k][j];
                }
            }
        }

        doGeneration = 0;

        if(firstBoot == 0)
        {
            if(runTwice == 0)
            {
                while(1) // keyboard inputs
                {
                    if(GetAsyncKeyState(VK_W))
                    {
                        if(cursor.y > 0)
                        {
                            --cursor.y;
                            Sleep(150);
                            doGeneration = 0; // so that it doesn't generate while you move the X
                        }
                        break;
                    }
                    else if(GetAsyncKeyState(VK_A))
                    {

                        if(cursor.x > 0)
                        {
                            --cursor.x;
                            Sleep(150);
                            doGeneration = 0;
                        }
                        break;
                    }
                    else if(GetAsyncKeyState(VK_S))
                    {
                        if(cursor.y < fieldY - 1)
                        {
                            ++cursor.y;
                            Sleep(150);
                            doGeneration = 0;
                        }
                        break;
                    }
                    else if(GetAsyncKeyState(VK_D))
                    {
                        if(cursor.x < fieldX - 1)
                        {
                            ++cursor.x;
                            Sleep(150);
                            doGeneration = 0;
                        }
                        break;
                    }
                    else if(GetAsyncKeyState(VK_Q))
                    {
                        doGeneration = 1;
                        Sleep(150);
                        ++rotation;
                        runTwice = 2; // don't ask
                        break;
                    }
                    if(GetAsyncKeyState(VK_E))
                    {
                        saveFlag = 1;
                        break;
                    }
                    else if(GetAsyncKeyState(VK_C))
                    {
                        if(field[cursor.y][cursor.x] == 1)
                        {
                            field[cursor.y][cursor.x] = 0;
                        }
                        else
                        {
                            field[cursor.y][cursor.x] = 1;
                        }

                        Sleep(500);
                        break;
                    }
                }
            }
        }

        if(runTwice > 0) // fixes bug
        {
            --runTwice;
        }

        firstBoot = 0;

        if(saveFlag == 1)
        {
            break;
        }
    }

    if(saveFlag == 1)
    {
        writeLog("Game progress is saved.");

        writeSave(field, fieldX, fieldY, rotation);
    }

    free(field);
    free(tempField);

    writeLog("Program has returned 0.");

    return 0;
}
