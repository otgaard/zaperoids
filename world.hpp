/* Created by Darren Otgaar on 2017/01/06. http://www.github.com/otgaard/zap */
#ifndef ZAPEROIDS_WORLD_HPP
#define ZAPEROIDS_WORLD_HPP

#include "common.hpp"

class game;

struct ship_command {
    bool thrust;
    bool left;
    bool right;
    bool fire;
    bool warp;

    ship_command() : thrust(false), left(false), right(false), fire(false), warp(false) { }
};

class world {
public:
    world();
    ~world();

    bool generate();

    bool generate_level(game* game_ptr, int level);

    void set_world_size(int width, int height);

    void command(int player, const ship_command& cmd);

    void update(double t, float dt);
    void draw(const camera& cam, program& shdr);

protected:

private:
    struct state_t;
    std::unique_ptr<state_t> state_;
    state_t& s;
};


#endif //ZAPEROIDS_WORLD_HPP
