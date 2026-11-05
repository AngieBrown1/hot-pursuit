#pragma once


#include <bn_string.h>
#include <bn_sprite_ptr.h>
#include <bn_sprite_text_generator.h>


#include "common_fixed_8x16_font.h"
class ScoreDisplay {
    public:

     int score; // current score
 int high_score; // best core
static constexpr int MAX_SCORE_CHARS = 22;
 bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> score_sprites; // Sprites to display scores
bn::sprite_text_generator text_generator; // Text generator for scores


// Score location
static constexpr int SCORE_X = 70;
static constexpr int SCORE_Y = -70;

// High score location
static constexpr int HIGH_SCORE_X = -70;
static constexpr int HIGH_SCORE_Y = -70;


        ScoreDisplay();
       
        /**
         * Increases score by 1 and updates high score if needed. Displays score and high score.
         */
        void update();

        /**
         * Displays a number at the given position
         */
        void show_number(int x, int y, int number);
        /**
         * Sets score back to 0. Does NOT reset high score.
         */
        void resetScore();

};
