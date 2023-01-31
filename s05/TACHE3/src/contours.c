#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "types_macros.h"
#include "image.h"
#include "geometrie2d.h"

// ====<<+---------------------------+>>====
// ====<<| LinkedList Implementation |>>====
// ====<<+---------------------------+>>====

typedef struct ListNode_ {
	Point pos;
	struct ListNode_* next;
} ListNode;

typedef struct {
	UINT len;
	ListNode* head;
	ListNode* tail;
} PointList;

#define EMPTY_POINT_LIST { 0, NULL, NULL }

void append_coordinates(PointList* list, double x, double y) {
	assert(list);
	list->len++;
	if (!list->head) {
		list->head = calloc(1, sizeof(ListNode));
		list->head->pos = set_point(x, y);
		list->tail = list->head;
	} else {
		list->tail->next = calloc(1, sizeof(ListNode));
		list->tail->next->pos = set_point(x, y);
		list->tail = list->tail->next;
	}
}

static void delete_node(ListNode* node) {
	if (!node) return;
	ListNode* next_node = node->next;
	free(node);
	delete_node(next_node);
}

void delete_list(PointList* list) {
	delete_node(list->head);
	free(list);
}

static void serialise_nodes(FILE* output_stream, ListNode* node) {
	if (!node) return;
	fprintf(output_stream, "%5.1f %5.1f\n", node->pos.x, node->pos.y);
	serialise_nodes(output_stream, node->next);
}

void serialise_list(FILE* output_stream, const PointList* list) {
	fprintf(output_stream, " 1\n\n %d\n", list->len);
	serialise_nodes(output_stream, list->head);
}

// ====<<+----------------------+>>====
// ====<<| Robot Implementation |>>====
// ====<<+----------------------+>>====

typedef enum {
	Nord, Est, Sud, Ouest
} Orientation;

typedef struct {
	Point pos;
	Orientation direction;
} Robot;

typedef struct {
	UINT x, y;
} PixelPos;

void step_robot(Robot* robot, const Image* image) {
	// Implicit values of enum variants are fun (:
	// pixels in front of the robot
	PixelPos positions_around_robot[] = {
		{ robot->pos.x,     robot->pos.y },
		{ robot->pos.x + 1, robot->pos.y },
		{ robot->pos.x + 1, robot->pos.y + 1 },
		{ robot->pos.x,     robot->pos.y + 1 }
	};
	PixelPos forward_left  = positions_around_robot[robot->direction];
	PixelPos forward_right = positions_around_robot[(robot->direction + 1) % 4];

	// A less cursed way of implementing it :
	/* PixelPos forward_left; */
	/* PixelPos forward_right; */
	/* switch (robot->direction) { */
	/* 	case Nord: */
	/* 		forward_left  = (PixelPos) { robot->pos.x,     robot->pos.y }; */
	/* 		forward_right = (PixelPos) { robot->pos.x + 1, robot->pos.y }; */
	/* 		break; */

	/* 	case Est: */
	/* 		forward_left  = (PixelPos) { robot->pos.x + 1, robot->pos.y }; */
	/* 		forward_right = (PixelPos) { robot->pos.x + 1, robot->pos.y + 1 }; */
	/* 		break; */

	/* 	case Sud: */
	/* 		forward_left  = (PixelPos) { robot->pos.x + 1, robot->pos.y + 1 }; */
	/* 		forward_right = (PixelPos) { robot->pos.x,     robot->pos.y + 1 }; */
	/* 		break; */

	/* 	case Ouest: */
	/* 		forward_left  = (PixelPos) { robot->pos.x, robot->pos.y + 1 }; */
	/* 		forward_right = (PixelPos) { robot->pos.x, robot->pos.y }; */
	/* 		break; */
	/* } */

	// Determine next orientation :
	if (get_pixel_image(*image, forward_left.x, forward_left.y)) {
		robot->direction = (robot->direction + 3) % 4;
	} else if (!get_pixel_image(*image, forward_right.x, forward_right.y)) {
		robot->direction = (robot->direction + 1) % 4;
	}

	// Step forward
	switch (robot->direction) {
		case Nord:  robot->pos.y--; break;
		case Sud:   robot->pos.y++; break;
		case Est:   robot->pos.x++; break;
		case Ouest: robot->pos.x--; break;
	}
}

PixelPos get_first_pixel_position(Image* image) {
	// Yes, I should use getters, but it's marginally faster, so funk it
	for (int i = 0; i < image->hauteur * image->largeur; i++) {
		if (image->pixels[i])
			return (PixelPos) {
				.x = (i % image->largeur) + 1,
				.y = (i / image->largeur) + 1,
			};
	};
	// Failsafe
	printf("Couldn't find a border, the image is white\n");
	supprimer_image(image);
	exit(1);
}

int main (int argc, char** argv) {
	if (argc == 1) ERREUR_FATALE("Nom de fichier image requis.\n");

	Image image = lire_fichier_image(argv[1]);

	FILE* output_stream = stdout;
	if (argc >= 3) {
		output_stream = fopen(argv[2], "w");
	}

	PixelPos position_depart = get_first_pixel_position(&image);
	Robot robot = {
		.pos = {  // Cast fields individually to properlly cast them.
			.x = position_depart.x - 1,
			.y = position_depart.y - 1,
		},
		.direction = Est,
	};

	PointList* points = calloc(1, sizeof(PointList));
	append_coordinates(points, robot.pos.x, robot.pos.y);
	do {
		step_robot(&robot, &image);
		append_coordinates(points, robot.pos.x, robot.pos.y);
	} while (robot.pos.x != position_depart.x - 1 || robot.pos.y != position_depart.y - 1);

	serialise_list(output_stream, points);

	delete_list(points);
	supprimer_image(&image);
	return 0;
}
