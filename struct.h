#ifndef STRUCT_H
#define STRUCT_H

#include <string>
#include <vector>
#include <iostream>
#include <random>
#include <map>

extern std::mt19937 generator;
extern std::uniform_int_distribution<int> zombieDmg;
extern std::uniform_int_distribution<int> zombieSpeedDmg;

// ------------------- Интерфейс урона -------------------
class IDamageable {
public:
    virtual void take_damage(int amount) = 0;
    virtual ~IDamageable() {}
};

// ------------------- Структуры предметов -------------------
struct LootItem {
    std::string name;
    int sell_price;
};

// ------------------- Аптечки -------------------
class Hel {
private:
    std::string hel_name;
    int regeneration = 35;
    int hel = 0;

public:
    std::string get_name() const { return hel_name; }
    int get_regeneration() const { return regeneration; }

    int get_hel() const { return hel; }

    void add_hel(int amount) { hel += amount; }
    void set_hel(int amount) { hel = amount; }
};

// ------------------- Оружие -------------------
class Weapon {
private:
    std::string name = "Без оружия";
    int current_weapon = 0;
    double weapon_damage_mult = 1.0;
    int accuracy_bonus = 0;
    double weapon_loot_speed = 1.0;
    double double_shot_chance = 0.0;
    double guaranteed_kill_chance = 0.0;
    double silencer_dodge_chance = 0.0;

    int cartridges = 30;
    int max_cartridges = 30;
    int endurance = 100;

public:
    // Геттеры
    std::string get_name() const { return name; }
    int get_current_weapon() const { return current_weapon; }
    double get_weapon_damage_mult() const { return weapon_damage_mult; }
    int get_accuracy_bonus() const { return accuracy_bonus; }
    double get_weapon_loot_speed() const { return weapon_loot_speed; }
    double get_double_shot_chance() const { return double_shot_chance; }
    double get_guaranteed_kill_chance() const { return guaranteed_kill_chance; }
    double get_silencer_dodge_chance() const { return silencer_dodge_chance; }
    int get_cartridges() const { return cartridges; }
    int get_max_cartridges() const { return max_cartridges; }
    int get_endurance() const { return endurance; }

    // Сеттеры
    void set_name(const std::string& n) { name = n; }
    void set_current_weapon(int value) { current_weapon = value; }
    void set_weapon_damage_mult(double value) { weapon_damage_mult = value; }
    void set_accuracy_bonus(int value) { accuracy_bonus = value; }
    void set_weapon_loot_speed(double value) { weapon_loot_speed = value; }
    void set_double_shot_chance(double value) { double_shot_chance = value; }
    void set_guaranteed_kill_chance(double value) { guaranteed_kill_chance = value; }
    void set_silencer_dodge_chance(double value) { silencer_dodge_chance = value; }
    void set_cartridges(int value) { cartridges = value; }
    void set_endurance(int value) { endurance = value; }

    // Добавление
    void add_cartridges(int amount) { cartridges += amount; }
    void add_endurance(int amount) { endurance += amount; }
    void add_guaranteed_kill_chance(double amount) { guaranteed_kill_chance += amount; }

    // Удаление
    void remuve_cartridges(int amount) { cartridges -= amount; }
    void remuve_endurance(int amount) { endurance -= amount; }
};

// ------------------- Броня -------------------
class Armor {
private:
    std::string armor_name = "Начальная броня";
    double armor_absorption = -0.05;
    double player_damage_multiplier = 1.0;
    double loot_speed_multiplier = 0.8;
    int player_max_hp = 100;
    int player_max_energe = 100;

public:
    // Геттеры
    std::string get_name() const { return armor_name; }
    double get_armor_absorption() const { return armor_absorption; }
    double get_player_damage_multiplier() const { return player_damage_multiplier; }
    double get_loot_speed_multiplier() const { return loot_speed_multiplier; }
    int get_player_max_hp() const { return player_max_hp; }
    int get_player_max_energe() const { return player_max_energe; }

    // Сеттеры
    void set_armor_absorption(double amount) { armor_absorption = amount; }
    void set_player_damage_multiplier(double amount) { player_damage_multiplier = amount; }
    void set_loot_speed_multiplier(double amount) { loot_speed_multiplier = amount; }
    void set_max_energe(int amount) { player_max_energe = amount; }
    void set_player_max_hp(int amount) { player_max_hp = amount; }

    // Методы добавления
    void add_armor_absorption(double amount) { armor_absorption += amount; }
    void add_player_damage_multiplier(double amount) { player_damage_multiplier += amount; }
    void add_loot_speed_multiplier(double amount) { loot_speed_multiplier += amount; }
    void add_max_energe(int amount) { player_max_energe += amount; }
    void add_player_max_hp(int amount) { player_max_hp += amount; }
};

// ------------------- Предмет в рюкзаке -------------------
struct InventoryItem {
    std::string name;
    int count = 0;
    int sell_price = 0;
    int value = 0;
};

// ------------------- Игрок -------------------
class Player : public IDamageable {
private:
    std::string player_name;
    int player_hp = 100;
    int player_damage = 20;
    int player_money = 0;
    int player_level = 1;
    int player_xp = 0;
    int player_reg = 0;
    int energe = 100;
    std::vector<InventoryItem> backpack;
    Armor current_armor;
    Weapon current_weapon;
    static std::string choose_phrase(const std::vector<InventoryItem>& bp);

public:
    Player(std::string name = "Unknown") {
        player_name = name;
        player_hp = 100;
        player_money = 500;
    }

    // Броня
    const Armor& get_armor() const { return current_armor; }
    Armor& get_armor_mutable() { return current_armor; }

    // Оружие
    const Weapon& get_weapon() const { return current_weapon; }
    Weapon& get_weapon_mutable() { return current_weapon; }

    // Энергия
    int get_energe() const { return energe; }
    void add_energe(int amount) { energe += amount; }
    void set_energe(int amount) { energe = amount; }
    void reduce_energe(int amount) { energe -= amount; }

    // Деньги
    int get_money() const { return player_money; }
    void set_money(int value) { player_money = value; }
    void add_money(int amount) { player_money += amount; }
    void reduce_money(int amount) {
        player_money -= amount;
        if (player_money < 0) player_money = 0;
    }

    // Здоровье
    int get_hp() const { return player_hp; }
    void set_hp(int value) { player_hp = value; }
    void take_damage(int damage) override {
        player_hp -= damage;
        if (player_hp < 0) player_hp = 0;
    }
    void heal_player(int amount, int max_hp) {
        player_hp += amount;
        if (player_hp > max_hp) player_hp = max_hp;
    }

    // Уровень и опыт
    int get_level() const { return player_level; }
    void set_level(int value) { player_level = value; }
    void add_level(int amount) { player_level += amount; }

    int get_xp() const { return player_xp; }
    void set_xp(int value) { player_xp = value; }
    void xp_up(int amount) { player_xp += amount; }
    void xp_nup(int amount) { player_xp -= amount; }

    int get_damage() const { return player_damage; }
    void set_damage(int value) { player_damage = value; }

    std::string get_name() const { return player_name; }
    void set_name(const std::string& name) { player_name = name; }

    int get_reg() const { return player_reg; }
    void set_reg(int value) { player_reg = value; }

    int xp_for_next_level() const {
        return player_level * 200 + player_level * player_level * 5;
    }

    void save_inventory(const std::string& filename) const;
    void load_inventory(const std::string& filename);

    void add_item(const std::string& item_name, int price, int val = 0) {
        for (auto& item : backpack) {
            if (item.name == item_name) {
                item.count++;
                return;
            }
        }
        backpack.push_back({item_name, 1, price, val});
    }

    void show_backpack() const {
        std::cout << "\n--- СОДЕРЖИМОЕ РЮКЗАКА ---\n";
        if (backpack.empty()) {
            std::cout << "Рюкзак пуст.\n";
        } else {
            for (const auto& item : backpack)
                std::cout << item.name << ": " << item.count << " шт.\n";
        }
        std::cout << "----------------------------\n";
    }

    void sell_all_loot();
};

// ------------------- Зомби (базовый) -------------------
struct Zombie : public IDamageable {
    int zombie_dmg = 10;
    int zombie_hp = 100;

    virtual void attack(Player& p);

    int get_random_damage() {
        return zombieDmg(generator);
    }

    void print_attack_phrase() {
        int action = generator() % 3;
        switch (action) {
            case 0: std::cout << "Зомби спотыкается о собственные кишки и падает на вас всем весом — вы придавлены!\n"; break;
            case 1: std::cout << "Зомби пытается плюнуть кислотой, но у него отваливается челюсть, и он просто шлёпает вас языком.\n"; break;
            case 2: std::cout << "Зомби делает страшное лицо и начинает быстро-быстро трясти головой — вы получаете удар болтающейся кожей!\n"; break;
        }
    }

    void take_damage(int amount) override {
        zombie_hp -= amount;
        if (zombie_hp < 0) zombie_hp = 0;
    }
};

// ------------------- Быстрый зомби -------------------
struct FastZombie : public Zombie {
    int speed = 2;

    void attack(Player& p) override;
};

#endif