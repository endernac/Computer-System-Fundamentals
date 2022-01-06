//
// Example plugin: it just swaps the blue and green color component
// values for each pixel in the source image.
//
#include <stdio.h>
#include <stdlib.h>
#include "image_plugin.h"

struct Arguments {
	// exposure factor
	float factor;
};

const char *get_plugin_name(void) {
	return "expose";
}

const char *get_plugin_desc(void) {
	return "adjust the intensity of all pixels";
}

void *parse_arguments(int num_args, char *args[]) {
	if (num_args != 1) {
		return NULL;
	}

    float f;
    sscanf(args[0], "%f", &f);
    if (f < 0) {
        return NULL;
    }

    struct Arguments *arg_struct = calloc(1, sizeof(struct Arguments));
    arg_struct->factor = f;
    return arg_struct;
}

// Helper function to swap the blue and green color component values.
static uint32_t expose(uint32_t pix, float factor) {
	uint8_t r, g, b, a;
	img_unpack_pixel(pix, &r, &g, &b, &a);

    uint32_t new_r = factor * r;
    uint32_t new_g = factor * g;
    uint32_t new_b = factor * b;

    if(new_r > 255) {
        new_r = 255;
    }
    if(new_g > 255) {
        new_g = 255;
    }
    if(new_b > 255) {
        new_b = 255;
    }

	return img_pack_pixel((uint8_t)new_r, (uint8_t)new_g, (uint8_t)new_b, a);
}

struct Image *transform_image(struct Image *source, void *arg_data) {
	struct Arguments *args = arg_data;
    float factor = args->factor;
    free(args);

	// Allocate a result Image
	struct Image *out = img_create(source->width, source->height);
	if (!out) {
		return NULL;
	}

	unsigned num_pixels = source->width * source->height;
	for (unsigned i = 0; i < num_pixels; i++) {
		out->data[i] = expose(source->data[i], factor);
	}

	return out;
}
