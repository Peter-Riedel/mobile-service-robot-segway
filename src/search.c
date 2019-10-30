#include <stdio.h>
#include <stdbool.h>

#include "ev3.h"
#include "ev3_port.h"
#include "ev3_sensor.h"

#include "search.h"

typedef enum {
	COLOR_NONE,
	BLACK,
	BLUE,
	GREEN,
	YELLOW,
	RED,
	WHITE,
	BROWN
} color_t;

static uint32_t num_barrels;
static const uint32_t MAX_NUM_BARRELS = 3;
static color_t found_barrels[3];

static bool isBarrelColor(color_t color);
static bool foundBarrel(color_t color);
static const char* getBarrelColor(color_t color);
static void speakColor(const char *color);

void search(sensors_t *sensors) {
	printf("search barrels...\n");
	num_barrels = 0;

	do {
		// TODO: implement detection of barrels with US sensor
		
		// memorize and identify barrels
		color_t color;
		if (get_sensor_value(0, sensors->sn_color, (int*) &color)) {
			if (isBarrelColor(color) && !foundBarrel(color)) {
				const char *barrel_color = getBarrelColor(color);
				printf("%s barrel found!\n", barrel_color);
				speakColor(barrel_color);
				found_barrels[num_barrels++] = color;
			}
		}
	} while (num_barrels < MAX_NUM_BARRELS);
}

static bool isBarrelColor(color_t color) {
	if (color == GREEN || color == YELLOW || color == RED)
		return true;

	return false;
}

static bool foundBarrel(color_t color) {
	for (uint32_t i = 0; i < num_barrels; i++) {
		if (found_barrels[i] == color)
			return true;
	}

	return false;
}

static const char* getBarrelColor(color_t color) {
	switch (color) {
		case GREEN: return "GREEN";
		case YELLOW: return "YELLOW";
		case RED: return "RED";
		default: return "NONE";
	}
}

static void speakColor(const char *color) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%s%s%s", "espeak '", color, "' --stdout | aplay");
	system(buf);
}
