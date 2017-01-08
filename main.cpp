#include <zap/maths/io.hpp>
#include <zap/tools/log.hpp>
#include <zap/maths/algebra.hpp>
#include <zap/host/GLFW/application.hpp>
#include <renderer/camera.hpp>
#include "vector_font.hpp"
#include "world.hpp"
#include "game.hpp"

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

class zaperoids : public application {
public:
    zaperoids() : application("zaperoids", 600, 1024, false), cam_(false), score_string_(size_t(-1)) { }

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
    game game_;
    size_t score_string_;
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
    world_.set_world_size(600,1024);    // For the generator

    if(!world_.generate()) {
        LOG_ERR("Failed to create world simulation");
        return false;
    }

    world_.generate_level(&game_, 1);

    shdr_.bind();

    return true;
}

void zaperoids::update(double t, float dt) {
    // Check if the game is over
    if(game_.game_over()) {
        if(score_string_ == size_t(-1)) return;
        if(score_string_ != size_t(-1)) font_.erase_string(score_string_);
        score_string_ = size_t(-1);

        std::string game_over = std::string("Game Over");
        auto aabb = font_.string_AABB(game_over);
        font_.insert_string(vec2f(sc_width_ - aabb.width(), sc_height_ - aabb.height())/2.f, game_over);

        std::string press_1 = std::string("Press 1 to Start");
        aabb = font_.string_AABB(press_1);
        font_.insert_string(vec2f(sc_width_ - aabb.width(), sc_height_ + 2.f*aabb.height())/2.f, press_1);
    } else {
        world_.command(0, command_);
        world_.update(t, dt);
        command_.fire = false;

        // Update the score
        std::string score = std::string("Score ") + lexical_cast<int>(game_.get_points(0));
        auto aabb = font_.string_AABB(score);
        if(score_string_ != size_t(-1)) font_.erase_string(score_string_);
        score_string_ = font_.insert_string(vec2f(600 - 20 - aabb.width(), 1024 - 10 - aabb.height()), score);
    }
}

void zaperoids::draw() {
    if(!game_.game_over()) world_.draw(cam_, shdr_);
    font_.draw(cam_, shdr_);
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
    world_.set_world_size(width, height);
}

void zaperoids::on_keypress(char ch) {
    if(ch == 9) command_.thrust = true;
    else if(ch == 7) command_.left = true;
    else if(ch == 6) command_.right = true;
    else if(ch == 32) command_.fire = true;

    if(ch == 49 && game_.game_over()) {
        font_.clear();
        game_.reset();
        world_.generate_level(&game_, game_.get_level());
    }
}

void zaperoids::on_keyrelease(char ch) {
    if(ch == 9) command_.thrust = false;
    else if(ch == 7) command_.left = false;
    else if(ch == 6) command_.right = false;
}

int main(int argc, char* argv[]) {
    zaperoids app;
    return app.run();
}
