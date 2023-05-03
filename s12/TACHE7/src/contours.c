#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "types_macros.h"
#include "image.h"
#include "geometrie2d.h"
#include "contours.h"
#include "linked_lists.h"

// ====<<+----------------------+>>====
// ====<<| Robot Implementation |>>====
// ====<<+----------------------+>>====

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
	}
	else if (!get_pixel_image(*image, forward_right.x, forward_right.y)) {
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

// Gives ownership of the return value to caller.
LL_Points* get_contour(const Image* image, Image* mask, u32 depart_x, u32 depart_y) {
	Robot robot = {
		.pos = {  // Specify fields individually to properlly cast them.
			.x = depart_x - 1,
			.y = depart_y - 1,
		},
		.direction = Est,
	};

	LL_Points* rv = LL_from_ptr(free, 1, new_point(robot.pos.x, robot.pos.y));
	do {
		step_robot(&robot, image);
		LL_push_tail_ptr(rv, new_point(robot.pos.x, robot.pos.y));
		if (robot.direction == Est) {
			set_pixel_image(*mask, robot.pos.x, robot.pos.y + 1, BLANC);
		}
	} while (robot.pos.x != depart_x - 1 || robot.pos.y != depart_y - 1);

	return rv;
}

// ====<<+-----------------------------+>>====
// ====<<| Get every contours in image |>>====
// ====<<+-----------------------------+>>====

Image get_mask(const Image* image) {
	Image rv = creer_image(image->largeur, image->hauteur);
	for (u32 y = 1; y <= image->hauteur; y++) {
		for (u32 x = 1; x <= image->largeur; x++) {
			Pixel new_pix = get_pixel_image(*image, x, y) &&
					!get_pixel_image(*image, x, y - 1);
			set_pixel_image(rv, x, y, new_pix);
		}
	}

	return rv;
}

// Gives ownership of the return value to the caller
LL_Contours* get_all_contours_image(const Image* image) {
	LL_Contours* rv = LL_new_empty((void (*)(void*)) LL_delete);
	Image mask = get_mask(image);

	for (u32 y = 1; y <= mask.hauteur; y++) {
		for (u32 x = 1; x <= mask.largeur; x++) {
			if (get_pixel_image(mask, x, y)) {
				LL_push_tail_ptr(rv, get_contour(image, &mask, x, y));
			}
		}
	}

	supprimer_image(&mask);
	return rv;
}
