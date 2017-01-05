/* Created by Darren Otgaar on 2017/01/05. http://www.github.com/otgaard/zap */
#ifndef ZAPEROIDS_VECTOR_FONT_HPP
#define ZAPEROIDS_VECTOR_FONT_HPP

#include <zap/engine/program.hpp>
#include <zap/graphics2/graphics2_types.hpp>

// Modelled on the classic Asteroids vector font, each char/digit is 30 x 50

using zap::engine::primitive_type;
using zap::engine::buffer_usage;
using zap::engine::index_buffer;
using zap::engine::vertex_stream;
using zap::graphics::vbuf_p2c3_t;
using zap::engine::program;
using zap::engine::mesh;
using zap::maths::vec2f;

using ibuf_lines_t = index_buffer<uint32_t, primitive_type::PT_LINES, buffer_usage::BU_STATIC_DRAW>;
using mesh_string_t = mesh<vertex_stream<vbuf_p2c3_t>, primitive_type::PT_LINES, ibuf_lines_t>;

class vector_font {
public:
    vector_font();
    ~vector_font();

    bool initialise();

    size_t insert_string(const vec2f& P, const std::string& str);

private:
    program shdr_;

    vbuf_p2c3_t vbuf_;
    ibuf_lines_t ibuf_;
    mesh_string_t mesh_;

    uint32_t line_count_;
};

#endif //ZAPEROIDS_VECTOR_FONT_HPP
