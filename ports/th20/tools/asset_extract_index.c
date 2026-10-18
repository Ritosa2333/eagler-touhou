/* A small read-only archive adapter around the pinned Touhou Toolkit library.
 * Unlike thdat's filename extraction, indices distinguish duplicate filenames.
 * Usage: asset_extract_index ARCHIVE [INDEX OUTPUT]
 */
#include <config.h>
#include <thtk/thtk.h>
#include <thtk/thdat.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    thtk_error_t *error = NULL;
    thtk_io_t *input = NULL, *output = NULL;
    thdat_t *archive = NULL;
    int result = 1;
    if (argc != 2 && argc != 4) {
        fprintf(stderr, "Usage: %s ARCHIVE [INDEX OUTPUT]\n", argv[0]);
        return 2;
    }
    input = thtk_io_open_file(argv[1], "rb", &error);
    if (!input) goto cleanup;
    archive = thdat_open(20, input, &error);
    if (!archive) goto cleanup;
    if (argc == 2) {
        for (size_t i = 0; i < archive->entry_count; ++i) {
            const thdat_entry_t *entry = &archive->entries[i];
            printf("%zu\t%s\t%zd\t%zd\t%zd\n", i, entry->name,
                entry->offset, entry->size, entry->zsize);
        }
    } else {
        char *end;
        unsigned long index = strtoul(argv[2], &end, 10);
        if (*end || index >= archive->entry_count) {
            fprintf(stderr, "Invalid archive index: %s\n", argv[2]);
            goto cleanup;
        }
        output = thtk_io_open_file(argv[3], "wb", &error);
        if (!output) goto cleanup;
        if (thdat_entry_read_data(archive, (size_t)index, output, &error) < 0)
            goto cleanup;
    }
    result = 0;
cleanup:
    if (error) {
        fprintf(stderr, "%s\n", thtk_error_message(error));
        thtk_error_free(&error);
    }
    if (output) thtk_io_close(output);
    if (archive) thdat_free(archive);
    if (input) thtk_io_close(input);
    return result;
}
