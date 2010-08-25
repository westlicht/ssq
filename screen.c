
#include <stdlib.h>
#include <stdio.h>

#include "SDL.h"
#include "SDL/SDL_gfxPrimitives.h"

#include "log.h"
#include "core.h"
#include "param.h"
#include "seq.h"
#include "pattern.h"
#include "line.h"
#include "mmi.h"
#include "screen.h"

/* dimensions */
#define LINE_HEIGHT 50
#define STEP_WIDTH 40
#define STEP_BORDER 10
#define HEADER_HEIGHT LINE_HEIGHT

#define WIDTH	(NUM_STEPS * STEP_WIDTH)
#define HEIGHT	(((NUM_LINES + 1) * LINE_HEIGHT) + HEADER_HEIGHT)

/** window title */
#define WINDOW_TITLE "ssq-32"

/** predefined colors */
typedef enum {
	COLOR_BLACK,
	COLOR_WHITE,
	COLOR_HEADER,
	COLOR_STEP_ON,
	COLOR_STEP_OFF,
	COLOR_STEP_SKIP,
	COLOR_STEP_ACTIVE,
	COLOR_FIRST_LAST,
	COLOR_LAST,
} color_t;

/** color entry */
typedef struct {
	color_t color;
	unsigned char r, g, b, a;
	Uint32 sdl_color;
} color_entry_t;

/* color table */
static color_entry_t s_color_table[] = {
	{ COLOR_BLACK,           0,   0,   0,   255 },
	{ COLOR_WHITE,           255, 255, 255, 255 },
	{ COLOR_HEADER,          0,   0,   0,   255 },
	{ COLOR_STEP_ON,         0,   150, 0,   255 },
	{ COLOR_STEP_OFF,        150, 0,   0,   255 },
	{ COLOR_STEP_SKIP,       150, 150, 0,   255 },
	{ COLOR_STEP_ACTIVE,     255, 255, 0,   255 },
	{ COLOR_FIRST_LAST,      255, 255, 255, 255 },
}; 

static int s_dirty = 1;
static SDL_Surface *s_screen;
static pattern_t *s_pattern;
static mmi_state_t *s_mmi_state;

static void init_colors();
static Uint32 get_color(color_t color);
static Uint32 get_rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
static void clear_screen();
static void draw_screen();
static void draw_header(int ox, int oy);
static void draw_sequence(int ox, int oy, sequence_t *sequence);
static void draw_line(int ox, int oy, line_t *line);
static void draw_step(int ox, int oy, line_t *line, int step);
static void draw_line_params(int ox, int oy, line_t *line);
static void format_pulse(int pulse, char *str, int len);
static void format_time(mio_timestamp_t time, char *str, int len);

/*
 * Initializes the screen.
 */
int scr_init(void)
{
	const SDL_VideoInfo *info;
	Uint8  video_bpp;
	Uint32 videoflags;
	
	s_pattern = seq_get_pattern();
	s_mmi_state = mmi_get_state();

	/* initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		LOG(LOG_ERROR, "couldn't initialize SDL: %s", SDL_GetError());
		return -1;
	}
	atexit(SDL_Quit);

	/* alpha blending doesn't work well at 8-bit color */
	info = SDL_GetVideoInfo();
	if (info->vfmt->BitsPerPixel > 8) {
		video_bpp = info->vfmt->BitsPerPixel;
	} else {
		video_bpp = 16;
	}
	videoflags = /*SDL_SWSURFACE | SDL_SRCALPHA |*/ SDL_RESIZABLE;
	videoflags |= SDL_HWSURFACE;
	videoflags |= SDL_HWPALETTE;
//	videoflags |= SDL_FULLSCREEN;

	/* set video mode */
	if ((s_screen = SDL_SetVideoMode(WIDTH, HEIGHT, video_bpp, videoflags)) == NULL) {
		LOG(LOG_ERROR, "couldn't set %ix%i video mode: %s", WIDTH, HEIGHT, SDL_GetError());
		return -1;
	}
	
	/* use alpha blending */
	//SDL_SetAlpha(s_screen, SDL_SRCALPHA, 0);
	
	/* set title for window */
 	SDL_WM_SetCaption(WINDOW_TITLE, WINDOW_TITLE);
 	
 	/* init colors */
 	init_colors();

	return 0;
}

/*
 * Shuts the screen down.
 */
void scr_shutdown(void)
{
}

/*
 * Updates the screen.
 */
void scr_update(void)
{
	SDL_Event event;
	
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			core_exit();
			break;
		}
	}
	
	if (s_dirty) {
		draw_screen();
		s_dirty = 0;
	}
}

/*
 * Sets the dirty flag.
 */
void scr_dirty(void)
{
	s_dirty = 1;
}

/**
 * Inits the colors.
 */
static void init_colors()
{
	int i;
	color_entry_t *entry;
	
	for (i = 0; i < COLOR_LAST; i++) {
		entry = &s_color_table[i];
		entry->sdl_color = get_rgba(entry->r, entry->g, entry->b, entry->a);
	}
}

/**
 * Returns a color value.
 * @return Returns color value.
 */
static Uint32 get_color(color_t color)
{
	return s_color_table[color].sdl_color;
}

/**
 * Returns a color from r, g, b, a values.
 */
static Uint32 get_rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return r << 24 | g << 16 | b << 8 | a;
}

/**
 * Clears the screen.
 */
static void clear_screen()
{
	SDL_Rect clip;

	clip.x = 0;
	clip.y = 0;
	clip.w = WIDTH;
	clip.h = HEIGHT;
	
	SDL_SetClipRect(s_screen, &clip);
	SDL_FillRect(s_screen, NULL, get_color(COLOR_BLACK));
}

/**
 * Draws the screen.
 */
static void draw_screen()
{
	clear_screen();

	draw_header(0, 0);	
	draw_sequence(0, HEADER_HEIGHT, s_mmi_state->sequence);
	
	SDL_Flip(s_screen);
}

/**
 * Draws the header.
 */
static void draw_header(int ox, int oy)
{
	int x, y;
	int size;
	char str[128];

	boxColor(s_screen, ox, oy, ox + WIDTH, oy + HEADER_HEIGHT, get_color(COLOR_HEADER));
	rectangleColor(s_screen, ox, oy, ox + WIDTH, oy + HEADER_HEIGHT, get_color(COLOR_WHITE));
	
	x = ox + HEADER_HEIGHT / 2;
	y = oy + HEADER_HEIGHT / 2;
	size = 10;
	
	switch (seq_get_run_state()) {
	case SEQ_RUNNING:
		filledTrigonColor(s_screen, x - size, y - size, x - size, y + size, x + size, y, get_color(COLOR_WHITE));
		break;
	case SEQ_STOPPED:
		boxColor(s_screen, x - size, y - size, x + size, y + size, get_color(COLOR_WHITE));
		break;
	}
	
	y -= 4;
	
	x += size * 2;
	param_get_str(&s_pattern->tempo, str, sizeof(str));
	stringColor(s_screen, x, y, str, get_color(COLOR_WHITE));
	
	x += 50;
	format_pulse(seq_get_pulse(), str, sizeof(str));
	stringColor(s_screen, x, y, str, get_color(COLOR_WHITE));
	
	x += 100;
	format_time(seq_get_elapsed_time(), str, sizeof(str));
	stringColor(s_screen, x, y, str, get_color(COLOR_WHITE));
	
	x += 100;
	snprintf(str, sizeof(str), "S%d-L%d", s_mmi_state->sequence_index + 1, s_mmi_state->line_index + 1);
	stringColor(s_screen, x, y, str, get_color(COLOR_WHITE));
	
}

/**
 * Draws a sequence.
 * @param ox Origin x
 * @param oy Origin y
 * @param sequence Sequence
 */
static void draw_sequence(int ox, int oy, sequence_t *sequence)
{
	int i;
	line_t *line;

	for (i = 0; i < NUM_LINES; i++) {
		line = &sequence->lines[i];
		draw_line(ox, oy, line);
		oy += LINE_HEIGHT;
		if (line == s_mmi_state->line) {
			draw_line_params(ox, oy, line);
			oy += LINE_HEIGHT;
		}
	}
}

/**
 * Draws a single sequencer line.
 * @param ox Origin x
 * @param oy Origin y
 * @param line Line
 */
static void draw_line(int ox, int oy, line_t *line)
{
	Uint32 color;
	int step;
	
	if (line == s_mmi_state->line)
		color = get_rgba(0, 0, 150, 255);
	else
		color = get_rgba(0, 0, 50, 255);
	
	boxColor(s_screen, ox, oy, ox + WIDTH, oy + LINE_HEIGHT, color);
	rectangleColor(s_screen, ox, oy, ox + WIDTH, oy + LINE_HEIGHT, get_color(COLOR_WHITE));
	
	for (step = 0; step < NUM_STEPS; step++) {
		draw_step(ox + step * STEP_WIDTH, oy, line, step);
	}
}

/**
 * Draws a single step.
 * @param ox Origin x
 * @param oy Origin y
 * @param line Line
 * @param step Step number
 */
static void draw_step(int ox, int oy, line_t *line, int step)
{
	char str[128];
	int first, last;
	int step_mode;
	Uint32 color;
	
	first = param_get(&line->first_step);
	last = param_get(&line->last_step);
	
	step_mode = param_get(&line->step_modes[step]);
	switch (step_mode) {
	case STEP_MODE_OFF: color = get_color(COLOR_STEP_OFF); break;
	case STEP_MODE_ON: color = get_color(COLOR_STEP_ON); break;
	case STEP_MODE_SKIP: color = get_color(COLOR_STEP_SKIP); break;
	}

	if (step == line->cur_step)
		color = get_color(COLOR_STEP_ACTIVE);
	
//	if (step == s_mmi_state->last_edited_step)
//		color = get_rgba(255, 255, 255, 255);
	
	boxColor(s_screen, ox, oy, ox + STEP_WIDTH, oy + STEP_BORDER, color);
	rectangleColor(s_screen, ox, oy, ox + STEP_WIDTH, oy + STEP_BORDER, get_color(COLOR_WHITE));
	
	if (step == first)
		filledTrigonColor(s_screen, ox + 1, oy + 1, ox + 1, oy + STEP_BORDER - 1, ox + STEP_BORDER, oy + STEP_BORDER / 2, get_color(COLOR_FIRST_LAST));
	
	if (step == last)
		filledTrigonColor(s_screen, ox + STEP_WIDTH - 1, oy + 1, ox + STEP_WIDTH - 1, oy + STEP_BORDER - 1, ox + STEP_WIDTH - STEP_BORDER - 1, oy + STEP_BORDER / 2, get_color(COLOR_FIRST_LAST));
		
	
	param_get_str(&line->step_values[step], str, sizeof(str));
	stringColor(s_screen, ox + 5, oy + 25, str, get_color(COLOR_WHITE));
}

/**
 * Draws line parameters.
 * @param ox Origin x
 * @param oy Origin y
 * @param line Line
 */
static void draw_line_params(int ox, int oy, line_t *line)
{
	char str[128];
	char val[128];
	int i;
	param_t *param;
	
	for (i = 0; i < NUM_LINE_PARAMS; i++) {
		param = line->params[i];
		if (param) {
			param_get_str(param, val, sizeof(val));
			snprintf(str, sizeof(str), "%s: %s", param_get_name(param), val);
			stringColor(s_screen, ox + (i % 8) * (STEP_WIDTH * 4) + 5, oy + 10 + (i / 8) * 20, str, get_color(COLOR_WHITE));
		}
	}
}

/**
 * Formats a pulse number to a beat display.
 * @param pulse Pulse
 * @param str String
 * @param len Length of string
 */
static void format_pulse(int pulse, char *str, int len)
{
	int bar, note, quarter;
	
	bar = pulse / (24 * 4 * 4);
	pulse -= bar * (24 * 4 * 4);
	
	note = pulse / (24 * 4);
	pulse -= note * (24 * 4);
	
	quarter = pulse / 24;
	pulse -= quarter * 24;
	
	snprintf(str, len, "%03d.%d.%d", bar + 1, note + 1, quarter + 1); 
}

/**
 * Formats a timestamp to a time display.
 * @param time Timestamp
 * @param str String
 * @param len Length of string
 */
static void format_time(mio_timestamp_t time, char *str, int len)
{
	int minutes, seconds;
	
	minutes = time / (60 * 1000);	
	time -= minutes * (60 * 1000);
	
	seconds = time / (1000);	
	time -= seconds * (1000);
	
	snprintf(str, len, "%02d:%02d.%03d", minutes, seconds, (int) time);
}
