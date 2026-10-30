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
#include "bn_sprite_items_dot.h"
#include "bn_sprite_items_square.h"

// Width and height of the the player bounding box
static constexpr bn::fixed BASE_SPEED = 2; 
static constexpr bn::size PLAYER_SIZE = {8, 8};
static constexpr bn::size ENEMY_SIZE = {8, 8};

static constexpr int MIN_Y = -bn::display::height() / 2;
static constexpr int MAX_Y = bn::display::height() / 2;
static constexpr int MIN_X = -bn::display::width() / 2;
static constexpr int MAX_X = bn::display::width() / 2;

// Number of characters required to show two of the longest numer possible in an int (-2147483647)
static constexpr int MAX_SCORE_CHARS = 22;

// Score location
static constexpr int SCORE_X = 70;
static constexpr int SCORE_Y = -70;

// High score location
static constexpr int HIGH_SCORE_X = -70;
static constexpr int HIGH_SCORE_Y = -70;

// Max Enemy Count
static constexpr int ENEMY_START_COUNT = 5;
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

/**
 * Displays a score and high score.
 * 
 * Score starts a 0 and is increased each time update is called, and reset to 0 when resetScore is
 * called. High score tracks the highest score ever reached.
*/
class ScoreDisplay {
    public:
        ScoreDisplay() :
            score(0), // Start score at 0
            high_score(0), // Start high score at 0
            score_sprites(bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS>()), // Start with empty vector for score sprites
            text_generator(bn::sprite_text_generator(common::fixed_8x16_sprite_font)) // Use a new text generator
        {}

        /**
         * Increases score by 1 and updates high score if needed. Displays score and high score.
         */
        void update() {
            // increase score and update high score if this is the new highest
            score++;
            if(score > high_score) {
                high_score = score;
            }

            // Stop displaying previous scores
            score_sprites.clear();

            // Display new scores
            show_number(SCORE_X, SCORE_Y, score);
            show_number(HIGH_SCORE_X, HIGH_SCORE_Y, high_score);
        }

        /**
         * Displays a number at the given position
         */
        void show_number(int x, int y, int number) {
            // Convert number to a string and then display it
            bn::string<MAX_SCORE_CHARS> number_string = bn::to_string<MAX_SCORE_CHARS>(number);
            text_generator.generate(x, y, number_string, score_sprites);
        }

        /**
         * Sets score back to 0. Does NOT reset high score.
         */
        void resetScore() {
            score = 0;
        }

        int score; // current score
        int high_score; // best core
        bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> score_sprites; // Sprites to display scores
        bn::sprite_text_generator text_generator; // Text generator for scores
};

class Player {
    public:

        bn::sprite_ptr sprite;
        bn::fixed speed; // The speed of the player
        bn::rect bounding_box;
        bn::size size; 

        Player(bn::fixed x_start, bn::fixed y_start, bn::fixed player_speed, bn::size player_size):
        sprite(bn::sprite_items::dot.create_sprite(x_start, y_start)), 
        speed(player_speed),
        size(player_size) {

        }
        /**
         * Update the position and bounding box of the player based on d-pad movement.
         */
        void update() {
            bn::fixed x = sprite.x();
            bn::fixed y = sprite.y();

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

            if(x < MIN_X){
                sprite.set_x(MIN_X);
            }

            if(y > MAX_Y) {
                sprite.set_y(MAX_Y);
            }

            if(y < MIN_Y){
                sprite.set_y(MIN_Y);
            }

            bounding_box = create_bounding_box(sprite, size);
        }

        void reset(bn::fixed start_x, bn::fixed start_y) {
            sprite.set_x(start_x); 
            sprite.set_y(start_y); 
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
        bn::fixed enemy_speed = 1;
        bn::size size; 

        Enemy(bn::fixed x_start, bn::fixed y_start, bn::size enemy_size):
        sprite(bn::sprite_items::square.create_sprite(x_start, y_start)),
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
            random_x = (random_generator.get_fixed() * MAX_X * 2) - MAX_X;
            random_y = (random_generator.get_fixed() * MAX_Y * 2) - MAX_Y;
            
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

        // Update the scores and disaply them
        scoreDisplay.update();
        bn::core::update();
    }
}