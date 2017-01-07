/* Created by Darren Otgaar on 2017/01/06. http://www.github.com/otgaard/zap */
#ifndef ZAPEROIDS_WORLD_HPP
#define ZAPEROIDS_WORLD_HPP

#include "common.hpp"

class world {
public:
    world();
    ~world();

    bool generate(int level=1, int players=1);

    void thrust();
    void left();
    void right();

    void update(double t, float dt);
    void draw(const camera& cam, program& shdr);

protected:

private:
    struct state_t;
    std::unique_ptr<state_t> state_;
    state_t& s;
};


#endif //ZAPEROIDS_WORLD_HPP