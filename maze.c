// David Stoček
// Compile with: gcc -std=c11 -Wall -Wextra -Werror maze.c -o maze

#include <stdio.h>
#include <stdlib.h>

#define MIN_VALUE 0
#define MAX_VALUE 7

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

int check_valid_number_range(char character){
    int number = character - '0';
    return number >= MIN_VALUE && number <= MAX_VALUE;
}

int check_valid_text_file(FILE *file){
    char character;
    int nl_count = 0;
    int spc_count = 0;
    int col = 0, row = 0;
    while ((character = fgetc(file)) != EOF) {
        printf("%c", character);
        if (character == '\n'){
            if (nl_count == 1 && spc_count+1 != col) return 0;
            ++nl_count, spc_count = 0;
        }
        else if (character == ' ') ++spc_count;
        else if (is_number(character)){
            if (!nl_count && !spc_count) row = character - '0';
            else if (!nl_count) col = character - '0';
            else if ((nl_count > row || (spc_count + 1) > col) || (!check_valid_number_range(character))) return 0;
        } else return 0;
    }
    if ((nl_count-1 != row && spc_count == 0) || (nl_count != row && spc_count != 0)) return 0;
    return 1;
}

FILE *open_file(FILE *file, char *argv){
    file = fopen(argv, "r");
    if (file == NULL) {
        fprintf(stderr, "ERROR! File %s didn't open. Program will now terminate.\n", argv);
        return 0;
    }
    return file;
}

int start_preparation(FILE *file, char *filename){
    printf("|%d|\n", check_valid_text_file(file));
    if (!check_valid_text_file(file)){
        fprintf(stderr, "ERROR! File %s doesn't have the correct matrix to solve the maze.\n", filename);
        return 0;
    }
    printf("SUCCESS! The matrix in the file %s is correct. You can run the solver.\n", filename);
    return 1;
}

void fill_map(Map *this, FILE *file){
    char character;
    int numbers_count = 0;
//    int row_count = 0;
    while ((character = fgetc(file)) != EOF) {
        if (is_number(character)) {
            printf("dd\n");
            if (!numbers_count) {
                this->rows = character - '0';
            } else if (numbers_count == 1) {
                this->cols = character - '0';
                printf("%d %d", this->rows, this->cols);
                this->cells = malloc((this->rows * this->cols) * sizeof(int));
            } else {
                printf("%d\n", this->cols%numbers_count);
//                if (!(this->cols%numbers_count)){
//                    printf("eyo");
//                    ++row_count;
//                }
            }
            ++numbers_count;
        }
    }
    this->rows = 0;
    this->cols = 0;
}

int main(int argc, char* argv[]) {
    FILE* file = NULL;
    Map map;
    if (argc == 2 && compare_strings("--help", argv[1])) {
        show_help();
    } else if (argc == 3 && compare_strings("--test", argv[1])){
        file = open_file(file, argv[2]);
        return start_preparation(file, argv[2]);
    } else if (argc == 5 && (compare_strings("--lpath", argv[1]) || compare_strings("--rpath", argv[1]) ||
    compare_strings("--shortest", argv[1]))){
        file = open_file(file, argv[4]);
//        printf("damni");
        printf("%d\n", start_preparation(file, argv[4]));
        printf("%d\n", start_preparation(file, argv[4]));
//        printf("%d", start_preparation(file, argv[4]));
//        if (start_preparation(file, argv[4])){
//            printf("what is this");
//            fill_map(&map, file);
//        }
    } else {
        fprintf(stderr, "ERROR! Incorrect number of arguments or unknown arguments. Program will now terminate.\n");
        return 0;
    }
    return 1;
}