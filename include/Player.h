#pragma once

#include <bn_core.h>
#include <bn_display.h>
#include <bn_keypad.h>
#include <bn_rect.h>
#include <bn_size.h>
#include <bn_string.h>
#include <bn_sprite_ptr.h>
#include <bn_sprite_text_generator.h>
#include <bn_random.h>
#include "common_fixed_8x16_font.h"

#include "bn_sprite_items_happy.h"


class Player {
    public:

static constexpr int MIN_Y = -bn::display::height() / 2;
static constexpr int MAX_Y = bn::display::height() / 2;
static constexpr int MIN_X = -bn::display::width() / 2;
static constexpr int MAX_X = bn::display::width() / 2;

        bn::sprite_ptr sprite;
        bn::fixed speed; // The speed of the player
        bn::rect bounding_box;
        bn::size size; 
        int boost_duration{180}; 
        int cooldown{300};
        bool boosted = false;

        Player(bn::fixed x_start, bn::fixed y_start, bn::fixed player_speed, bn::size player_size);
        /**
         * Update the position and bounding box of the player based on d-pad movement.
         */
        void update();
        void activate_boost(); 
        void update_boost(); 

        void reset(bn::fixed start_x, bn::fixed start_y); 

        void update_speed(bn::fixed new_speed); 

        // Create the sprite. This will be moved to a constructor
         // The rectangle around the sprite for checking collision

    bn::fixed get_x() const { return sprite.x(); }
    bn::fixed get_y() const { return sprite.y(); } 


};