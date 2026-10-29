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

        bn::sprite_ptr sprite = bn::sprite_items::dot.create_sprite();
        bn::fixed speed; // The speed of the player
        bn::size size; // The width and height of the sprite
        bn::rect bounding_box;
        bn::fixed x_speed;
        bn::fixed y_speed;

        Player(bn::fixed x_start, bn::fixed y_start, bn::fixed player_speed, bn::size player_size):
        sprite(bn::sprite_items::dot.create_sprite(x_start, y_start)), 
        speed(player_speed), 
        bounding_box(create_bounding_box(sprite, player_size))  {

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

        // Create the sprite. This will be moved to a constructor
         // The rectangle around the sprite for checking collision

    bn::fixed get_x() const { return sprite.x(); }
    bn::fixed get_y() const { return sprite.y(); } 
};
class Enemy {
    public:
        bn::sprite_ptr enemy_sprite = bn::sprite_items::dot.create_sprite(); 
        bn::size size; 
        bn::rect enemy_bounding_box;
        bn::fixed enemy_speed = 2;

        Enemy(bn::fixed x_start, bn::fixed y_start, bn::size enemy_size):
        enemy_sprite(bn::sprite_items::square.create_sprite(x_start, y_start)),
        enemy_bounding_box(create_bounding_box(enemy_sprite, enemy_size)) {

        }

    //start of logic for enemy chase        
    int get_distance(const Player& player) const {
        int dx = (player.get_x() - enemy_sprite.x()).round_integer();
        int dy = (player.get_y() - enemy_sprite.y()).round_integer();

        int max_dist = std::max(dx, dy); 
        int min_dist = std::min(dx, dy); 

        int dist = max_dist + (min_dist >> 2);

        return dist; 
    }

  

        void update(Player& player){
        bn::fixed move_x = 0;
        bn::fixed move_y = 0; 

            int dist_to_player = get_distance(player); 
            //the part that broke
            if(dist_to_player > 0){
                if (player.get_x() > enemy_sprite.x()) { move_x += enemy_speed; }
                if (player.get_x() < enemy_sprite.x()) { move_x -= enemy_speed; }
                if (player.get_y() > enemy_sprite.y()) { move_y += enemy_speed; }
                if (player.get_y() < enemy_sprite.y()) { move_y -= enemy_speed; }
            }
        enemy_sprite.set_x(enemy_sprite.x() + move_x);
        enemy_sprite.set_y(enemy_sprite.y() + move_y);

            //trying to update enemy bounding box
        enemy_bounding_box = create_bounding_box(enemy_sprite, ENEMY_SIZE);
        }


};

int main() {
    bn::core::init();

    bn::random rng = bn::random();

    // Create a new score display
    ScoreDisplay scoreDisplay = ScoreDisplay();

    // Create a player and initialize it
    // TODO: we will move the initialization logic to a constructor.
    Player player = Player(44, 22, 1.5, PLAYER_SIZE);
    Enemy enemy = Enemy(0, 22, ENEMY_SIZE); 
    

    bn::sprite_ptr enemy_sprite = bn::sprite_items::square.create_sprite(-30, 22);
    bn::rect enemy_bounding_box = create_bounding_box(enemy_sprite, ENEMY_SIZE);

    while(true) {
        player.update();
        enemy.update(player);

        // Reset the current score and player position if the player collides with enemy
        if(enemy_bounding_box.intersects(player.bounding_box)) {
            scoreDisplay.resetScore();
            player.sprite.set_x(44);
            player.sprite.set_y(22);

            int new_x = rng.get_int(MIN_X, MAX_X);
            int new_y = rng.get_int(MIN_Y, MAX_Y);
            enemy.enemy_sprite.set_x(new_x);
            enemy.enemy_sprite.set_y(new_y);
        }

        // Update the scores and disaply them
        scoreDisplay.update();
        

        bn::core::update();
    }
}