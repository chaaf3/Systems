/*******************************************************************************
 * Name        : pfind.c
 * Author      : Ming Lin & Connor Haaf
 * Date        : June 8, 2021
 * Description : program that finds files with a specified set of permissions.
 * Pledge : I pledge my Honor that I have abided by the Stevens Honor System.
 ******************************************************************************/

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// from lab 5-permstat
int perms[] = {S_IRUSR, S_IWUSR, S_IXUSR,
               S_IRGRP, S_IWGRP, S_IXGRP,
               S_IROTH, S_IWOTH, S_IXOTH};

char *permission_string(struct stat *statbuf) {
    char *p_string = malloc(10 * sizeof(char));
    int permission_valid;
    for (int i = 0; i < 9; i += 3) {
        // check r
        permission_valid = statbuf->st_mode & perms[i];
        if (permission_valid)
            p_string[i] = 'r';
        else
            p_string[i] = '-';

        // check w
        permission_valid = statbuf->st_mode & perms[i + 1];
        if (permission_valid)
            p_string[i + 1] = 'w';
        else
            p_string[i + 1] = '-';

        // check x
        permission_valid = statbuf->st_mode & perms[i + 2];
        if (permission_valid)
            p_string[i + 2] = 'x';
        else
            p_string[i + 2] = '-';
    }

    p_string[9] = '\0';
    return p_string;
}

int find_same_permissions(const char *direct, const char *perm) {
    //Constructing full path is from statfullpath.c on Canvas.
    char path[PATH_MAX];
    if (realpath(direct, path) == NULL) {
        fprintf(stderr, "Error: Cannot get full path of file '%s'. %s.\n",
                direct, strerror(errno));
        return EXIT_FAILURE;
    }

    DIR *dir;
    if ((dir = opendir(path)) == NULL) { // danger directory, permission denied
        fprintf(stderr, "Error: Cannot open directory '%s'. %s.\n",
                path, strerror(errno));
        return EXIT_FAILURE;
    }

    struct dirent *entry;
    struct stat sb;
    char full_filename[PATH_MAX + 1];
    size_t pathlen = 0;

    // Set the initial character to the NULL byte.
    // If the path is root '/', you can now take the strlen of a properly
    // terminated empty string.
    full_filename[0] = '\0';
    if (strcmp(path, "/")) {
        // If path is not the root - '/', then ...

        // If there is no NULL byte among the first n bytes of path,
        // the full_filename will not be terminated. So, copy up to and
        // including PATH_MAX characters.
        strncpy(full_filename, path, PATH_MAX);
    }
    // Add + 1 for the trailing '/' that we're going to add.
    pathlen = strlen(full_filename) + 1;
    full_filename[pathlen - 1] = '/';
    full_filename[pathlen] = '\0';

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        strncpy(full_filename + pathlen, entry->d_name, PATH_MAX - pathlen);
        if (lstat(full_filename, &sb) < 0) {
            fprintf(stderr, "Error: Cannot stat file '%s'. %s.\n",
                    full_filename, strerror(errno));
            continue;
        }
        //Get Permission string of current file.
        char *permissions_str = permission_string(&sb);
        //Compare to argument. If same permission, print out the full path.
        if (strcmp(perm, permissions_str) == 0)
            printf("%s\n", full_filename);

        //If the current file is a directory, recurse.
        if (entry->d_type == DT_DIR)
            find_same_permissions(full_filename, perm);

        free(permissions_str);
    }

    closedir(dir);
    return 0;
}

void printUsage() {
    fprintf(stderr, "Usage: ./pfind -d <directory> -p <permissions string> [-h]\n");
}

int main(int argc, char **argv) {
    bool d_flag = false, p_flag = false, h_flag = false;
    char *directory = NULL, *permissions = NULL;
    int c;

    opterr = 0;

    while ((c = getopt(argc, argv, ":d:p:h")) != -1)
        switch (c) {
            case 'd':
                d_flag = true;
                directory = optarg;
                break;
            case 'p':
                p_flag = true;
                permissions = optarg;
                break;
            case 'h':
                h_flag = true;
                break;
            case '?':
                if (isprint(optopt)) fprintf(stderr, "Error: Unknown option `-%c' received.\n", optopt);
                return EXIT_FAILURE;
        }

    if (!d_flag && !p_flag && !h_flag) {
        printUsage();
        return EXIT_FAILURE;
    }

    if (h_flag) {
        printUsage();
        return EXIT_SUCCESS;
    }

    if (!d_flag) {  // if -d is missing
        fprintf(stderr, "Error: Required argument -d <directory> not found.\n");
        return EXIT_FAILURE;
    }

    if (!p_flag) {  // if -p is missing
        fprintf(stderr, "Error: Required argument -p <permissions string> not found.\n");
        return EXIT_FAILURE;
    }

    struct stat statbuf;
    if (stat(directory, &statbuf) < 0) {  // if invalid directory is passed to -d
        fprintf(stderr, "Error: Cannot stat '%s'. %s.\n", directory, strerror(errno));
        return EXIT_FAILURE;
    }

    if (strlen(permissions) != 9) {
        fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", permissions);
        return EXIT_FAILURE;
    }

    printf("permission: %s\n", permissions);

    for (int i = 0; i < 9; i += 3) {
        if (permissions[i] != '-' && permissions[i] != 'r') {
            fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", permissions);
            return EXIT_FAILURE;
        }
        if (permissions[i + 1] != '-' && permissions[i + 1] != 'w') {
            fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", permissions);
            return EXIT_FAILURE;
        }
        if (permissions[i + 2] != '-' && permissions[i + 2] != 'x') {
            fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", permissions);
            return EXIT_FAILURE;
        }
    }

    find_same_permissions(directory, permissions);
    return EXIT_SUCCESS;
}