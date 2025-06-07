#include <dpp/dpp.h>
#include <cstdlib>
#include <iostream>
#include <random>
#include <algorithm>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <vector>

// hey ian whats a struct, a struct is a way to store a collection of variables of different types under a single name
// in this case, we're storing the x and y coordinates of a point in a 2D grid, good job student
struct Coord { // coordinate system for the game
    int x;
    int y;

    bool operator==(const Coord& other) const { // we are basicall just checking if they are the same and c++ makes this look really complicated
        return x == other.x && y == other.y;
    }

};

class SnakeGame { // oooo a class since we need to store the state of the game and theres probably mutliple games going on at once
    public:
        SnakeGame(int w, int h, dpp::snowflake ch_id);
        dpp::snowflake message_id;
        void move();
        void change_direction(std::string new_direction);
        void grow();
        void check_collision();
        void check_food();
        void check_wall();
        void place_food();
        std::string render(); // alot of functions that go into making the game work
        bool alive;
        int length;  // this is public so storing is able to be used
        dpp::snowflake channel_id;

    private:
        Coord head; // this is private so like theres no way to access it from outside the class
        std::vector<Coord> body; // dude basically this is just a list of coordinates
        std::string direction; // r u stupid
        Coord food; // coord of the food
        int width; // grid width
        int height; // grid height
};

SnakeGame::SnakeGame(int w, int h, dpp::snowflake ch_id)
    : alive(true), message_id(0), length(1), channel_id(ch_id), direction("right"), width(w), height(h) {
    head = {w/2, h/2};
    body.push_back(head);
    place_food();// starting positions to initialize the game
}

void SnakeGame::move() {
    Coord old_head = head;
    
    if (direction == "up") {
        head.y--;
    } else if (direction == "down") {
        head.y++;
    } else if (direction == "left") {
        head.x--;
    } else if (direction == "right") {
        head.x++;
    }
    
    body.insert(body.begin(), old_head);
    if (static_cast<int>(body.size()) > length) {
        body.pop_back();
    }
} // this is the move helper function all it does is just see what direction you want and move the snake

void SnakeGame::change_direction(std::string new_direction) {
    if ((new_direction == "up" && direction != "down") ||
        (new_direction == "down" && direction != "up") ||
        (new_direction == "left" && direction != "right") ||
        (new_direction == "right" && direction != "left")) {
        direction = new_direction;
    }
} // this is the change direction function all it does is just see what direction you want and change the direction of the snake

void SnakeGame::grow() {
    length++;
} // im pretty sure you know what this does

void SnakeGame::check_collision() {
    // check if ur a dumbass and hit the wall
    if (head.x < 0 || head.x >= width || head.y < 0 || head.y >= height) {
        alive = false;
        return;
    }
    // check if ur a dumbass and hit ur own body
    for (const auto& segment : body) {
        if (head == segment) {
            alive = false;
            return;
        }
    }
}

void SnakeGame::check_food() {
    if (head == food) {
        grow();
        place_food();
    } // check if you collide wit the food and if so grow and place new food
}

void SnakeGame::check_wall() {
    // out of bounds
    if (head.x < 0 || head.x >= width || head.y < 0 || head.y >= height) {
        alive = false;
    }
}

void SnakeGame::place_food() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis_x(0, width - 1);
    std::uniform_int_distribution<> dis_y(0, height - 1);
    // i fucking hate random number generators in c++ wheres the random.randint()



    // chatgpt gave me this so heres how it works
    // yooo wheres bob in this list? [bob, tim, kehisa]
    // oh hey thats bob we found him, so basically for each item in the list we are checking
    // if its apart of the snake, if not great we can put food there, if yes we just keep looking
    // until we find a spot that is not apart of the snake
    Coord f;
    do {
        f = { dis_x(gen), dis_y(gen) };
    } while (std::find(body.begin(), body.end(), f) != body.end() || f == head);

    food = f;
}

std::string SnakeGame::render() {
    std::string grid = "";
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Coord current = {x, y};
            // render the game grid
            if (head == current) {
                grid += "🐍";
            } else if (food == current) {
                grid += "🍎";
            } else if (std::find(body.begin(), body.end(), current) != body.end()) { // check if the current position is apart of the snake
                grid += "🟩";
            } else {
                grid += "⬜";
            }
        }
        grid += "\n";
    }
    return grid;
}

std::unordered_map<dpp::snowflake, SnakeGame> games;

int main() {
    const char* token = std::getenv("BOT_TOKEN");
    if (!token) {
        std::cerr << "❌ BOT_TOKEN is not set. Run: export BOT_TOKEN=your_token_here\n";
        return 1;
    }

    dpp::cluster bot(token);

    bot.on_log(dpp::utility::cout_logger());

    bot.on_button_click([](const dpp::button_click_t& event) {
        auto game_it = games.find(event.command.get_issuing_user().id);
        if (game_it != games.end() && game_it->second.alive) { // oh hey we are editing it here so we can change the direction of the snake and edit the grid n shit
            std::string direction = event.custom_id;
            game_it->second.change_direction(direction);

            std::string grid = game_it->second.render();
            dpp::message msg(grid);
            
            dpp::component up_button = dpp::component()
                .set_label("⬆️")
                .set_type(dpp::cot_button)
                .set_style(dpp::cos_primary)
                .set_id("up");

            dpp::component down_button = dpp::component()
                .set_label("⬇️")
                .set_type(dpp::cot_button)
                .set_style(dpp::cos_primary)
                .set_id("down");

            dpp::component left_button = dpp::component()
                .set_label("⬅️")
                .set_type(dpp::cot_button)
                .set_style(dpp::cos_primary)
                .set_id("left");

            dpp::component right_button = dpp::component()
                .set_label("➡️")
                .set_type(dpp::cot_button)
                .set_style(dpp::cos_primary)
                .set_id("right");
                
            msg.add_component(dpp::component()
                .add_component(up_button)
                .add_component(down_button)
                .add_component(left_button)
                .add_component(right_button)
                .set_type(dpp::cot_action_row)
            );
            // fuckass buttons
            event.reply(dpp::ir_update_message, msg);
        }
    });

    bot.on_slashcommand([](const dpp::slashcommand_t& event) {
        std::cout << "Slash command received: " << event.command.get_command_name() << std::endl;
        if (event.command.get_command_name() == "startsnake") {
            dpp::component up_button = dpp::component()
            .set_label("⬆️")
            .set_type(dpp::cot_button)
            .set_style(dpp::cos_primary)
            .set_id("up");

            dpp::component down_button = dpp::component()
            .set_label("⬇️")
            .set_type(dpp::cot_button)
            .set_style(dpp::cos_primary)
            .set_id("down");

            dpp::component left_button = dpp::component()
            .set_label("⬅️")
            .set_type(dpp::cot_button)
            .set_style(dpp::cos_primary)
            .set_id("left");

            dpp::component right_button = dpp::component()
            .set_label("➡️")
            .set_type(dpp::cot_button)
            .set_style(dpp::cos_primary)
            .set_id("right");
            
            dpp::snowflake user_id = event.command.get_issuing_user().id;
            auto game_pair = games.emplace(user_id, SnakeGame(9, 9, event.command.channel_id)); // store the user id and the game so we can edit it later
            std::string initial_grid = game_pair.first->second.render(); // init the game and render the grid
            
            dpp::message msg(initial_grid);
            // add the buttons to the message
            msg.add_component(dpp::component()
                .add_component(up_button)
                .add_component(down_button)
                .add_component(left_button)
                .add_component(right_button)
                .set_type(dpp::cot_action_row)
            );

            event.reply(msg, [user_id](const dpp::confirmation_callback_t& callback) {
                if (!callback.is_error()) {
                    auto msg_response = callback.get<dpp::message>();
                    auto game_it = games.find(user_id);
                    if (game_it != games.end()) {
                        game_it->second.message_id = msg_response.id;
                    } // store the message id so we can edit it later
                }
            });
        }
    });

    bot.on_ready([&bot](const dpp::ready_t&) {
        std::cout << "✅ Bot is ready\n";

        if (dpp::run_once<struct register_bot_commands>()) {
            bot.global_command_create(
                dpp::slashcommand("startsnake", "Start a game of snake!", bot.me.id)
            );
        }
    });

    // start the game loop in a separate thread
    // keep note that the game thread and edit message is VERY seperate
    std::thread game_thread([&bot]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));

            std::vector<dpp::snowflake> dead_games;

            for (auto& [user_id, game] : games) {
                if (!game.alive) continue;

                game.move();
                game.check_collision();
                game.check_food();

                if (!game.alive) {
                    dpp::message game_over_msg("💀 Game Over! Final Score: " + std::to_string(game.length));
                    game_over_msg.channel_id = game.channel_id;
                    dead_games.push_back(user_id);
                } else if (game.message_id != 0) {
                    // update the game display in real-time
                    std::string current_grid = game.render();
                    dpp::message update_msg(current_grid);
                    
                    // re add the control buttons
                    dpp::component up_button = dpp::component()
                        .set_label("⬆️")
                        .set_type(dpp::cot_button)
                        .set_style(dpp::cos_primary)
                        .set_id("up");

                    dpp::component down_button = dpp::component()
                        .set_label("⬇️")
                        .set_type(dpp::cot_button)
                        .set_style(dpp::cos_primary)
                        .set_id("down");

                    dpp::component left_button = dpp::component()
                        .set_label("⬅️")
                        .set_type(dpp::cot_button)
                        .set_style(dpp::cos_primary)
                        .set_id("left");

                    dpp::component right_button = dpp::component()
                        .set_label("➡️")
                        .set_type(dpp::cot_button)
                        .set_style(dpp::cos_primary)
                        .set_id("right");
                        
                    update_msg.add_component(dpp::component()
                        .add_component(up_button)
                        .add_component(down_button)
                        .add_component(left_button)
                        .add_component(right_button)
                        .set_type(dpp::cot_action_row)
                    );
                    
                    update_msg.channel_id = game.channel_id;
                    update_msg.id = game.message_id;
                    bot.message_edit(update_msg);
                }
            }
            
            // some memory c++ shit to avoid iterator invalidation
            for (auto user_id : dead_games) {
                games.erase(user_id);
            }
        }
    });
    game_thread.detach();

    bot.start(dpp::st_wait);
    return 0;
}