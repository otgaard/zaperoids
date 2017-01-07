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
    float rotation;

    body() : vbuf_idx(0, 0), velocity(0.f, 0.f), rotation(0.f) { }
    body(const body& rhs) = default;
    body& operator=(const body& rhs) = default;
};

struct bullet {
    vec2f position;
    vec2f velocity;
    float distance_travelled;

    bullet() : position(0.f, 0.f), velocity(0.f, 0.f), distance_travelled(0.f) { }
    bullet(const bullet& rhs) = default;
    bullet& operator=(const bullet& rhs) = default;
};

struct world::state_t {
    int width, height;
    vbuf_p2c3_t vbuf;
    mesh_p2c3_ls_t mesh;
    std::vector<body> ships;
    std::vector<body> asteroids;
    std::vector<bullet> bullets;        // map the bullets to the vbuf range [1000,2000]
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

    // Support up to 1000 bullets
    s.bullets.reserve(1000);

    body player_ship;
    player_ship.vbuf_idx.set(0, ship_shape.size());
    player_ship.transform.translate(vec2f(300,512));
    player_ship.velocity = vec2f(0,0);
    s.ships.push_back(player_ship);

    if(s.vbuf.map(buffer_access::BA_WRITE_ONLY)) {
        std::transform(ship_shape.begin(), ship_shape.end(), s.vbuf.begin(), [&player_ship](const vec2f& sP) {
            return vtx_p2c3_t(player_ship.transform.affine().transform(sP), vec3b(255,255,255));
        });

        std::for_each(s.vbuf.begin()+1000, s.vbuf.begin()+2000, [](auto& vtx) {
            vtx.position.set(-100,-100);
            vtx.colour1.set(255,0,0);
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

    if(s1P.x > s.width) s1P.x = 0; else if(s1P.x < 0) s1P.x = s.width;
    if(s1P.y > s.height) s1P.y = 0; else if(s1P.y < 0) s1P.y = s.height;

    player_ship.transform.translate(s1P);
    player_ship.transform.rotate(make_rotation(player_ship.rotation));

    // Update Bullet Physics, max range (1000 units squared)
    static const float max_bullet_range = 100.f*100.f;
    for(auto& b : s.bullets) {
        auto dv = b.velocity * dt;
        b.position += dv;
        b.distance_travelled += dv.length_sqr();

        if(b.position.x > s.width)  b.position.x = 0; else if(b.position.x < 0) b.position.x = s.width;
        if(b.position.y > s.height) b.position.y = 0; else if(b.position.y < 0) b.position.y = s.height;
    }

    auto ne = std::remove_if(s.bullets.begin(), s.bullets.end(), [](auto& b) {
        return b.distance_travelled > max_bullet_range;
    });
    if(ne != s.bullets.end()) s.bullets.erase(ne, s.bullets.end());

    // Update Buffer
    s.vbuf.bind();
    if(s.vbuf.map(buffer_access::BA_WRITE_ONLY)) {
        std::transform(ship_shape.begin(), ship_shape.end(), s.vbuf.begin(), [=](const vec2f& sP) {
            return vtx_p2c3_t(s.ships[0].transform.affine().transform(sP), vec3b(255,255,255));
        });

        std::transform(s.bullets.begin(), s.bullets.end(), s.vbuf.begin()+1000, [](const bullet& bP) {
            return vtx_p2c3_t(bP.position, vec3b(255,255,255));
        });
        s.vbuf.unmap();
    }
    s.vbuf.release();
}

void world::draw(const camera& cam, program& shdr) {
    shdr.bind_uniform("PVM", cam.proj_view());
    s.mesh.bind();
    s.mesh.draw(primitive_type::PT_LINES, 0, ship_shape.size());
    if(s.bullets.size() > 0) s.mesh.draw(primitive_type::PT_POINTS, 1000, s.bullets.size());
    s.mesh.release();
}

void world::command(int player, const ship_command& cmd) {
    static const auto bullet_emit_pos = vec2f(0.f, 20.f);

    if(cmd.thrust) s.ships[player].velocity += s.ships[player].transform.rotation().col(1);
    if(cmd.left)   s.ships[player].rotation += +.1f;
    if(cmd.right)  s.ships[player].rotation += -.1f;
    if(cmd.fire && s.bullets.size() < 1000) {   // 1000 for now
        auto b = bullet();
        b.position = s.ships[player].transform.affine().transform(bullet_emit_pos);
        b.velocity = 500*s.ships[player].transform.rotation().col(1) + s.ships[player].velocity;
        s.bullets.push_back(b);
    }
}

void world::set_world_size(int width, int height) {
    s.width = width; s.height = height;
}
