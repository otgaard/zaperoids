/* Created by Darren Otgaar on 2017/01/06. http://www.github.com/otgaard/zap */
#include "world.hpp"
#include <zap/maths/rand_lcg.hpp>
#include <zap/maths/geometry/hull.hpp>
#include <zap/maths/geometry/disc.hpp>
#include <zap/maths/transform.hpp>
#include <zap/maths/io.hpp>
#include "game.hpp"

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::graphics;
using namespace zap::maths::geometry;

using zap::graphics::vtx_p2c3_t;

using vbuf_bullet_t = vertex_buffer<vtx_p2c3_t, buffer_usage::BU_DYNAMIC_DRAW>;
using mesh_bullet_t = mesh<vertex_stream<vbuf_bullet_t>, primitive_type::PT_POINTS>;

// Could generate them using incremental hull, but for now, statically defined
static const float TWO_PI_OVER_FIVE = float(TWO_PI)/5.f;
const static std::vector<std::vector<vec2f>> asteroid_shapes = {
        { {std::cos(0.f), std::sin(0.f)},
          {std::cos(TWO_PI_OVER_FIVE), std::sin(TWO_PI_OVER_FIVE)},
          {std::cos(TWO_PI_OVER_FIVE), std::sin(TWO_PI_OVER_FIVE)},
          {std::cos(2*TWO_PI_OVER_FIVE), std::sin(2*TWO_PI_OVER_FIVE)},
          {std::cos(2*TWO_PI_OVER_FIVE), std::sin(2*TWO_PI_OVER_FIVE)},
          {std::cos(3*TWO_PI_OVER_FIVE), std::sin(3*TWO_PI_OVER_FIVE)},
          {std::cos(3*TWO_PI_OVER_FIVE), std::sin(3*TWO_PI_OVER_FIVE)},
          {std::cos(4*TWO_PI_OVER_FIVE), std::sin(4*TWO_PI_OVER_FIVE)},
          {std::cos(4*TWO_PI_OVER_FIVE), std::sin(4*TWO_PI_OVER_FIVE)},
          {std::cos(0.f), std::sin(0.f)} }
};

const static std::vector<vec2f> ship_shape = {
        {0,0}, {-10,-10}, {-10,-10}, {0,20}, {0,20}, {10,-10}, {10,-10}, {0,0}
};

struct body {
    transform3f transform;
    vec2f velocity;
    float rotation;
    float orientation;

    body() : velocity(0.f, 0.f), rotation(0.f), orientation(0.f) { }
    body(const body& rhs) = default;
    body& operator=(const body& rhs) = default;
};

struct bullet {
    int owner;
    vec2f position;
    vec2f velocity;
    float distance_travelled;

    bullet(int owner=0) : position(0.f, 0.f), velocity(0.f, 0.f), distance_travelled(0.f) { }
    bullet(const bullet& rhs) = default;
    bullet& operator=(const bullet& rhs) = default;
};

struct world::state_t {
    int width, height;
    vbuf_p2c3_t vbuf_shapes;
    mesh_p2c3_ls_t mesh_shapes;
    vbuf_bullet_t vbuf_bullets;
    mesh_bullet_t mesh_bullets;
    std::vector<body> ships;
    std::vector<body> asteroids;
    std::vector<bullet> bullets;
    rand_lcg rand;
    game* game_ptr;
};

world::world() : state_(new state_t()), s(*state_) {
}

world::~world() = default;



bool world::generate() {
    if(!s.vbuf_shapes.allocate() || !s.mesh_shapes.allocate()) {
        LOG_ERR("Error allocating world GPU resources.");
        return false;
    }

    s.mesh_shapes.set_stream(&s.vbuf_shapes);
    s.mesh_shapes.bind(); s.vbuf_shapes.bind();

    s.vbuf_shapes.initialise(1000);

    if(s.vbuf_shapes.map(buffer_access::BA_WRITE_ONLY)) {
        std::transform(ship_shape.begin(), ship_shape.end(), s.vbuf_shapes.begin(), [](const vec2f& sP) {
            return vtx_p2c3_t(sP, vec3b(255,255,255));
        });

        std::transform(asteroid_shapes[0].begin(), asteroid_shapes[0].end(), s.vbuf_shapes.begin()+ship_shape.size(),
                       [](const vec2f& aP) {
                           return vtx_p2c3_t(aP, vec3b(255,255,255));
                       });

        s.vbuf_shapes.unmap();
    }
    s.mesh_shapes.release();

    if(!s.vbuf_bullets.allocate() || !s.mesh_bullets.allocate()) {
        LOG_ERR("Error allocating bullet GPU resources");
        return false;
    }

    s.mesh_bullets.set_stream(&s.vbuf_bullets);
    s.mesh_bullets.bind(); s.vbuf_bullets.bind();
    s.vbuf_bullets.initialise(200);

    return true;
}

bool world::generate_level(game* game_ptr, int level) {
    body player_ship;
    player_ship.transform.translate(vec2f(s.width/2.f, s.height/2.f));
    player_ship.velocity = vec2f(0,0);
    s.ships.push_back(player_ship);

    s.asteroids.reserve(50);
    for(int i = 0; i != 5; ++i) {
        body asteroid;
        asteroid.transform.translate(vec2f(s.rand.random() * s.width, s.rand.random() * s.height));
        asteroid.rotation = 5.f*s.rand.random_s();
        asteroid.orientation = float(TWO_PI)*s.rand.random();
        asteroid.transform.uniform_scale(std::max(s.rand.random()*40, 4.f));
        asteroid.velocity.set(s.rand.random_s() * 30, s.rand.random_s() * 30);
        s.asteroids.push_back(asteroid);
    }

    // Allocate the bullets
    s.bullets.reserve(200);
}

inline void wrap_position(vec2f& P, float width, float height) {
    if(P.x > width) P.x = 0; else if(P.x < 0) P.x = width;
    if(P.y > height) P.y = 0; else if(P.y < 0) P.y = height;
}

void world::update(double t, float dt) {
    // Update Physics
    auto& player_ship = s.ships[0];
    auto s1P = player_ship.transform.translation();
    s1P += player_ship.velocity*dt;

    wrap_position(s1P, s.width, s.height);

    player_ship.transform.translate(s1P);
    player_ship.transform.rotate(make_rotation(player_ship.orientation));

    // Update Asteroid Motion
    for(auto& asteroid : s.asteroids) {
        auto aP = asteroid.transform.translation();
        aP += asteroid.velocity * dt;
        asteroid.orientation += asteroid.rotation * dt;
        wrap_position(aP, s.width, s.height);
        asteroid.transform.translate(aP);
        asteroid.transform.rotate(make_rotation(asteroid.orientation));
    }

    // Update Bullet Physics
    static const float max_bullet_range = 100.f*100.f;
    for(auto& b : s.bullets) {
        auto dv = b.velocity * dt;
        b.position += dv;
        b.distance_travelled += dv.length_sqr();

        wrap_position(b.position, s.width, s.height);
    }

    // Collision Detection
    for(auto& b : s.bullets) {
        if(b.distance_travelled >= max_bullet_range) continue;
        for(auto it = s.asteroids.begin(); it != s.asteroids.end(); ++it) {
            auto d = maths::geometry::disc<float>(it->transform.translation(), it->transform.uniform_scale());
            if(distance(b.position, d) <= 0.f) {
                auto old_asteroid = *it;
                s.asteroids.erase(it);
                b.distance_travelled = max_bullet_range;
                if(old_asteroid.transform.uniform_scale() > 4.f) {
                    auto new_size = std::max(old_asteroid.transform.uniform_scale()/3.f, 4.f);
                    auto N = perp(b.velocity).normalise();
                    auto mag = old_asteroid.velocity.length();

                    body A;
                    A.transform.translate(old_asteroid.transform.translation()+new_size*N);
                    A.rotation = 5.f*s.rand.random_s();
                    A.orientation = float(TWO_PI)*s.rand.random();
                    A.transform.uniform_scale(new_size);
                    A.velocity = 2.f * mag * N;
                    s.asteroids.push_back(A);

                    body B;
                    B.transform.translate(old_asteroid.transform.translation()-new_size*N);
                    B.rotation = 5.f*s.rand.random_s();
                    B.orientation = float(TWO_PI)*s.rand.random();
                    B.transform.uniform_scale(new_size);
                    B.velocity = -2.f * mag * N;
                    s.asteroids.push_back(B);
                }
                break;
            }
        }
    }

    auto ne = std::remove_if(s.bullets.begin(), s.bullets.end(), [](auto& b) {
        return b.distance_travelled > max_bullet_range;
    });
    if(ne != s.bullets.end()) s.bullets.erase(ne, s.bullets.end());

    // Update Buffer
    s.vbuf_bullets.bind();
    if(s.vbuf_bullets.map(buffer_access::BA_WRITE_ONLY)) {
        std::transform(s.bullets.begin(), s.bullets.end(), s.vbuf_bullets.begin(), [](const bullet& bP) {
            return vtx_p2c3_t(bP.position, vec3b(255,255,255));
        });
        s.vbuf_bullets.unmap();
    }
    s.vbuf_bullets.release();
}

void world::draw(const camera& cam, program& shdr) {
    s.mesh_shapes.bind();
    auto pvm_loc = shdr.uniform_location("PVM");
    shdr.bind_uniform(pvm_loc, cam.proj_view()*s.ships[0].transform.gl_matrix());
    s.mesh_shapes.draw(primitive_type::PT_LINES, 0, ship_shape.size());

    for(auto& asteroid : s.asteroids) {
        shdr.bind_uniform(pvm_loc, cam.proj_view()*asteroid.transform.gl_matrix());
        s.mesh_shapes.draw(primitive_type::PT_LINES, ship_shape.size(), asteroid_shapes[0].size());
    }
    s.mesh_shapes.release();

    s.mesh_bullets.bind();
    shdr.bind_uniform(pvm_loc, cam.proj_view());
    if(s.bullets.size() > 0) s.mesh_shapes.draw(primitive_type::PT_POINTS, 0, s.bullets.size());

    s.mesh_bullets.release();
}

void world::command(int player, const ship_command& cmd) {
    static const auto bullet_emit_pos = vec2f(0.f, 20.f);

    if(cmd.thrust) s.ships[player].velocity += s.ships[player].transform.rotation().col(1);
    if(cmd.left)   s.ships[player].orientation += +.1f;
    if(cmd.right)  s.ships[player].orientation += -.1f;
    if(cmd.fire && s.bullets.size() < 200) {
        auto b = bullet(player);
        b.position = s.ships[player].transform.affine().transform(bullet_emit_pos);
        b.velocity = 500*s.ships[player].transform.rotation().col(1) + s.ships[player].velocity;
        s.bullets.push_back(b);
    }
}

void world::set_world_size(int width, int height) {
    s.width = width; s.height = height;
}
