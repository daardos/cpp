// g++ struct.h -o struct

#ifndef STRUCT_H
#define STRUCT_H

#include <string>

struct Player;
struct Armor;

struct Weapon {
    std::string weapon_name;
    int current_weapon = 0;
    double weapon_damage_mult = 1.0;
    int accuracy_bonus = 0;
    double weapon_loot_speed = 1.0;
    double double_shot_chance = 0.0;
    double guaranteed_kill_chance = 0.0;
    double silencer_dodge_chance = 0.0;
};

struct Armor {
    std::string armor_name;
    double armor_absorption = 0.0;
    double player_damage_multiplier = 1.0;
    double loot_speed_multiplier = 1.0;
    int player_max_hp = 100;
};

struct Player {
    std::string player_name;
    int player_hp = 100;
    int player_damage = 20;
    int player_money = 0;
    int player_level = 1;
    int player_xp = 0;
    int player_reg = 0;

    int xp_for_next_level() const {
        return player_level * 200 + player_level * player_level * 5;
    }
};

struct Zombie {
    int zombie_dmg = 10;
    int zombie_hp = 100;

    virtual void attack(Player& p, Armor& a);
};

struct FastZombie : Zombie {
    int speed = 2;

    void attack(Player& p, Armor& a) override;
};

#endif