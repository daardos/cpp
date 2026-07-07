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

#include "struct.h"

// -------------------- Глобальные генераторы --------------------
std::mt19937 generator(std::random_device{}());

std::uniform_int_distribution<int> loot_location_one(0, 20);
std::uniform_int_distribution<int> loot_location_two(10, 100);
std::uniform_int_distribution<int> loot_location_three(100, 1000);

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

// -------------------- Глобальные переменные --------------------
int choise_damage = 1;

// -------------------- Константы боя --------------------
const std::string body_parts[3] = {"Голову", "Тело", "Конечность"};
const double body_multipliers[3] = {2.0, 1.0, 0.5};
std::uniform_int_distribution<int> body_choice(0, 2);

// -------------------- Прототипы --------------------
void quitGame(Player& p, Armor& a, Weapon& w, Zombie& z);
void hospital(Player& p, Armor& a, Weapon& w, Zombie& z);
void hunting(Player& p, Armor& a, Weapon& w, Zombie& z, FastZombie& fz);
void menu(Player& p, Armor& a, Weapon& w, Zombie& z, FastZombie& fz);
void number_hunting_one(Player& p, Armor& a, Weapon& w, Zombie& z);
void number_hunting_two(Player& p, Armor& a, Weapon& w, Zombie& z, FastZombie& fz);
void number_hunting_three(Player& p, Armor& a, Weapon& w, Zombie& z);
void add_xp(int amount, Player& p, Armor& a, Weapon& w);
void level_up(Player& p, Armor& a, Weapon& w);
void command_not_exist();
void dead(Player& p, Armor& a, Weapon& w, Zombie& z);
int player_attack_body(Player& p, Armor& a, Weapon& w, Zombie& z);
bool player_withdrawal(Player& p, Armor& a, Weapon& w, Zombie& z);
void player_attack(Player& p, Armor& a, Weapon& w, Zombie& z);
void an_incomprehensible_action(Player& p, Armor& a, Weapon& w, Zombie& z);
void fight(Player& p, Armor& a, Weapon& w, Zombie& z);
void escape(Player& p, Armor& a, Weapon& w, Zombie& z);
void hold_still_and_not_breathe(Player& p, Armor& a, Weapon& w, Zombie& z);
void distract_by_throwing_stone(Player& p, Armor& a, Weapon& w, Zombie& z);
void quietly_behind_back(Player& p, Armor& a, Weapon& w, Zombie& z);
void stealth(Player& p, Armor& a, Weapon& w, Zombie& z);
void workshop(Player& p, Armor& a, Weapon& w, Zombie& z);
void save_game(const Player& p, const Armor& a, const Weapon& w, Zombie& z);
void load_game(Player& p, Armor& a, Weapon& w, Zombie& z);
void show_status(const Player& p, const Armor& a, const Weapon& w);
void hello(Player& p, Armor& a, Weapon& w, Zombie& z, FastZombie& fz);

// -------------------- Реализации методов зомби --------------------
void Zombie::attack(Player& p, Armor& a) {
    int raw = zombieDmg(generator);
    int dmg = static_cast<int>(raw * (1.0 - a.armor_absorption));
    p.player_hp -= dmg;
    std::cout << "Зомби наносит " << dmg << " урона.\n";
}

void FastZombie::attack(Player& p, Armor& a) {
    int raw = zombieSpeedDmg(generator);
    int dmg = static_cast<int>(raw * (1.0 - a.armor_absorption));
    int total_dmg = dmg * speed;   // двойной урон за счёт скорости
    p.player_hp -= total_dmg;
    std::cout << "Быстрый зомби наносит двойной урон: " << total_dmg << "\n";
}

// -------------------- Остальные функции --------------------

void command_not_exist() {
    std::cout << "Такой команды не существует!\n";
}

void quitGame(Player& p, Armor& a, Weapon& w, Zombie& z) {
    save_game(p, a, w, z);
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

    std::cout << "│ Имя:             " << p.player_name   << "\n";
    std::cout << "│ Здоровье:        " << p.player_hp << "/" << a.player_max_hp << "\n";
    std::cout << "│ Монеты:          " << p.player_money << "\n";
    std::cout << "│ Уровень:         " << p.player_level
              << "  (XP " << p.player_xp << "/" << p.xp_for_next_level() << ")\n";
    std::cout << "│ Урон:            " << p.player_damage << "\n";
    std::cout << "├──────────────────────────────────────┤\n";

    std::cout << "│ Броня:\n";
    std::cout << "│   Поглощение:    " << percent(a.armor_absorption) << "\n";
    std::cout << "│   Бонус к урону: " << dmg_bonus(a.player_damage_multiplier) << "\n";
    std::cout << "│   Скорость лута: x" << a.loot_speed_multiplier << "\n";
    std::cout << "├──────────────────────────────────────┤\n";

    std::cout << "│ Оружие:          " << weapon_name(w.current_weapon) << "\n";
    std::cout << "│   Множитель урона: " << dmg_bonus(w.weapon_damage_mult) << "\n";
    std::cout << "│   Меткость:       " << (w.accuracy_bonus >= 0 ? "+" : "") << w.accuracy_bonus << "%\n";
    std::cout << "│   Скорость лута:  x" << w.weapon_loot_speed << "\n";
    if (w.double_shot_chance > 0.0)
        std::cout << "│   Двойной выстрел: " << percent(w.double_shot_chance) << "\n";
    if (w.guaranteed_kill_chance > 0.0)
        std::cout << "│   Мгнов. убийство: " << percent(w.guaranteed_kill_chance) << "\n";
    if (w.silencer_dodge_chance > 0.0)
        std::cout << "│   Шанс уклонения: " << percent(w.silencer_dodge_chance) << "\n";

    std::cout << "│ Множитель угрозы: x" << choise_damage << "\n";
    std::cout << "└──────────────────────────────────────┘\n";
}

void save_game(const Player& p, const Armor& a, const Weapon& w, Zombie& z) {
    std::ofstream file("save.txt");
    if (file.is_open()) {
        file << "Имя: " << p.player_name << "\n"
             << "Здоровье: " << p.player_hp << "\n"
             << "Деньги: " << p.player_money << "\n"
             << "Макс_здоровье: " << a.player_max_hp << "\n"
             << "Урон: " << p.player_damage << "\n"
             << "Поглощение: " << a.armor_absorption << "\n"
             << "Множитель_урона: " << a.player_damage_multiplier << "\n"
             << "Множитель_лута: " << a.loot_speed_multiplier << "\n"
             << "Оружие_тип: " << w.current_weapon << "\n"
             << "Множитель_оружия: " << w.weapon_damage_mult << "\n"
             << "Меткость: " << w.accuracy_bonus << "\n"
             << "Скорость_лута_оружия: " << w.weapon_loot_speed << "\n"
             << "Двойной_выстрел: " << w.double_shot_chance << "\n"
             << "Мгновенное_убийство: " << w.guaranteed_kill_chance << "\n"
             << "Шанс_уклонения: " << w.silencer_dodge_chance << "\n"
             << "Множитель_зомби: " << choise_damage << "\n"
             << "Уровень: " << p.player_level << "\n"
             << "Опыт: " << p.player_xp << "\n"
             << "Регистрация: " << p.player_reg << "\n";
        file.close();
        std::cout << "Прогресс сохранён в save.txt\n";
    } else {
        std::cout << "Ошибка при сохранении!\n";
    }
}

void load_game(Player& p, Armor& a, Weapon& w, Zombie& z) {
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

        if (key == "Имя")
            p.player_name = value_str;
        else if (key == "Здоровье")
            p.player_hp = std::stoi(value_str);
        else if (key == "Деньги")
            p.player_money = std::stoi(value_str);
        else if (key == "Макс_здоровье")
            a.player_max_hp = std::stoi(value_str);
        else if (key == "Урон")
            p.player_damage = std::stoi(value_str);
        else if (key == "Поглощение")
            a.armor_absorption = std::stod(value_str);
        else if (key == "Множитель_урона")
            a.player_damage_multiplier = std::stod(value_str);
        else if (key == "Множитель_лута")
            a.loot_speed_multiplier = std::stod(value_str);
        else if (key == "Оружие_тип")
            w.current_weapon = std::stoi(value_str);
        else if (key == "Множитель_оружия")
            w.weapon_damage_mult = std::stod(value_str);
        else if (key == "Меткость")
            w.accuracy_bonus = std::stoi(value_str);
        else if (key == "Скорость_лута_оружия")
            w.weapon_loot_speed = std::stod(value_str);
        else if (key == "Двойной_выстрел")
            w.double_shot_chance = std::stod(value_str);
        else if (key == "Мгновенное_убийство")
            w.guaranteed_kill_chance = std::stod(value_str);
        else if (key == "Шанс_уклонения")
            w.silencer_dodge_chance = std::stod(value_str);
        else if (key == "Множитель_зомби")
            choise_damage = std::stoi(value_str);
        else if (key == "Уровень")
            p.player_level = std::stoi(value_str);
        else if (key == "Опыт")
            p.player_xp = std::stoi(value_str);
        else if (key == "Регистрация")
            p.player_reg = std::stoi(value_str);
    }
    file.close();

    std::cout << "Сохранение загружено. Добро пожаловать обратно!\n";
    if (p.player_hp > a.player_max_hp) p.player_hp = a.player_max_hp;
}

void add_xp(int amount, Player& p, Armor& a, Weapon& w) {
    p.player_xp += amount;
    std::cout << " (+" << amount << " XP)";
    while (p.player_xp >= p.xp_for_next_level()) {
        p.player_xp -= p.xp_for_next_level();
        p.player_level++;
        std::cout << "\nПоздравляем! Вы достигли уровня " << p.player_level << "!\n";
        level_up(p, a, w);
    }
}

void level_up(Player& p, Armor& a, Weapon& w) {
    int gif_level = random_gif(generator);
    int choise_gif;
    std::cout << "На " << p.player_level << " уровне вы можете выбрать улучшение:\n";

    while (true) {
        if (gif_level == 1) {
            std::cout << "[1]: +20 к максимальному здоровью.\n"
                      << "[2]: +1200 монет к балансу.\n"
                      << "[3]: +10% к скорости лута.\n";
            std::cin >> choise_gif;
            if (choise_gif == 1) {
                a.player_max_hp += 20;
                p.player_hp += 20;
                if (p.player_hp > a.player_max_hp) p.player_hp = a.player_max_hp;
                std::cout << "Максимальное здоровье увеличено до " << a.player_max_hp << ".\n";
                break;
            } else if (choise_gif == 2) {
                p.player_money += 1200;
                std::cout << "Теперь у вас " << p.player_money << " монет.\n";
                break;
            } else if (choise_gif == 3) {
                a.loot_speed_multiplier += 0.1;
                std::cout << "Скорость лута увеличена на 10%.\n";
                break;
            } else { command_not_exist(); }
        } else if (gif_level == 2) {
            std::cout << "[1]: +10% к урону.\n"
                      << "[2]: +200 XP сразу.\n"
                      << "[3]: +10 к максимальному здоровью.\n";
            std::cin >> choise_gif;
            if (choise_gif == 1) {
                a.player_damage_multiplier += 0.1;
                std::cout << "Урон увеличен на 10%.\n";
                break;
            } else if (choise_gif == 2) {
                add_xp(200, p, a, w);
                break;
            } else if (choise_gif == 3) {
                a.player_max_hp += 10;
                p.player_hp += 10;
                if (p.player_hp > a.player_max_hp) p.player_hp = a.player_max_hp;
                std::cout << "Максимальное здоровье увеличено до " << a.player_max_hp << ".\n";
                break;
            } else { command_not_exist(); }
        } else if (gif_level == 3) {
            std::cout << "[1]: +2400 монет к балансу.\n"
                      << "[2]: +1% к шансу мгновенного убийства.\n"
                      << "[3]: +5% к поглощению урона.\n";
            std::cin >> choise_gif;
            if (choise_gif == 1) {
                p.player_money += 2400;
                std::cout << "Теперь у вас " << p.player_money << " монет.\n";
                break;
            } else if (choise_gif == 2) {
                w.guaranteed_kill_chance += 0.01;
                std::cout << "Шанс мгновенного убийства увеличен на 1%.\n";
                break;
            } else if (choise_gif == 3) {
                a.armor_absorption += 0.05;
                if (a.armor_absorption > 0.9) a.armor_absorption = 0.9;
                std::cout << "Поглощение урона увеличено на 5%.\n";
                break;
            } else { command_not_exist(); }
        }
    }

    int bonus = level_gif(generator);
    p.player_money += bonus;
    std::cout << "Бонусом вам зачислили " << bonus << " монет" << std::endl;
}

void hospital(Player& p, Armor& a, Weapon& w, Zombie& z) {
    std::cout << "\nВы в больнице\n";
    std::cout << "(Баланс: " << p.player_money << ", здоровье: " << p.player_hp << ")\n";
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
        if (p.player_money < cost) {
            std::cout << "Недостаточно монет!\n";
            continue;
        }
        std::cout << "Лечение " << hospital_choise << " HP за " << cost << " монет. Согласны? (да/нет): ";
        std::string ans;
        std::cin >> ans;
        if (ans == "да") {
            p.player_money -= cost;
            p.player_hp += hospital_choise;
            if (p.player_hp > a.player_max_hp) p.player_hp = a.player_max_hp;
            std::cout << "Теперь у вас " << p.player_hp << " здоровья.\n";
            break;
        } else {
            std::cout << "Лечение отменено.\n";
            break;
        }
    }
}

void hunting(Player& p, Armor& a, Weapon& w, Zombie& z, FastZombie& fz) {
    std::cout << "\nВы выдвинулись на охоту\n";
    int number_hunting;
    std::cout << "Вам надо обдумать план вылазки\n";
    while (true) {
        std::cout << "Вариант №1: Вы спокойно обыскиваете все ящики и уходите на базу [УРОВЕНЬ ЛУТА: МИНИМАЛЕН]\n";
        std::cout << "Вариант №2: Вы прорываетесь сквозь зомби и лутаете самые лучшие ящики [УРОВЕНЬ ЛУТА ВЫСОКИЙ]\n";
        std::cout << "Вариант №3: Вы незаметно протискиваетесь сквозь толпы зомби и лутаете лучший лут, "
                     "но велик шанс что зомби вас учуят [УРОВЕНЬ ЛУТА МАКСИМАЛЬНЫЙ]\n";
        std::cin >> number_hunting;
        if (number_hunting == 1) { number_hunting_one(p, a, w, z); break; }
        else if (number_hunting == 2) { number_hunting_two(p, a, w, z, fz); break; }
        else if (number_hunting == 3) { number_hunting_three(p, a, w, z); break; }
        else {
            std::cout << "Такой команды не существует! Выберите 1, 2 или 3.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

void dead(Player& p, Armor& a, Weapon& w, Zombie& z) {
    std::cout << "Вы погибли... Игра окончена.\n";
    std::cout << "Вы потеряли " << p.player_money << " монет.\n";
    p.player_money = 0;
}

int player_attack_body(Player& p, Armor& a, Weapon& w, Zombie& z) {
    int idx = body_choice(generator);
    double mult = body_multipliers[idx];
    int dmg = static_cast<int>(p.player_damage * mult * a.player_damage_multiplier * w.weapon_damage_mult);
    z.zombie_hp -= dmg;
    std::cout << "Вы попали в " << body_parts[idx] << " и нанесли " << dmg << " урона!\n";
    return dmg;
}

bool player_withdrawal(Player& p, Armor& a, Weapon& w, Zombie& z) {
    std::cout << "Вы пытаетесь отойти...\n";
    if (shot_chance(generator) <= 3) {
        z.attack(p, a);    // виртуальный вызов
        std::cout << "Вас задели, не получилось уйти.\n";
        return false;
    } else {
        std::cout << "Вы благополучно отошли.\n";
        return true;
    }
}

void player_attack(Player& p, Armor& a, Weapon& w, Zombie& z) {
    int shot = shot_chance(generator);
    int threshold = 6 + (w.accuracy_bonus / 10);
    if (threshold > 10) threshold = 10;
    if (shot <= threshold) {
        if (w.guaranteed_kill_chance > 0.0) {
            std::uniform_real_distribution<double> d(0.0, 1.0);
            if (d(generator) < w.guaranteed_kill_chance) {
                std::cout << "AWP пробивает зомби насквозь! Мгновенное убийство!\n";
                z.zombie_hp = 0;
                return;
            }
        }
        int shots = 1;
        if (w.double_shot_chance > 0.0) {
            std::uniform_real_distribution<double> d(0.0, 1.0);
            if (d(generator) < w.double_shot_chance) {
                shots = 2;
                std::cout << "Двойной выстрел! ";
            }
        }
        for (int s = 0; s < shots; ++s) {
            player_attack_body(p, a, w, z);
        }
        std::cout << "У зомби осталось " << z.zombie_hp << " здоровья.\n";
    } else {
        z.attack(p, a);   // виртуальный вызов
        std::cout << "Вы промахнулись!\n";
        std::cout << "У вас осталось " << p.player_hp << " здоровья.\n";
    }
}

void an_incomprehensible_action(Player& p, Armor& a, Weapon& w, Zombie& z) {
    std::cout << "Непонятное действие. Зомби заметил вас!\n";
    int raw = zombieDmg(generator) * (2 * choise_damage);
    int dmg = static_cast<int>(raw * (1.0 - a.armor_absorption));
    p.player_hp -= dmg;
}

void fight(Player& p, Armor& a, Weapon& w, Zombie& z) {
    std::cout << "\nВы встретили зомби!\n";
    while (p.player_hp > 0 && z.zombie_hp > 0) {
        std::cout << "Действия: Выстрел[1] / Отойти[2]\n";
        std::string choice;
        std::cin >> choice;
        for (auto& c : choice) c = std::tolower(static_cast<unsigned char>(c));
        if (choice == "1") {
            player_attack(p, a, w, z);
        } else if (choice == "2") {
            if (player_withdrawal(p, a, w, z)) break;
        } else {
            std::cout << "Непонятная команда. Зомби атакует!\n";
            z.attack(p, a);
        }
    }
    if (p.player_hp <= 0) { dead(p, a, w, z); exit(0); }
    if (z.zombie_hp <= 0) {
        std::cout << "\nЗомби повержен! Поздравляю!\n";
        int xp_gain = xp_kill(generator);
        std::cout << "Вы получили " << xp_gain << " опыта!\n";
        add_xp(xp_gain, p, a, w);
        z.zombie_hp = 100;
    }
}

void escape(Player& p, Armor& a, Weapon& w, Zombie& z) {
    std::cout << "\nВы пытаетесь убежать...\n";
    if (shot_chance(generator) <= 3) {
        int raw = zombieDmg(generator) * (2 * choise_damage);
        int dmg = static_cast<int>(raw * (1.0 - a.armor_absorption));
        p.player_hp -= dmg;
        std::cout << "Зомби задел вас, нанеся " << dmg << " урона.\n";
    } else {
        std::cout << "Вы благополучно убежали, но ящик не тронут.\n";
    }
}

void hold_still_and_not_breathe(Player& p, Armor& a, Weapon& w, Zombie& z) {
    if (still_no_breathe(generator) == 1) {
        int raw = zombieDmg(generator) * (2 * choise_damage);
        int dmg = static_cast<int>(raw * (1.0 - a.armor_absorption));
        p.player_hp -= dmg;
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
        p.player_hp -= dmg;
        std::cout << "\nВас обнаружили, нанеся " << dmg << " урона.\n";
    } else {
        std::cout << "\nВы обошли зомби и обезвредили его!\n";
        z.zombie_hp = 0;
        std::cout << "\nВы получили опыт за нейтрализацию зомби!\n";
        add_xp(xp_kill(generator), p, a, w);
    }
}

void stealth(Player& p, Armor& a, Weapon& w, Zombie& z) {
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

    if (p.player_hp <= 0) { dead(p, a, w, z); exit(0); }
}

void number_hunting_one(Player& p, Armor& a, Weapon& w, Zombie& z) {
    const int BOX = 5;
    int earned = 0;
    bool lucky = loot_location_one(generator) > 10;
    if (lucky) std::cout << "Повезло! Хороший лут.\n";
    else std::cout << "Плохой лут.\n";
    int minL = lucky ? 10 : 0, maxL = lucky ? 20 : 10;
    for (int i = 0; i < BOX; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(3000 / (a.loot_speed_multiplier * w.weapon_loot_speed))));
        std::uniform_int_distribution<int> loot(minL, maxL);
        int coins = loot(generator);
        int xp_gain = xp_box(generator);
        std::cout << "Ящик " << i+1 << ": " << coins << " монет, +" << xp_gain << " XP\n";
        earned += coins;
        add_xp(xp_gain, p, a, w);
    }
    p.player_money += earned;
    std::cout << "Всего монет: " << earned << "\n";
}

void number_hunting_two(Player& p, Armor& a, Weapon& w, Zombie& z, FastZombie& fz) {
    const int BOX = 5;
    int earned = 0;
    std::cout << "Прорываетесь к лучшим ящикам!\n";
    std::uniform_int_distribution<int> encounter(1, 5);
    for (int i = 0; i < BOX; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(3000 / (a.loot_speed_multiplier * w.weapon_loot_speed))));
        if (encounter(generator) >= 4) {
            std::cout << "Зомби у ящика " << i+1 << "!\n";
            Zombie* currentZ = &z;
            if (zombieClass(generator) == 1) {
                currentZ = &fz;
                std::cout << "(Это быстрый зомби!)\n";
            }
            fight(p, a, w, *currentZ);
            if (p.player_hp <= 0) return;
            if (currentZ->zombie_hp > 0) { std::cout << "Зомби помешал обыскать!\n"; continue; }
        }
        int coins = loot_location_two(generator);
        int xp_gain = xp_box(generator);
        std::cout << "Ящик " << i+1 << ": " << coins << " монет, +" << xp_gain << " XP\n";
        earned += coins;
        add_xp(xp_gain, p, a, w);
    }
    p.player_money += earned;
    std::cout << "Всего монет: " << earned << "\n";
}

void number_hunting_three(Player& p, Armor& a, Weapon& w, Zombie& z) {
    const int BOX = 5;
    int earned = 0;
    std::uniform_int_distribution<int> enc(1, 5);
    std::uniform_int_distribution<int> event(1, 11);
    int ev = event(generator);
    if (ev <= 5) {
        for (int i = 0; i < BOX; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(3000 / (a.loot_speed_multiplier * w.weapon_loot_speed))));
            if (enc(generator) == 5) {
                std::cout << "Зомби у ящика " << i+1 << "!\n";
                stealth(p, a, w, z);
                if (p.player_hp <= 0) return;
                if (z.zombie_hp > 0) { std::cout << "Зомби помешал обыскать!\n"; continue; }
            }
            int coins = loot_location_three(generator);
            int xp_gain = xp_box(generator);
            std::cout << "Ящик " << i+1 << ": " << coins << " монет, +" << xp_gain << " XP\n";
            earned += coins;
            add_xp(xp_gain, p, a, w);
        }
    } else {
        std::cout << "Зомби очень активны, но лут лучше!\n";
        for (int i = 0; i < BOX; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(3000 / (a.loot_speed_multiplier * w.weapon_loot_speed))));
            if (enc(generator) >= 3) {
                std::cout << "Зомби у ящика " << i+1 << "!\n";
                stealth(p, a, w, z);
                if (p.player_hp <= 0) return;
                if (z.zombie_hp > 0) { std::cout << "Зомби помешал обыскать!\n"; continue; }
            }
            int coins = static_cast<int>(loot_location_three(generator) * 1.5);
            int xp_gain = xp_box(generator);
            std::cout << "Ящик " << i+1 << ": " << coins << " монет (бонус), +" << xp_gain << " XP\n";
            earned += coins;
            add_xp(xp_gain, p, a, w);
        }
    }
    p.player_money += earned;
    std::cout << "Всего монет: " << earned << "\n";
}

void workshop(Player& p, Armor& a, Weapon& w, Zombie& z) {
    std::cout << "\n=== Мастерская ===\n"
              << "[1]: Экипировка\n"
              << "[2]: Оружие\n"
              << "[3]: Верстак\n"
              << "[4]: Обвесы\n";
    int choise_workshop;
    std::cin >> choise_workshop;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (choise_workshop == 1) {
        std::cout << "\nЭкипировка:\n"
                  << "[1] Тяжёлая броня. Поглощение 75%. Здоровье 120. Урон 70%. Скорость лута 50%\n"
                  << "[2] Лёгкая броня. Поглощение -25%. Здоровье 100. Урон 110%. Скорость лута 500%\n"
                  << "[3] Средняя броня. Поглощение 10%. Здоровье 110. Урон 110%. Скорость лута 110%\n"
                  << "[4] Начальная броня. Поглощение -5%. Здоровье 100. Урон 100%. Скорость лута 80%\n";

        int workshop_equipment;
        while (true) {
            std::cin >> workshop_equipment;
            if (workshop_equipment >= 1 && workshop_equipment <= 4) break;
            std::cout << "Неверный выбор. Введите число от 1 до 4: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        a.armor_absorption = 0.0;
        a.player_damage_multiplier = 1.0;
        a.loot_speed_multiplier = 1.0;
        a.player_max_hp = 100;

        switch (workshop_equipment) {
            case 1:
                std::cout << "Вы надели тяжёлую броню.\n";
                a.armor_absorption = 0.75;
                a.player_damage_multiplier = 0.7;
                a.loot_speed_multiplier = 0.5;
                a.player_max_hp = 120;
                break;
            case 2:
                std::cout << "Вы надели лёгкую броню.\n";
                a.armor_absorption = -0.25;
                a.player_damage_multiplier = 1.1;
                a.loot_speed_multiplier = 5.0;
                a.player_max_hp = 100;
                break;
            case 3:
                std::cout << "Вы надели среднюю броню.\n";
                a.armor_absorption = 0.10;
                a.player_damage_multiplier = 1.1;
                a.loot_speed_multiplier = 1.1;
                a.player_max_hp = 110;
                break;
            case 4:
                std::cout << "Вы надели начальную броню.\n";
                a.armor_absorption = -0.05;
                a.player_damage_multiplier = 1.0;
                a.loot_speed_multiplier = 0.8;
                a.player_max_hp = 100;
                break;
        }

        if (p.player_hp > a.player_max_hp) p.player_hp = a.player_max_hp;
    }
    else if (choise_workshop == 2) {
        std::cout << "\nОружие:\n"
                  << "[1] AK-47. Урон 160%. +0% попадание. Скорость лута +10%. Стоимость: 5400 монет.\n"
                  << "    Особенность: можно ставить обвесы.\n"
                  << "[2] AWP. Урон 250%. +50% попадание. Скорость лута -50%. Стоимость: 8400 монет.\n"
                  << "    Особенность: шанс 5% убить зомби мгновенно.\n"
                  << "[3] LR-300. Урон 140%. +5% попадание. Скорость лута +0%. Стоимость: 3600 монет.\n"
                  << "    Особенностей нет.\n"
                  << "[4] MP-5. Урон 100%. +5% попадание. Скорость лута +15%. Стоимость: 5400 монет.\n"
                  << "    Особенность: шанс 20% сделать двойной выстрел.\n";

        int weapon_choice;
        while (true) {
            std::cin >> weapon_choice;
            if (weapon_choice >= 1 && weapon_choice <= 4) break;
            std::cout << "Неверный выбор. Введите от 1 до 4: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        int cost = 0;
        double dmg_mult = 1.0;
        int acc_bonus = 0;
        double loot_mult = 1.0;
        double double_shot = 0.0;
        double kill_chance = 0.0;

        switch (weapon_choice) {
            case 1: cost = 5400; dmg_mult = 1.6; acc_bonus = 0; loot_mult = 1.1; break;
            case 2: cost = 8400; dmg_mult = 2.5; acc_bonus = 50; loot_mult = 0.5; kill_chance = 0.05; break;
            case 3: cost = 3600; dmg_mult = 1.4; acc_bonus = 5; loot_mult = 1.0; break;
            case 4: cost = 5400; dmg_mult = 1.0; acc_bonus = 5; loot_mult = 1.15; double_shot = 0.2; break;
        }

        if (p.player_money < cost) {
            std::cout << "Недостаточно монет! Нужно " << cost << ", у вас " << p.player_money << ".\n";
        } else {
            p.player_money -= cost;
            w.current_weapon = weapon_choice;
            w.weapon_damage_mult = dmg_mult;
            w.accuracy_bonus = acc_bonus;
            w.weapon_loot_speed = loot_mult;
            w.double_shot_chance = double_shot;
            w.guaranteed_kill_chance = kill_chance;
            w.silencer_dodge_chance = 0.0;

            std::cout << "Вы купили и экипировали оружие!\n";
        }
    }
    else if (choise_workshop == 3) {
        std::cout << "Верстак пока в разработке.\n";
    }
    else if (choise_workshop == 4) {
        std::cout << "Обвесы в разработке.\n";
    }
    else {
        std::cout << "Неверный выбор.\n";
    }
}

void menu(Player& p, Armor& a, Weapon& w, Zombie& z, FastZombie& fz) {
    std::cout << "\n=== БАЗА ===\n";
    int choice;
    while (true) {
        std::cout << "Баланс: " << p.player_money << " монет | Здоровье: " << p.player_hp << "/" << a.player_max_hp
                  << " | Уровень: " << p.player_level << " (XP: " << p.player_xp << "/" << p.xp_for_next_level() << ")\n";
        std::cout << "1 - Охота\n2 - Больница\n3 - Мастерская\n4 - Сохранить\n5 - Статистика\n6 - выход\n";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (choice == 1) { hunting(p, a, w, z, fz); std::cout << "\nВы вернулись на базу.\n"; }
        else if (choice == 2) hospital(p, a, w, z);
        else if (choice == 3) workshop(p, a, w, z);
        else if (choice == 4) save_game(p, a, w, z);
        else if (choice == 5) show_status(p, a, w);
        else if (choice == 6) { quitGame(p, a, w, z); break; }
        else std::cout << "Неверная команда!\n" << std::endl;
    }
}

void hello(Player& p, Armor& a, Weapon& w, Zombie& z, FastZombie& fz) {
    load_game(p, a, w, z);

    if (p.player_reg == 1) {
        std::cout << "С возвращением, " << p.player_name << "!\n";
        menu(p, a, w, z, fz);
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
        p.player_name = name;
        if (p.player_damage == 20) {
            p.player_damage = 20 + playerDmg(generator);
        }
        p.player_reg = 1;
        save_game(p, a, w, z);
        std::cout << "Добро пожаловать, " << name << "!\n";
        menu(p, a, w, z, fz);
    }
}

// -------------------- main --------------------
int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    Player p;
    Armor a;
    Weapon w;
    Zombie z;
    FastZombie fz;

    hello(p, a, w, z, fz);

    return 0;
}