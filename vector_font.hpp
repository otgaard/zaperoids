/* Created by Darren Otgaar on 2017/01/05. http://www.github.com/otgaard/zap */
#ifndef ZAPEROIDS_VECTOR_FONT_HPP
#define ZAPEROIDS_VECTOR_FONT_HPP

#include <memory>
#include <zap/graphics2/graphics2_types.hpp>

// Modelled on the classic Asteroids vector font, each char/digit is 30 x 50

using zap::engine::primitive_type;
using zap::engine::buffer_usage;
using zap::engine::index_buffer;
using zap::engine::vertex_stream;
using zap::engine::mesh;

using ibuf_lines_t = index_buffer<uint32_t, primitive_type::PT_LINES, buffer_usage::BU_STATIC_DRAW>;
using mesh_string_t = mesh<vertex_stream<zap::graphics::vbuf_p2c3_t>, primitive_type::PT_LINES, ibuf_lines_t>;
using mesh_str_ptr = std::unique_ptr<mesh_string_t>;

class vector_font {
public:
    vector_font();
    ~vector_font();

    mesh_str_ptr build_string(const std::string& str);

private:

};

#endif //ZAPEROIDS_VECTOR_FONT_HPP
