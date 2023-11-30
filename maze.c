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

#define METHOD_HELP 3
#define METHOD_TEST 2
#define METHOD_RPATH 1
#define METHOD_LPATH 0

int open_file(FILE **file, const char *filename){
    *file = fopen(filename, "r");
    if (*file == NULL) {
        fprintf(stderr, "ERROR! File %s did not open. Program will now terminate.\n", filename);
        return 0;
    }
    return 1;
}

int close_file(FILE *file, const char *filename) {
    if (fclose(file) == EOF) {
        fprintf(stderr, "ERROR! File %s could not be closed.", filename);
        return 0;
    }
    return 1;
}

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

void copy_string(char *a, const char *b) {
    const int length = get_string_length(b);
    for (int i = 0; i != length; i++) {
        a[i] = b[i];
    }
}

int is_number(const char *characters){
    int passed = 1;
    for (int i = 0; characters[i] != '\0'; i++) {
        const int number = characters[i] - '0';
        if (!(number >= 0 && number <= 9)) passed = 0;
    }
    return passed;
}

int chars_to_number(const char *characters){
    return *characters - '0';
}

typedef struct {
    int rows;
    int cols;
    unsigned char *cells;
} Map;

void map_constructor(Map *this, const int rows, const int cols) {
    this->rows = rows;
    this->cols = cols;
    this->cells = malloc(rows * cols * sizeof(int));
}

void map_destructor(Map *this) {
    this->rows = 0;
    this->cols = 0;
    free(this->cells);
    this->cells = NULL;
}

void map_put_in_cells(int* cells, const int index, const int number) {
    cells[index] = number;
}

typedef struct {
    int method;
    char *filename;
    int row;
    int col;
} Argument;

int argument_constructor(Argument *this, const int argc, char **argv) {
    if (argc == 2 && compare_strings("--help", argv[1])) {
        this->method = METHOD_HELP;
        return 1;
    } if (argc == 3) {
        if (compare_strings("--test", argv[1])) {
            this->method = METHOD_TEST;
            this->filename = malloc(get_string_length(argv[2]) * sizeof(char));
            copy_string(this->filename, argv[2]);
            return 1;
        }
    } else if (argc == 5) {
        if (is_number(argv[2]) && is_number(argv[3])) {
            this->filename = malloc(get_string_length(argv[4]) * sizeof(char));
            copy_string(this->filename, argv[4]);
            this->row = chars_to_number(argv[2]);
            this->col = chars_to_number(argv[3]);
            if (compare_strings("--lpath", argv[1])) {
                this->method = METHOD_LPATH;
                return 1;
            }
            if (compare_strings("--rpath", argv[1])) {
                this->method = METHOD_RPATH;
                return 1;
            }
        }
    }
    fprintf(stderr, "ERROR! Inccorect arguments. Program will now terminate.\n");
    return 0;
}

void argument_destructor(Argument *this) {
    this->method = 0;
    this->row = 0;
    this->col = 0;
    free(this->filename);
    this->filename = NULL;
}

void show_help(){
    //printf("");
}

int check_valid_number_range(const int number){
    return number >= MIN_VALUE && number <= MAX_VALUE;
}

int get_index(const Map *map, const int row, const int col){
    return (row-1)*map->cols+col-1;
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
    if (r == 1) return SOUTH;
    if (r == map->rows) return NORTH;
    return 0;
}

int hand_solver(const Map *map, int current_row, int current_col, const int rule) {
    int directions = start_border(map, current_row, current_col, rule);
    int borders[3];
    while ((current_col <= map->cols && current_col > 0) && (current_row <= map->rows && current_row > 0)) {
        printf("%d,%d\n", current_row, current_col);

        for (int i = 0; i < 3; i++) {
            borders[i] = is_border(map, current_row, current_col, i);
        }

        const int is_current_odd = is_odd_tile(current_row, current_col);
        directions = get_direction(directions, is_current_odd, borders, rule);
        move(directions, &current_row, &current_col);
    }
    return 0;
}

int check_neighboring_fields(const Map *map, const int index, const int number) {
    // printf("%d %d\n", index / map->cols, index % map->cols);
    const int odd = is_odd_tile(index / map->cols + 1, index % map->cols + 1);
    if (index == 0) return 1;
    if (index < map->cols) {
        if (is_left(number) == is_right(map->cells[index-1])) return 1;
    }
    if (index >= map->cols){
        if (index % map->cols == 0) {
            if (odd) return 1;
            if (is_horizontal(number) == is_horizontal(map->cells[index - map->cols])) return 1;
        } else {
            if (is_left(number) == is_right(map->cells[index-1])) {
                if (odd) return 1;
                if (is_horizontal(number) == is_horizontal(map->cells[index - map->cols])) return 1;
            }
        }
    }
    return 0;
}

int fill_map(FILE *file, Map *map, int method) {
    fscanf(file, "%d %d", &map->rows, &map->cols);
    const int size = map->rows * map->cols;
    map->cells = malloc(size * sizeof(int));
    for (int i = 0; i < size; ++i) {
        int temp;
        if (fscanf(file, "%d", &temp) == 0) {
            printf("Invalid\n");
            return 0;
        }
        if (check_valid_number_range(temp)) {
            if (check_neighboring_fields(map, i, temp)) {
                map->cells[i] = temp;
            } else {
                printf("Invalid\n");
                return 0;
            }
        } else {
            printf("Invalid\n");
            return 0;
        }
    }
    if (method == 2) printf("Valid\n");
    return 1;
}

int main(const int argc, char* argv[]) {
    FILE* file = NULL;
    Map map;
    Argument argument;
    if (argument_constructor(&argument, argc, argv)) {
        if (argument.method != METHOD_HELP) {
            if (open_file(&file, argument.filename)) {
                if (fill_map(file, &map, argument.method)) {
                    if (argument.method != METHOD_TEST) {
                        hand_solver(&map, argument.row, argument.col, argument.method);
                    }
                } else return 0;
                map_destructor(&map);
            } else return 0;
        } else show_help();
        argument_destructor(&argument);
    }
    if (file != NULL) {
        close_file(file, argument.filename);
    }
    return 1;
}