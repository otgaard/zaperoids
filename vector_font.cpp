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
        { {0,0}, {0,35}, {0,35}, {15,50}, {15,50}, {30,35}, {30,35}, {30,0}, {0,20}, {30,20} },             // A
        { {0,0}, {0,50}, {0,50}, {25,50}, {25,50}, {30,42}, {30,42}, {30,33}, {30,33}, {25,25}, {25,25},
          {0,25}, {25,25}, {30,17}, {30,17}, {30,8}, {30,8}, {25,0}, {25,0}, {0,0}  },                      // B
        { {30,0}, {0,0}, {0,0}, {0,50}, {0,50}, {30,50} },                                                  // C
        { {0,0}, {0,50}, {0,50}, {10,50}, {10,50}, {30,35}, {30,35}, {30,15}, {30,15}, {10,0}, {10,0},      // D
          {0,0} },
        { {30,0}, {0,0}, {0,0}, {0,50}, {0,50}, {30,50}, {0,25}, {25,25} },                                 // E
        { {0,0}, {0,50}, {0,50}, {30,50}, {0,25}, {25,25} },                                                // F
        { {15,15}, {30,15}, {30,15}, {30,0}, {30,0}, {0,0}, {0,0}, {0,50}, {0,50}, {30,50}, {30,50},
          {30,30} },                                                                                        // G
        { {0,0}, {0,50}, {30,0}, {30,50}, {0,25}, {30,25} },                                                // H
        { {0,0}, {30,0}, {15,0}, {15,50}, {0,50}, {30,50} },                                                // I
        { {0,15}, {15,0}, {15,0}, {30,0}, {30,0}, {30,50} },                                                // J
        { {0,0}, {0,50}, {30,50}, {0,25}, {0,25}, {30,0} },                                                 // K
        { {30,0}, {0,0}, {0,0}, {0,50} },                                                                   // L
        { {0,0}, {0,50}, {0,50}, {15,40}, {15,40}, {30,50}, {30,50}, {30,0} },                              // M
        { {0,0}, {0,50}, {0,50}, {30,0}, {30,0}, {30,50} },                                                 // N
        { {0,0}, {0,50}, {0,50}, {30,50}, {30,50}, {30,0}, {30,0}, {0,0} },                                 // O
        { {0,0}, {0,50}, {0,50}, {30,50}, {30,50}, {30,25}, {30,25}, {0,25} },                              // P
        { {0,0}, {0,50}, {0,50}, {30,50}, {30,50}, {30,15}, {30,15}, {15,0}, {15,0}, {0,0}, {15,15},
          {30,0} },                                                                                         // Q
        { {0,0}, {0,50}, {0,50}, {30,50}, {30,50}, {30,25}, {30,25}, {0,25}, {0,25}, {30,0} },              // R
        { {0,0}, {30,0}, {30,0}, {30,25}, {30,25}, {0,25}, {0,25}, {0,50}, {0,50}, {30,50} },               // S
        { {15,0}, {15,50}, {0,50}, {30,50} },                                                               // T
        { {0,50}, {0,0}, {0,0}, {30,0}, {30,0}, {30,50} },                                                  // U
        { {0,50}, {15,0}, {15,0}, {30,50} },                                                                // V
        { {0,50}, {0,0}, {0,0}, {15,15}, {15,15}, {30,0}, {30,0}, {30,50} },                                // W
        { {0,50}, {30,0}, {0,0}, {30,50} },                                                                 // X
        { {0,50}, {15,35}, {15,35}, {30,50}, {15,35}, {15,0} },                                             // Y
        { {0,50}, {30,50}, {30,50}, {0,0}, {0,0}, {30,0} },                                                 // Z
        { {0,0}, {0,50}, {0,50}, {30,50}, {30,50}, {30,0}, {30,0}, {0,0}, {30,35}, {0,15} },                // 0
        { {15,0}, {15, 50} },                                                                               // 1
        { {0,50}, {30,50}, {30,50}, {30,25}, {30,25}, {0,25}, {0,25}, {0,0}, {0,0}, {30,0} },               // 2
        { {0,0}, {30,0}, {30,0}, {30,50}, {30,50}, {0,50}, {0,25}, {30,25} },                               // 3
        { {0,50}, {0,25}, {0,25}, {30,25}, {30,50}, {30,0} },                                               // 4
        { {0,0}, {30,0}, {30,0}, {30,25}, {30,25}, {0,25}, {0,25}, {0,50}, {0,50}, {30,50} },               // 5
        { {0,50}, {0,0}, {0,0}, {30,0}, {30,0}, {30,25}, {30,25}, {0,25} },                                 // 6
        { {0,50}, {30,50}, {30,50}, {30,0} },                                                               // 7
        { {0,0}, {30,0}, {30,0}, {30,25}, {30,25}, {0,25}, {0,25}, {0,50}, {0,50}, {30,50},
          {30,50}, {30,25}, {0,0}, {0,25} },                                                                // 8
        { {30,0}, {30,50}, {30,50}, {0,50}, {0,50}, {0,25}, {0,25}, {30,25}}                                // 9
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
        // Test Char x
        auto char_id = 35;
        std::transform(chars[char_id].begin(), chars[char_id].end(), vbuf_.begin(), [](const vec2f& P) {
            return vtx_p2c3_t(P, vec3b(255,255,255));
        });
        line_count_ += chars[char_id].size();
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
