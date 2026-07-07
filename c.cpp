// g++ c.cpp -o c
// .\c
#include <iostream>
#include <string>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <Windows.h>
#include <random>
#include <thread>
#include <chrono>
#include <fstream>
#include <limits>

std::mt19937 generator(std::random_device{}());

// Генераторы лута
std::uniform_int_distribution<int> loot_location_one(0, 20);
std::uniform_int_distribution<int> loot_location_two(10, 100);
std::uniform_int_distribution<int> loot_location_three(100, 1000);
// Генераторы урона и шансов
std::uniform_int_distribution<int> zombieDmg(10, 40);
std::uniform_int_distribution<int> playerDmg(1, 40);
std::uniform_int_distribution<int> shot_chance(1, 10);
// Генераторы для стелса
std::uniform_int_distribution<int> still_no_breathe(1, 3);
std::uniform_int_distribution<int> throw_stone(1, 2);
std::uniform_int_distribution<int> around(1, 4);
// Генератор случайного набора улучшений при повышении уровня
std::uniform_int_distribution<int> random_gif(1, 3);
// Генераторы опыта
std::uniform_int_distribution<int> xp_box(1, 10);          // опыт за ящик
std::uniform_int_distribution<int> xp_kill(20, 50);        // опыт за убийство зомби

// Характеристики игрока
int player_hp = 100;
int player_damage = 20 + playerDmg(generator);
int player_money = 0;
int player_level = 1;
int player_xp = 0;
int xp_for_next_level() { return player_level * 100; }     // необходимо опыта до следующего уровня

// Множители от брони
double armor_absorption = 0.0;   
double player_damage_multiplier = 1.0;
double loot_speed_multiplier = 1.0;
int player_max_hp = 100;
// Оружие
int current_weapon = 0;
double weapon_damage_mult = 1.0; 
int accuracy_bonus = 0; 
double weapon_loot_speed = 1.0;  
double double_shot_chance = 0.0;
double guaranteed_kill_chance = 0.0;
double silencer_dodge_chance = 0.0;

int zombie_hp = 100;
int choise_damage = 1;

// Прототипы функций
void quitGame();
void hospital();
void hunting();
void menu();
void number_hunting_one();
void number_hunting_two();
void number_hunting_three();
void add_xp(int amount);
void level_up();

// Части тела и множители
const std::string body_parts[3] = {"Голову", "Тело", "Конечность"};
const double body_multipliers[3] = {2.0, 1.0, 0.5};
std::uniform_int_distribution<int> body_choice(0, 2);

// Сохранение
void save_game() {
    std::ofstream file("save.txt");
    if (file.is_open()) {
        file << player_hp << "\n" << player_money << "\n" << player_max_hp << "\n"
             << player_damage << "\n" << armor_absorption << "\n" << player_damage_multiplier << "\n"
             << loot_speed_multiplier << "\n" << current_weapon << "\n" << weapon_damage_mult << "\n"
             << accuracy_bonus << "\n" << weapon_loot_speed << "\n" << double_shot_chance << "\n"
             << guaranteed_kill_chance << "\n" << silencer_dodge_chance << "\n" << choise_damage << "\n"
             << player_level << "\n" << player_xp << "\n";
        file.close();
        std::cout << "Прогресс сохранён в save.txt\n";
    } else {
        std::cout << "Ошибка при сохранении!\n";
    }
}

// Загрузка
void load_game() {
    std::ifstream file("save.txt");
    if (file.is_open()) {
        file >> player_hp >> player_money >> player_max_hp >> player_damage
             >> armor_absorption >> player_damage_multiplier >> loot_speed_multiplier
             >> current_weapon >> weapon_damage_mult >> accuracy_bonus >> weapon_loot_speed
             >> double_shot_chance >> guaranteed_kill_chance >> silencer_dodge_chance
             >> choise_damage >> player_level >> player_xp;
        file.close();
        std::cout << "Сохранение загружено. Добро пожаловать обратно!\n";
        if (player_hp > player_max_hp) player_hp = player_max_hp;
    } else {
        std::cout << "Сохранение не найдено. Начинаем новую игру.\n";
    }
}

// Начисление опыта и проверка повышения уровня
void add_xp(int amount) {
    player_xp += amount;
    std::cout << " (+" << amount << " XP)";
    while (player_xp >= xp_for_next_level()) {
        player_xp -= xp_for_next_level();
        player_level++;
        std::cout << "\nПоздравляем! Вы достигли уровня " << player_level << "!\n";
        level_up();
    }
}

// Улучшения при повышении уровня
void level_up() {
    int gif_level = random_gif(generator);   // случайный набор бонусов
    int choise_gif;
    std::cout << "На " << player_level << " уровне вы можете выбрать улучшение:\n";

    while (true) {
        if (gif_level == 1) {
            std::cout << "[1]: +20 к максимальному здоровью.\n"
                      << "[2]: +1200 монет к балансу.\n"
                      << "[3]: +10% к скорости лута.\n";
            std::cin >> choise_gif;
            if (choise_gif == 1) {
                player_max_hp += 20;
                player_hp += 20; // дополнительно восстанавливаем здоровье
                if (player_hp > player_max_hp) player_hp = player_max_hp;
                std::cout << "Максимальное здоровье увеличено до " << player_max_hp << ".\n";
                break;
            } else if (choise_gif == 2) {
                player_money += 1200;
                std::cout << "Теперь у вас " << player_money << " монет.\n";
                break;
            } else if (choise_gif == 3) {
                loot_speed_multiplier += 0.1;
                std::cout << "Скорость лута увеличена на 10%.\n";
                break;
            } else { command_not_exist(); }
        } else if (gif_level == 2) {
            std::cout << "[1]: +10% к урону.\n"
                      << "[2]: +200 XP сразу.\n"
                      << "[3]: +10 к максимальному здоровью.\n";
            std::cin >> choise_gif;
            if (choise_gif == 1) {
                player_damage_multiplier += 0.1;
                std::cout << "Урон увеличен на 10%.\n";
                break;
            } else if (choise_gif == 2) {
                add_xp(200);   // добавим опыт и проверим повышение уровня
                break;
            } else if (choise_gif == 3) {
                player_max_hp += 10;
                player_hp += 10;
                if (player_hp > player_max_hp) player_hp = player_max_hp;
                std::cout << "Максимальное здоровье увеличено до " << player_max_hp << ".\n";
                break;
            } else { command_not_exist(); }
        } else if (gif_level == 3) {
            std::cout << "[1]: +2400 монет к балансу.\n"
                      << "[2]: +1% к шансу мгновенного убийства.\n"
                      << "[3]: +5% к поглощению урона.\n";
            std::cin >> choise_gif;
            if (choise_gif == 1) {
                player_money += 2400;
                std::cout << "Теперь у вас " << player_money << " монет.\n";
                break;
            } else if (choise_gif == 2) {
                guaranteed_kill_chance += 0.01;
                std::cout << "Шанс мгновенного убийства увеличен на 1%.\n";
                break;
            } else if (choise_gif == 3) {
                armor_absorption += 0.05;
                if (armor_absorption > 0.9) armor_absorption = 0.9; // ограничение
                std::cout << "Поглощение урона увеличено на 5%.\n";
                break;
            } else { command_not_exist(); }
        }
    }
}

void command_not_exist() {
    std::cout << "Такой команды не существует!\n";
}

void quitGame() {
    std::cout << "\nВыход из системы...\n";
}

void hospital() {
    std::cout << "\nВы в больнице\n";
    std::cout << "(Баланс: " << player_money << ", здоровье: " << player_hp << ")\n";
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
        if (player_money < cost) {
            std::cout << "Недостаточно монет!\n";
            continue;
        }
        std::cout << "Лечение " << hospital_choise << " HP за " << cost << " монет. Согласны? (да/нет): ";
        std::string ans;
        std::cin >> ans;
        if (ans == "да") {
            player_money -= cost;
            player_hp += hospital_choise;
            if (player_hp > player_max_hp) player_hp = player_max_hp;
            std::cout << "Теперь у вас " << player_hp << " здоровья.\n";
            break;
        } else {
            std::cout << "Лечение отменено.\n";
            break;
        }
    }
}

void hunting() {
    std::cout << "\nВы выдвинулись на охоту\n" << std::endl;
    int number_hunting;
    std::cout << "Вам надо обдумать план вылазки" << std::endl;
    while (true) {
        std::cout << "Вариант №1: Вы спокойно обыскиваете все ящики и уходите на базу [УРОВЕНЬ ЛУТА: МИНИМАЛЕН]" << std::endl;
        std::cout << "Вариант №2: Вы прорываетесь сквозь зомби и лутаете самые лучшие ящики [УРОВЕНЬ ЛУТА ВЫСОКИЙ]" << std::endl;
        std::cout << "Вариант №3: Вы незаметно протискиваетесь сквозь толпы зомби и лутаете лучший лут, "
                     "но велик шанс что зомби вас учуят [УРОВЕНЬ ЛУТА МАКСИМАЛЬНЫЙ]" << std::endl;
        std::cin >> number_hunting;
        if (number_hunting == 1) { number_hunting_one(); break; }
        else if (number_hunting == 2) { number_hunting_two(); break; }
        else if (number_hunting == 3) { number_hunting_three(); break; }
        else {
            std::cout << "Такой команды не существует! Выберите 1, 2 или 3." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

void dead() {
    std::cout << "Вы погибли... Игра окончена.\n";
    std::cout << "Вы потеряли " << player_money << " монет.\n";
    player_money = 0;
    // Можно также сбросить опыт или сохранить, по желанию.
}

int zombie_attack() {
    int raw = zombieDmg(generator);
    int dmg = static_cast<int>(raw * (1.0 - armor_absorption));
    player_hp -= dmg;
    std::cout << "Зомби нанёс " << dmg << " урона.\n";
    return dmg;
}

int player_attack_body() {
    int idx = body_choice(generator);
    double mult = body_multipliers[idx];
    int dmg = static_cast<int>(player_damage * mult * player_damage_multiplier * weapon_damage_mult);
    zombie_hp -= dmg;
    std::cout << "Вы попали в " << body_parts[idx] << " и нанесли " << dmg << " урона!\n";
    return dmg;
}

bool player_withdrawal() {
    std::cout << "Вы пытаетесь отойти...\n";
    if (shot_chance(generator) <= 3) {
        zombie_attack();
        std::cout << "Вас задели, не получилось уйти.\n";
        return false;
    } else {
        std::cout << "Вы благополучно отошли.\n";
        return true;
    }
}

void player_attack() {
    int shot = shot_chance(generator);
    int threshold = 6 + (accuracy_bonus / 10);
    if (threshold > 10) threshold = 10;
    if (shot <= threshold) {
        if (guaranteed_kill_chance > 0.0) {
            std::uniform_real_distribution<double> d(0.0, 1.0);
            if (d(generator) < guaranteed_kill_chance) {
                std::cout << "AWP пробивает зомби насквозь! Мгновенное убийство!\n";
                zombie_hp = 0;
                return;
            }
        }
        int shots = 1;
        if (double_shot_chance > 0.0) {
            std::uniform_real_distribution<double> d(0.0, 1.0);
            if (d(generator) < double_shot_chance) {
                shots = 2;
                std::cout << "Двойной выстрел! ";
            }
        }
        for (int s = 0; s < shots; ++s) {
            player_attack_body();
        }
        std::cout << "У зомби осталось " << zombie_hp << " здоровья.\n";
    } else {
        zombie_attack();
        std::cout << "Вы промахнулись!\n";
        std::cout << "У вас осталось " << player_hp << " здоровья.\n";
    }
}

void an_incomprehensible_action() {
    std::cout << "Непонятное действие. Зомби заметил вас!\n";
    int raw = zombieDmg(generator) * (2 * choise_damage);
    int dmg = static_cast<int>(raw * (1.0 - armor_absorption));
    player_hp -= dmg;
}

void fight() {
    std::cout << "\nВы встретили зомби!\n";
    while (player_hp > 0 && zombie_hp > 0) {
        std::cout << "Действия: Выстрел[1] / Отойти[2]\n";
        std::string choice;
        std::cin >> choice;
        for (auto& c : choice) c = std::tolower(static_cast<unsigned char>(c));
        if (choice == "1") {
            player_attack();
        } else if (choice == "2") {
            if (player_withdrawal()) break;
        } else {
            std::cout << "Непонятная команда. Зомби атакует!\n";
            zombie_attack();
        }
    }
    if (player_hp <= 0) { dead(); exit(0); }
    if (zombie_hp <= 0) {
        std::cout << "Зомби повержен! Поздравляю!\n";
        int xp_gain = xp_kill(generator);
        std::cout << "Вы получили " << xp_gain << " опыта!\n";
        add_xp(xp_gain);
        zombie_hp = 100;   // следующий зомби
    }
}

// stealth и охоты теперь тоже дают опыт

void escape() {
    std::cout << "Вы пытаетесь убежать...\n";
    if (shot_chance(generator) <= 3) {
        int raw = zombieDmg(generator) * (2 * choise_damage);
        int dmg = static_cast<int>(raw * (1.0 - armor_absorption));
        player_hp -= dmg;
        std::cout << "Зомби задел вас, нанеся " << dmg << " урона.\n";
    } else {
        std::cout << "Вы благополучно убежали, но ящик не тронут.\n";
    }
}

void hold_still_and_not_breathe() {
    if (still_no_breathe(generator) == 1) {
        int raw = zombieDmg(generator) * (2 * choise_damage);
        int dmg = static_cast<int>(raw * (1.0 - armor_absorption));
        player_hp -= dmg;
        std::cout << "Зомби вас учуял, нанеся " << dmg << " урона.\n";
    } else {
        std::cout << "Вы скрылись.\n";
        zombie_hp = 0;
        std::cout << "Вы получили опыт за нейтрализацию зомби!\n";
        add_xp(xp_kill(generator));
    }
}

void distract_by_throwing_stone() {
    int res = throw_stone(generator);
    if (res == 1) {
        choise_damage += 1;
        std::cout << "Вы привлекли больше зомби! Урон удвоен.\n";
    } else {
        std::cout << "Вы отвлекли зомби.\n";
        zombie_hp = 0;
        std::cout << "Вы получили опыт за нейтрализацию зомби!\n";
        add_xp(xp_kill(generator));
    }
}

void quietly_behind_back() {
    if (around(generator) <= 2) {
        int raw = zombieDmg(generator) * (2 * choise_damage);
        int dmg = static_cast<int>(raw * (1.0 - armor_absorption));
        player_hp -= dmg;
        std::cout << "Вас обнаружили, нанеся " << dmg << " урона.\n";
    } else {
        std::cout << "Вы обошли зомби и обезвредили его!\n";
        zombie_hp = 0;
        std::cout << "Вы получили опыт за нейтрализацию зомби!\n";
        add_xp(xp_kill(generator));
    }
}

void stealth() {
    std::cout << "Зомби уже совсем рядом\n"
              << "[1] Убежать\n[2] Затаиться\n[3] Отвлечь камнем\n[4] Обойти за спину\n";
    int c;
    std::cin >> c;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (c == 1) escape();
    else if (c == 2) hold_still_and_not_breathe();
    else if (c == 3) distract_by_throwing_stone();
    else if (c == 4) quietly_behind_back();
    else an_incomprehensible_action();

    if (player_hp <= 0) { dead(); exit(0); }
}

void number_hunting_one() {
    const int BOX = 5;
    int earned = 0;
    bool lucky = loot_location_one(generator) > 10;
    if (lucky) std::cout << "Повезло! Хороший лут.\n";
    else std::cout << "Плохой лут.\n";
    int minL = lucky ? 10 : 0, maxL = lucky ? 20 : 10;
    for (int i = 0; i < BOX; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(3000 / (loot_speed_multiplier * weapon_loot_speed))));
        std::uniform_int_distribution<int> loot(minL, maxL);
        int coins = loot(generator);
        int xp_gain = xp_box(generator);
        std::cout << "Ящик " << i+1 << ": " << coins << " монет, +" << xp_gain << " XP\n";
        earned += coins;
        add_xp(xp_gain);
    }
    player_money += earned;
    std::cout << "Всего монет: " << earned << "\n";
}

void number_hunting_two() {
    const int BOX = 5;
    int earned = 0;
    std::cout << "Прорываетесь к лучшим ящикам!\n";
    std::uniform_int_distribution<int> encounter(1, 5);
    for (int i = 0; i < BOX; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(3000 / (loot_speed_multiplier * weapon_loot_speed))));
        if (encounter(generator) >= 4) {
            std::cout << "Зомби у ящика " << i+1 << "!\n";
            fight();
            if (player_hp <= 0) return;
            if (zombie_hp > 0) { std::cout << "Зомби помешал обыскать!\n"; continue; }
        }
        int coins = loot_location_two(generator);
        int xp_gain = xp_box(generator);
        std::cout << "Ящик " << i+1 << ": " << coins << " монет, +" << xp_gain << " XP\n";
        earned += coins;
        add_xp(xp_gain);
    }
    player_money += earned;
    std::cout << "Всего монет: " << earned << "\n";
}

void number_hunting_three() {
    const int BOX = 5;
    int earned = 0;
    std::uniform_int_distribution<int> enc(1, 5);
    std::uniform_int_distribution<int> event(1, 11);
    int ev = event(generator);
    if (ev <= 5) {
        for (int i = 0; i < BOX; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(3000 / (loot_speed_multiplier * weapon_loot_speed))));
            if (enc(generator) == 5) {
                std::cout << "Зомби у ящика " << i+1 << "!\n";
                stealth();
                if (player_hp <= 0) return;
                if (zombie_hp > 0) { std::cout << "Зомби помешал обыскать!\n"; continue; }
            }
            int coins = loot_location_three(generator);
            int xp_gain = xp_box(generator);
            std::cout << "Ящик " << i+1 << ": " << coins << " монет, +" << xp_gain << " XP\n";
            earned += coins;
            add_xp(xp_gain);
        }
    } else {
        std::cout << "Зомби очень активны, но лут лучше!\n";
        for (int i = 0; i < BOX; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(3000 / (loot_speed_multiplier * weapon_loot_speed))));
            if (enc(generator) >= 3) {
                std::cout << "Зомби у ящика " << i+1 << "!\n";
                stealth();
                if (player_hp <= 0) return;
                if (zombie_hp > 0) { std::cout << "Зомби помешал обыскать!\n"; continue; }
            }
            int coins = static_cast<int>(loot_location_three(generator) * 1.5);
            int xp_gain = xp_box(generator);
            std::cout << "Ящик " << i+1 << ": " << coins << " монет (бонус), +" << xp_gain << " XP\n";
            earned += coins;
            add_xp(xp_gain);
        }
    }
    player_money += earned;
    std::cout << "Всего монет: " << earned << "\n";
}

void workshop() {
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
                  << "[2] Лёгкая броня. Поглощение -25% (получаете больше урона). Здоровье 100. Урон 200% в спину, 110% в других случаях. Скорость лута 500%\n"
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

        armor_absorption = 0.0;
        player_damage_multiplier = 1.0;
        loot_speed_multiplier = 1.0;
        player_max_hp = 100;

        switch (workshop_equipment) {
            case 1:
                std::cout << "Вы надели тяжёлую броню.\n";
                armor_absorption = 0.75;
                player_damage_multiplier = 0.7;
                loot_speed_multiplier = 0.5;
                player_max_hp = 120;
                break;
            case 2:
                std::cout << "Вы надели лёгкую броню.\n";
                armor_absorption = -0.25;
                player_damage_multiplier = 1.1;
                loot_speed_multiplier = 5.0;
                player_max_hp = 100;
                break;
            case 3:
                std::cout << "Вы надели среднюю броню.\n";
                armor_absorption = 0.10;
                player_damage_multiplier = 1.1;
                loot_speed_multiplier = 1.1;
                player_max_hp = 110;
                break;
            case 4:
                std::cout << "Вы надели начальную броню.\n";
                armor_absorption = -0.05;
                player_damage_multiplier = 1.0;
                loot_speed_multiplier = 0.8;
                player_max_hp = 100;
                break;
        }

        if (player_hp > player_max_hp) player_hp = player_max_hp;
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

        if (player_money < cost) {
            std::cout << "Недостаточно монет! Нужно " << cost << ", у вас " << player_money << ".\n";
        } else {
            player_money -= cost;
            current_weapon = weapon_choice;
            weapon_damage_mult = dmg_mult;
            accuracy_bonus = acc_bonus;
            weapon_loot_speed = loot_mult;
            double_shot_chance = double_shot;
            guaranteed_kill_chance = kill_chance;
            silencer_dodge_chance = 0.0;

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

void menu() {
    std::cout << "\n=== БАЗА ===\n";
    int choice;
    while (true) {
        std::cout << "Баланс: " << player_money << " монет | Здоровье: " << player_hp << "/" << player_max_hp
                  << " | Уровень: " << player_level << " (XP: " << player_xp << "/" << xp_for_next_level() << ")\n";
        std::cout << "1 - Охота\n2 - Больница\n3 - Мастерская\n4 - Сохранить\n5 - Выход\n";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (choice == 1) { hunting(); std::cout << "\nВы вернулись на базу.\n"; }
        else if (choice == 2) hospital();
        else if (choice == 3) workshop();
        else if (choice == 4) save_game();
        else if (choice == 5) { quitGame(); break; }
        else std::cout << "Неверная команда!\n";
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
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
    load_game();
    std::cout << "Добро пожаловать, " << name << "!\n";
    menu();
    return 0;
}