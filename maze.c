// David Stoček
// Compile with: gcc -std=c11 -Wall -Wextra -Werror maze.c -o maze

#include <stdio.h>
#include <stdlib.h>

#define MIN_VALUE 0
#define MAX_VALUE 7
#define BORDER_HORIZONTAL 0
#define BORDER_LEFT 1
#define BORDER_RIGHT 2

typedef struct {
    int rows;
    int cols;
    unsigned char *cells;
} Map;

int get_string_length(const char *string){
    int i = 0;
    while (string[i] != '\0'){
        ++i;
    }
    return i;
}

int compare_strings(const char *a, const char *b){
    int length = get_string_length(a);
    if (length == get_string_length(b)){
        for (int i = 0; i < length; ++i){
            if (a[i] != b[i]) return 0;
        }
        return 1;
    }
    return 0;
}

void show_help(){
    //printf("");
}

int is_number(char character){
    int number = character - '0';
    return number >= 0 && number <= 9;
}

int char_to_number(char character){
    return character - '0';
}

int check_valid_number_range(char character){
    int number = character - '0';
    return number >= MIN_VALUE && number <= MAX_VALUE;
}

int is_horizontal(int value){
    if (value == 4 || value == 5 || value == 6 || value == 7) return 1;
    return 0;
}

int is_left(int value){
    if (value == 1 || value == 3 || value == 5 || value == 7) return 1;
    return 0;
}

int is_right(int value){
    if (value == 2 || value == 3 || value == 6 || value == 7) return 1;
    return 0;
}

int get_index(Map *map, int row, int col){
    return (row-1)*map->cols+col-1;
}


int is_border(Map *map, int r, int c, int border){
    if (border == 0 && is_horizontal(map->cells[get_index(map, r, c)])) return 1;
    else if (border == 1 && is_left(map->cells[get_index(map, r, c)])) return 1;
    else if (border == 2 && is_right(map->cells[get_index(map, r, c)])) return 1;
    return 0;
}

int solve_for_right(Map *map, int current_row, int current_col) {
    int directions = 1;
    while ((current_col <= map->cols && current_col > 0) && (current_row <= map->rows && current_row > 0)) {
        printf("[%d,%d]\n", current_row, current_col);

        int horizontal = is_border(map, current_row, current_col, BORDER_HORIZONTAL);
        int left = is_border(map, current_row, current_col, BORDER_LEFT);
        int right = is_border(map, current_row, current_col, BORDER_RIGHT);

        int free_space = 0;
        while (!free_space){
            if ((current_row+current_col)%2){
                if (directions == 0){
                    if (right) directions = 3;
                    else free_space = 1;
                } else if (directions == 1){
                    if (horizontal) directions--;
                    else free_space = 1;
                } else if (directions == 2){
                    if (left) directions--;
                    else free_space = 1;
                } else if (directions == 3){
                    directions--;
                } else if (directions == 4) directions = 0;
            } else {
                if (directions == 0){
                    if (right) directions = 3;
                    else free_space = 1;
                } else if (directions == 1){
                    directions--;
                } else if (directions == 2){
                    if (left) directions--;
                    else free_space = 1;
                } else if (directions == 3){
                    if (horizontal) directions--;
                    else free_space = 1;
                } else if (directions == 4) directions = 0;
            }
        }

        if ((current_row + current_col) % 2) {
            if (directions == 0){
                current_col++;
            }
            if (directions == 1){
                current_row++;
            } else if (directions == 2){
                current_col--;
            }
        } else {
            if (directions == 0){
                current_col++;
            } else if (directions == 2){
                current_col--;
            } else if (directions == 3){
                current_row--;
            }
        }
        directions++;
    }
    return 0;
}

int solve_for_left(){
    return 0;
}

int solve_for_shortest(){
    return 0;
}

FILE *open_file(FILE *file, char *filename){
    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "ERROR! File %s didn't open. Program will now terminate.\n", filename);
        return 0;
    }
    return file;
}

int fill_map(FILE *file, Map *map, int is_test){
    char character;
    int nl_count = 0;
    int spc_count = 0;
    while ((character = fgetc(file)) != EOF) {
        printf("%c", character);
        if (character == '\n'){
            if (nl_count == 1 && spc_count+1 != map->cols) return 0;
            ++nl_count, spc_count = 0;
        }
        else if (character == ' ') ++spc_count;
        else if (is_number(character)){
            if (!nl_count && !spc_count) map->rows = character - '0';
            else if (!nl_count){
                map->cols = character - '0';
                map->cells = malloc((map->cols * map->rows) * sizeof(int));
            }
            else if ((nl_count > map->rows || (spc_count + 1) > map->cols) || (!check_valid_number_range(character))) return 0;
            else if (!is_test) map->cells[(nl_count-1)*map->cols+spc_count] = character - '0';
        } else return 0;
    }
    if ((nl_count-1 != map->rows && spc_count == 0) || (nl_count != map->rows && spc_count != 0)) return 0;
    return 1;
}

int start_preparation(FILE *file, char *filename, Map *map, int is_test){
    file = open_file(file, filename);
    if (!file || !fill_map(file, map, is_test)){
        fprintf(stderr, "ERROR! File %s doesn't have the correct matrix to solve the maze.\n", filename);
        return 0;
    }
    printf("SUCCESS! The matrix in the file %s is correct. You can run the solver.\n", filename);
    return 1;
}

int main(int argc, char* argv[]) {
    FILE* file = NULL;
    Map map;
    if (argc == 2 && compare_strings("--help", argv[1])) {
        show_help();
    } else if (argc == 3 && compare_strings("--test", argv[1])){
        return start_preparation(file, argv[2], &map, 1);
    } else if (argc == 5 && (compare_strings("--lpath", argv[1]) || compare_strings("--rpath", argv[1]) ||
    compare_strings("--shortest", argv[1]))){
        if (!start_preparation(file, argv[4], &map, 0)) return 0;
        if (compare_strings("--lpath", argv[1])) solve_for_left();
        else if (compare_strings("--rpath", argv[1])) solve_for_right(&map, char_to_number(argv[2][0]), char_to_number(argv[3][0]));
        else solve_for_shortest();
    } else {
        fprintf(stderr, "ERROR! Incorrect number of arguments or unknown arguments. Program will now terminate.\n");
        return 0;
    }
    return 1;
}