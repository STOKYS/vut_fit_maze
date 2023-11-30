// David Stoček
// Compile with: gcc -std=c11 -Wall -Wextra -Werror maze.c -o maze

#include <stdio.h>
#include <stdlib.h>

#define MIN_VALUE 0
#define MAX_VALUE 7

#define BORDER_HORIZONTAL 0
#define BORDER_LEFT 1
#define BORDER_RIGHT 2

#define RIGHT_HAND_RULE 1
#define LEFT_HAND_RULE 0

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

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
    const int length = get_string_length(a);
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

int is_number(const char character){
    const int number = character - '0';
    return number >= 0 && number <= 9;
}

int char_to_number(const char character){
    return character - '0';
}

int check_valid_number_range(const char character){
    const int number = character - '0';
    return number >= MIN_VALUE && number <= MAX_VALUE;
}

int is_horizontal(const int value){
    if (value >= 4) return 1;
    return 0;
}

int is_left(const int value){
    if (value % 2) return 1;
    return 0;
}

int is_right(const int value){
    if (value == 2 || value == 3 || value == 6 || value == 7) return 1;
    return 0;
}

int get_index(const Map *map, const int row, const int col){
    return (row-1)*map->cols+col-1;
}


int is_border(const Map *map, const int r, const int c, const int border){
    if (border == BORDER_HORIZONTAL && is_horizontal(map->cells[get_index(map, r, c)])) return 1;
    if (border == BORDER_LEFT && is_left(map->cells[get_index(map, r, c)])) return 1;
    if (border == BORDER_RIGHT && is_right(map->cells[get_index(map, r, c)])) return 1;
    return 0;
}

int is_odd_tile(int const row, const int col){
    return (row+col)%2;
}

int get_direction(int directions, const int is_odd, const int *borders, const int rule){
    int free_space = 0;
    while (!free_space) {
        if (directions == NORTH) {
            if (rule ? !borders[BORDER_RIGHT] : !borders[BORDER_LEFT]) {
                directions = rule ? EAST : WEST;
                free_space = 1;
            } else directions = rule ? WEST : EAST;
        } else if (directions == SOUTH) {
            if (rule ? !borders[BORDER_LEFT] : !borders[BORDER_RIGHT]) {
                directions = rule ? WEST : EAST;
                free_space = 1;
            }
            else directions = rule ? EAST : WEST;
        } else if (rule ? is_odd : !is_odd) {
             if (directions == EAST) {
                if (!borders[BORDER_HORIZONTAL]) {
                    directions = rule ? SOUTH : NORTH;
                    free_space = 1;
                }
                else directions = rule ? NORTH : SOUTH;
            } else if (directions == WEST) {
                directions = rule ? SOUTH : NORTH;
            }
        } else {
            if (directions == EAST) {
                directions = rule ? NORTH : SOUTH;
            } else if (directions == WEST) {
                if (!borders[BORDER_HORIZONTAL]) {
                    directions = rule ? NORTH : SOUTH;
                    free_space = 1;
                }
                else directions = rule ? SOUTH : NORTH;
            }
        }
    }
    return directions;
}

void move(const int directions, int* row, int* col){
    if (directions == EAST) (*col)++;
    if (directions == WEST) (*col)--;
    if (directions == SOUTH) (*row)++;
    if (directions == NORTH) (*row)--;
}

int start_border(const Map *map, const int r, const int c, const int leftright) {
    if (c == 1 && (r % 2)) return leftright ? NORTH : EAST;
    if (c == 1 && !(r % 2)) return leftright ? EAST : SOUTH;
    if (c == map->cols && !is_odd_tile(r, c)) return leftright ? WEST : NORTH;
    if (c == map->cols && is_odd_tile(r, c)) return leftright ? SOUTH : WEST;
    if (r == 1) return leftright ? EAST : WEST;
    if (r == map->rows) return leftright ? WEST : EAST;
    return 0;
}

int hand_solver(const Map *map, int current_row, int current_col, const int rule) {
    int directions = start_border(map, current_row, current_col, rule);
    int borders[3];
    while ((current_col <= map->cols && current_col > 0) && (current_row <= map->rows && current_row > 0)) {
        printf("[%d,%d]\n", current_row, current_col);

        for (int i = 0; i < 3; i++) {
            borders[i] = is_border(map, current_row, current_col, i);
        }

        const int is_current_odd = is_odd_tile(current_row, current_col);
        directions = get_direction(directions, is_current_odd, borders, rule);
        move(directions, &current_row, &current_col);
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

void map_constructor() {

}

void map_destructor() {

}

void map_fill_cells() {

}

int fill_map(FILE *file, Map *map, const int is_test){
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

int start_preparation(FILE *file, char *filename, Map *map, const int is_test){
    file = open_file(file, filename);
    if (!file || !fill_map(file, map, is_test)){
        fprintf(stderr, "ERROR! File %s doesn't have the correct matrix to solve the maze.\n", filename);
        return 0;
    }
    printf("SUCCESS! The matrix in the file %s is correct. You can run the solver.\n", filename);
    return 1;
}

int main(const int argc, char* argv[]) {
    FILE* file = NULL;
    Map map;
    if (argc == 2 && compare_strings("--help", argv[1])) {
        show_help();
    } else if (argc == 3 && compare_strings("--test", argv[1])){
        return start_preparation(file, argv[2], &map, 1);
    } else if (argc == 5 && (compare_strings("--lpath", argv[1]) || compare_strings("--rpath", argv[1]) ||
    compare_strings("--shortest", argv[1]))){
        if (!start_preparation(file, argv[4], &map, 0)) return 0;
        hand_solver(&map, char_to_number(argv[2][0]), char_to_number(argv[3][0]), compare_strings("--lpath", argv[1]) ? LEFT_HAND_RULE : RIGHT_HAND_RULE);
    } else {
        fprintf(stderr, "ERROR! Incorrect number of arguments or unknown arguments. Program will now terminate.\n");
        return 0;
    }
    return 1;
}