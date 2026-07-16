// g++ main.cpp -o a

#include <iostream>
#include <string>
#include <cctype>
#include <Windows.h>
#include <random>
#include <thread>
#include <chrono>
#include <fstream>
#include <limits>
#include <sstream>
#include <functional>

#include "struct.h"

// ---------- Глобальные генераторы и распределения ----------
std::mt19937 generator(std::random_device{}());

std::uniform_int_distribution<int> loot_location_one(0, 2);
std::uniform_int_distribution<int> item_loot_location_two(0, 3);
std::uniform_int_distribution<int> loot_location_three(0, 2);

std::uniform_int_distribution<int> zombieDmg(10, 40);
std::uniform_int_distribution<int> zombieSpeedDmg(5, 30);
std::uniform_int_distribution<int> playerDmg(1, 40);
std::uniform_int_distribution<int> shot_chance(1, 10);
std::uniform_int_distribution<int> zombieClass(1, 2);

std::uniform_int_distribution<int> still_no_breathe(1, 3);
std::uniform_int_distribution<int> throw_stone(1, 2);
std::uniform_int_distribution<int> around(1, 4);

std::uniform_int_distribution<int> random_gif(1, 3);
std::uniform_int_distribution<int> xp_box(1, 10);
std::uniform_int_distribution<int> xp_kill(20, 50);
std::uniform_int_distribution<int> level_gif(1000, 2000);
std::uniform_int_distribution<int> body_choice(0, 2);

int choise_damage = 1;
int can_rest = true;
bool retreated = false;

// ---------- Таблицы лута ----------
LootItem drop_table_location_one[3] = {
    {"Металлолом", 10},
    {"Батарейка", 25},
    {"Тряпка", 5}
};

LootItem drop_table_location_two[4] = {
    {"Аптечка", 50},
    {"Патроны", 40},
    {"Деталь оружия", 100},
    {"Золотая цепочка", 150}
};

LootItem drop_table_location_three[3] = {
    {"Кристалл", 300},
    {"Редкий артефакт", 800},
    {"Схема обвеса", 1200}
};

const std::string body_parts[3] = {"Голову", "Тело", "Конечность"};
const double body_multipliers[3] = {2.0, 1.0, 0.5};

// ---------- Прототипы функций ----------
void quitGame(Player& p, Armor& a, Weapon& w, Zombie& z, Hel& h);
void hospital(Player& p, Armor& a, Weapon& w, Zombie& z);
void hunting(Player& p, Armor& a, Weapon& w, Zombie& z, FastZombie& fz, Hel& h);
void menu(Player& p, Armor& a, Weapon& w, Zombie& z, FastZombie& fz, Hel& h);
void number_hunting_one(Player& p, Armor& a, Weapon& w, Zombie& z);
void number_hunting_two(Player& p, Armor& a, Weapon& w, Zombie& z, FastZombie& fz, Hel& h);
void number_hunting_three(Player& p, Armor& a, Weapon& w, Zombie& z, Hel& h);
void add_xp(int amount, Player& p, Armor& a, Weapon& w);
void level_up(Player& p, Armor& a, Weapon& w);
void command_not_exist();
void dead(Player& p, Armor& a, Weapon& w, Zombie& z, Hel& h);
int player_attack_body(Player& p, Armor& a, Weapon& w, Zombie& z);
bool player_withdrawal(Player& p, Armor& a, Weapon& w, Zombie& z, Hel& h);
void player_attack(Player& p, Armor& a, Weapon& w, Zombie& z, Hel& h);
void an_incomprehensible_action(Player& p, Armor& a, Weapon& w, Zombie& z);
void fight(Player& p, Armor& a, Weapon& w, Zombie& z, Hel& h);
void escape(Player& p, Armor& a, Weapon& w, Zombie& z);
void hold_still_and_not_breathe(Player& p, Armor& a, Weapon& w, Zombie& z);
void distract_by_throwing_stone(Player& p, Armor& a, Weapon& w, Zombie& z);
void quietly_behind_back(Player& p, Armor& a, Weapon& w, Zombie& z);
void stealth(Player& p, Armor& a, Weapon& w, Zombie& z, Hel& h);
void workshop(Player& p, Armor& a, Weapon& w, Zombie& z, Hel& h);
void save_game(const Player& p, const Armor& a, const Weapon& w, Zombie& z, Hel& h);
void load_game(Player& p, Armor& a, Weapon& w, Zombie& z, Hel& h);
void show_status(const Player& p, const Armor& a, const Weapon& w);
void hello(Player& p, Armor& a, Weapon& w, Zombie& z, FastZombie& fz, Hel& h);

// ---------- Реализация методов классов ----------

// Zombie::attack – базовая атака
void Zombie::attack(Player& p, Armor& a) {
    this->print_attack_phrase();
    int raw_dmg = this->get_random_damage();
    int final_dmg = static_cast<int>(raw_dmg * (1.0 - a.armor_absorption));
    p.take_damage(final_dmg);
}

// FastZombie::attack – двойной урон
void FastZombie::attack(Player& p, Armor& a) {
    int raw = zombieSpeedDmg(generator);
    int dmg = static_cast<int>(raw * (1.0 - a.armor_absorption));
    int total_dmg = dmg * speed;
    p.take_damage(total_dmg);
    std::cout << "Быстрый зомби наносит двойной урон: " << total_dmg << "\n";
}

void Player::sell_all_loot() {
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

    std::string chosen_phrase;
    for (const auto& item : backpack) {
        auto it = item_phrases.find(item.name);
        if (it != item_phrases.end() && !it->second.empty()) {
            std::uniform_int_distribution<int> dist(0, it->second.size() - 1);
            chosen_phrase = it->second[dist(generator)];
            break;
        }
    }
    if (chosen_phrase.empty()) {
        std::uniform_int_distribution<int> dist(0, sizeof(generic_phrases)/sizeof(generic_phrases[0]) - 1);
        chosen_phrase = generic_phrases[dist(generator)];
    }
    std::cout << chosen_phrase << "\n";

    int total_earned = 0;
    for (const auto& item : backpack)
        total_earned += item.count * item.sell_price;

    add_money(total_earned);
    backpack.clear();
    std::cout << "Скупщик прикинул вес рюкзака и выдал вам " << total_earned
              << " монет. Баланс: " << player_money << ".\n";
}

void command_not_exist() {
    std::cout << "Такой команды не существует!\n";
}

void player_hel(Player& p, Armor& a, Weapon& w, Zombie& z, Hel& h) {
    if (h.get_hel() <= 0) {
        std::cout << "У вас нет аптечек!\n";
        return;
    }
    h.add_hel(-1);
    p.heal_player(h.get_regeneration(), a.player_max_hp);
    std::cout << "Вы использовали аптечку! Здоровье: " << p.get_hp() << "\n";
}

void quitGame(Player& p, Armor& a, Weapon& w, Zombie& z, Hel& h) {
    save_game(p, a, w, z, h);
    std::cout << "\nВыход из системы...\n";
}

void show_status(const Player& p, const Armor& a, const Weapon& w) {
    auto weapon_name = [](int type) -> std::string {
        switch (type) {
            case 1: return "AK-47";
            case 2: return "AWP";
            case 3: return "LR-300";
            case 4: return "MP-5";
            default: return "Без оружия";
        }
    };
    auto percent = [](double val) {
        return std::to_string(static_cast<int>(val * 100)) + "%";
    };
    auto dmg_bonus = [&](double mult) -> std::string {
        if (mult == 1.0) return "нет";
        int diff = static_cast<int>((mult - 1.0) * 100);
        return (diff >= 0 ? "+" : "") + std::to_string(diff) + "%";
    };

    std::cout << "\n┌──────────────────────────────────────┐\n";
    std::cout << "│           СТАТУС  ГЕРОЯ              │\n";
    std::cout << "├──────────────────────────────────────┤\n";
    std::cout << "│ Имя:             " << p.get_name()   << "\n";
    std::cout << "│ Здоровье:        " << p.get_hp() << "/" << a.player_max_hp << "\n";
    std::cout << "│ Монеты:          " << p.get_money() << "\n";
    std::cout << "│ Уровень:         " << p.get_level()
              << "  (XP " << p.get_xp() << "/" << p.xp_for_next_level() << ")\n";
    std::cout << "│ Урон:            " << p.get_damage() << "\n";
    std::cout << "├──────────────────────────────────────┤\n";
    std::cout << "│ Броня:\n";
    std::cout << "│   Поглощение:    " << percent(a.armor_absorption) << "\n";
    std::cout << "│   Бонус к урону: " << dmg_bonus(a.player_damage_multiplier) << "\n";
    std::cout << "│   Скорость лута: x" << a.loot_speed_multiplier << "\n";
    std::cout << "├──────────────────────────────────────┤\n";
    std::cout << "│ Оружие:          " << w.get_name() << "\n";
    std::cout << "│   Множитель урона: " << dmg_bonus(w.get_weapon_damage_mult()) << "\n";
    std::cout << "│   Меткость:       " << (w.get_accuracy_bonus() >= 0 ? "+" : "") << w.get_accuracy_bonus() << "%\n";
    std::cout << "│   Скорость лута:  x" << w.get_weapon_loot_speed() << "\n";
    if (w.get_double_shot_chance() > 0.0)
        std::cout << "│   Двойной выстрел: " << percent(w.get_double_shot_chance()) << "\n";
    if (w.get_guaranteed_kill_chance() > 0.0)
        std::cout << "│   Мгновен. убийство: " << percent(w.get_guaranteed_kill_chance()) << "\n";
    if (w.get_silencer_dodge_chance() > 0.0)
        std::cout << "│   Шанс уклонения: " << percent(w.get_silencer_dodge_chance()) << "\n";
    std::cout << "│ Множитель угрозы: x" << choise_damage << "\n";
    std::cout << "└──────────────────────────────────────┘\n";
}

void save_game(const Player& p, const Armor& a, const Weapon& w, Zombie& z, Hel& h) {
    std::ofstream file("save.txt");
    if (file.is_open()) {
        file << "Имя: " << p.get_name() << "\n"
             << "Здоровье: " << p.get_hp() << "\n"
             << "Деньги: " << p.get_money() << "\n"
             << "Макс_здоровье: " << a.player_max_hp << "\n"
             << "Максимальная энергия: " << a.player_max_energe << "\n"
             << "Урон: " << p.get_damage() << "\n"
             << "Патроны: " << w.get_cartridges() << "\n"
             << "Прочность оружия: " << w.get_endurance() << "\n"
             << "Поглощение: " << a.armor_absorption << "\n"
             << "Множитель_урона: " << a.player_damage_multiplier << "\n"
             << "Множитель_лута: " << a.loot_speed_multiplier << "\n"
             << "Оружие_тип: " << w.get_current_weapon() << "\n"
             << "Множитель_оружия: " << w.get_weapon_damage_mult() << "\n"
             << "Меткость: " << w.get_accuracy_bonus() << "\n"
             << "Скорость_лута_оружия: " << w.get_weapon_loot_speed() << "\n"
             << "Двойной_выстрел: " << w.get_double_shot_chance() << "\n"
             << "Мгновенное_убийство: " << w.get_guaranteed_kill_chance() << "\n"
             << "Шанс_уклонения: " << w.get_silencer_dodge_chance() << "\n"
             << "Множитель_зомби: " << choise_damage << "\n"
             << "Уровень: " << p.get_level() << "\n"
             << "Опыт: " << p.get_xp() << "\n"
             << "Аптечки: " << h.get_hel() << "\n"
             << "Регистрация: " << p.get_reg() << "\n";
        file.close();
        std::cout << "Прогресс сохранён в save.txt\n";
    } else {
        std::cout << "Ошибка при сохранении!\n";
    }
}

void load_game(Player& p, Armor& a, Weapon& w, Zombie& z, Hel& h) {
    std::ifstream file("save.txt");
    if (!file.is_open()) {
        std::cout << "Сохранение не найдено. Начинаем новую игру.\n";
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        size_t colon = line.find(':');
        if (colon == std::string::npos) continue;
        std::string key = line.substr(0, colon);
        std::string value_str = line.substr(colon + 1);
        size_t start = value_str.find_first_not_of(" \t");
        size_t end = value_str.find_last_not_of(" \t");
        if (start != std::string::npos)
            value_str = value_str.substr(start, end - start + 1);
        else
            value_str = "";

        try{
            if (key == "Имя") p.set_name(value_str);
            else if (key == "Здоровье") p.set_hp(std::stoi(value_str));
            else if (key == "Деньги") p.set_money(std::stoi(value_str));
            else if (key == "Макс_здоровье") a.player_max_hp = std::stoi(value_str);
            else if (key == "Максимальная энергия") a.player_max_energe = std::stoi(value_str);
            else if (key == "Урон") p.set_damage(std::stoi(value_str));
            else if (key == "Патроны") w.set_cartridges(std::stoi(value_str));
            else if (key == "Прочность оружия") w.set_endurance(std::stoi(value_str));
            else if (key == "Поглощение") a.armor_absorption = std::stod(value_str);
            else if (key == "Множитель_урона") a.player_damage_multiplier = std::stod(value_str);
            else if (key == "Множитель_лута") a.loot_speed_multiplier = std::stod(value_str);
            else if (key == "Оружие_тип") w.set_current_weapon(std::stoi(value_str));
            else if (key == "Множитель_оружия") w.set_weapon_damage_mult(std::stod(value_str));
            else if (key == "Меткость") w.set_accuracy_bonus(std::stoi(value_str));
            else if (key == "Скорость_лута_оружия") w.set_weapon_loot_speed(std::stod(value_str));
            else if (key == "Двойной_выстрел") w.set_double_shot_chance(std::stod(value_str));
            else if (key == "Мгновенное_убийство") w.set_guaranteed_kill_chance(std::stod(value_str));
            else if (key == "Шанс_уклонения") w.set_silencer_dodge_chance(std::stod(value_str));
            else if (key == "Множитель_зомби") choise_damage = std::stoi(value_str);
            else if (key == "Уровень") p.set_level(std::stoi(value_str));
            else if (key == "Опыт") p.set_xp(std::stoi(value_str));
            else if (key == "Аптечки") h.set_hel(std::stoi(value_str));
            else if (key == "Регистрация") p.set_reg(std::stoi(value_str));
        }
        catch (const std::exception& e) {
            std::cerr << "⚠️ Ошибка загрузки поля '" << key << "': " << e.what()
                      << ". Использую значение по умолчанию.\n";
        }
    }
    file.close();
    std::cout << "Сохранение загружено. Добро пожаловать обратно!\n";
    if (p.get_hp() > a.player_max_hp) p.set_hp(a.player_max_hp);
}

void add_xp(int amount, Player& p, Armor& a, Weapon& w) {
    p.xp_up(amount);
    std::cout << " (+" << amount << " XP)\n";
    while (p.get_xp() >= p.xp_for_next_level()) {
        p.xp_nup(p.xp_for_next_level());
        p.add_level(1);
        std::cout << "\nПоздравляем! Вы достигли уровня " << p.get_level() << "!\n";
        level_up(p, a, w);
    }
}

void level_up(Player& p, Armor& a, Weapon& w) {
    int gif_level = random_gif(generator);
    int choise_gif;
    std::cout << "На " << p.get_level() << " уровне вы можете выбрать улучшение:\n";

    struct Perk {
        std::string description;
        int effect_type;
        double value;
    };

    Perk level_gifts[3][3] = {
        {
            {"[1]: +20 к максимальному здоровью.", 1, 20},
            {"[2]: +1200 монет к балансу.", 2, 1200},
            {"[3]: +10% к скорости лута.", 3, 0.1}
        },
        {
            {"[1]: +10% к урону.", 4, 0.1},
            {"[2]: +200 XP сразу.", 5, 200},
            {"[3]: +10 к максимальному здоровью.", 1, 10}
        },
        {
            {"[1]: +2400 монет к балансу.", 2, 2400},
            {"[2]: +1% к шансу мгновенного убийства.", 6, 0.01},
            {"[3]: +5% к поглощению урона.", 7, 0.05}
        }
    };

    int lvl_idx = gif_level - 1;

    while (true) {
        for (int i = 0; i < 3; i++) {
            std::cout << level_gifts[lvl_idx][i].description << "\n";
        }
        std::cin >> choise_gif;
        if (choise_gif >= 1 && choise_gif <= 3) {
            int perk_idx = choise_gif - 1;
            int type = level_gifts[lvl_idx][perk_idx].effect_type;
            double val = level_gifts[lvl_idx][perk_idx].value;

            if (type == 1) {
                a.player_max_hp += val;
                p.set_hp(a.player_max_hp);
                if (p.get_hp() > a.player_max_hp) p.set_hp(a.player_max_hp);
                std::cout << "Максимальное здоровье увеличено.\n";
            } else if (type == 2) {
                p.add_money(val);
                std::cout << "Баланс пополнен.\n";
            } else if (type == 3) {
                a.loot_speed_multiplier += val;
            } else if (type == 4) {
                a.player_damage_multiplier += val;
            } else if (type == 5) {
                add_xp(val, p, a, w);
            } else if (type == 6) {
                w.add_guaranteed_kill_chance(val);
            } else if (type == 7) {
                a.armor_absorption += val;
                if (a.armor_absorption > 0.9) a.armor_absorption = 0.9;
            }
            break;
        } else {
            command_not_exist();
        }
    }

    int bonus = level_gif(generator);
    p.add_money(bonus);
    std::cout << "Бонусом вам зачислили " << bonus << " монет" << std::endl;
}

void hospital(Player& p, Armor& a, Weapon& w, Zombie& z) {
    std::cout << "\nВы в больнице\n";
    std::cout << "(Баланс: " << p.get_money() << ", здоровье: " << p.get_hp() << ")\n";
    std::cout << "1 хп = 2 монеты\n";
    int hospital_choise;
    while (true) {
        std::cout << "Сколько здоровья восстановить? (0 - выход): ";
        std::cin >> hospital_choise;
        if (hospital_choise == 0) {
            std::cout << "Вы вышли из больницы.\n";
            break;
        }
        if (hospital_choise < 0 || hospital_choise > 100) {
            std::cout << "Некорректное значение. Введите от 0 до 100.\n";
            continue;
        }
        int cost = hospital_choise * 2;
        if (p.get_money() < cost) {
            std::cout << "Недостаточно монет!\n";
            continue;
        }
        std::cout << "Лечение " << hospital_choise << " HP за " << cost << " монет. Согласны? (да/нет): ";
        std::string ans;
        std::cin >> ans;
        if (ans == "да") {
            p.reduce_money(cost);
            p.heal_player(hospital_choise, a.player_max_hp);
            std::cout << "Теперь у вас " << p.get_hp() << " здоровья.\n";
            break;
        } else {
            std::cout << "Лечение отменено.\n";
            break;
        }
    }
}

void hunting(Player& p, Armor& a, Weapon& w, Zombie& z, FastZombie& fz, Hel& h) {
    std::cout << "\nВы выдвинулись на охоту\n";
    int number_hunting;
    std::cout << "Вам надо обдумать план вылазки\n";
    while (true) {
        std::cout << "Здоровье: " << p.get_hp() << "\n"
                  << "Энергия: " << p.get_energe() << "\n";
        std::cout << "Вариант №1: Вы спокойно обыскиваете все ящики и уходите на базу [УРОВЕНЬ ЛУТА: МИНИМАЛЕН]\n";
        std::cout << "Вариант №2: Вы прорываетесь сквозь зомби и лутаете самые лучшие ящики [УРОВЕНЬ ЛУТА ВЫСОКИЙ]\n";
        std::cout << "Вариант №3: Вы незаметно протискиваетесь сквозь толпы зомби и лутаете лучший лут, "
                     "но велик шанс что зомби вас учуят [УРОВЕНЬ ЛУТА МАКСИМАЛЬНЫЙ]\n";
        std::cout << "Вариант №4: Вернуться на базу\n";
        std::cin >> number_hunting;
        if (number_hunting == 1) { number_hunting_one(p, a, w, z); break; }
        else if (number_hunting == 2) { number_hunting_two(p, a, w, z, fz, h); break; }
        else if (number_hunting == 3) { number_hunting_three(p, a, w, z, h); break; }
        else if (number_hunting == 4) { break; }
        else {
            std::cout << "Такой команды не существует! Выберите 1, 2 или 3.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

void dead(Player& p, Armor& a, Weapon& w, Zombie& z, Hel& h) {
    std::cout << "Вы погибли... Игра окончена.\n";
    std::cout << "Вы потеряли " << p.get_money() << " монет.\n";
    p.set_money(0);
    p.set_hp(100);
    h.set_hel(0);
    can_rest = true;
    save_game(p, a, w, z, h);
}

int player_attack_body(Player& p, Armor& a, Weapon& w, Zombie& z) {
    if (w.get_cartridges() <= 0) {
        std::cout << "Осечка! Нет патронов.\n";
        return 0;
    }
    if (w.get_endurance() <= 0) {
        std::cout << "Оружие сломано! Вы не можете стрелять.\n";
        return 0;
    }
    w.remuve_cartridges(1);
    w.remuve_endurance(1);
    int idx = body_choice(generator);
    double mult = body_multipliers[idx];
    int dmg = static_cast<int>(p.get_damage() * mult * a.player_damage_multiplier * w.get_weapon_damage_mult());
    z.zombie_hp -= dmg;
    std::cout << "Вы попали в " << body_parts[idx] << " и нанесли " << dmg << " урона! (патронов: "
              << w.get_cartridges() << "/" << w.get_max_cartridges() << ")\n";
    return dmg;
}

bool player_withdrawal(Player& p, Armor& a, Weapon& w, Zombie& z, Hel& h) {
    std::cout << "Вы пытаетесь отойти...\n";
    if (shot_chance(generator) <= 3) {
        z.attack(p, a);
        std::cout << "Вас задели, не получилось уйти.\n";
        return false;
    } else {
        std::cout << "Вы благополучно отошли.\n";
        retreated = true;
        return true;
    }
}

void player_attack(Player& p, Armor& a, Weapon& w, Zombie& z, Hel& h) {
    int shot = shot_chance(generator);
    int threshold = 6 + (w.get_accuracy_bonus() / 10);
    if (threshold > 10) threshold = 10;
    if (shot <= threshold) {
        if (w.get_guaranteed_kill_chance() > 0.0) {
            std::uniform_real_distribution<double> d(0.0, 1.0);
            if (d(generator) < w.get_guaranteed_kill_chance()) {
                std::cout << "AWP пробивает зомби насквозь! Мгновенное убийство!\n";
                z.zombie_hp = 0;
                return;
            }
        }
        int shots = 1;
        if (w.get_double_shot_chance() > 0.0) {
            std::uniform_real_distribution<double> d(0.0, 1.0);
            if (d(generator) < w.get_double_shot_chance()) {
                shots = 2;
                std::cout << "Двойной выстрел! ";
            }
        }
        for (int s = 0; s < shots; ++s) {
            player_attack_body(p, a, w, z);
        }
        std::cout << "У зомби осталось " << z.zombie_hp << " здоровья.\n";
    } else {
        z.attack(p, a);
        std::cout << "Вы промахнулись!\n";
        std::cout << "У вас осталось " << p.get_hp() << " здоровья.\n";
    }
}

void an_incomprehensible_action(Player& p, Armor& a, Weapon& w, Zombie& z) {
    std::cout << "Непонятное действие. Зомби заметил вас!\n";
    int raw = zombieDmg(generator) * (2 * choise_damage);
    int dmg = static_cast<int>(raw * (1.0 - a.armor_absorption));
    p.take_damage(dmg);
}

void fight(Player& p, Armor& a, Weapon& w, Zombie& z, Hel& h) {
    std::cout << "\nВы встретили зомби!\n"
              << "\nВаше здоровье: " << p.get_hp()
              << "\nВаш урон: " << p.get_damage()
              << "\nВаша энергия: " << p.get_energe() << "\n\n";

    struct ChoiceInfo {
        std::string name;
        int energe;
        std::function<void()> action;
    };

    ChoiceInfo choice[] = {
        {"Выстрел", 5, [&]() { player_attack(p, a, w, z, h); }},
        {"Отойти",  15, [&]() { player_withdrawal(p, a, w, z, h); }},
        {"Аптечка", 10, [&]() { player_hel(p, a, w, z, h); }}
    };

    while (p.get_hp() > 0 && z.zombie_hp > 0) {
        for (int i = 0; i < 3; i++) {
            std::cout << "[" << choice[i].name << "]: " << choice[i].energe << " энергии" << std::endl;
        }

        std::cout << "Ваши действия: \n";
        int fight_choice;
        std::cin >> fight_choice;
        retreated = false;
        if (fight_choice > 0 && fight_choice < 4) {
            int idx = fight_choice - 1;
            if (idx >= 0 && idx < 3) {
                p.reduce_energe(choice[idx].energe);
                choice[idx].action();
            }
        } else {
            std::cout << "Непонятная команда. Зомби атакует!\n";
            z.attack(p, a);
        }

        if (p.get_hp() <= 0) { dead(p, a, w, z, h); exit(0); }
        if (z.zombie_hp <= 0) {
            std::cout << "\nЗомби повержен! Поздравляю!\n";
            int xp_gain = xp_kill(generator);
            std::cout << "Вы получили " << xp_gain << " опыта!\n";
            add_xp(xp_gain, p, a, w);
            z.zombie_hp = 100;
        }

        if (retreated) {
            break;
        }
    }
    can_rest = true;
}

void escape(Player& p, Armor& a, Weapon& w, Zombie& z) {
    std::cout << "\nВы пытаетесь убежать...\n";
    if (shot_chance(generator) <= 3) {
        int raw = zombieDmg(generator) * (2 * choise_damage);
        int dmg = static_cast<int>(raw * (1.0 - a.armor_absorption));
        p.take_damage(dmg);
        std::cout << "Зомби задел вас, нанеся " << dmg << " урона.\n";
    } else {
        std::cout << "Вы благополучно убежали, но ящик не тронут.\n";
    }
}

void hold_still_and_not_breathe(Player& p, Armor& a, Weapon& w, Zombie& z) {
    if (still_no_breathe(generator) == 1) {
        int raw = zombieDmg(generator) * (2 * choise_damage);
        int dmg = static_cast<int>(raw * (1.0 - a.armor_absorption));
        p.take_damage(dmg);
        std::cout << "\nЗомби вас учуял, нанеся " << dmg << " урона.\n";
    } else {
        std::cout << "\nВы скрылись.\n";
        z.zombie_hp = 0;
        std::cout << "\nВы получили опыт за нейтрализацию зомби!\n";
        add_xp(xp_kill(generator), p, a, w);
    }
}

void distract_by_throwing_stone(Player& p, Armor& a, Weapon& w, Zombie& z) {
    int res = throw_stone(generator);
    if (res == 1) {
        choise_damage += 1;
        std::cout << "\nВы привлекли больше зомби! Урон удвоен.\n";
    } else {
        std::cout << "\nВы отвлекли зомби.\n";
        z.zombie_hp = 0;
        std::cout << "\nВы получили опыт за нейтрализацию зомби!\n";
        add_xp(xp_kill(generator), p, a, w);
    }
}

void quietly_behind_back(Player& p, Armor& a, Weapon& w, Zombie& z) {
    if (around(generator) <= 2) {
        int raw = zombieDmg(generator) * (2 * choise_damage);
        int dmg = static_cast<int>(raw * (1.0 - a.armor_absorption));
        p.take_damage(dmg);
        std::cout << "\nВас обнаружили, нанеся " << dmg << " урона.\n";
    } else {
        std::cout << "\nВы обошли зомби и обезвредили его!\n";
        z.zombie_hp = 0;
        std::cout << "\nВы получили опыт за нейтрализацию зомби!\n";
        add_xp(xp_kill(generator), p, a, w);
    }
}

void stealth(Player& p, Armor& a, Weapon& w, Zombie& z, Hel& h) {
    std::cout << "Зомби уже совсем рядом\n"
              << "[1] Убежать\n[2] Затаиться\n[3] Отвлечь камнем\n[4] Обойти за спину\n";
    int c;
    std::cin >> c;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (c == 1) escape(p, a, w, z);
    else if (c == 2) hold_still_and_not_breathe(p, a, w, z);
    else if (c == 3) distract_by_throwing_stone(p, a, w, z);
    else if (c == 4) quietly_behind_back(p, a, w, z);
    else an_incomprehensible_action(p, a, w, z);

    if (p.get_hp() <= 0) { dead(p, a, w, z, h); exit(0); }

    can_rest = true;
}

void number_hunting_one(Player& p, Armor& a, Weapon& w, Zombie& z) {
    const int BOX = 5;
    for (int i = 0; i < BOX; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(3000 / (a.loot_speed_multiplier * w.get_weapon_loot_speed()))));
        int item_idx = loot_location_one(generator);
        int xp_gain = xp_box(generator);
        std::cout << "Ящик " << i+1 << ": найдено: " << drop_table_location_one[item_idx].name
                  << " (цена: " << drop_table_location_one[item_idx].sell_price << ")"
                  << " +[" << xp_gain << "] опыта!\n";
        add_xp(xp_gain, p, a, w);
        p.add_item(drop_table_location_one[item_idx].name, drop_table_location_one[item_idx].sell_price);
    }
    p.show_backpack();
}

void number_hunting_two(Player& p, Armor& a, Weapon& w, Zombie& z, FastZombie& fz, Hel& h) {
    const int BOX = 5;
    std::cout << "Прорываетесь к лучшим ящикам!\n";
    std::uniform_int_distribution<int> encounter(1, 5);
    for (int i = 0; i < BOX; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(3000 / (a.loot_speed_multiplier * w.get_weapon_loot_speed()))));
        if (encounter(generator) >= 4) {
            std::cout << "Зомби у ящика " << i+1 << "!\n";
            Zombie* currentZ = &z;
            if (zombieClass(generator) == 1) {
                currentZ = &fz;
                std::cout << "(Это быстрый зомби!)\n";
            }
            fight(p, a, w, *currentZ, h);
            if (p.get_hp() <= 0) return;
            if (currentZ->zombie_hp > 0) { std::cout << "Зомби помешал обыскать!\n"; continue; }
        }
        int item_idx = item_loot_location_two(generator);
        int xp_gain = xp_box(generator);
        std::cout << "Ящик " << i+1 << ": найдено: " << drop_table_location_two[item_idx].name
                  << " (цена: " << drop_table_location_two[item_idx].sell_price << ")"
                  << " +[" << xp_gain << "] опыта!\n";
        add_xp(xp_gain, p, a, w);
        p.add_item(drop_table_location_two[item_idx].name, drop_table_location_two[item_idx].sell_price);
    }
    p.show_backpack();
}

void number_hunting_three(Player& p, Armor& a, Weapon& w, Zombie& z, Hel& h) {
    const int BOX = 5;
    std::uniform_int_distribution<int> enc(1, 5);
    std::uniform_int_distribution<int> event(1, 11);
    int ev = event(generator);
    if (ev <= 5) {
        for (int i = 0; i < BOX; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(3000 / (a.loot_speed_multiplier * w.get_weapon_loot_speed()))));
            if (enc(generator) == 5) {
                std::cout << "Зомби у ящика " << i+1 << "!\n";
                stealth(p, a, w, z, h);
                if (p.get_hp() <= 0) return;
                if (z.zombie_hp > 0) { std::cout << "Зомби помешал обыскать!\n"; continue; }
            }
            int item_idx = loot_location_three(generator);
            int xp_gain = xp_box(generator);
            std::cout << "Ящик " << i+1 << ": найдено: " << drop_table_location_three[item_idx].name
                      << " (цена: " << drop_table_location_three[item_idx].sell_price << ")"
                      << " +[" << xp_gain << "] опыта!\n";
            add_xp(xp_gain, p, a, w);
            p.add_item(drop_table_location_three[item_idx].name, drop_table_location_three[item_idx].sell_price);
        }
        p.show_backpack();
    } else {
        std::cout << "Зомби очень активны, но лут лучше!\n";
        for (int i = 0; i < BOX; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(3000 / (a.loot_speed_multiplier * w.get_weapon_loot_speed()))));
            if (enc(generator) >= 3) {
                std::cout << "Зомби у ящика " << i+1 << "!\n";
                stealth(p, a, w, z, h);
                if (p.get_hp() <= 0) return;
                if (z.zombie_hp > 0) { std::cout << "Зомби помешал обыскать!\n"; continue; }
            }
            int item_idx = loot_location_three(generator);
            int xp_gain = xp_box(generator);
            int price = drop_table_location_three[item_idx].sell_price * 2;
            std::cout << "Ящик " << i+1 << ": найдено: " << drop_table_location_three[item_idx].name
                      << " (цена: " << price << ")"
                      << " +[" << xp_gain << "] опыта!\n";
            add_xp(xp_gain, p, a, w);
            p.add_item(drop_table_location_three[item_idx].name, price);
        }
    }
    p.show_backpack();
}

void workshop(Player& p, Armor& a, Weapon& w, Zombie& z, Hel& h) {
    std::cout << "\n=== МАСТЕРСКАЯ ===\n"
              << "[1]: Экипировка\n"
              << "[2]: Оружие\n"
              << "[3]: Верстак\n"
              << "[4]: Обвесы\n";
    int choise_workshop;
    std::cin >> choise_workshop;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (choise_workshop == 1) {
        struct ArmorInfo {
            std::string name;
            double absorption;
            double damage_multiplier;
            double loot_speed;
            int max_hp;
        };
        ArmorInfo armors[] = {
            {"Тяжёлая броня", 0.75, 0.7, 0.5, 120},
            {"Лёгкая броня", -0.25, 1.1, 5.0, 100},
            {"Средняя броня", 0.10, 1.1, 1.1, 110},
            {"Начальная броня", -0.05, 1.0, 0.8, 100}
        };
        std::cout << "\n--- ЭКИПИРОВКА ---\n";
        for (int i = 0; i < 4; ++i) {
            std::cout << "[" << i+1 << "] " << armors[i].name
                      << " | Поглощение: " << static_cast<int>(armors[i].absorption * 100) << "%"
                      << " | Урон: x" << armors[i].damage_multiplier
                      << " | Скорость лута: x" << armors[i].loot_speed
                      << " | Макс. HP: " << armors[i].max_hp << "\n";
        }
        int choise_armor;
        std::cout << "Что хотите надеть? (1-4): ";
        std::cin >> choise_armor;
        if (choise_armor < 1 || choise_armor > 4) {
            command_not_exist();
            return;
        }
        int idx = choise_armor - 1;
        a.armor_absorption = armors[idx].absorption;
        a.player_damage_multiplier = armors[idx].damage_multiplier;
        a.loot_speed_multiplier = armors[idx].loot_speed;
        a.player_max_hp = armors[idx].max_hp;
        a.armor_name = armors[idx].name;
        if (p.get_hp() > a.player_max_hp) p.set_hp(a.player_max_hp);
        std::cout << "Вы успешно экипировали " << armors[idx].name << "!\n";
    }
    else if (choise_workshop == 2) {
        struct WeaponInfo {
            std::string name;
            int price;
            double dmg_mult;
            int acc_bonus;
            double loot_mult;
            double kill_chance;
            double double_shot;
        };
        WeaponInfo shop[] = {
            {"AK-47", 5400, 1.6,  0, 1.1,  0.0, 0.0},
            {"AWP",   8400, 2.5, 50, 0.5,  0.05, 0.0},
            {"LR-300",3600, 1.4,  5, 1.0,  0.0, 0.0},
            {"MP-5",  5400, 1.0,  5, 1.15, 0.0, 0.2}
        };
        std::cout << "\n--- МАГАЗИН ОРУЖИЯ ---\n";
        for (int i = 0; i < 4; ++i) {
            std::cout << "[" << i+1 << "] " << shop[i].name
                      << " | Цена: " << shop[i].price
                      << " | Урон: x" << shop[i].dmg_mult
                      << " | Меткость: +" << shop[i].acc_bonus << "%"
                      << " | Лут: x" << shop[i].loot_mult
                      << " | Ваншот: " << static_cast<int>(shop[i].kill_chance * 100) << "%"
                      << " | Двойной: " << static_cast<int>(shop[i].double_shot * 100) << "%\n";
        }
        int choice_weapon;
        std::cout << "Что хотите купить? (1-4): ";
        std::cin >> choice_weapon;
        if (choice_weapon < 1 || choice_weapon > 4) {
            command_not_exist();
            return;
        }
        int idx = choice_weapon - 1;
        if (p.get_money() < shop[idx].price) {
            std::cout << "Недостаточно монет! Нужно " << shop[idx].price
                      << ", у вас " << p.get_money() << ".\n";
        } else {
            p.reduce_money(shop[idx].price);
            w.set_current_weapon(choice_weapon);
            w.set_weapon_damage_mult(shop[idx].dmg_mult);
            w.set_accuracy_bonus(shop[idx].acc_bonus);
            w.set_weapon_loot_speed(shop[idx].loot_mult);
            w.set_guaranteed_kill_chance(shop[idx].kill_chance);
            w.set_double_shot_chance(shop[idx].double_shot);
            w.set_silencer_dodge_chance(0.0);
            w.set_name(shop[idx].name);
            std::cout << "Вы купили и экипировали " << shop[idx].name << "!\n";
        }
    }
    else if (choise_workshop == 3) {
        struct CraftInfo {
            std::string craft_name;
            int craft_price;
        };

        CraftInfo craft[] = {
            {"Починка", 250},
            {"Патрон", 10},
            {"Аптечка", 150}
        };

        std::cout << "Текущая прочность: " << w.get_endurance() << "/100\n";
        std::cout << "Патроны: " << w.get_cartridges() << "/" << w.get_max_cartridges() << "\n";
        for (int i = 0; i < 3; ++i) {
            std::cout << "[" << i+1 << "] " << craft[i].craft_name
                      << " будет стоить: " << craft[i].craft_price << " монет\n";
        }
        int g;
        std::cout << "Что выбираете? ";
        std::cin >> g;

        if (g < 1 || g > 3) {
            std::cout << "\n";
            return;
        }

        int idx = g - 1;
        if (p.get_money() < craft[idx].craft_price) {
            std::cout << "Недостаточно монет! Нужно " << craft[idx].craft_price
                      << ", у вас " << p.get_money() << ".\n";
        } else {
            if (g == 1) {
                p.reduce_money(craft[idx].craft_price);
                w.add_endurance(100 - w.get_endurance());
                std::cout << "Оружие полностью починено! Прочность: " << w.get_endurance() << "/100\n";
            } else if (g == 2) {
                int f;
                std::cout << "Сколько хотите сделать патронов? ";
                std::cin >> f;
                int cost = f * craft[idx].craft_price;
                if (p.get_money() >= cost) {
                    p.reduce_money(cost);
                    w.add_cartridges(f);
                    if (w.get_cartridges() > 30) w.set_cartridges(30);
                    std::cout << "Теперь у вас " << w.get_cartridges() << " патронов\n";
                } else {
                    std::cout << "Недостаточно монет!\n";
                }
            } else if (g == 3) {
                int f;
                std::cout << "Сколько хотите сделать аптечек? ";
                std::cin >> f;
                int cost = f * craft[idx].craft_price;
                while (true) {
                    if (h.get_hel() >= 5) {
                        std::cout << "Аптечек и так 5!" << std::endl;
                        h.set_hel(5);
                        break;
                    }
                    if (p.get_money() < cost) {
                        std::cout << "Не достаточно монет\n";
                        break;
                    } else if ((h.get_hel() + f) > 5) {
                        std::cout << "Аптечек не может быть больше 5\n";
                        break;
                    } else {
                        p.reduce_money(cost);
                        h.add_hel(f);
                        std::cout << "Теперь у вас " << h.get_hel() << " аптечекn\n";
                        break;
                    }
                }
            }
        }
    }
    else if (choise_workshop == 4) {
        std::cout << "Обвесы в разработке.\n";
    }
    else {
        command_not_exist();
    }
}


void mine_ferm(Player& p, Armor& a, Weapon& w, Zombie& z, Hel& h) {
    std::cout << "Потом" << std::endl;
}

void rest(Player& p, Armor& a, Weapon& w, Zombie& z, Hel& h) {
    if (!can_rest) {
        std::cout << "Вы недавно отдыхали, эта функция временна заблокирована" << std::endl;
    } else {
        if (p.get_energe() <= 5) {
            a.add_max_energe(10);
            p.set_energe(a.player_max_energe);
            std::cout << "Вы так сильно устали, что не только восстановили выносливость, но и подняли максимальный уровень!\n"
                      << "Максимальный уровень выносливости: " << a.player_max_energe << "\n"
                      << "Выносливость: " << p.get_energe() << std::endl;
            can_rest = false;
        } else {
            p.set_energe(a.player_max_energe);
            std::cout << "Вы отдохнули как обычно, выносливость: " << p.get_energe() << std::endl;
            can_rest = false;
        }
    }
}

void home(Player& p, Armor& a, Weapon& w, Zombie& z, Hel& h) {
    std::cout << "\n--- ДОМ ---\n";
    struct HomeInfo {
        std::string room_name;
        std::function<void()> room;
    };

    HomeInfo home[] = {
        {"Отдых", [&]() { rest(p, a, w, z, h); }},
        {"Майнинг ферма", [&]() { mine_ferm(p, a, w, z, h); }}
    };

    int room_count = sizeof(home) / sizeof(home[0]);

    for (int i = 0; i < room_count; i++) {
        std::cout << "[" << i+1 << "]: " << home[i].room_name << "\n";
    }

    int f;
    std::cout << "В какую комнату пойти? \n";
    std::cin >> f;

    if (f < 1 || f > room_count) {
        command_not_exist();
    } else {
        int idx = f - 1;
        home[idx].room();
    }
}

void menu(Player& p, Armor& a, Weapon& w, Zombie& z, FastZombie& fz, Hel& h) {
    std::cout << "\n=== БАЗА ===\n";

    struct MenuInfo {
        std::string name;
        std::function<void()> action;
    };

    MenuInfo items[] = {
        {"Охота",      [&]() { hunting(p, a, w, z, fz, h); }},
        {"Больница",   [&]() { hospital(p, a, w, z); }},
        {"Мастерская", [&]() { workshop(p, a, w, z, h); }},
        {"Сохранить",  [&]() { save_game(p, a, w, z, h); }},
        {"Статистика", [&]() { show_status(p, a, w); }},
        {"Скупщик",    [&]() { p.sell_all_loot(); }},
        {"Дом",        [&]() { home(p, a, w, z, h); }},
        {"Выход",      [&]() { quitGame(p, a, w, z, h); }}
    };

    const int count = sizeof(items) / sizeof(items[0]);

    while (true) {
        if (p.get_hp() <= 0) dead(p, a, w, z, h);

        std::cout << "Баланс: " << p.get_money() << " монет | Здоровье: " << p.get_hp() << "/" << a.player_max_hp
                  << " | Уровень: " << p.get_level() << " (XP: " << p.get_xp() << "/" << p.xp_for_next_level() << ")\n";

        for (int i = 0; i < count; ++i)
            std::cout << "[" << i + 1 << "] " << items[i].name << "\n";

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice >= 1 && choice <= count) {
            int idx = choice - 1;
            items[idx].action();

            if (idx == count - 1) break;
            std::cout << "\nВы вернулись на базу.\n";
        } else {
            command_not_exist();
        }
    }
}

void hello(Player& p, Armor& a, Weapon& w, Zombie& z, FastZombie& fz, Hel& h) {
    load_game(p, a, w, z, h);
    if (p.get_reg() == 1) {
        std::cout << "С возвращением, " << p.get_name() << "!\n";
        menu(p, a, w, z, fz, h);
    } else {
        std::cout << "Текстовая RPG с системой уровней!\nВведите никнейм: ";
        std::string name;
        while (true) {
            std::getline(std::cin, name);
            if (name.size() >= 6 && name.size() <= 17) {
                bool ok = true;
                for (char c : name) if (!std::isalpha(static_cast<unsigned char>(c))) { ok = false; break; }
                if (ok) break;
                else std::cout << "Только буквы!\n";
            } else {
                std::cout << "Длина от 6 до 17 символов!\n";
            }
        }
        p.set_name(name);
        if (p.get_damage() == 20) {
            p.set_damage(20 + playerDmg(generator));
        }
        p.set_reg(1);
        save_game(p, a, w, z, h);
        std::cout << "Добро пожаловать, " << name << "!\n";
        menu(p, a, w, z, fz, h);
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    Player p;
    Armor a;
    Weapon w;
    Zombie z;
    FastZombie fz;
    Hel h;

    hello(p, a, w, z, fz, h);

    return 0;
}