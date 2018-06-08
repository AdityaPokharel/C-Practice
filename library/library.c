#include <stdio.h>
#include <stdlib.h>

typedef struct book {
    int id;
    char* name;
    int ref_size;
    int* reference;
} book;

int SIZE;
/*
 * Prints the books* library passed (only upto index SIZE)
 */
void print_library(book* books, int SIZE) {
    for (int i = 0; i < SIZE; i++) {
        printf("Book ID: %d Book Name: %s\n", books[i].id, books[i].name);
        for (int j = 0; j < books[i].ref_size; j++) {
            int cur_ref_pos = books[i].reference[j];
            printf("   -ref %d: %s\n", j, books[cur_ref_pos].name);
        }
    }
}

/*
 * Opens and reads the given filename and creates a library with references.
 * Returns the created library as a books* array.
 */
book* create_library(char* filename) {
    FILE* fp = fopen(filename, "r");
    fscanf(fp, "%d", &SIZE);

    book* books = (book*) malloc(sizeof(book) * SIZE);
    for (int i = 0; i < SIZE; i++) {
        books[i].name = (char*) malloc(sizeof(char)* 100);

        fscanf(fp, "%d%s%d", &books[i].id, books[i].name, &books[i].ref_size);

        books[i].reference = (int*) malloc(sizeof(int) * books[i].ref_size);
        for (int j = 0; j < books[i].ref_size; j++) {
            int cur_ref;
            fscanf(fp, "%d", &cur_ref);
            books[i].reference[j] = cur_ref - 1;
        }

    }

    return books;
}

int main(int args, char* argv[]) {
    if (args < 2) {
        printf("Not enough args\n");
        return 1;
    }

    book* books = create_library(argv[1]);
    print_library(books, SIZE);
}
