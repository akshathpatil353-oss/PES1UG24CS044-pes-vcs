#include "index.h"
#include "pes.h"
#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INDEX_FILE ".pes/index"

// 🔴 ADD FILE TO INDEX
int index_add(Index *idx, const char *path) {
    if (idx->count >= MAX_INDEX_ENTRIES)
        return -1;

    FILE *f = fopen(path, "rb");
    if (!f) return -1;

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    void *data = malloc(size);
    if (!data) {
        fclose(f);
        return -1;
    }

    // read file content
    if (fread(data, 1, size, f) != size) {
        fclose(f);
        free(data);
        return -1;
    }

    fclose(f);

    ObjectID id;

    // create blob object
    if (object_write(OBJ_BLOB, data, size, &id) != 0) {
        free(data);
        return -1;
    }

    IndexEntry *e = &idx->entries[idx->count++];

    strncpy(e->path, path, sizeof(e->path) - 1);
    e->path[sizeof(e->path) - 1] = '\0';

    e->mode = get_file_mode(path);
    e->hash = id;

    free(data);
    return 0;
}

// 🔴 SAVE INDEX (FIXED SIGNATURE)
int index_save(const Index *idx) {
    FILE *f = fopen(INDEX_FILE, "wb");
    if (!f) return -1;

    // write count
    if (fwrite(&idx->count, sizeof(int), 1, f) != 1) {
        fclose(f);
        return -1;
    }

    // write entries
    for (int i = 0; i < idx->count; i++) {
        const IndexEntry *e = &idx->entries[i];

        fwrite(e->path, sizeof(e->path), 1, f);
        fwrite(&e->mode, sizeof(e->mode), 1, f);
        fwrite(e->hash.hash, HASH_SIZE, 1, f);
    }

    fclose(f);
    return 0;
}

// 🔴 LOAD INDEX
int index_load(Index *idx) {
    FILE *f = fopen(INDEX_FILE, "rb");
    if (!f) {
        idx->count = 0;
        return 0;
    }

    if (fread(&idx->count, sizeof(int), 1, f) != 1) {
        fclose(f);
        return -1;
    }

    for (int i = 0; i < idx->count; i++) {
        IndexEntry *e = &idx->entries[i];

        fread(e->path, sizeof(e->path), 1, f);
        fread(&e->mode, sizeof(e->mode), 1, f);
        fread(e->hash.hash, HASH_SIZE, 1, f);
    }

    fclose(f);
    return 0;
}
int index_status(const Index *idx) {
    printf("Staged files:\n");

    for (int i = 0; i < idx->count; i++) {
        printf("  %s\n", idx->entries[i].path);
    }

    return 0;
}
// Phase 3: index_add implemented

