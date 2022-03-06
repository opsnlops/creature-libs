#pragma once

/**
 * @brief Topics for the home data feed
 *
 * I've got a complex home data feed available in MQTT. This
 * file defines the MQTT paths for devices to use.
 */

#define HALLWAY_BATHROOM_MOTION_TOPIC "home/hallway_bathroom/motion"
#define BEDROOM_MOTION_TOPIC "home/bedroom/motion"
#define OFFICE_MOTION_TOPIC "home/office/motion"
#define LIVING_ROOM_MOTION_TOPIC "home/living_room/motion"
#define WORKSHOP_MOTION_TOPIC "home/workshop/motion"

#define BUNNYS_ROOM_TEMPERATURE_TOPIC "home/bunnys_room/temperature"
#define GUEST_ROOM_TEMPERATURE_TOPIC "home/guest_room/temperature"
#define HALF_BATHROOM_TEMPERATURE_TOPIC "home/half_bathroom/temperature"
#define HALLWAY_BATHROOM_TEMPERATURE_TOPIC "home/hallway_bathroom/temperature"
#define KITCHEN_TEMPERATURE_TOPIC "home/kitchen/temperature"
#define OFFICE_TEMPERATURE_TOPIC "home/office/temperature"
#define FAMILY_ROOM_TEMPERATURE_TOPIC "home/family_room/temperature"
#define WORKSHOP_TEMPERATURE_TOPIC "home/workshop/temperature"

// Outside stuff
#define OUTSIDE_TEMPERATURE_TOPIC "home/outside/temperature"
#define OUTSIDE_WIND_SPEED_TOPIC "home/outside/wind_speed"

// Fireplaces
#define FAMILY_ROOM_FLAMETHROWER_TOPIC "home/family_room/flamethrower"
#define OFFICE_FLAMETHROWER_TOPIC "home/office/flamethrower"

// Power Use
#define HOME_POWER_USE_WATTS "home/power/watts"
