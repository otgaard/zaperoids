#include <zap/maths/io.hpp>
#include <zap/tools/log.hpp>
#include <zap/maths/algebra.hpp>
#include <zap/host/GLFW/application.hpp>
#include <renderer/camera.hpp>

using namespace zap;
using namespace zap::maths;
using namespace zap::renderer;

class zaperoids : public application {
public:
    zaperoids() : application("zaperoids", 600, 1024, false) { }

    bool initialise() override final;
    void update(double t, float dt) override final;
    void draw() override final;
    void shutdown() override final;

    void on_resize(int width, int height) override final;
    void on_mousemove(double x, double y) override final;
    void on_mousewheel(double xinc, double yinc) override final;

protected:
    camera cam_;
};

bool zaperoids::initialise() {
    return application::initialise();
}

void zaperoids::update(double t, float dt) {
    application::update(t, dt);
}

void zaperoids::draw() {
    application::draw();
}

void zaperoids::shutdown() {
    application::shutdown();
}

void zaperoids::on_resize(int width, int height) {
    application::on_resize(width, height);
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
