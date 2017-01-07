//
// Created by Darren Otgaar on 2017/01/06.
//

#ifndef ZAPEROIDS_COMMON_HPP
#define ZAPEROIDS_COMMON_HPP

#include <zap/engine/program.hpp>
#include <zap/maths/algebra.hpp>
#include <zap/graphics2/graphics2_types.hpp>
#include <zap/renderer/camera.hpp>
#include <zap/maths/geometry/AABB.hpp>

using zap::engine::primitive_type;
using zap::engine::buffer_usage;
using zap::engine::vertex_stream;
using zap::graphics::vtx_p2c3_t;
using zap::graphics::vbuf_p2c3_t;
using zap::engine::program;
using zap::engine::mesh;
using zap::maths::geometry::AABB2i;
using zap::maths::vec2i;
using zap::maths::vec2f;
using zap::renderer::camera;

using mesh_string_t = mesh<vertex_stream<vbuf_p2c3_t>, primitive_type::PT_LINES>;

#endif //ZAPEROIDS_COMMON_HPP
