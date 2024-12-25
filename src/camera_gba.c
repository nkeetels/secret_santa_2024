#include <gba_input.h>
#include "fixedmath.h"
#include "camera.h"

void update_camera(camera_t *cam)
{
    cam->speed = 32;

    scanKeys();
    uint16_t keys = keysHeld();

    if (keys) {
        if (keys & KEY_DOWN) {
            cam->position.x -= (cam->speed * sin(cam->angles.y)) >> 8;
            cam->position.z -= (cam->speed * cos(cam->angles.y)) >> 8;
        }

        if (keys & KEY_UP) {
            cam->position.x += (cam->speed * sin(cam->angles.y)) >> 8;
            cam->position.z += (cam->speed * cos(cam->angles.y)) >> 8;	
        }

        if (keys & KEY_RIGHT) {
            cam->angles.y -= 8;
        }

        if (keys & KEY_LEFT) {
            cam->angles.y += 8;
        }

        if ((keys & KEY_L) || (keys & KEY_R)) {
            if (keys & KEY_UP) {
                cam->angles.x -= 8;
            }

            if (keys & KEY_DOWN) {
                cam->angles.x += 8;
            }
            if (keys & KEY_LEFT) {
                cam->angles.z -= 8;
            }

            if (keys & KEY_RIGHT) {
                cam->angles.z += 8;
            }
        }
        else {
            if (keys & KEY_A) {
                cam->position.y -= 25;
            }

            if (keys & KEY_B) {
                cam->position.y += 25;
            }
        }
    }
}
