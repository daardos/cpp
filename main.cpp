// g++ main.cpp -o s
// .\s

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

std::mt19937 generator(std::random_device{}());

std::uniform_int_distribution<int> loot_location_one(0, 20);
std::uniform_int_distribution<int> loot_location_two(10, 100);
std::uniform_int_distribution<int> loot_location_three(100, 1000);
std::uniform_int_distribution<int> zombieDmg(10, 40);
std::uniform_int_distribution<int> playerDmg(1, 40);
std::uniform_int_distribution<int> shot_chance(1, 10);
std::uniform_int_distribution<int> still_no_breathe(1, 3);
std::uniform_int_distribution<int> throw_stone(1, 2);
std::uniform_int_distribution<int> around(1, 4);

int player_hp = 100;
int player_damage = 20 + playerDmg(generator);
int player_money = 0;
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

void quitGame();
void hospital();
void hunting();
void menu();
void number_hunting_one();
void number_hunting_two();
void number_hunting_three();

const std::string body_parts[3] = {"Голову", "Тело", "Конечность"};
const double body_multipliers[3] = {2.0, 1.0, 0.5};
std::uniform_int_distribution<int> body_choice(0, 2);

void save_game() {
    std::cout << "Сохранение прогресса в save.txt в разработке"
}

void quitGame() {
    std::cout << "\nВыход из системы...\n" << std::endl;
}

void hospital() {
    std::cout << "\nВы в больнице\n" << std::endl;
    std::cout << "(выш баланс: " << player_money << ", ваше здоровье: " << player_hp << std::endl; 
    std::cout << "1 хп = 2 монеты" << std::endl;
    int hospital_choise;

    while (true) {
        std::cout << "Сколько хотите выстоновить здоровья? " << std::endl;
        std::cin >> hospital_choise;
        if (player_money < hospital_choise * 2) {
            std::cout << "У вас не хватает баланса!";
        } else if (hospital_choise > 0 && hospital_choise <= 100) {
            std::cout << "Лечение " << hospital_choise << " здоровья займет " << hospital_choise * 2 << " монет.\nВы согласны (да / нет)?" << std::endl;
            std::string hospital_choise_hp;
            std::cin >> hospital_choise_hp;

            if (hospital_choise_hp == "да") {
                player_money -= hospital_choise * 2;
                player_hp += hospital_choise;
                if (player_hp > 100) {
                    player_hp = 100;
                    std::cout << "Ваше здоровье не может превышать 100, установлено 100." << std::endl;
                }
                std::cout << "Вы успешно вылечились! Теперь у вас " << player_hp << " здоровья!" << std::endl;
                break;
            } else {
                std::cout << "Вы вышли из больницы... " << std::endl;
                break;
            }
        }
    
        if (player_hp > 100) {
            player_hp = 100;
            std::cout << "Так как у вас было больше 100 здоровья, ваше хп сбросилось до 100" << std::endl;
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

        if (number_hunting == 1) {
            number_hunting_one();
            break;
        } else if (number_hunting == 2) {
            number_hunting_two();
            break;
        } else if (number_hunting == 3) {
            number_hunting_three();
            break;
        } else {
            std::cout << "Такой команды не существует! Выберите 1, 2 или 3." << std::endl;

            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

void dead() {
    std::cout << "Вы погибли... Игра окончена." << std::endl;
    std::cout << "Вы потеряли " << player_money << " моенет" << std::endl;
    player_money = 0;
}

void fight() {
    std::cout << "\nВы встретили зомби!\n";
    while (player_hp > 0 && zombie_hp > 0) {
        std::cout << "Ваши действия: Выстрел[1] / Отойти[2]" << std::endl;
        std::string fight_choice;
        std::cin >> fight_choice;
        for (size_t i = 0; i < fight_choice.size(); ++i)
            fight_choice[i] = std::tolower(static_cast<unsigned char>(fight_choice[i]));

        if (fight_choice == "1") {
            int shot = shot_chance(generator);
            int threshold = 6 + (accuracy_bonus / 10);
            if (threshold > 10) threshold = 10;
            if (shot <= threshold) {
                // Проверка на мгновенное убийство (AWP)
                if (guaranteed_kill_chance > 0.0) {
                    std::uniform_real_distribution<double> kill_dist(0.0, 1.0);
                    if (kill_dist(generator) < guaranteed_kill_chance) {
                        std::cout << "AWP пробивает зомби насквозь! Мгновенное убийство!\n";
                        zombie_hp = 0;
                        break;
                    }
                }

                int shots = 1;
                if (double_shot_chance > 0.0) {
                    std::uniform_real_distribution<double> dbl(0.0, 1.0);
                    if (dbl(generator) < double_shot_chance) {
                        shots = 2;
                        std::cout << "Двойной выстрел! ";
                    }
                }

                for (int s = 0; s < shots; s++) {
                    int part_index = body_choice(generator);
                    std::string part = body_parts[part_index];
                    double multiplier = body_multipliers[part_index];
                    int final_damage = static_cast<int>(player_damage * multiplier * player_damage_multiplier * weapon_damage_mult);
                    zombie_hp -= final_damage;
                    std::cout << "Вы попали в " << part << " и нанесли " << final_damage << " урона!\n";
                }
                std::cout << "У зомби осталось " << zombie_hp << " здоровья.\n";
            } else {
                int raw_damage = zombieDmg(generator);
                int actual_damage = static_cast<int>(raw_damage * (1.0 - armor_absorption));
                player_hp -= actual_damage;
                std::cout << "Вы промахнулись!\nЗомби ударил вас и нанёс " << actual_damage
                          << " урона.\nУ вас осталось " << player_hp << " здоровья.\n";
            }
        } else if (fight_choice == "2") {
            std::cout << "Вы пытаетесь отойти..." << std::endl;
            if (shot_chance(generator) <= 3) {
                int raw_damage = zombieDmg(generator);
                int actual_damage = static_cast<int>(raw_damage * (1.0 - armor_absorption));
                player_hp -= actual_damage;
                std::cout << "Зомби задел вас, нанеся " << actual_damage << " урона.\n";
            } else {
                std::cout << "Вы благополучно отошли.\n";
                break;
            }
        } else {
            std::cout << "Непонятная команда. Зомби атакует!\n";
            int raw_damage = zombieDmg(generator);
            int actual_damage = static_cast<int>(raw_damage * (1.0 - armor_absorption));
            player_hp -= actual_damage;
            std::cout << "Зомби нанёс " << actual_damage << " урона.\n";
        }
    }
    if (player_hp <= 0) { dead(); exit(0); }
    if (zombie_hp <= 0) {
        std::cout << "Зомби повержен! Поздравляю!\n";
        zombie_hp = 100;
    }
}

void stealth() {
    std::cout << "Зомби уже совсем рядом\nВаше действие?\n";
    std::cout << "[1] Убежать\n";
    std::cout << "[2] Затаиться и не дышать\n";
    std::cout << "[3] Отвлечь, бросив камень\n";
    std::cout << "[4] Тихо обойти за спину\n";

    int choise_stealth;
    std::cin >> choise_stealth;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (choise_stealth == 1) {
        std::cout << "Вы пытаетесь убежать..." << std::endl;
        if (shot_chance(generator) <= 3) {
            int raw_damage = zombieDmg(generator) * (2 * choise_damage);
            int actual_damage = static_cast<int>(raw_damage * (1.0 - armor_absorption));
            player_hp -= actual_damage;
            std::cout << "Зомби задел вас, нанеся " << actual_damage << " урона." << std::endl;
        } else {
            std::cout << "Вы благополучно убежали, но ящик остался не тронут." << std::endl;
        }
    }
    else if (choise_stealth == 2) {
        if (still_no_breathe(generator) == 1) {
            int raw_damage = zombieDmg(generator) * (2 * choise_damage);
            int actual_damage = static_cast<int>(raw_damage * (1.0 - armor_absorption));
            player_hp -= actual_damage;
            std::cout << "Зомби вас учуял, вам нанесли " << actual_damage << " урона" << std::endl;
        } else {
            std::cout << "У вас получилось скрыться" << std::endl;
            zombie_hp = 0;
        }
    }
    else if (choise_stealth == 3) {
        int throw_stone_result = throw_stone(generator);
        if (throw_stone_result == 1) {
            choise_damage += 1;
            std::cout << "Вы привлекли ещё больше зомби! Теперь урон удвоен." << std::endl;
        } else {
            std::cout << "Вы успешно отвлекли зомби и продолжаете фармить боксы" << std::endl;
            zombie_hp = 0;
        }
    }
    else if (choise_stealth == 4) {
        if (around(generator) <= 2) {
            int raw_damage = zombieDmg(generator) * (2 * choise_damage);
            int actual_damage = static_cast<int>(raw_damage * (1.0 - armor_absorption));
            player_hp -= actual_damage;
            std::cout << "Вас обнаружил зомби. Вам нанесли: " << actual_damage << " урона!" << std::endl;
        } else {
            std::cout << "Вы успешно обошли зомби и обезвредили его!" << std::endl;
            zombie_hp = 0;
        }
    }
    else {
        std::cout << "Непонятное действие. Зомби заметил вас!" << std::endl;
        int raw_damage = zombieDmg(generator) * (2 * choise_damage);
        int actual_damage = static_cast<int>(raw_damage * (1.0 - armor_absorption));
        player_hp -= actual_damage;
    }

    if (player_hp <= 0) {
        dead();
        exit(0);
    }
}

void number_hunting_one() {
    const int BOX_COUNT = 5;
    int earned = 0;

    bool good_luck = loot_location_one(generator) > 10;

    if (good_luck) {
        std::cout << "Вам повезло, вы попали в локацию с хорошим лутом!" << std::endl;
    } else {
        std::cout << "Вы попали в локацию с плохим лутом" << std::endl;
    }

    int min_loot = good_luck ? 10 : 0;
    int max_loot = good_luck ? 20 : 10;

    for (int i = 0; i < BOX_COUNT; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(3000 / (loot_speed_multiplier * weapon_loot_speed))));

        int box_number = i + 1;
        std::uniform_int_distribution<int> loot(min_loot, max_loot);
        int coins_found = loot(generator);

        std::cout << "В ящике под номером " << box_number
                  << " вы нашли " << coins_found << " монет!" << std::endl;
        earned += coins_found;
    }
    player_money += earned;

    std::cout << "Всего собрано монет: " << earned << std::endl;
}

void number_hunting_two() {
    const int BOX_COUNT = 5;
    int earned = 0;

    std::cout << "Вы прорываетесь сквозь зомби к лучшим ящикам!" << std::endl;

    std::uniform_int_distribution<int> zombie_encounter(1, 5);

    for (int i = 0; i < BOX_COUNT; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(3000 / (loot_speed_multiplier * weapon_loot_speed))));

        int box_number = i + 1;
        int encounter = zombie_encounter(generator);

        if (encounter >= 4) {
            std::cout << "На вас нападает зомби у ящика " << box_number << "!" << std::endl;
            fight();
            if (player_hp <= 0) return;
            if (zombie_hp > 0) {
                std::cout << "Зомби помешал вам обыскать ящик!" << std::endl;
                continue;
            }
        }

        int coins_found = loot_location_two(generator);
        std::cout << "В ящике под номером " << box_number
                  << " вы нашли " << coins_found << " монет!" << std::endl;
        earned += coins_found;
    }
    player_money += earned;

    std::cout << "Всего собрано монет: " << earned << std::endl;
}

void number_hunting_three() {
    const int BOX_COUNT = 5;
    int earned = 0;

    std::uniform_int_distribution<int> zombie_encounter(1, 5);
    std::uniform_int_distribution<int> loot_location_three_event_one(1, 11);
    int loot_event = loot_location_three_event_one(generator);

    if (loot_event <= 5) {
        for (int i = 0; i < BOX_COUNT; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(3000 / (loot_speed_multiplier * weapon_loot_speed))));

            int box_number = i + 1;
            int encounter = zombie_encounter(generator);

            if (encounter == 5) {
                std::cout << "На вас нападает зомби у ящика " << box_number << "!" << std::endl;
                stealth();
                if (player_hp <= 0) return;
                if (zombie_hp > 0) {
                    std::cout << "Зомби помешал вам обыскать ящик!" << std::endl;
                    continue;
                }
            }

            int coins_found = loot_location_three(generator);
            std::cout << "В ящике под номером " << box_number
                      << " вы нашли " << coins_found << " монет!" << std::endl;
            earned += coins_found;
        }
    } else {
        std::cout << "Вы чувствуете, что зомби особенно активны... Но награда стоит риска!" << std::endl;
        for (int i = 0; i < BOX_COUNT; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(3000 / (loot_speed_multiplier * weapon_loot_speed))));

            int box_number = i + 1;
            int encounter = zombie_encounter(generator);

            if (encounter >= 3) {
                std::cout << "На вас нападает зомби у ящика " << box_number << "!" << std::endl;
                stealth();
                if (player_hp <= 0) return;
                if (zombie_hp > 0) {
                    std::cout << "Зомби помешал вам обыскать ящик!" << std::endl;
                    continue;
                }
            }

            int coins_found = static_cast<int>(loot_location_three(generator) * 1.5);
            std::cout << "В ящике под номером " << box_number
                      << " вы нашли " << coins_found << " монет (бонус за риск)!" << std::endl;
            earned += coins_found;
        }
    }

    player_money += earned;
    std::cout << "Всего собрано монет: " << earned << std::endl;
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
    std::cout << "\nВы на базе\n";
    int choice;
    while (true) {
        std::cout << "Ваш баланс: " << player_money << " монет\nЗдоровье: " << player_hp << std::endl;
        std::cout << "Введите 1 чтобы пойти на охоту\n"
                  << "Введите 2 чтобы пойти в больницу\n"
                  << "Введите 3 чтобы зайти в мастерскую\n"
                  << "Введите 4 чтобы выйти из игры\n";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 1) { hunting(); std::cout << "\nВы вернулись на базу.\n"; }
        else if (choice == 2) { hospital(); }
        else if (choice == 3) { workshop(); }
        else if (choice == 4) { quitGame(); break; }
        else {
            std::cout << "Такой команды не существует!" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    std::cout << "Здравствуйте, игрок!\nДобро пожаловать в текстовую RPG игру!" << std::endl;

    std::string player_name;
    while (true) {
        std::cout << "\nВведите свой никнейм: ";
        std::getline(std::cin, player_name);

        if (player_name.size() < 6) {
            std::cout << "Никнейм должен быть не короче 6 символов!\n";
            continue;
        }
        if (player_name.size() > 17) {
            std::cout << "Никнейм должен быть не длиннее 17 символов!\n";
            continue;
        }

        bool only_letters = true;
        for (char ch : player_name) {
            if (!std::isalpha(static_cast<unsigned char>(ch))) {
                only_letters = false;
                break;
            }
        }

        if (!only_letters) {
            std::cout << "Никнейм должен содержать только буквы (без цифр и спецсимволов)!\n";
            continue;
        }

        break;
    }

    std::cout << "Приятной игры, " << player_name << "!\n";
    menu();
    return 0;
}