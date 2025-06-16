//conway_heacer.c

#include <stdio.h>
#include <stdlib.h>

int** readSave(int *x, int *y, int *rotation)
{
    FILE *sp = fopen("save.bin", "rb");
    if(!sp)
    {
        fprintf(stderr, "Error opening file 'save.bin'\n");
        return NULL;
    }

    if(fread(rotation, sizeof(*rotation), 1, sp) != 1 || fread(x, sizeof(*x), 1, sp) != 1 || fread(y, sizeof(*y), 1, sp) != 1)
    {
        fprintf(stderr, "Error reading from file 'save.bin'\n");
        fclose(sp);
        return NULL;
    }

    int** field = (int**) malloc(*x * sizeof(int*)); // allocate mem
    if(!field)
    {
        fprintf(stderr, "Error allocating memory for array\n");
        fclose(sp);
        return NULL;
    }
    for (int i = 0; i < *x; ++i)
    {
        field[i] = (int*) malloc(*y * sizeof(int));
        if(!field[i])
        {
            fprintf(stderr, "Error allocating memory for array\n");
            fclose(sp);
            return NULL;
        }
    }

    for(int k = 0;k < *x; ++k)
    {
        for(int j = 0; j < *y; ++j)
        {
            if(fread(&field[k][j], sizeof(field[k][j]), 1, sp) != 1)
            {
                fprintf(stderr, "Error reading from file 'save.bin'\n");
                fclose(sp);
                return NULL;
            }
        }
    }

    if(fclose(sp) != 0)
    {
        fprintf(stderr, "Error closing file 'save.bin'\n");
        return NULL;
    }

    return field;
}

void writeSave(int** field, int x, int y, int rotation)
{
    FILE *sp = fopen("save.bin", "wb");

    if(!sp)
    {
        fprintf(stderr, "Error opening file 'save.bin'\n");

        return;
    }

    if(fwrite(&rotation, sizeof(rotation), 1, sp) != 1 || fwrite(&x, sizeof(x), 1, sp) != 1 || fwrite(&y, sizeof(y), 1, sp) != 1)
    {
        fprintf(stderr, "Error writing to file 'save.bin'\n");

        fclose(sp);
        return;
    }

    for(int k = 0; k < x; ++k)
    {
        for(int j = 0; j < y; ++j)
        {
            if(fwrite(&field[k][j], sizeof(field[k][j]), 1, sp) != 1)
            {
                fprintf(stderr, "Error writing to file 'save.bin'\n");
                fclose(sp);
                return;
            }
        }
    }

    if(fclose(sp) != 0)
    {
        fprintf(stderr, "Error closing file 'save.bin'\n");
        return;
    }
}
