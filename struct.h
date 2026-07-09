// g++ struct.h -o struct

#ifndef STRUCT_H
#define STRUCT_H

#include <string>
#include <vector>
#include <iostream>
#include <random>
#include <map>

extern std::mt19937 generator;

struct Player;
struct Armor;

struct Weapon {
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
};

struct Armor {
    std::string armor_name;
    double armor_absorption = 0.0;
    double player_damage_multiplier = 1.0;
    double loot_speed_multiplier = 1.0;
    int player_max_hp = 100;
};

struct InventoryItem {
    std::string name;
    int count = 0;
    int sell_price = 0;
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

    std::vector<InventoryItem> backpack;

    void show_backpack() const {
        std::cout << "\n--- СОДЕРЖИМОЕ РЮКЗАКА ---\n";
        if (backpack.empty()) {
            std::cout << "Рюкзак пуст.\n";
        } else {
            for (size_t i = 0; i < backpack.size(); ++i) {
                std::cout << backpack[i].name << ": " << backpack[i].count << " шт.\n";
            }
        }
        std::cout << "----------------------------\n";
    }
    
    void sell_all_loot() {
        // Контекстные фразы для конкретных предметов
        static const std::map<std::string, std::vector<std::string>> item_phrases = {
            {"Металлолом", {
                "Скупщик: Металлолом? А я думал, ты решил меня удивить ржавым ведром. О, так и есть.",
                "Скупщик: Опять этот металлолом... Ты случайно не разбираешь старые танки? Ладно, беру.",
                "Скупщик: Ржавое железо! Моя любимая головная боль. Давай сюда."
            }},
            {"Батарейка", {
                "Скупщик: Батарейка... Пальчиковая? Из пульта от телевизора? Беру, но только из жалости.",
                "Скупщик: Батарейки кончились? А у тебя их целый мешок! Заряжай цену."
            }},
            {"Тряпка", {
                "Скупщик: Тряпка. Она хоть стираная? Ладно, не отвечай, я всё равно её сожгу.",
                "Скупщик: Тряпки, тряпки… У меня скоро склад лоскутов будет. Скидывай."
            }},
            {"Аптечка", {
                "Скупщик: Аптечка? Ты хоть знаешь, сколько она стоит? Ладно, давай, пока не просрочена."
            }},
            {"Патроны", {
                "Скупщик: Патроны! Надеюсь, не холостые. Проверим на ближайшем зомби, но деньги сначала."
            }},
            {"Деталь оружия", {
                "Скупщик: Деталь оружия… Ты что, разобрал чей-то ствол? Молодец, неси."
            }},
            {"Золотая цепочка", {
                "Скупщик: Золотая цепочка. Скажи честно: снял с мертвеца или стащил у бывшей?",
                "Скупщик: Золотишко блестит – монеты хрустят. Выкладывай."
            }},
            {"Кристалл", {
                "Скупщик: Ого, кристалл! Ты что, ограбил ювелирный? Или это из зубов зомби выковырял?",
                "Скупщик: Кристаллы… они мне напоминают о разбитых мечтах и высокой прибыли."
            }},
            {"Редкий артефакт", {
                "Скупщик: Редкий артефакт! Выглядит так, будто ему место в музее... или в мусорном баке, одно из двух.",
                "Скупщик: Артефакт? Да за такое антиквары удавятся. Но я дам хорошую цену, не бойся."
            }},
            {"Схема обвеса", {
                "Скупщик: Схема обвеса? Ты хоть понимаешь, что это? Я тоже нет, но звучит дорого. Заверни.",
                "Скупщик: Бумажка с каракулями? Или правда схема? Ладно, инженеры разберутся."
            }}
        };

        // Общие фразы
        static const std::string generic_phrases[] = {
            "Скупщик: Ну-с, показывай, что нарыл. Только не говори, что опять припёр ржавые гвозди.",
            "Скупщик: О! Опять этот сталкер со своим барахлом. Давай, выкладывай, пока зомби не набежали.",
            "Скупщик: Снова здравствуй, мой любимый поставщик мусора.",
            "Скупщик: Я надеюсь, ты принёс что-то ценнее, чем собственная печень?",
            "Скупщик: Ну что, опять «золотые» часы, которые уже трижды переплавляли?",
            "Скупщик: Ещё один «артефакт» из помойки. Ты случайно не экстрасенс?",
            "Скупщик: Тряпки, кристаллы... Ты случайно не сорока, блестящее тащишь?",
            "Скупщик: Надеюсь, ты эти вещи вытащил из ящика, а не из чьего-то кармана? Хотя мне всё равно.",
            "Скупщик: Сегодня у меня философское настроение. Видишь этот хлам? Вот так и жизнь — пыль и тлен. Но монеты приму."
        };

        if (backpack.empty()) {
            std::cout << "Скупщик: Эй, у тебя рюкзак пустой! Проваливай, не морочь мне голову!\n";
            return;
        }

        // Ищем предмет, для которого есть особая фраза
        std::string chosen_phrase;
        for (const auto& item : backpack) {
            auto it = item_phrases.find(item.name);
            if (it != item_phrases.end() && !it->second.empty()) {
                // Берём случайную фразу для этого предмета
                std::uniform_int_distribution<int> dist(0, it->second.size() - 1);
                chosen_phrase = it->second[dist(generator)];
                break;
            }
        }

        // Если не нашли подходящую, берём случайную общую фразу
        if (chosen_phrase.empty()) {
            std::uniform_int_distribution<int> dist(0, sizeof(generic_phrases)/sizeof(generic_phrases[0]) - 1);
            chosen_phrase = generic_phrases[dist(generator)];
        }

        std::cout << chosen_phrase << "\n";

        int total_earned = 0;
        for (size_t i = 0; i < backpack.size(); ++i) {
            total_earned += backpack[i].count * backpack[i].sell_price;
        }

        player_money += total_earned;
        backpack.clear();

        std::cout << "Скупщик прикинул вес рюкзака и выдал вам " << total_earned 
                  << " монет. Баланс: " << player_money << ".\n";
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

struct LootItem {
    std::string name;
    int sell_price;
};

#endif