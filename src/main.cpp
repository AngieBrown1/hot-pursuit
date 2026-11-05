#include <bn_core.h>
#include <bn_display.h>
#include <bn_keypad.h>
#include <bn_rect.h>
#include <bn_size.h>
#include <bn_string.h>
#include <bn_sprite_ptr.h>
#include <bn_sprite_text_generator.h>
#include <bn_random.h>


#include "ScoreDisplay.h"
#include "common_fixed_8x16_font.h"
#include "bn_sprite_items_dot.h"
#include "bn_sprite_items_square.h"
#include "bn_sprite_items_happy.h"
#include "bn_sprite_items_angry.h"

// Width and height of the the player bounding box
static constexpr bn::fixed BASE_SPEED = 2; 
static constexpr bn::size PLAYER_SIZE = {8, 8};
static constexpr bn::size ENEMY_SIZE = {8, 8};

static constexpr int MIN_Y = -bn::display::height() / 2;
static constexpr int MAX_Y = bn::display::height() / 2;
static constexpr int MIN_X = -bn::display::width() / 2;
static constexpr int MAX_X = bn::display::width() / 2;

// Max Enemy Count
static constexpr int ENEMY_START_COUNT = 2;
static constexpr int MAX_ENEMY_COUNT = 10;

/**
 * Creates a rectangle centered at a sprite's location with a given size.
 * sprite the sprite to center the box around
 * box_size the dimensions of the bounding box
 */
bn::rect create_bounding_box(bn::sprite_ptr sprite, bn::size box_size) {
    return bn::rect(sprite.x().round_integer(),
                    sprite.y().round_integer(),
                    box_size.width(),
                    box_size.height());
}


class Player {
    public:

        bn::sprite_ptr sprite;
        bn::fixed speed; // The speed of the player
        bn::rect bounding_box;
        bn::size size; 

        Player(bn::fixed x_start, bn::fixed y_start, bn::fixed player_speed, bn::size player_size):
        sprite(bn::sprite_items::happy.create_sprite(x_start, y_start)), 
        speed(player_speed),
        size(player_size) {

        }
        /**
         * Update the position and bounding box of the player based on d-pad movement.
         */
        void update() {
            if(bn::keypad::right_held()) {
                sprite.set_x(sprite.x() + speed);
            }
            if(bn::keypad::left_held()) {
                sprite.set_x(sprite.x() - speed);
            }
            // TODO: Add logic for up and down
            if(bn::keypad::up_held()) {
                sprite.set_y(sprite.y() - speed);
            }
            if(bn::keypad::down_held()) {
                sprite.set_y(sprite.y() + speed);
            }

             if(sprite.x() > MAX_X) {
                sprite.set_x(MAX_X);
            }

            if(sprite.x() < MIN_X){
                sprite.set_x(MIN_X);
            }

            if(sprite.y() > MAX_Y) {
                sprite.set_y(MAX_Y);
            }

            if(sprite.y() < MIN_Y){
                sprite.set_y(MIN_Y);
            }

            bounding_box = create_bounding_box(sprite, size);
        }

        void reset(bn::fixed start_x, bn::fixed start_y) {
            sprite.set_x(start_x); 
            sprite.set_y(start_y); 
        }

        void update_speed() {
            speed = speed - 0.1; 
        }

        // Create the sprite. This will be moved to a constructor
         // The rectangle around the sprite for checking collision

    bn::fixed get_x() const { return sprite.x(); }
    bn::fixed get_y() const { return sprite.y(); } 
};

class Enemy {
    public:
        bn::sprite_ptr sprite; 
        bn::rect bounding_box;
        bn::fixed enemy_speed = 0.5;
        bn::size size; 

        Enemy(bn::fixed x_start, bn::fixed y_start, bn::size enemy_size):
        sprite(bn::sprite_items::angry.create_sprite(x_start, y_start)),
        size(enemy_size) {}

        //start of logic for enemy chase        
        int get_distance(const Player& player) const {
            int dx = bn::abs(player.get_x() - sprite.x()).round_integer();
            int dy = bn::abs(player.get_y() - sprite.y()).round_integer();

            int max_dist = std::max(dx, dy); 
            int min_dist = std::min(dx, dy); 

            int dist = max_dist + (min_dist >> 2);

            return dist; 
        }

  

        bool update(Player& player){
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

        void reset(bn::fixed start_x, bn::fixed start_y) {
            sprite.set_x(start_x); 
            sprite.set_y(start_y); 
        }
};

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

int main() {
    bn::core::init();

    bn::random rng = bn::random();
    bn::vector<Enemy, MAX_ENEMY_COUNT> enemyVector = {}; 

    // Create a new score display
    ScoreDisplay scoreDisplay = ScoreDisplay();

    // Create a player and initialize it
    // TODO: we will move the initialization logic to a constructor.
    Player player = Player(0, 0, 1.5, PLAYER_SIZE);
    int timer{420};

    initializeGame(enemyVector, ENEMY_START_COUNT, rng); 
    while(true) {
        player.update();

        bool player_hit = false; 

        for (Enemy& e : enemyVector) {
            if (e.update(player)) {
                player_hit = true; 
            }
        }
        
        // Reset the current score and player position if the player collides with enemy
        if (player_hit) {
            scoreDisplay.resetScore();
            player.reset(0, 0); 
            
            initializeGame(enemyVector, ENEMY_START_COUNT, rng); 
        }

        if (timer <= 0) {
            addEnemy(enemyVector, -100, -100);
            timer = 420; 
        }

        // Update the scores and disaply them
        timer--; 
        scoreDisplay.update();
        bn::core::update();
    }
}