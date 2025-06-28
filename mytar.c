#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#define BLOCK_SIZE 512
#define REGTYPE '0'
#define AREGTYPE '\0'

struct posix_header {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char pad[12];
};

// globalni k pamatovani ze doslo k chybe
int had_errors = 0;

// vraci true, pokud je blok cely nulovy
int is_zero_block(const char *block) {
    // iteruju skrz kazdy byte v bloku
    for (int i = 0; i < BLOCK_SIZE; ++i)
        if (block[i] != 0)
            return 0;
    return 1;
}

// parsuje z tar header podle formatu
uint64_t parse_size(const char *data, size_t len) {
    if ((unsigned char)data[0] & 0x80) {
        // base-256
        uint64_t value = 0;
        for (size_t i = 1; i < len; ++i) {
            value = value * 256 + (unsigned char)data[i];
        }
        return value;
    } else {
        // oktal
        char tmp[21];
        memcpy(tmp, data, len);
        tmp[len] = '\0';
        return strtoull(tmp, NULL, 8);
    }
}

/// vrací true, pokud soubor name je mezi hledanymi (nebo zadny hledan7 neni)
int should_list(const char *name, char **wanted, int wanted_count) {
    // kdyz zadny hledany neni vypis vsechno
    if (wanted_count == 0) return 1;
    for (int i = 0; i < wanted_count; ++i)
        if (strcmp(name, wanted[i]) == 0)
            return 1;
    return 0;
}

/// oznaci soubor jako nalezeny
void mark_found(const char *name, char **wanted, int wanted_count, int *found) {
    for (int i = 0; i < wanted_count; ++i)
        if (strcmp(name, wanted[i]) == 0)
            found[i] = 1;
}

// kontrola zda je soubor tar archivem
int is_tar_archive(const struct posix_header *hdr) {
    if ((strncmp(hdr->magic, "ustar", 5) == 0 && hdr->magic[5] == '\0' &&
         strncmp(hdr->version, "00", 2) == 0) ||
        (strncmp(hdr->magic, "ustar ", 6) == 0 && strncmp(hdr->version, " ", 1) == 0)) {
        return 1;
    }
    return 0;
}

// main :)
int main(int argc, char *argv[]) {
    // mam alespon jeden arg
    if (argc < 2) {
        fprintf(stderr, "mytar: need at least one option\n");
        return 2;
    }

    int t_flag = 0;
    int x_flag = 0;
    int v_flag = 0;

    const char *archive_name = NULL;
    int file_args_start = argc;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-t") == 0) {
        	t_flag = 1;
        }
        else if (strcmp(argv[i], "-x") == 0) {
    		x_flag = 1;
        } 
        else if (strcmp(argv[i], "-v") == 0) { 
            v_flag = 1;
        } 
        else if (strcmp(argv[i], "-f") == 0) {
            // -f musi mit arg
            if (++i >= argc) {
                fprintf(stderr, "mytar: option requires an argument -- f\n");
                return 2;
            }
            archive_name = argv[i];
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "mytar: Unknown option: %s\n", argv[i]);
            return 2;
        } else {
            file_args_start = i;
            break;
        }
    }

    // kontrola zda jsou tam potrebne veci
    if (!archive_name) {
        fprintf(stderr, "mytar: you must specify a file with -f\n");
        return 2;
    }
    if ((t_flag && x_flag) || (!t_flag && !x_flag)) {
        fprintf(stderr, "mytar: you may not specify both -t and -x\n"); 
        return 2;
    }

    int wanted_count = argc - file_args_start;
    char **wanted_files = (file_args_start < argc) ? &argv[file_args_start] : NULL;
    int *found = (wanted_count > 0) ? calloc(wanted_count, sizeof(int)) : NULL;

    // otevrit cely ke cteni
    FILE *fp = fopen(archive_name, "rb");
    if (!fp) {
        fprintf(stderr, "mytar: Cannot open %s: %s\n", archive_name, strerror(errno));

        free(found);
        return 2;
    }

    char block[BLOCK_SIZE];
    int zero_blocks = 0;

    // main loop
    // cte dokonce nebo do dvou po sobe jdoucich nulovych blocich
    while (fread(block, 1, BLOCK_SIZE, fp) == BLOCK_SIZE) {

        if (is_zero_block(block)) {
            zero_blocks++;
            if (zero_blocks == 2)
                break;
            continue;
        } else {
            zero_blocks = 0;
        }

        struct posix_header *hdr = (struct posix_header *)block;
        uint64_t size = parse_size(hdr->size, 12);
        char type = hdr->typeflag;

        if (type != REGTYPE && type != AREGTYPE) {
            fprintf(stderr, "mytar: Unsupported header type: %d\n", (int)(unsigned char)type);
            fclose(fp);
            return 2;
        }

        if (should_list(hdr->name, wanted_files, wanted_count)) {
            printf("%s\n", hdr->name);
            fflush(stdout);
            mark_found(hdr->name, wanted_files, wanted_count, found);
        }

        // preskoci datove bloky souboru
        uint64_t blocks_to_skip = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
        for (uint64_t i = 0; i < blocks_to_skip; ++i) {
            if (fread(block, 1, BLOCK_SIZE, fp) != BLOCK_SIZE) {
                fprintf(stderr, "mytar: Unexpected EOF in archive\n");
                fprintf(stderr, "mytar: Error is not recoverable: exiting now\n");

                fclose(fp);
                return 2;
            }
        }
    }

    // jestli nenasel nejaky hledany
    if (wanted_files) {
        for (int i = 0; i < wanted_count; ++i) {
            if (!found[i]) {
                fprintf(stderr, "mytar: %s: Not found in archive\n", wanted_files[i]);
                had_errors = 1;
            }
        }
        free(found);
    }

    if (zero_blocks == 1) {
        fprintf(stderr, "mytar: A lone zero block at %ld\n", ftell(fp) / BLOCK_SIZE);
    }
    
    fclose(fp);

    if (had_errors) {
        fprintf(stderr, "mytar: Exiting with failure status due to previous errors\n");
        return 2;
    }

    return 0;
}
