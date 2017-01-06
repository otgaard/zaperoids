/* Created by Darren Otgaar on 2017/01/05. http://www.github.com/otgaard/zap */
#ifndef ZAPEROIDS_VECTOR_FONT_HPP
#define ZAPEROIDS_VECTOR_FONT_HPP

#include <zap/engine/program.hpp>
#include <zap/graphics2/graphics2_types.hpp>
#include <zap/renderer/camera.hpp>

// Modelled on the classic Asteroids vector font, each char/digit is 30 x 50

using zap::engine::primitive_type;
using zap::engine::buffer_usage;
using zap::engine::vertex_stream;
using zap::graphics::vtx_p2c3_t;
using zap::graphics::vbuf_p2c3_t;
using zap::engine::program;
using zap::engine::mesh;
using zap::maths::vec2i;
using zap::maths::vec2f;

using mesh_string_t = mesh<vertex_stream<vbuf_p2c3_t>, primitive_type::PT_LINES>;

class vector_font {
public:
    int char_spacing;

    vector_font();
    ~vector_font();

    bool initialise();

    size_t insert_string(const vec2f& P, const std::string& str);
    void erase_string(size_t id);
    void clear() { prim_count_ = 0; }

    void draw(const zap::renderer::camera& cam);

private:
    program shdr_;

    vbuf_p2c3_t vbuf_;
    mesh_string_t mesh_;

    uint32_t prim_count_;
    std::vector<vec2i> string_index_;

};

#endif //ZAPEROIDS_VECTOR_FONT_HPP
