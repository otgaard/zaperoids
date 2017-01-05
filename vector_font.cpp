/* Created by Darren Otgaar on 2017/01/05. http://www.github.com/otgaard/zap */

#include "vector_font.hpp"

/*
 * NOTE:  Each char is defined in local space of 30 x 50 pixels.  Upper case = Lower case
 */

using zap::maths::vec2b;
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
static std::vector<std::vector<vec2b>> chars = {
        { {0,0}, {0,35}, {0,35}, {25,50}, {25,50}, {30,35}, {30,35}, {30,0}, {0,20}, {30,20} },         // A
        { {0,0}, {30,0} } // B
};


vector_font::vector_font() : line_count_(0) {
}

vector_font::~vector_font() = default;

bool vector_font::initialise() {
    shdr_.add_shader(shader_type::ST_VERTEX, vector_font_vshdr);
    shdr_.add_shader(shader_type::ST_FRAGMENT, vector_font_fshdr);

    if(!vbuf_.allocate() || !ibuf_.allocate() || !mesh_.allocate()) {
        LOG_ERR("Error initialising vector_font resources.");
        return false;
    }

    mesh_.set_stream(&vbuf_); mesh_.set_index(&ibuf_);
    mesh_.bind();

    vbuf_.initialise(10000);            // Storage for 10k lines for now
    ibuf_.initialise(10000);            // ditto

    // Test the alphabet & digits




    return true;
}

size_t vector_font::insert_string(const vec2f& P, const std::string& str) {

}
