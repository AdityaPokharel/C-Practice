#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

char* imgtype;
size_t width, height;
int max_val;
int bytes;


/*
 * read the .ppm image passed as "filename"
 * and turn it into a 3 * height * width array
 * where 3 = RGB.
 */
int*** read_img(char* filename) {
    FILE* fp = fopen(filename, "rb");
    imgtype = (char*)malloc(sizeof(char) * 3);  // 2 chars + whitespace
    fscanf(fp, "%s", imgtype);

    fscanf(fp, "%lu%lu", &width, &height);
    fscanf(fp, "%i", &max_val);
    char nl;
    fscanf(fp, "%c", &nl); // capture the trailing newline


    int*** mat = (int***)malloc(sizeof(int**) * 3); // RGB
    mat[0] = (int**) malloc(sizeof(int*) * height);
    mat[1] = (int**) malloc(sizeof(int*) * height);
    mat[2] = (int**) malloc(sizeof(int*) * height);

    bytes = (max_val > 256? 2 : 1);
    for (size_t i = 0; i < height; i++) {
        mat[0][i] = (int*)malloc(sizeof(int) * width);
        mat[1][i] = (int*)malloc(sizeof(int) * width);
        mat[2][i] = (int*)malloc(sizeof(int) * width);
        for(size_t j = 0; j < width; j++) {
            fread(&mat[0][i][j], (size_t)bytes, 1, fp);
            fread(&mat[1][i][j], (size_t)bytes, 1, fp);
            fread(&mat[2][i][j], (size_t)bytes, 1, fp);
            if( mat[0][i][j] > max_val || mat[1][i][j] > max_val || mat[2][i][j] > max_val) {
                fclose(fp);
                return NULL;
            }
        }

    }

    fclose(fp);

    return mat;
}


/*
 * Calculates the average
 */
int* calculate_avg(size_t i_pos, size_t j_pos, int rt, int*** mat) {
    int r_sum = 0, g_sum = 0, b_sum = 0;
    for (size_t i = i_pos; i < rt + i_pos; i++) {
        if (i > height) {
            continue;
        }
        for (size_t j = j_pos; j < rt + j_pos; j++) {
            if (j > width) {
                continue;
            }
           r_sum += mat[0][i][j];
           g_sum += mat[1][i][j];
           b_sum += mat[2][i][j];
        }
    }
    int* calc = (int*) malloc(sizeof(float) * 3);
    calc[0] = r_sum/(int)pow(rt, 2);
    calc[1] = g_sum/(int)pow(rt, 2);
    calc[2] = b_sum/(int)pow(rt, 2);

    return calc;
}


void apply_avg(size_t i_pos, size_t j_pos, int rt, int*** mat, int* avg) {
    for (size_t i = i_pos; i < rt + i_pos; i++) {
        if (i > height) {
            continue;
        }
        for (size_t j = j_pos; j < rt + j_pos; j++) {
            if (j > width) {
                continue;
            }
            mat[0][i][j] = avg[0];
            mat[1][i][j] = avg[1];
            mat[2][i][j] = avg[2];
        }
    }
}


/*
 * cluster k nearest neighbors for an image and
 * return the resulting clustered matrix.
 */
void nearest_neighbor(int*** mat, int k) {
    int rt = (int)sqrt(k);
    for (size_t i = 0; i < height - rt; i+=rt) {
        for (size_t j = 0; j < width - rt; j+=rt) {
            int* avg = calculate_avg(i, j, rt, mat);
            apply_avg(i, j, rt, mat, avg);
        }
    }
}


/*
 * Only used for testing.
 */
void printimg(int*** mat) {
    printf("\nRED:\n");
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            printf("%i\t", mat[0][i][j]);

        }
        printf("\n");
    }
    printf("\nGREEN:\n");
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            printf("%i\t", mat[1][i][j]);
        }
        printf("\n");
    }

    printf("\nBLUE:\n");
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            printf("%i\t", mat[2][i][j]);
        }
        printf("\n");
    }

}

/*
 * Checks whether the passed value is a perfect square.
 */
int perfect_square(int k) {
    double fvar = sqrt((double) k);
    int ivar = (int)fvar;

    return (ivar == fvar? 1 : 0);
}


void write_to_img(int*** mat) {
    FILE* fp = fopen("output_img.ppm", "wb");
    fprintf(fp,"%s\n", imgtype);
    fprintf(fp, "%i %i\n", (int)width, (int)height);
    fprintf(fp, "%i ", max_val);

    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            fwrite(&mat[0][i][j], (size_t)bytes, 1, fp);
            fwrite(&mat[1][i][j], (size_t)bytes, 1, fp);
            fwrite(&mat[2][i][j], (size_t)bytes, 1, fp);
        }
    }

    fclose(fp);

}



int main (int argc, char** argv) {
    if (argc != 3) {
        printf("ERROR: Not enough args.\n");
        printf("Usage: ./exec img.ppm k\n");
        exit(1);
    }

    int*** mat = read_img(argv[1]);

    int k; sscanf(argv[2], "%i", &k);
    if(!perfect_square(k)) {
        printf("ERROR: Number passed needs\nto be a perfect square\n");
        exit(3);
    }


    if(mat == NULL) {
        printf("Err: values read higher than max_val\n");
        exit(2);
    }

    printf("Width: %lu, height: %lu\n", width, height);

    nearest_neighbor(mat, k);
    write_to_img(mat);
    printf("New image written\n");
}
