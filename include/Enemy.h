#pragma once
#include "functions.h"
#include "bn_sprite_items_angry.h"
#include "Player.h"
class Enemy {
    public:
        bn::sprite_ptr sprite; 
        bn::rect bounding_box;
        bn::fixed enemy_speed = 0.5;
        bn::size size; 

        Enemy(bn::fixed x_start, bn::fixed y_start, bn::size enemy_size);

        //start of logic for enemy chase        
        int get_distance(const Player& player) const;
  

        bool update(Player& player);

        void reset(bn::fixed start_x, bn::fixed start_y);
};