#include "Player.h"
#include "functions.h"

Player::Player(bn::fixed x_start, bn::fixed y_start, bn::fixed player_speed, bn::size player_size):
    sprite(bn::sprite_items::happy.create_sprite(x_start, y_start)), 
    speed(player_speed),
    size(player_size) {}

void Player::update() {
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

    if (bn::keypad::a_pressed()) {
        activate_boost(); 
    }

    update_boost();

    if (sprite.x() > MAX_X) {
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

void Player::update_boost() {
    if (boosted) {
        boost_duration--;
        if (boost_duration <= 0) {
            boosted = false; 
            cooldown = 300; 
            update_speed(speed / 2); 
        }
    } else {
        if (cooldown > 0) {
            cooldown--;
        }
    }
}

void Player::reset(bn::fixed start_x, bn::fixed start_y) {
    sprite.set_x(start_x); 
    sprite.set_y(start_y); 
}

void Player::activate_boost() {
    if (cooldown <= 0 && !boosted) {
        boosted = true; 
        boost_duration = 180; 
        update_speed(speed * 2); 
    }
}

void Player::update_speed(bn::fixed new_speed) {
    speed = new_speed; 
}