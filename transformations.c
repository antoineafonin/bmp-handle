#include "transformations.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Оголошення функції round_to_4
size_t round_to_4(size_t n) { return n % 4 ? (n - n % 4 + 4) : n; }

// Оголошення функції get_pixel
struct pixel* get_pixel(const struct bmp_image* image, size_t x, size_t y,
                        bool pad) {
    size_t row_size = image->header->width * sizeof(struct pixel);
    if (pad) row_size = round_to_4(row_size);

    uint8_t* data = (uint8_t*)image->data;
    return (struct pixel*)(data + y * row_size + x * sizeof(struct pixel));
}

// Допоміжна функція для застосування трансформацій
static struct bmp_image* apply_transformation(
    const struct bmp_image* image,
    void (*transform_func)(const struct bmp_image*, struct bmp_image*)) {
    if (!image) return NULL;

    struct bmp_image* new_image = malloc(sizeof(struct bmp_image));
    if (!new_image) return NULL;

    new_image->header = malloc(sizeof(struct bmp_header));
    if (!new_image->header) {
        free(new_image);
        return NULL;
    }

    memcpy(new_image->header, image->header, sizeof(struct bmp_header));

    new_image->data = calloc(1, new_image->header->image_size);
    if (!new_image->data) {
        free(new_image->header);
        free(new_image);
        return NULL;
    }

    transform_func(image, new_image);
    return new_image;
}

// Реалізація фліпа по горизонталі
static void flip_horizontally_impl(const struct bmp_image* image_src,
                                   struct bmp_image* image_dst) {
    uint32_t width = image_src->header->width;
    uint32_t height = image_src->header->height;

    for (size_t i = 0; i < width; ++i) {
        for (size_t j = 0; j < height; ++j) {
            struct pixel* src_pixel =
                get_pixel(image_src, width - 1 - i, j, false);
            struct pixel* dst_pixel = get_pixel(image_dst, i, j, false);
            *dst_pixel = *src_pixel;
        }
    }
}

struct bmp_image* flip_horizontally(const struct bmp_image* image) {
    return apply_transformation(image, flip_horizontally_impl);
}

// Реалізація фліпа по вертикалі
static void flip_vertically_impl(const struct bmp_image* image_src,
                                 struct bmp_image* image_dst) {
    uint32_t width = image_src->header->width;
    uint32_t height = image_src->header->height;

    for (size_t i = 0; i < width; ++i) {
        for (size_t j = 0; j < height; ++j) {
            struct pixel* src_pixel =
                get_pixel(image_src, i, height - 1 - j, false);
            struct pixel* dst_pixel = get_pixel(image_dst, i, j, false);
            *dst_pixel = *src_pixel;
        }
    }
}

struct bmp_image* flip_vertically(const struct bmp_image* image) {
    return apply_transformation(image, flip_vertically_impl);
}

// Реалізація повороту вправо
static void rotate_right_impl(const struct bmp_image* image_src,
                              struct bmp_image* image_dst) {
    uint32_t width = image_src->header->width;
    uint32_t height = image_src->header->height;

    image_dst->header->width = height;
    image_dst->header->height = width;
    image_dst->header->image_size =
        round_to_4(sizeof(struct pixel) * height) * width;
    image_dst->header->size =
        image_dst->header->offset + image_dst->header->image_size;

    for (size_t i = 0; i < width; ++i) {
        for (size_t j = 0; j < height; ++j) {
            struct pixel* src_pixel =
                get_pixel(image_src, width - 1 - i, j, false);
            struct pixel* dst_pixel = get_pixel(image_dst, j, i, false);
            *dst_pixel = *src_pixel;
        }
    }
}

struct bmp_image* rotate_right(const struct bmp_image* image) {
    return apply_transformation(image, rotate_right_impl);
}

// Реалізація повороту вліво
static void rotate_left_impl(const struct bmp_image* image_src,
                             struct bmp_image* image_dst) {
    uint32_t width = image_src->header->width;
    uint32_t height = image_src->header->height;

    image_dst->header->width = height;
    image_dst->header->height = width;
    image_dst->header->image_size =
        round_to_4(sizeof(struct pixel) * height) * width;
    image_dst->header->size =
        image_dst->header->offset + image_dst->header->image_size;

    for (size_t i = 0; i < width; ++i) {
        for (size_t j = 0; j < height; ++j) {
            struct pixel* src_pixel =
                get_pixel(image_src, i, height - 1 - j, false);
            struct pixel* dst_pixel = get_pixel(image_dst, j, i, false);
            *dst_pixel = *src_pixel;
        }
    }
}

struct bmp_image* rotate_left(const struct bmp_image* image) {
    return apply_transformation(image, rotate_left_impl);
}

// Реалізація масштабування
struct bmp_image* scale(const struct bmp_image* image, float factor) {
    if (!image || factor <= 0.0f) return NULL;

    struct bmp_image* scaled_image = malloc(sizeof(struct bmp_image));
    scaled_image->header = malloc(sizeof(struct bmp_header));
    memcpy(scaled_image->header, image->header, sizeof(struct bmp_header));

    size_t new_width = (size_t)round(image->header->width * factor);
    size_t new_height = (size_t)round(image->header->height * factor);
    size_t pixel_size = sizeof(struct pixel);
    size_t new_data_size = round_to_4(pixel_size * new_width) * new_height;

    scaled_image->header->width = new_width;
    scaled_image->header->height = new_height;
    scaled_image->header->image_size = new_data_size;
    scaled_image->header->size = scaled_image->header->offset + new_data_size;

    size_t original_width = image->header->width;
    size_t original_height = image->header->height;

    scaled_image->data = calloc(1, new_data_size);

    for (size_t i = 0; i < new_width; ++i) {
        for (size_t j = 0; j < new_height; ++j) {
            size_t original_x = i * original_width / new_width;
            size_t original_y = j * original_height / new_height;
            struct pixel* src_pixel =
                get_pixel(image, original_x, original_y, false);
            struct pixel* dst_pixel = get_pixel(scaled_image, i, j, false);
            *dst_pixel = *src_pixel;
        }
    }

    return scaled_image;
}

// Реалізація обрізки
struct bmp_image* crop(const struct bmp_image* image, uint32_t start_x,
                       uint32_t start_y, uint32_t width, uint32_t height) {
    if (!image || start_x < 0 || start_y < 0 || width <= 0 || height <= 0 ||
        start_x + width > image->header->width ||
        start_y + height > image->header->height)
        return NULL;

    struct bmp_image* cropped_image = malloc(sizeof(struct bmp_image));
    cropped_image->header = malloc(sizeof(struct bmp_header));
    memcpy(cropped_image->header, image->header, sizeof(struct bmp_header));

    size_t pixel_size = sizeof(struct pixel);
    size_t cropped_data_size = round_to_4(pixel_size * width) * height;

    cropped_image->header->width = width;
    cropped_image->header->height = height;
    cropped_image->header->image_size = cropped_data_size;
    cropped_image->header->size =
        cropped_image->header->offset + cropped_data_size;

    cropped_image->data = calloc(1, cropped_data_size);

    for (size_t i = start_x; i < start_x + width; ++i) {
        for (size_t j = start_y; j < start_y + height; ++j) {
            struct pixel* src_pixel = get_pixel(image, i, j, false);
            struct pixel* dst_pixel =
                get_pixel(cropped_image, i - start_x, j - start_y, false);
            *dst_pixel = *src_pixel;
        }
    }

    return cropped_image;
}

// Реалізація екстракції кольорів
struct bmp_image* extract(const struct bmp_image* image,
                          const char* colors_to_keep) {
    if (!image || !colors_to_keep) return NULL;

    uint32_t color_mask = 0;
    size_t len = strlen(colors_to_keep);

    if (len > 3 || len == 0) return NULL;

    for (size_t i = 0; i < len; ++i) {
        switch (colors_to_keep[i]) {
            case 'r':
                color_mask |= 0xFF0000;
                break;
            case 'g':
                color_mask |= 0x00FF00;
                break;
            case 'b':
                color_mask |= 0x0000FF;
                break;
            default:
                return NULL;
        }
    }

    struct bmp_image* extracted_image = malloc(sizeof(struct bmp_image));
    extracted_image->header = malloc(sizeof(struct bmp_header));
    memcpy(extracted_image->header, image->header, sizeof(struct bmp_header));

    size_t total_pixels = image->header->width * image->header->height;
    size_t data_size = sizeof(struct pixel) * total_pixels;

    extracted_image->data = calloc(1, data_size);
    memcpy(extracted_image->data, image->data, data_size);

    if (color_mask != 0xFFFFFF) {
        for (size_t i = 0; i < total_pixels; ++i) {
            extracted_image->data[i].red &= (color_mask & 0xFF0000) >> 16;
            extracted_image->data[i].green &= (color_mask & 0x00FF00) >> 8;
            extracted_image->data[i].blue &= (color_mask & 0x0000FF);
        }
    }

    return extracted_image;
}
