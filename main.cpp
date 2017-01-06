#include <zap/maths/io.hpp>
#include <zap/tools/log.hpp>
#include <zap/maths/algebra.hpp>
#include <zap/host/GLFW/application.hpp>
#include <renderer/camera.hpp>
#include "vector_font.hpp"

using namespace zap;
using namespace zap::maths;
using namespace zap::renderer;

class zaperoids : public application {
public:
    zaperoids() : application("zaperoids", 600, 1024, false), ortho_cam_(false) { }

    bool initialise() override final;
    void update(double t, float dt) override final;
    void draw() override final;
    void shutdown() override final;

    void on_resize(int width, int height) override final;
    void on_mousemove(double x, double y) override final;
    void on_mousewheel(double xinc, double yinc) override final;

protected:
    camera world_cam_;
    camera ortho_cam_;
    vector_font font_;
};

bool zaperoids::initialise() {
    // Test the vector font
    if(!font_.initialise()) {
        LOG_ERR("Failure to initialise font resources");
        return false;
    }

    font_.insert_string(vec2f(10,10), "Hello Asteroid Player");
    font_.insert_string(vec2f(100,100), "Test Me");
    font_.insert_string(vec2f(10,900), "Score 100");

    return true;
}

void zaperoids::update(double t, float dt) {
    if(font_.string_count() > 2) font_.erase_string(1);
}

void zaperoids::draw() {
    font_.draw(ortho_cam_);
}

void zaperoids::shutdown() {
    application::shutdown();
}

void zaperoids::on_resize(int width, int height) {
    LOG("resize", width, height);
    application::on_resize(width, height);
    world_cam_.world_pos(vec3f(0,0,1));
    world_cam_.frustum(45.f, float(width)/height, .5f, 30.f);
    world_cam_.orthogonolise(vec3f(0,0,-1));
    world_cam_.viewport(0, 0, width, height);

    ortho_cam_.world_pos(vec3f(0,0,1));
    ortho_cam_.frustum(0, width, 0, height, 0, 1);
    ortho_cam_.orthogonolise(vec3f(0,0,-1));
    ortho_cam_.viewport(0, 0, width, height);
}

void zaperoids::on_mousemove(double x, double y) {
    application::on_mousemove(x, y);
}

void zaperoids::on_mousewheel(double xinc, double yinc) {
    application::on_mousewheel(xinc, yinc);
}

int main(int argc, char* argv[]) {
    zaperoids app;
    return app.run();
}
