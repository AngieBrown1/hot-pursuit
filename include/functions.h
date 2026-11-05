#pragma once


#include <bn_core.h>
#include <bn_display.h>

#include <bn_rect.h>
#include <bn_size.h>
#include <bn_string.h>
#include <bn_sprite_ptr.h>
#include <bn_sprite_text_generator.h>
#include <bn_random.h>


class Enemy;
static constexpr int MAX_ENEMY_COUNT = 10;
static constexpr bn::size ENEMY_SIZE = {8, 8};
static constexpr int MIN_Y = -bn::display::height() / 2;
static constexpr int MAX_Y = bn::display::height() / 2;
static constexpr int MIN_X = -bn::display::width() / 2;
static constexpr int MAX_X = bn::display::width() / 2;


bn::rect create_bounding_box(bn::sprite_ptr sprite, bn::size box_size);

void addEnemy(bn::vector<Enemy, MAX_ENEMY_COUNT>& enemies, bn::fixed start_x, bn::fixed start_y); 

void initializeGame(bn::vector<Enemy, MAX_ENEMY_COUNT>& enemies, int enemy_start, bn::random& random_generator); 