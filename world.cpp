/* Created by Darren Otgaar on 2017/01/06. http://www.github.com/otgaard/zap */
#include "world.hpp"
#include <zap/maths/rand_lcg.hpp>
#include <zap/maths/geometry/hull.hpp>
#include <zap/maths/transform.hpp>

#include <zap/maths/io.hpp>

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::graphics;
using namespace zap::maths::geometry;

using zap::graphics::vtx_p2c3_t;

// Could generate them using incremental hull, but for now, statically defined
const static std::vector<std::vector<vec2f>> asteroid_shapes = {
        { {-20,-20}, {20,-20}, {0,20} }
};

const static std::vector<vec2f> ship_shape = {
        {0,0}, {-10,-10}, {-10,-10}, {0,20}, {0,20}, {10,-10}, {10,-10}, {0,0}
};

struct body {
    vec2i vbuf_idx;
    transform3f transform;
    vec2f velocity;

    body() = default;
    body(const body& rhs) = default;
    body& operator=(const body& rhs) = default;
};

struct bullet {
    size_t vbuf_idx;
    vec2f velocity;
    float age;
    float distance_travelled;

    bullet() = default;
    bullet(const bullet& rhs) = default;
    bullet& operator=(const bullet& rhs) = default;
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
    if(!s.vbuf.allocate() || !s.mesh.allocate()) {
        LOG_ERR("Error allocating world GPU resources.");
        return false;
    }

    s.mesh.set_stream(&s.vbuf);
    s.mesh.bind(); s.vbuf.bind();

    s.vbuf.initialise(20000);       // Buffer of 20000 reserved vertices

    body player_ship;
    player_ship.vbuf_idx.set(0, ship_shape.size());
    player_ship.transform.translate(vec2f(300,512));
    player_ship.velocity = vec2f(0,0);
    s.ships.push_back(player_ship);

    if(s.vbuf.map(buffer_access::BA_WRITE_ONLY)) {
        std::transform(ship_shape.begin(), ship_shape.end(), s.vbuf.begin(), [&player_ship](const vec2f& sP) {
            return vtx_p2c3_t(player_ship.transform.translation() + sP, vec3b(255,255,255));
        });
        s.vbuf.unmap();
    }
    s.mesh.release();
    return true;
}

void world::update(double t, float dt) {
    // Update Physics
    auto& player_ship = s.ships[0];
    auto s1P = player_ship.transform.translation();
    s1P += player_ship.velocity*dt;

    if(s1P.x > 600) s1P.x = 0; else if(s1P.x < 0) s1P.x = 600;
    if(s1P.y > 1024) s1P.y = 0; else if(s1P.y < 0) s1P.y = 1024;

    player_ship.transform.translate(s1P);

    // Update Buffer
    s.vbuf.bind();
    if(s.vbuf.map(buffer_access::BA_WRITE_ONLY)) {
        std::transform(ship_shape.begin(), ship_shape.end(), s.vbuf.begin(), [=](const vec2f& sP) {
            return vtx_p2c3_t(s.ships[0].transform.translation() + sP, vec3b(255,255,255));
        });
        s.vbuf.unmap();
    }
    s.vbuf.release();
}

void world::draw(const camera& cam, program& shdr) {
    shdr.bind_uniform("PVM", cam.proj_view());
    s.mesh.bind();
    s.mesh.draw(primitive_type::PT_LINES, 0, ship_shape.size());
    s.mesh.release();
}

void world::thrust() {
    s.ships[0].velocity += s.ships[0].transform.rotation().col(1);
}

void world::left() {

}

void world::right() {

}
