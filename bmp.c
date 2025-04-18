#include "bmp.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static size_t align_to_4(size_t size) { return (size + 3) & ~3; }

struct bmp_image* read_bmp(FILE* stream) {
    if (!stream) return NULL;

    struct bmp_header* header = read_bmp_header(stream);
    if (!header) {
        fprintf(stderr, "Error: Invalid BMP header.\n");
        return NULL;
    }

    struct pixel* pixels = read_data(stream, header);
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

struct bmp_header* read_bmp_header(FILE* stream) {
    if (!stream) return NULL;

    struct bmp_header* header = malloc(sizeof(struct bmp_header));
    if (!header) return NULL;

    rewind(stream);
    if (fread(header, sizeof(struct bmp_header), 1, stream) != 1) {
        free(header);
        return NULL;
    }

    if (header->type != 0x4D42) {
        free(header);
        return NULL;
    }

    return header;
}

struct pixel* read_data(FILE* stream, const struct bmp_header* header) {
    if (!stream || !header) return NULL;

    size_t row_size = align_to_4(header->width * sizeof(struct pixel));
    size_t total_pixels = header->width * header->height;

    struct pixel* data = malloc(total_pixels * sizeof(struct pixel));
    if (!data) return NULL;

    if (fseek(stream, header->offset, SEEK_SET) != 0) {
        free(data);
        return NULL;
    }

    for (size_t y = 0; y < header->height; ++y) {
        struct pixel* row_ptr = data + y * header->width;
        if (fread(row_ptr, sizeof(struct pixel), header->width, stream) !=
            header->width) {
            free(data);
            return NULL;
        }

        size_t padding = row_size - header->width * sizeof(struct pixel);
        for (size_t p = 0; p < padding; ++p) fgetc(stream);
    }

    return data;
}

bool write_bmp(FILE* stream, const struct bmp_image* image) {
    if (!stream || !image) return false;

    if (fwrite(image->header, sizeof(struct bmp_header), 1, stream) != 1)
        return false;

    size_t row_size = align_to_4(image->header->width * sizeof(struct pixel));
    size_t padding = row_size - image->header->width * sizeof(struct pixel);

    if (fseek(stream, image->header->offset, SEEK_SET) != 0) return false;

    for (size_t y = 0; y < image->header->height; ++y) {
        const struct pixel* row = image->data + y * image->header->width;
        if (fwrite(row, sizeof(struct pixel), image->header->width, stream) !=
            image->header->width)
            return false;

        for (size_t p = 0; p < padding; ++p) fputc(0x00, stream);
    }

    return true;
}

void free_bmp_image(struct bmp_image* image) {
    if (!image) return;
    free(image->header);
    free(image->data);
    free(image);
}
