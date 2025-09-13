# include <stdio.h>
# include <stdbool.h>
# include <ctype.h>
# include <string.h>
#include "parse_circ.h"

# define MAX_LENGTH 1000

int main()
{
    FILE* intermediate_file = fopen("intermediate.txt", 'r');

    char line[MAX_LENGTH];

    float e_err = 0;
    char filename_in[MAX_LENGTH];
    char filename_out[MAX_LENGTH];
    int e_det_list[5];
    int e_det_list_len = 0;

    int sect = 0;
    int edet_i = 0;

    while (fgets(line, MAX_LENGTH, intermediate_file ) != NULL) {
        if ('!' == line[0]) {
            sect++;
            continue;
        }
        else {
            if (sect == 1 ) {
                e_err = atof(line);
            }
            else if (sect == 2) {
                strcpy(filename_in,line);
            }
            else if (sect == 3) {
                strcpy(filename_out,line);
            }
            else if (sect == 4) {
                e_det_list[edet_i] = atoi(line);
            }
            else if (sect == 5) {
                e_det_list_len = atoi(line);
            }
        }

    }

    create_erasure_file(e_err, filename_in, filename_out, e_det_list, e_det_list_len);
    return 0;
}