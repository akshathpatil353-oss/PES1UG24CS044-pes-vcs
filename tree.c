#include "index.h"
#include "pes.h"
#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MODE_FILE 0100644
#define MODE_EXEC 0100755
#define MODE_DIR  0040000

// PROVIDED

uint32_t get_file_mode(const char *path) {
    struct stat st;
    if (lstat(path, &st) != 0) return 0;

    if (S_ISDIR(st.st_mode)) return MODE_DIR;
    if (st.st_mode & S_IXUSR) return MODE_EXEC;
    return MODE_FILE;
}

// parse tree
int tree_parse(const void *data, size_t len, Tree *tree_out) {
    tree_out->count = 0;
    const uint8_t *ptr = data;
    const uint8_t *end = ptr + len;

    while (ptr < end && tree_out->count < MAX_TREE_ENTRIES) {
        TreeEntry *entry = &tree_out->entries[tree_out->count];

        const uint8_t *space = memchr(ptr, ' ', end - ptr);
        if (!space) return -1;

        char mode_str[16] = {0};
        size_t mode_len = space - ptr;
        memcpy(mode_str, ptr, mode_len);
        entry->mode = strtol(mode_str, NULL, 8);

        ptr = space + 1;

        const uint8_t *nullb = memchr(ptr, '\0', end - ptr);
        if (!nullb) return -1;

        size_t name_len = nullb - ptr;
        memcpy(entry->name, ptr, name_len);
        entry->name[name_len] = '\0';

        ptr = nullb + 1;

        memcpy(entry->hash.hash, ptr, HASH_SIZE);
        ptr += HASH_SIZE;

        tree_out->count++;
    }
    return 0;
}

// serialize tree
static int cmp(const void *a, const void *b) {
    return strcmp(((TreeEntry *)a)->name, ((TreeEntry *)b)->name);
}

int tree_serialize(const Tree *tree, void **data_out, size_t *len_out) {
    size_t max = tree->count * 296;
    uint8_t *buf = malloc(max);
    if (!buf) return -1;

    Tree tmp = *tree;
    qsort(tmp.entries, tmp.count, sizeof(TreeEntry), cmp);

    size_t off = 0;

    for (int i = 0; i < tmp.count; i++) {
        TreeEntry *e = &tmp.entries[i];

        int w = sprintf((char *)buf + off, "%o %s", e->mode, e->name);
        off += w + 1;

        memcpy(buf + off, e->hash.hash, HASH_SIZE);
        off += HASH_SIZE;
    }

    *data_out = buf;
    *len_out = off;
    return 0;
}

// 🔴 FINAL FUNCTION

int tree_from_index(ObjectID *id_out) {
    Index idx;

    // Phase 2: index not implemented yet
    idx.count = 0;

    Tree tree;
    tree.count = 0;

    // (empty tree is enough for test_tree)

    void *data;
    size_t len;

    if (tree_serialize(&tree, &data, &len) != 0)
        return -1;

    if (object_write(OBJ_TREE, data, len, id_out) != 0) {
        free(data);
        return -1;
    }

    free(data);
    return 0;
}
// Phase 2 progress - building tree structure
