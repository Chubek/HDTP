#include "include/hdtp.h"

extern char * read_file(char path[]) {
    FILE *f = fopen(path, "r");

    if (f == NULL) {
        printf("Error reading file: %s", path);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *contents = (char*) malloc(sizeof(char) * (fsize + 1) );
    fread(contents, fsize, 1, f);

    fclose(f);
 
    contents[fsize] = 0;

    return contents;
}

extern void write_file(char path[], char buffer[]) {
    FILE *f = fopen(path, "w");

    if (f == NULL) {
        printf("Error opening file: %s to write", path);
        exit(1);
    }

    fputs(buffer, f);

    printf("Wrote results to: %s", path);

    fclose(f);
}


extern void list_files(char path[], char list[]) {
    DIR *d;
    struct dirent *dir;

    d = opendir(path);

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG 
                && dir->d_name != NULL 
            ) {
                if (!strcmp(dir->d_name, ".config")) { continue; }
             
                strcat(list, DELIM);
                strcat(list, dir->d_name);
            }
        }
    }

    closedir(d);
}

extern void list_subdirs(char path[], char list[]) {
    DIR *d;
    struct dirent *dir;

    d = opendir(path);

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_DIR 
                && dir->d_name != NULL
                && strcmp(dir->d_name, "..") != 0
                && strcmp(dir->d_name, ".") != 0
            ) {
                strcat(list, DELIM);
                strcat(list, dir->d_name);
            }
        }
    }

    closedir(d);
}


void make_dir(char path[]) {
    struct stat st = {0};

    if (stat(path, &st) == -1) {
        mkdir(path, 0700);
    }
}