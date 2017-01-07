/* Created by Darren Otgaar on 2017/01/06. http://www.github.com/otgaard/zap */
#include "world.hpp"
#include <zap/maths/rand_lcg.hpp>
#include <zap/maths/geometry/hull.hpp>
#include <zap/maths/transform.hpp>

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::graphics;
using namespace zap::maths::geometry;

// Could generate them using incremental hull, but for now, statically defined
std::vector<std::vector<vec2f>> asteroid_shapes = {
        { {-20,-20}, {20,-20}, {0,20} }
};

struct body {
    vec2i vbuf_idx;
    transform3f transform;
    vec2f velocity;
};

struct bullet {
    size_t vbuf_idx;
    vec2f velocity;
    float age;
    float distance_travelled;
};

struct world::state_t {
    vbuf_p2c3_t vbuf;
    mesh_p2c3_ls_t mesh;
    std::vector<body> ships;
    std::vector<body> asteroids;
    std::vector<bullet> bullets;
};

world::world() : state_(new state_t()), s(*state_) {
}

world::~world() = default;

bool world::generate(int level, int players) {

}

void world::update(double t, float dt) {

}

void world::draw(const camera& cam) {

}
