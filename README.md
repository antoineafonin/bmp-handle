# bmp-handle

This C program processes BMP image files using a series of operations

Here’s an example of documentation for the given code, describing the functionality and purpose of each module, function, and the overall structure:

---

# **BMP Image Manipulation Library Documentation**

This library provides functionality to load, manipulate, and save BMP image files. The available operations include flipping images horizontally and vertically, rotating images to the left and right, scaling images, cropping a region of the image, and extracting specific color channels. The code also contains utility functions for handling pixel data.

### **Table of Contents**

1. [Data Structures](#data-structures)
2. [Functions](#functions)
    - [Main Functions](#main-functions)
    - [Helper Functions](#helper-functions)
3. [Compilation and Usage](#compilation-and-usage)

## **Data Structures**

### `struct bmp_header`

Contains metadata about the BMP image file, such as the file size, image width, height, bit depth, and the offset where the pixel data starts.

```c
struct bmp_header {
    uint16_t type;              // "BM" (0x42, 0x4D)
    uint32_t size;              // File size
    uint16_t reserved1;         // Not used
    uint16_t reserved2;         // Not used
    uint32_t offset;            // Offset to image data
    uint32_t dib_size;          // DIB header size
    uint32_t width;             // Width of the image
    uint32_t height;            // Height of the image
    uint16_t planes;            // Must be 1
    uint16_t bpp;               // Bits per pixel
    uint32_t compression;       // Compression type (0 for no compression)
    uint32_t image_size;        // Size of the image data
    uint32_t x_ppm;             // Pixels per meter in X direction
    uint32_t y_ppm;             // Pixels per meter in Y direction
    uint32_t num_colors;        // Number of colors in the palette
    uint32_t important_colors;  // Number of important colors
};
```

### `struct pixel`

Represents a pixel in the BMP image, containing the red, green, and blue color values.

```c
struct pixel {
    uint8_t blue;  // Blue channel
    uint8_t green; // Green channel
    uint8_t red;   // Red channel
};
```

### `struct bmp_image`

Represents the full image, including the header and pixel data.

```c
struct bmp_image {
    struct bmp_header* header;  // Pointer to the BMP header
    struct pixel* data;         // Pointer to the pixel data
};
```

## **Functions**

### **Main Functions**

#### `struct bmp_image* read_bmp(FILE* stream)`

Reads a BMP image from the provided file stream. It returns a pointer to a `bmp_image` structure containing the image header and pixel data.

**Parameters**:

-   `stream`: The input file stream where the BMP image is located.

**Returns**:

-   A pointer to a `bmp_image` structure or `NULL` if an error occurs.

---

#### `bool write_bmp(FILE* stream, const struct bmp_image* image)`

Writes a BMP image to the provided file stream. It saves the image header and pixel data to the output stream.

**Parameters**:

-   `stream`: The output file stream where the BMP image will be written.
-   `image`: The BMP image to be saved.

**Returns**:

-   `true` if the image was successfully written, `false` if an error occurs.

---

#### `struct bmp_image* flip_horizontally(const struct bmp_image* image)`

Creates a horizontally flipped copy of the provided image.

**Parameters**:

-   `image`: The image to be flipped.

**Returns**:

-   A new `bmp_image` structure containing the horizontally flipped image.

---

#### `struct bmp_image* flip_vertically(const struct bmp_image* image)`

Creates a vertically flipped copy of the provided image.

**Parameters**:

-   `image`: The image to be flipped.

**Returns**:

-   A new `bmp_image` structure containing the vertically flipped image.

---

#### `struct bmp_image* rotate_right(const struct bmp_image* image)`

Creates a 90-degree clockwise rotated copy of the provided image.

**Parameters**:

-   `image`: The image to be rotated.

**Returns**:

-   A new `bmp_image` structure containing the rotated image.

---

#### `struct bmp_image* rotate_left(const struct bmp_image* image)`

Creates a 90-degree counterclockwise rotated copy of the provided image.

**Parameters**:

-   `image`: The image to be rotated.

**Returns**:

-   A new `bmp_image` structure containing the rotated image.

---

#### `struct bmp_image* scale(const struct bmp_image* image, float factor)`

Creates a scaled copy of the image by resizing it based on the provided factor.

**Parameters**:

-   `image`: The image to be scaled.
-   `factor`: The scaling factor. Values greater than 1 increase the size, values less than 1 decrease it.

**Returns**:

-   A new `bmp_image` structure containing the scaled image.

---

#### `struct bmp_image* crop(const struct bmp_image* image, uint32_t start_x, uint32_t start_y, uint32_t width, uint32_t height)`

Creates a cropped copy of the image based on the specified rectangle starting at `(start_x, start_y)` with the specified width and height.

**Parameters**:

-   `image`: The image to be cropped.
-   `start_x`: The x-coordinate of the top-left corner of the cropping region.
-   `start_y`: The y-coordinate of the top-left corner of the cropping region.
-   `width`: The width of the cropping region.
-   `height`: The height of the cropping region.

**Returns**:

-   A new `bmp_image` structure containing the cropped image.

---

#### `struct bmp_image* extract(const struct bmp_image* image, const char* colors_to_keep)`

Extracts specific color channels from the image. The `colors_to_keep` parameter can include any combination of `'r'`, `'g'`, and `'b'` to keep the red, green, and blue channels, respectively.

**Parameters**:

-   `image`: The image to process.
-   `colors_to_keep`: A string specifying which color channels to keep (e.g., "rgb", "r", etc.).

**Returns**:

-   A new `bmp_image` structure containing the image with the specified color channels.

---

### **Helper Functions**

#### `struct pixel* get_pixel(const struct bmp_image* image, size_t x, size_t y, bool pad)`

Accesses a specific pixel in the image at coordinates `(x, y)`. If `pad` is `true`, the row size will be padded to a multiple of 4.

**Parameters**:

-   `image`: The image containing the pixel data.
-   `x`: The x-coordinate of the pixel.
-   `y`: The y-coordinate of the pixel.
-   `pad`: If true, padding will be applied to the row size.

**Returns**:

-   A pointer to the specified `pixel`.

---

### **Compilation and Usage**

To compile the program, use the following `Makefile`:

```makefile
OUTPUT=bmp-handle
CC=gcc
CFLAGS=-std=c11 -Wall -Werror
C_FILES=main.c transformations.c bmp.c

all: bmp-handle

bmp-handle: main.o transformations.o bmp.o
	@echo "Building application"
	$(CC) $(CFLAGS) $^ -o $(OUTPUT)

main.o: main.c
	@echo "Building main.c file"
	$(CC) $(CFLAGS) -c $^ -o $@

transformations.o: transformations.c
	@echo "Building transformations.c file"
	$(CC) $(CFLAGS) -c $^ -o $@

bmp.o: bmp.c
	@echo "Building bmp.c file"
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	@echo "Cleaning project..."
	rm -rf *.o $(OUTPUT)
```

---

### **Conclusion**

This library offers simple and effective manipulation of BMP images, including transformations such as flipping, rotating, scaling, cropping, and extracting color channels. The provided functions can be used to easily apply these transformations to BMP images loaded from a file, and the results can be saved back to a file.
