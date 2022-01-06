#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <dlfcn.h>

#include "image.h"
#include "image_plugin.h"

int init_plugin(Plugin *p, char *addr) {
    p->handle = dlopen(addr, RTLD_LAZY);

    if (!p->handle) {
        return 1;
    }

    *(void **) (&(p->get_plugin_name)) = dlsym(p->handle, "get_plugin_name");
    *(void **) (&(p->get_plugin_desc)) = dlsym(p->handle, "get_plugin_desc");
    *(void **) (&(p->parse_arguments)) = dlsym(p->handle, "parse_arguments");
    *(void **) (&(p->transform_image)) = dlsym(p->handle, "transform_image");

    return 0;
}

Plugin_lib *get_plugins() {
    Plugin_lib *pluglib = malloc(sizeof(Plugin_lib));
    Plugin *lib = malloc(10 * sizeof(Plugin));

    struct dirent *pDirent;
    DIR *pDir;
    Plugin *p = lib;

    char addr[100] = "./plugins/";
    char *env_pathname = getenv("PLUGIN_DIR");
    int s = 0;
    
    if(env_pathname != NULL) {
        strcpy(addr, env_pathname);
        strcat(addr, "/");
    }

    pDir = opendir(addr);
    if (pDir == NULL) {
        printf("Cannot open plugins directory\n");
        free(pluglib);
        free(lib);
        exit(1);
    }

    while ((pDirent = readdir(pDir)) != NULL) {
        char path[100];
        strcpy(path, addr);
        char *filename = pDirent->d_name;
        strcat(path, filename);
        
        if(strcmp(filename, ".") != 0 && strcmp(filename, "..") != 0) {
            init_plugin(p, path);
            p++;
            s++;
        }
    }

    closedir(pDir);

    pluglib->size = s;
    pluglib->lib = lib;
    return pluglib;
}

Plugin *find_plugin(char *pname, Plugin *p, int size) {
    for(int i = 0; i < size; i++) {
        if(strcmp(p->get_plugin_name(), pname) == 0) {
            return p;
        }
        p++;
    }

    return NULL;
}


void default_print() {
    printf("Usage: imgproc <command> [<command args...>]\n");
    printf("Commands are:\n");
    printf("\tlist\n");
    printf("\texec <plugin> <input img> <output img> [<plugin args...>]\n");
}

void list_print(Plugin *p, int size) {
    printf("Loaded %d plugin(s)\n", size);
    for(int i = 0; i < size; i++) {
        printf("\t%s: %s\n", p->get_plugin_name(), p->get_plugin_desc());
        p++;
    }
}

void clean_up(Plugin_lib *plib) {
    Plugin *p = plib->lib;
    int size = plib->size;

    for(int i = 0; i < size; i++) {
        if(p->handle != NULL) {
            dlclose(p->handle);
        }
        p++;
    }
    
    free(plib->lib);
    free(plib);
}

void print_flag(int f) {
    if(f == 1) {
        fprintf(stderr, "Error: invalid image");
    }

    if(f == 2) {
        fprintf(stderr, "Error: invalid plugin");
    }

    if(f == 3) {
        fprintf(stderr, "Error: invalid plugin arguments");
    }
}

int main(int argc, char **argv) {
    Plugin_lib *plib = get_plugins();
    Plugin *p = plib->lib;
    int size = plib->size;
    int flag = 0;

    if(argc == 1) {
        default_print();
    } else if(argc == 2 && strcmp(argv[1], "list") == 0) {
        list_print(p, size);
    } else if(argc >= 5 && strcmp(argv[1], "exec") == 0) {
        struct Image *in = img_read_png(argv[3]);
        p = find_plugin(argv[2], p, size);
        void *args = NULL;

        if(in == NULL) {
            flag = 1;
        } else if(p == NULL) {
            flag = 2;
        } else {
            args = p->parse_arguments(argc - 5, &argv[5]);
        }

        if (args != NULL) {
            struct Image *out = p->transform_image(in, args);
            img_write_png(out, argv[4]);

            free(out->data);
            free(out);
        } else {
            flag = 3;
        }

        if(in != NULL) {
            free(in->data);
            free(in);
        }
    }

    clean_up(plib);
    print_flag(flag);
    return flag;
}
