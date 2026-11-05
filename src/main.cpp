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
#include "Player.h"
#include "Enemy.h"
#include "functions.h"
#include "common_fixed_8x16_font.h"
#include "bn_sprite_items_dot.h"
#include "bn_sprite_items_square.h"
#include "bn_sprite_items_happy.h"
#include "bn_sprite_items_angry.h"


// Width and height of the the player bounding box
static constexpr bn::fixed BASE_SPEED = 2; 
static constexpr bn::size PLAYER_SIZE = {8, 8};
// static constexpr bn::size ENEMY_SIZE = {8, 8};

// static constexpr int MIN_Y = -bn::display::height() / 2;
// static constexpr int MAX_Y = bn::display::height() / 2;
// static constexpr int MIN_X = -bn::display::width() / 2;
// static constexpr int MAX_X = bn::display::width() / 2;

// Max Enemy Count
static constexpr int ENEMY_START_COUNT = 2;
// static constexpr int MAX_ENEMY_COUNT = 10;






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