#include <zap/maths/io.hpp>
#include <zap/tools/log.hpp>
#include <zap/maths/algebra.hpp>
#include <zap/host/GLFW/application.hpp>
#include <renderer/camera.hpp>
#include "vector_font.hpp"
#include "world.hpp"

using namespace zap;
using namespace zap::maths;
using namespace zap::renderer;

// Single shader for whole game for now
static const char* const vector_font_vshdr = GLSL(
        uniform mat4 PVM;

        in vec2 position;
        in ivec3 colour1;

        out vec3 colour;

        void main() {
            colour = vec3(colour1.r/255., colour1.g/255., colour1.b/255.);
            gl_Position = PVM * vec4(position, 0., 1.);
        }
);

static const char* const vector_font_fshdr = GLSL(
        in vec3 colour;
        out vec4 frag_colour;
        void main() {
            frag_colour = vec4(colour, 1.);
        }
);

struct ship_command {
    bool thrust;
    bool left;
    bool right;

    ship_command() : thrust(false), left(false), right(false) { }
};

class zaperoids : public application {
public:
    zaperoids() : application("zaperoids", 600, 1024, false), cam_(false) { }

    bool initialise() override final;
    void update(double t, float dt) override final;
    void draw() override final;
    void shutdown() override final;

    void on_resize(int width, int height) override final;

    void on_keypress(char ch) override;
    void on_keyrelease(char ch) override;

protected:
    camera cam_;
    vector_font font_;
    world world_;
    program shdr_;
    ship_command command_;
};

bool zaperoids::initialise() {
    shdr_.add_shader(zap::engine::shader_type::ST_VERTEX, vector_font_vshdr);
    shdr_.add_shader(zap::engine::shader_type::ST_FRAGMENT, vector_font_fshdr);
    if(!shdr_.link()) {
        LOG_ERR("Failed to compile/link vector_font shader resources.");
        return false;
    }

    // Test the vector font
    if(!font_.initialise()) {
        LOG_ERR("Failure to initialise font resources");
        return false;
    }

    font_.set_scale(0.334f);
    auto aabb = font_.string_AABB("Score 0");
    font_.insert_string(vec2f(600 - 20 - aabb.width(), 1024-10-aabb.height()), "Score 0");

    if(!world_.generate(1,1)) {
        LOG_ERR("Failed to create world simulation");
        return false;
    }

    return true;
}

void zaperoids::update(double t, float dt) {
    if(command_.thrust) world_.thrust();
    if(command_.left) world_.left();
    if(command_.right) world_.right();

    world_.update(t, dt);
}

void zaperoids::draw() {
    shdr_.bind();
    world_.draw(cam_, shdr_);
    font_.draw(cam_, shdr_);
    shdr_.release();
}

void zaperoids::shutdown() {
    application::shutdown();
}

void zaperoids::on_resize(int width, int height) {
    application::on_resize(width, height);
    cam_.world_pos(vec3f(0,0,1));
    cam_.frustum(0, width, 0, height, 0, 1);
    cam_.orthogonolise(vec3f(0,0,-1));
    cam_.viewport(0, 0, width, height);
}

void zaperoids::on_keypress(char ch) {
    LOG(int(ch));
    if(ch == 9) command_.thrust = true;
    else if(ch == 7) command_.left = true;
    else if(ch == 6) command_.right = true;
}

void zaperoids::on_keyrelease(char ch) {
    LOG(int(ch));
    if(ch == 9) command_.thrust = false;
    else if(ch == 7) command_.left = false;
    else if(ch == 6) command_.right = false;
}

int main(int argc, char* argv[]) {
    zaperoids app;
    return app.run();
}
