//
// Example plugin: it just swaps the blue and green color component
// values for each pixel in the source image.
//
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "image_plugin.h"

struct Arguments {
	// exposure factor
	uint32_t factor;
};

const char *get_plugin_name(void) {
	return "tile";
}

const char *get_plugin_desc(void) {
	return "tile source image in an NxN arrangement";
}

void *parse_arguments(int num_args, char *args[]) {
	if (num_args != 1) {
		return NULL;
	}

    int i = 0;
    while(args[0][i] != '\0') {
        if (!isdigit(args[0][i])) {
            return NULL;
        }
        i++;
    }

    int f;
    sscanf(args[0], "%d", &f);

    struct Arguments *arg_struct = calloc(1, sizeof(struct Arguments));
    arg_struct->factor = (uint32_t) f;
    return arg_struct;
}

// Helper function to fill in one tile in new image
void fill_tile(struct Image *source, struct Image *out, int factor, int x, int y, int x_end, int y_end) {
	for (int i = 0; i < y_end - y; i++) {
        for (int j = 0; j < x_end - x; j++) {
            int pix = (i + y) * source->width + (j + x);
            int old_pix = (factor * i) * source->width + (factor * j);
            //printf("%d\t%d\t||\t%d\t%d\n", i + y, j + x, factor * i, factor * j);
            out->data[pix] = source->data[old_pix];
        }
    }

}

struct Image *transform_image(struct Image *source, void *arg_data) {
	struct Arguments *args = arg_data;
    uint32_t factor = args->factor;
    free(args);

	// Allocate a result Image
	struct Image *out = img_create(source->width, source->height);
	if (!out) {
		return NULL;
	}
    // create an array for all the (x,y) coords of the upper left of each new tile
    int x_grid[factor + 1];
    int y_grid[factor + 1];

    x_grid[0] = 0;
    y_grid[0] = 0;
    x_grid[factor] = source->width;
    y_grid[factor] = source->height;

    for(uint32_t p = 1; p < factor; p++) {
        x_grid[p] = x_grid[p-1] + source->width / factor;
        y_grid[p] = y_grid[p-1] + source->height / factor;

        if(p - 1 < source->width - (source->width / factor) * factor) {
            x_grid[p]++;
        }

        if(p - 1 < source->height - (source->height / factor) * factor) {
            y_grid[p]++;
        }
    }

    // fill in each tile
    for(uint32_t py = 0; py < factor; py++) {
        for(uint32_t px = 0; px < factor; px++) {
            fill_tile(source, out, factor, x_grid[px], y_grid[py], x_grid[px+1], y_grid[py+1]);
            printf("%d\t%d\n", x_grid[px], y_grid[py]);
        }
    }

	return out;
}
