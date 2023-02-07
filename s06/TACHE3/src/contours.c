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

static void delete_nodes(ListNode* node) {
	if (!node) return;
	ListNode* next_node = node->next;
	free(node);
	delete_nodes(next_node);
}

void delete_list(PointList* list) {
	delete_nodes(list->head);
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

#define ROTATE_LEFT(direction)  (((direction) + 3) % 4)
#define ROTATE_RIGHT(direction) (((direction) + 1) % 4)

typedef struct {
	Point pos;
	Orientation direction;
} Robot;

typedef struct {
	UINT x, y;
} PixelPos;

void step_robot(Robot* robot, const Image* image) {
	// Get pixels in front of the robot
	PixelPos space_front_left_per_dir[] = {
		[Nord]  = { robot->pos.x    , robot->pos.y },
		[Est]   = { robot->pos.x + 1, robot->pos.y },
		[Sud]   = { robot->pos.x + 1, robot->pos.y + 1 },
		[Ouest] = { robot->pos.x    , robot->pos.y + 1 }
	};
	PixelPos forward_left  = space_front_left_per_dir[robot->direction];
	PixelPos forward_right = space_front_left_per_dir[ROTATE_RIGHT(robot->direction)];

	// Determine next orientation :
	if (get_pixel_image(*image, forward_left.x, forward_left.y)) {
		robot->direction = ROTATE_LEFT(robot->direction);
	} else if (!get_pixel_image(*image, forward_right.x, forward_right.y)) {
		robot->direction = ROTATE_RIGHT(robot->direction);
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
	for (int y = 0; y < image->hauteur; y++) {
		for (int x = 0; x < image->largeur; x++) {
			if (get_pixel_image(*image, x, y)) return (PixelPos) { x, y };
		}
	}
	// Failsafe
	supprimer_image(image);
	ERREUR_FATALE("Couldn't find a border, the image is white\n");
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
		.pos = {  // Specify fields individually to properlly cast them.
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
