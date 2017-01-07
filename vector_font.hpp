/* Created by Darren Otgaar on 2017/01/05. http://www.github.com/otgaard/zap */
#ifndef ZAPEROIDS_VECTOR_FONT_HPP
#define ZAPEROIDS_VECTOR_FONT_HPP

#include "common.hpp"
// Modelled on the classic Asteroids vector font, each char/digit is 30 x 50

class vector_font {
public:
    int char_spacing;

    vector_font();
    ~vector_font();

    bool initialise();

    void set_scale(float scale) {
        scale_ = scale;
        inv_scale_ = 1.f/scale;
    }

    float get_scale() const { return scale_; }
    float get_inv_scale() const { return inv_scale_; }

    AABB2i string_AABB(const std::string& str);

    size_t insert_string(const vec2f& P, const std::string& str);
    void erase_string(size_t idx, bool compress=true);
    void clear() { vertex_count_ = 0; }
    size_t string_count() { return string_index_.size(); }

    void draw(const zap::renderer::camera& cam);

private:
    program shdr_;

    vbuf_p2c3_t vbuf_;
    mesh_string_t mesh_;

    float scale_;
    float inv_scale_;

    uint32_t vertex_count_;
    std::vector<vec2i> string_index_;
};

#endif //ZAPEROIDS_VECTOR_FONT_HPP
