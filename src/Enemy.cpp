#include "Enemy.h"





Enemy::Enemy(bn::fixed x_start, bn::fixed y_start, bn::size enemy_size):
sprite(bn::sprite_items::angry.create_sprite(x_start, y_start)),
size(enemy_size) {};

     int Enemy::get_distance(const Player& player) const {
            int dx = bn::abs(player.get_x() - sprite.x()).round_integer();
            int dy = bn::abs(player.get_y() - sprite.y()).round_integer();

            int max_dist = std::max(dx, dy); 
            int min_dist = std::min(dx, dy); 

            int dist = max_dist + (min_dist >> 2);

            return dist; 
        }

         bool Enemy::update(Player& player){
            bn::fixed move_x = 0;
            bn::fixed move_y = 0; 

            int dist_to_player = get_distance(player); 
            //the part that broke
            if(dist_to_player > 0){
                if (player.get_x() > sprite.x()) { move_x += enemy_speed; }
                if (player.get_x() < sprite.x()) { move_x -= enemy_speed; }
                if (player.get_y() > sprite.y()) { move_y += enemy_speed; }
                if (player.get_y() < sprite.y()) { move_y -= enemy_speed; }
            }
            sprite.set_x(sprite.x() + move_x);
            sprite.set_y(sprite.y() + move_y);

            //trying to update enemy bounding box
            bounding_box = create_bounding_box(sprite, size);

            return bounding_box.intersects(player.bounding_box); 
        }

        void Enemy::reset(bn::fixed start_x, bn::fixed start_y) {
            sprite.set_x(start_x); 
            sprite.set_y(start_y); 
        }
