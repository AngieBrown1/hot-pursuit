#include "functions.h"
#include "Enemy.h"


bn::rect create_bounding_box(bn::sprite_ptr sprite, bn::size box_size) {
    return bn::rect(sprite.x().round_integer(),
                    sprite.y().round_integer(),
                    box_size.width(),
                    box_size.height());
}

void addEnemy(bn::vector<Enemy, MAX_ENEMY_COUNT>& enemies, bn::fixed start_x, bn::fixed start_y) {
    if (enemies.size() < enemies.max_size()) {
        enemies.push_back(Enemy(start_x, start_y, ENEMY_SIZE));
    }
}

void initializeGame(bn::vector<Enemy, MAX_ENEMY_COUNT>& enemies, int enemy_start, bn::random& random_generator) {
    bn::fixed box_left = -30;
    bn::fixed box_right = 30;
    bn::fixed box_top = -20;
    bn::fixed box_bottom = 20;

    enemies.clear(); 

    for (int i = 0; i < enemy_start; i++) {
        bn::fixed random_x{0}; 
        bn::fixed random_y{0};
        bool inside_box = true; 

        while (inside_box) {
            random_x = random_generator.get_fixed(MIN_X, MAX_X);
            random_y = random_generator.get_fixed(MIN_Y, MAX_Y);
            
            if (random_x >= box_left && random_x <= box_right && 
                random_y >= box_top && random_y <= box_bottom) {
                inside_box = true; 
            } else {
                inside_box = false; 
            }
        }

        addEnemy(enemies, random_x, random_y); 
    }
}