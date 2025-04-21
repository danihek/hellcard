/*  hellcard - v0.0.1 - MIT LICENSE */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <libgen.h>
#include <strings.h>
#include <unistd.h>

#define HELL_COLORS_IMPLEMENTATION
#include "hell_colors.h"

#define HELL_PARSER_IMPLEMENTATION
#include "hell_parser.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

/***
 * MACROS
 ***/

#define PALETTE_SIZE 16

/***
 * STRUCTURES
 ***/

/* IMG
 * 
 * image structure that contains all image data,
 * we need to create color palette
 */
typedef struct
{
    /* With 3 channels it goes from 0 -> size:
     * pixels[0] = Red;
     * pixels[1] = Green;
     * pixels[2] = Blue;
     * pixels[3] = Red;
     * .. And so on
     *
     * Also, we are using uint8_t instead of RGB structure
     * here, only because I dont know how to do it.
     */

    size_t size; /* Size is width * height * channels */
    uint8_t *pixels; 

    unsigned width;
    unsigned height;
    unsigned channels;
} IMG;

/* stores all 16 RGB colors as palette */
typedef struct
{
    RGB colors[PALETTE_SIZE];
} PALETTE;

/***
 * GLOBAL VARIABLES
 ***/

/* GLOBAL COLORS */
RGB WHITE = {255, 255, 255};
RGB BLACK = {0, 0, 0};

RGB RED = {255, 0, 0};
RGB BLUE = {0, 0, 255};
RGB GREEN = {0, 255, 0};
RGB YELLOW = {255, 255, 0};
RGB AQUA = {0, 255, 255};
RGB PINK = {255, 0, 255};

/* these are being set in set_args() */
char HELLCARD_DELIM = '%';
char HELLCARD_DELIM_COUNT = 2;

/* image path, which will be used to create card */
char *IMAGE_ARG = NULL;

/* output filename */
char *OUTPUT_ARG = NULL;

/* theme path */
char *THEME_ARG = NULL;

/* style type */
char *STYLE_ARG = NULL;

/*** 
 * FUNCTIONS DECLARATIONS
 ***/

/* args */
int set_args(int argc, char *argv[]);
void hellcard_usage(const char *name);

/* logging utils */
void eu(const char *format, ...);
void err(const char *format, ...);
void warn(const char *format, ...);
void log_c(const char *format, ...);

/* themes */
char *load_file(char *filename);
char *load_theme(char *themename);

/* IMG */
IMG *img_load(char *filename);
void img_free(IMG *img);

/* color related stuff */
void print_term_colors();
void print_term_colors_small();
int color_distance(RGB c1, RGB c2);
int find_closest_color(uint8_t r, uint8_t g, uint8_t b, const PALETTE *palette);

/*** 
 * FUNCTIONS DECLARATIONS
 ***/

/* prints usage to stdout */
void hellcard_usage(const char *name)
{
    printf("Usage:\n");
    printf("  %s -i <image> [OPTIONS]\n\n", name);
    printf("Options:\n");
    printf("  -i, --image  <image>               Set input image file\n");
    printf("  -t, --theme  <file>                Set theme hellwal file\n");
    printf("  -o, --output <image>               Set output filename\n");
    printf("  -s, --style  <int>                 Set image style (1, 2)\n");
    printf("  -h, --help                         Display this help and exit\n\n");
}

/* 
 * prints to stderr formatted output and exits with EXIT_FAILURE,
 * also prints hellcard_usage()
 */
void eu(const char *format, ...)
{
    fprintf(stderr, "\033[91m[ERROR]: ");

    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);

    fprintf(stderr, "\033[0m\n");

    hellcard_usage("hellcard");

    exit(EXIT_FAILURE);
}

/* prints to stderr formatted output and exits with EXIT_FAILURE */
void err(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);

    fprintf(stderr, "\033[91m[ERROR]: ");
    vfprintf(stderr, format, ap);
    fprintf(stderr, "\033[0m");

    va_end(ap);
    fprintf(stderr, "\n");

    exit(EXIT_FAILURE);
}

/* prints to stderr formatted output, but not exits */
void warn(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    fprintf(stderr, "\033[93m[WARN]: ");

    vfprintf(stderr, format, ap);
    fprintf(stderr, "\033[0m");
    va_end(ap);
    fprintf(stderr, "\n");
}

/* prints formatted output to stdout with colors */
void log_c(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);

    const char *default_color = "\033[96m";

    fprintf(stdout, "%s[INFO]: ", default_color);

    vfprintf(stdout, format, ap);
    fprintf(stdout, "\033[0m");

    va_end(ap);
    fprintf(stdout, "\n");
}

/* set given arguments */
int set_args(int argc, char *argv[])
{
    int j = 0; /* 'int i' from for loop counter to track incomplete option error */

    for (int i = 1; i < argc; i++)
    {
        j = i;

        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
        {
            hellcard_usage(argv[0]);
            exit(EXIT_SUCCESS);
        }
        else if ((strcmp(argv[i], "--image") == 0 || strcmp(argv[i], "-i") == 0))
        {
            if (i + 1 < argc)
                IMAGE_ARG = argv[++i];
            else {
                argc = -1;
            }
        }
        else if ((strcmp(argv[i], "--output") == 0 || strcmp(argv[i], "-o") == 0))
        {
            if (i + 1 < argc)
                OUTPUT_ARG = argv[++i];
            else {
                argc = -1;
            }
        }
        else if ((strcmp(argv[i], "--theme") == 0 || strcmp(argv[i], "-t") == 0))
        {
            if (i + 1 < argc)
                THEME_ARG = argv[++i];
            else {
                argc = -1;
            }
        }
        else if ((strcmp(argv[i], "--style") == 0 || strcmp(argv[i], "-s") == 0))
        {
            if (i + 1 < argc)
                STYLE_ARG = argv[++i];
            else {
                argc = -1;
            }
        }
        else {
            eu("Unknown option: %s", argv[i]);
        }
    }

    if (argc == -1)
        err("Incomplete option: %s", argv[j]);

    if (IMAGE_ARG == NULL)
        eu("No image provided");

    if (OUTPUT_ARG == NULL)
    {
        const char delim = '.';
        //char *folder = "samples/";
        char *o_str = "_hellcard.png";
        char *img_arg = basename(strdup(IMAGE_ARG));
        strtok(img_arg, &delim);
        size_t o_size = strlen(img_arg) + strlen(o_str);// + strlen(folder);

        OUTPUT_ARG = malloc(o_size);
        bzero(OUTPUT_ARG, o_size);
        //strcat(OUTPUT_ARG, folder);
        strcat(OUTPUT_ARG, img_arg);
        strcat(OUTPUT_ARG, o_str);
    }
    
    if (THEME_ARG == NULL)
    {
        char *home = "/home/";
        char *login = getlogin();
        char *cache_hellwal = ".cache/hellwal/colors.hellwal";

        size_t path_size = strlen(home) + strlen(login) + strlen(cache_hellwal) + strlen("/");
        THEME_ARG  = malloc(path_size);
        bzero(THEME_ARG , path_size);
        strcat(THEME_ARG , home);
        strcat(THEME_ARG , login);
        strcat(THEME_ARG , "/");
        strcat(THEME_ARG , cache_hellwal);
    }

    return 0;
}

// prints current terminal colors
void print_term_colors()
{
    for (int i = 0; i < PALETTE_SIZE; i++)
    {
        // set foreground color
        printf("\033[38;5;%dm", i);
        printf(" FG %2d ", i);

        // reset and set background color
        printf("\033[0m");
        printf("\033[48;5;%dm", i);
        printf(" BG %2d ", i);

        // reset again
        printf("\033[0m\n");
    }
    printf("\n");

    print_term_colors_small();

    // reset at the end
    printf("\033[0m\n");
}

void print_term_colors_small()
{
    for (int i=0; i<PALETTE_SIZE; i++)
    {
        if (i == PALETTE_SIZE/2)
            printf("\n");

        //printf("\033[38;5;%dm", i); // if you want to set foreground color
        printf("\033[48;5;%dm", i);   // if you want to set background color
        printf("   ");                // two spaces as a "block" of a color
    }
    // reset at the end
    printf("\033[0m\n");
}

/* Load image file using stb, return IMG structure */
IMG *img_load(char *filename)
{
    if (IMAGE_ARG == NULL)
        err("No image provided");
    log_c("Loading image %s", IMAGE_ARG);

    int width, height;
    int numberOfChannels;

    uint8_t *imageData = stbi_load(filename, &width, &height, &numberOfChannels, 3);

    if (imageData == 0) err("Error while loading the file: %s", filename);

    IMG *img = malloc(sizeof(IMG));

    img->width = width;
    img->height = height;
    img->pixels = imageData;
    img->channels = 3;
    img->size = width * height * numberOfChannels;

    log_c("Loaded!");
    return img;
}

void img_print_stats(IMG *img)
{
    log_c("-=-=-=-=-= )%s( =-=-=-=-=-", IMAGE_ARG);
    log_c("size:%d", img->size);
    log_c("--------------------------");
    log_c("width:%d", img->width);
    log_c("height:%d", img->height);
    log_c("channels:%d", img->channels);
    log_c("-=-=-=-=-=-=-=-=-=-=-=-=-=");
}

/* free all allocated stuff in IMG */
void img_free(IMG *img)
{
    stbi_image_free(img->pixels);
    free(img);
}

int is_color_palette_var(char *name)
{
    for (size_t i = 0; i < PALETTE_SIZE; i++) {
        char *col = calloc(1, 16);
        sprintf(col, "color%lu", i);

        if (strcmp(col, name) == 0)
            return i;
    }
    return -1;
}

void remove_whitespaces(char *str)
{
    if (!str) return;

    char *read = str;
    char *write = str;

    while (*read) {
        if (*read != ' ' && *read != '\t' && *read != '\n' && *read != '\r') {
            *write++ = *read;
        }
        read++;
    }
    *write = '\0';
}

int hex_to_rgb(const char *hex, RGB *p)
{
    if (!hex || hex[0] != '#' || (strlen(hex) != 7 && strlen(hex) != 4)) {
        return 0;
    }

    if (strlen(hex) == 7) {
        // #RRGGBB format
        p->R = (int)strtol(hex + 1, NULL, 16) >> 16 & 0xFF;
        p->G = (int)strtol(hex + 3, NULL, 16) >> 8 & 0xFF;
        p->B = (int)strtol(hex + 5, NULL, 16) & 0xFF;
    } else if (strlen(hex) == 4) {
        // #RGB format (expand each digit to 2 digits)
        p->R = (int)strtol((char[]){hex[1], hex[1], '\0'}, NULL, 16);
        p->G = (int)strtol((char[]){hex[2], hex[2], '\0'}, NULL, 16);
        p->B = (int)strtol((char[]){hex[3], hex[3], '\0'}, NULL, 16);
    }

    return 1;
}

/* process theme, return color palette - return 0 on error */
int color_palette_from_file(char *t, PALETTE *pal)
{
    if (t == NULL)
        return 0;

    int processed_colors = 0;
    hell_parser_t *p = hell_parser_create(t);

    if (p == NULL) 
    {
        printf("Failed to create parser\n");
        exit(EXIT_FAILURE);
    }

    while (!hell_parser_eof(p))
    {
        char ch;
        if (hell_parser_next(p, &ch) == HELL_PARSER_OK)
        {
            if (ch == HELLCARD_DELIM)
            {
                p->pos -= 1;  
                char *delim_buf = NULL;

                if (hell_parser_delim_buffer_between(p, HELLCARD_DELIM, HELLCARD_DELIM_COUNT, &delim_buf) == HELL_PARSER_OK)
                {
                    hell_parser_t *pd = hell_parser_create(delim_buf);
                    if (pd == NULL)
                    {
                        printf("Failed to allocate parser\n");
                        exit(EXIT_FAILURE);
                    }

                    if (hell_parser_delim(pd, '=', 1) == HELL_PARSER_OK)
                    {
                        size_t var_size = pd->pos - 1;
                        size_t val_size = pd->length - pd->pos + 1;

                        char *variable = calloc(1, var_size);
                        char *value = calloc(1, val_size);

                        strncpy(variable, pd->input, var_size);
                        strncpy(value, pd->input + pd->pos + 1, val_size);

                        remove_whitespaces(variable);
                        remove_whitespaces(value);

                        RGB p;
                        if (hex_to_rgb(value, &p))
                        {
                            int idx = is_color_palette_var(variable);
                            if (idx != -1) {
                                pal->colors[idx] = p;
                                processed_colors++;
                            }
                        }
                        else
                        {
                            free(variable);
                            free(value);
                        }
                    }

                    free(delim_buf);
                }
            }
        }
    }
    hell_parser_destroy(p);

    if (processed_colors == PALETTE_SIZE)
        return 1;

    return 0;
}

char *load_file(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return NULL;

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);

    char *buffer = (char*)malloc(size + 1); // +1 for null terminator
    if (buffer == NULL)
    {
        printf("Failed to allocate memory for file buffer\n");
        fclose(file);
        return NULL;
    }

    int bytes_read = fread(buffer, 1, size, file);
    if (bytes_read != size)
    {
        perror("Failed to read the complete file");
        free(buffer);
        fclose(file);
        return NULL;
    }
    buffer[size] = '\0';

    fclose(file);

    return buffer;
}

char *load_theme(char *themename)
{
    printf("Loading theme: %s\n", themename);
    char *t = load_file(themename);

    if (t != NULL)
        return t;

    err("Failed to open file: %s\n", themename);
    return NULL;
}

PALETTE process_theme(char *theme)
{
    char *t = load_theme(theme);
    PALETTE pal;

    if (t!=NULL)
    {
        if (!color_palette_from_file(t, &pal))
        {
            printf("Not enough colors were specified in color palette: %s", theme);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        err("Theme not found: %s", theme);
    }

    return pal;
}

void apply_palette_filter(IMG *img, const PALETTE *palette) {
    for (size_t i = 0; i < img->size; i += 3) {
        uint8_t r = img->pixels[i];
        uint8_t g = img->pixels[i + 1];
        uint8_t b = img->pixels[i + 2];

        int index = find_closest_color(r, g, b, palette);
        RGB new_color = palette->colors[index];

        img->pixels[i]     = new_color.R;
        img->pixels[i + 1] = new_color.G;
        img->pixels[i + 2] = new_color.B;
    }
}

/* Writes color as block to stdout - it does not perform new line by itself */
void print_color(RGB col)
{
    char *color_block = "   ";                 // color_block is 3 spaces wide
                                              // color_block + ↓↓↓↓↓↓
    /* Write color from as colored block */
    fprintf(stdout, "\x1b[48;2;%d;%d;%dm%s\033[0m", col.R, col.G, col.B, color_block);
}

/* Writes palete to stdout */
void print_palette(PALETTE pal)
{
    for (size_t i=0; i<PALETTE_SIZE; i++)
    {
        print_color(pal.colors[i]);
        if (i+1 == PALETTE_SIZE/2) printf("\n");
    }
    printf("\n");
}

int find_closest_color(uint8_t r, uint8_t g, uint8_t b, const PALETTE *palette) {
    int best_index = 0;
    int min_dist = INT32_MAX;

    for (int i = 0; i < PALETTE_SIZE; ++i) {
        RGB c = palette->colors[i];
        int dist = color_distance((RGB){r, g, b}, c);
        if (dist < min_dist) {
            min_dist = dist;
            best_index = i;
        }
    }
    return best_index;
}

int color_distance(RGB c1, RGB c2) {
    int dr = c1.R - c2.R;
    int dg = c1.G - c2.G;
    int db = c1.B - c2.B;
    return dr * dr + dg * dg + db * db;
}

void draw_rect(IMG *img, unsigned r_width, unsigned r_height, int posx, int posy, RGB color)
{
    if (img == NULL)
        err("Image is NULL");

    unsigned iw = img->width;
    unsigned ih = img->height;

    for (unsigned y = 0; y < r_height; y++)
    {
        unsigned py = posy + y;
        if (py >= ih) continue;

        for (unsigned x = 0; x < r_width; x++)
        {
            unsigned px = posx + x;
            if (px >= iw) continue;

            unsigned index = (py * iw + px) * 3;

            img->pixels[index]     = color.R;
            img->pixels[index + 1] = color.G;
            img->pixels[index + 2] = color.B;
        }
    }
}

void draw_rect_w_3(
        IMG *img, unsigned r_width, unsigned r_height,
        int posx, int posy, RGB color,
        unsigned border_width, RGB border_color
        )
{

    /* border */
    draw_rect(img, r_width, r_height, posx, posy, border_color);

    //r_width -= border_width;
    //r_height -= border_width;
    posx += border_width;
    posy += border_width;

    /* actual rectangle */
    draw_rect(img, r_width, r_height, posx, posy, color);
}

void draw_rect_w_border(
        IMG *img, unsigned r_width, unsigned r_height,
        int posx, int posy, RGB color,
        unsigned border_width, RGB border_color
        )
{

    /* border */
    draw_rect(img, r_width, r_height, posx, posy, border_color);

    r_width -= border_width * 2;
    r_height -= border_width * 2;
    posx += border_width;
    posy += border_width;

    /* actual rect */
    draw_rect(img, r_width, r_height, posx, posy, color);
}

void draw_border(IMG *img, unsigned width, RGB color)
{
    if (img == NULL)
        err("Image is NULL");

    unsigned iw = img->width;
    unsigned ih = img->height;

    draw_rect(img, iw, width, 0, 0, color);
    draw_rect(img, width, ih, 0, 0, color);

    draw_rect(img, width, ih, iw-width, 0, color);
    draw_rect(img, iw, width, 0, ih-width, color);
}

void draw_palette(IMG *img, PALETTE p, unsigned posx, unsigned posy, unsigned spacing, unsigned border, unsigned size)
{
    printf("mult: %d\n", img->width * img->height);
    printf("size: %d\n", size);
    printf("border: %d\n", border);
    printf("spacing: %d\n", spacing);

    int x = posx;
    int y = posy;

    for (size_t i = 0; i < PALETTE_SIZE; i++)
    {
        if (i==PALETTE_SIZE/2)
        {
            x =  posx;
            y += size + spacing;
        }

        draw_rect_w_border(img, size, size, x, y, p.colors[i],
                border, p.colors[14]);
        x += size + spacing;
    }
}

void crop(IMG *img, unsigned x1, unsigned y1, unsigned x2, unsigned y2)
{
    if (img == NULL || img->pixels == NULL)
        err("Image is NULL");

    if (x2 <= x1 || y2 <= y1 || x2 > img->width || y2 > img->height)
        err("Invalid crop dimensions");

    unsigned iw = x2 - x1;
    unsigned ih = y2 - y1;
    unsigned channels = img->channels;

    uint8_t *cropped = malloc(iw * ih * channels);
    if (!cropped)
        err("Memory allocation failed");

    for (unsigned y = 0; y < ih; y++)
    {
        for (unsigned x = 0; x < iw; x++)
        {
            for (unsigned c = 0; c < channels; c++)
            {
                cropped[(y * iw + x) * channels + c] =
                    img->pixels[((y + y1) * img->width + (x + x1)) * channels + c];
            }
        }
    }

    // free old pixel data, swap with new one and update
    free(img->pixels);
    img->pixels = cropped;
    img->width = iw;
    img->height = ih;
}

void paste(IMG *dst, IMG *src, unsigned dest_x, unsigned dest_y)
{
    if (!dst || !dst->pixels || !src || !src->pixels)
        err("NULL image in paste");

    if (dst->channels != src->channels)
        err("Channel mismatch between images");

    unsigned channels = dst->channels;

    for (unsigned y = 0; y < src->height; y++)
    {
        if (y + dest_y >= dst->height)
            continue; // skip rows that are out of bounds

        for (unsigned x = 0; x < src->width; x++)
        {
            if (x + dest_x >= dst->width)
                continue; // skip columns that are out of bounds

            for (unsigned c = 0; c < channels; c++)
            {
                dst->pixels[((y + dest_y) * dst->width + (x + dest_x)) * channels + c] =
                    src->pixels[(y * src->width + x) * channels + c];
            }
        }
    }
}

IMG *create_img(unsigned width, unsigned height, unsigned channels, RGB color)
{
    if (channels < 3 || channels > 4)
        err("Only 3 (RGB) or 4 (RGBA) channels are supported");

    IMG *img = malloc(sizeof(IMG));
    if (!img)
        err("Failed to allocate IMG");

    img->width = width;
    img->height = height;
    img->channels = channels;

    size_t size = (size_t)width * height * channels;
    img->pixels = malloc(size);
    if (!img->pixels)
    {
        free(img);
        err("Failed to allocate pixel data");
    }

    for (unsigned i = 0; i < width * height; i++)
    {
        img->pixels[i * channels + 0] = color.R;
        img->pixels[i * channels + 1] = color.G;
        img->pixels[i * channels + 2] = color.B;

        if (channels == 4)
            img->pixels[i * channels + 3] = 255;
    }

    return img;
}

void fill(IMG *img, RGB color)
{
    if (!img || !img->pixels)
        err("Image is NULL");

    unsigned total_pixels = img->width * img->height;
    unsigned ch = img->channels;

    if (ch < 3)
        err("Unsupported image format: must have at least 3 channels");

    for (unsigned i = 0; i < total_pixels; i++)
    {
        img->pixels[i * ch + 0] = color.R;
        img->pixels[i * ch + 1] = color.G;
        img->pixels[i * ch + 2] = color.B;

        if (ch == 4)
            img->pixels[i * ch + 3] = 255; // full alpha
    }
}

void style1(IMG *img, PALETTE p)
{
    print_palette(p);

    img_print_stats(img);

    unsigned x1 = img->width * 0.3;
    unsigned x2 = img->width * 0.7;
    unsigned y1 = img->height * 0.1;
    unsigned y2 = img->height * 0.9;

    if (img->width > img->height)
    {
        x1 = img->width * 0.3;
        x2 = img->width * 0.7;
        y1 = img->height * 0.1;
        y2 = img->height * 0.9;
    }
    else
    {
        x1 = img->width * 0.1;
        x2 = img->width * 0.9;
        y1 = img->height * 0.3;
        y2 = img->height * 0.7;
    }

    log_c("x1:%d", x1);
    log_c("y1:%d", y1);
    log_c("x2:%d", x2);
    log_c("y2:%d", y2);

    crop(img, x1, y1, x2, y2);
    img_print_stats(img);

    draw_border(img, 10, p.colors[14]);
    draw_border(img, 5,  p.colors[15]);

    unsigned px = img->width * 0.03;
    unsigned py = img->height * 0.03;

    unsigned spacing = img->width * img->height * 0.000007;
    unsigned border = img->width * img->height * 0.000003;
    unsigned size = img->width * img->height * 0.00006;

    draw_palette(img, p, px, py, spacing, border, size );

    stbi_write_png(OUTPUT_ARG, img->width, img->height, img->channels, img->pixels, img->width * img->channels);
}

void style2(IMG *img, PALETTE p)
{
    print_palette(p);

    img_print_stats(img);

    unsigned factor = 0;
    unsigned x1 = img->width * 0.3;
    unsigned x2 = img->width * 0.7;
    unsigned y1 = img->height * 0.1;
    unsigned y2 = img->height * 0.9;

    if (img->width > img->height)
    {
        factor = img->height * 0.3;

        x1 = img->width * 0.3;
        x2 = img->width * 0.3 + factor;
        y1 = img->height * 0.3;
        y2 = img->height * 0.3 + factor;
    }
    else
    {
        factor = img->width * 0.3;

        x1 = img->width * 0.3;
        x2 = img->width * 0.3 + factor;
        y1 = img->height * 0.3;
        y2 = img->height * 0.3 + factor;
    }

    log_c("x1:%d", x1);
    log_c("y1:%d", y1);
    log_c("x2:%d", x2);
    log_c("y2:%d", y2);

    crop(img, x1, y1, x2, y2);
    img_print_stats(img);

    unsigned ss = factor*2;
    unsigned ssx = (ss/2)-(img->width/2);
    unsigned ssy = (ss/3)-(img->width/2);

    IMG *blank = create_img(ss, ss, img->channels, p.colors[15]); 

    draw_rect_w_border(blank, img->width, img->height, ssx, ssy, p.colors[14],
            5, p.colors[15]);
    paste(blank, img, ssx+20, ssy+20);

    draw_border(img, 10, p.colors[14]);
    draw_border(img, 5,  p.colors[15]);

    unsigned spacing = blank->width * blank->height * 0.000008;
    unsigned border = blank->width * blank->height * 0.000005;
    unsigned size = blank->width * blank->height * 0.00008;

    unsigned total_w = spacing * 8  + size * 8 ;
    unsigned total_h = spacing * 2 + size * 2;

    unsigned ppx = (blank->width/2) - (total_w/2);
    unsigned ppy = blank->height*0.9 - total_h * 0.5 - size * 0.5;

    draw_palette(blank, p, ppx, ppy, spacing, border, size);
    draw_border(blank, 10, p.colors[5]);
    draw_border(blank, 5, p.colors[15]);

    stbi_write_png(OUTPUT_ARG, blank->width, blank->height, blank->channels, blank->pixels, blank->width * blank->channels);
}

int main(int argc, char **argv)
{
    /* read cmd line arguments, and set default ones */
    if (set_args(argc,argv) != 0)
        err("arguments error");


    PALETTE p = process_theme(THEME_ARG);
    IMG *img = img_load(IMAGE_ARG);

    if (STYLE_ARG != NULL && !strcmp(STYLE_ARG, "1"))
    {
        style1(img, p);
    }
    else
        style2(img, p);

    stbi_image_free(img->pixels);
    free(img);
    return 0;
}
