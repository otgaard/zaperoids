/* Created by Darren Otgaar on 2017/01/05. http://www.github.com/otgaard/zap */

#include "vector_font.hpp"

#include <zap/maths/io.hpp>

/*
 * NOTE:  Each char is defined in local space of 30 x 50 pixels.  Upper case = Lower case
 */

using namespace zap::maths;
using namespace zap::engine;

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

// Lower case (65 - 90), Upper case (97 - 122), Digits (48 - 57)
static std::vector<std::vector<vec2f>> chars = {
        { {0,0}, {0,35}, {0,35}, {15,50}, {15,50}, {30,35}, {30,35}, {30,0}, {0,20}, {30,20} },         // A
        { {0,0}, {30,0} } // B
};


vector_font::vector_font() : line_count_(0) {
}

vector_font::~vector_font() = default;

bool vector_font::initialise() {
    shdr_.add_shader(shader_type::ST_VERTEX, vector_font_vshdr);
    shdr_.add_shader(shader_type::ST_FRAGMENT, vector_font_fshdr);
    if(!shdr_.link()) {
        LOG_ERR("Failed to compile/link vector_font shader resources.");
        return false;
    }

    if(!vbuf_.allocate() || !mesh_.allocate()) {
        LOG_ERR("Error initialising vector_font resources.");
        return false;
    }

    mesh_.set_stream(&vbuf_);
    mesh_.bind(); vbuf_.bind();

    vbuf_.initialise(10000);            // Storage for 10k lines for now

    // Test the alphabet & digits
    if(vbuf_.map(buffer_access::BA_WRITE_ONLY)) {
        // Let's start with the A char
        std::transform(chars[0].begin(), chars[0].end(), vbuf_.begin(), [](const vec2f& P) {
            return vtx_p2c3_t(P, vec3b(255,255,255));
        });
        line_count_ += chars[0].size();
        vbuf_.unmap();
    }

    return true;
}

size_t vector_font::insert_string(const vec2f& P, const std::string& str) {

}

void vector_font::draw(const zap::renderer::camera& cam) {
    shdr_.bind();
    shdr_.bind_uniform("PVM", cam.proj_view()*make_scale(0.0005f, 0.0005f, 1.f));
    mesh_.bind();
    mesh_.draw(primitive_type::PT_LINES, 0, line_count_);
    mesh_.release();
    shdr_.release();
}
