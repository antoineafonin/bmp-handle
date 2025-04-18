#include "bmp.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static size_t align_to_4(size_t size) { return (size + 3) & ~3; }

struct bmp_image* load_bmp(FILE* input) {
    if (!input) return NULL;

    struct bmp_header* header = load_bmp_header(input);
    if (!header) {
        fprintf(stderr, "Error: Invalid BMP header.\n");
        return NULL;
    }

    struct pixel* pixels = load_pixels(input, header);
    if (!pixels) {
        fprintf(stderr, "Error: Failed to read BMP data.\n");
        free(header);
        return NULL;
    }

    struct bmp_image* image = malloc(sizeof(struct bmp_image));
    if (!image) {
        free(header);
        free(pixels);
        return NULL;
    }

    image->header = header;
    image->data = pixels;
    return image;
}

struct bmp_header* load_bmp_header(FILE* file) {
    if (!file) return NULL;

    struct bmp_header* header = malloc(sizeof(struct bmp_header));
    if (!header) return NULL;

    rewind(file);
    if (fread(header, sizeof(struct bmp_header), 1, file) != 1) {
        free(header);
        return NULL;
    }

    if (header->type != 0x4D42) {
        free(header);
        return NULL;
    }

    return header;
}

struct pixel* load_pixels(FILE* file, const struct bmp_header* header) {
    if (!file || !header) return NULL;

    size_t row_bytes = align_to_4(header->width * sizeof(struct pixel));
    size_t total_pixels = header->width * header->height;

    struct pixel* data = malloc(total_pixels * sizeof(struct pixel));
    if (!data) return NULL;

    if (fseek(file, header->offset, SEEK_SET) != 0) {
        free(data);
        return NULL;
    }

    for (int y = 0; y < header->height; ++y) {
        struct pixel* row_ptr = data + y * header->width;
        if (fread(row_ptr, sizeof(struct pixel), header->width, file) !=
            header->width) {
            free(data);
            return NULL;
        }

        size_t padding = row_bytes - header->width * sizeof(struct pixel);
        for (size_t p = 0; p < padding; ++p) fgetc(file);
    }

    return data;
}

bool save_bmp(FILE* output, const struct bmp_image* image) {
    if (!output || !image) return false;

    if (fwrite(image->header, sizeof(struct bmp_header), 1, output) != 1)
        return false;

    size_t width = image->header->width;
    size_t height = image->header->height;
    size_t row_bytes = align_to_4(width * sizeof(struct pixel));
    size_t padding = row_bytes - width * sizeof(struct pixel);

    if (fseek(output, image->header->offset, SEEK_SET) != 0) return false;

    for (int y = 0; y < height; ++y) {
        const struct pixel* row = image->data + y * width;
        if (fwrite(row, sizeof(struct pixel), width, output) != width)
            return false;

        for (size_t p = 0; p < padding; ++p) fputc(0x00, output);
    }

    return true;
}

void free_bmp_image(struct bmp_image* image) {
    if (!image) return;
    free(image->header);
    free(image->data);
    free(image);
}
