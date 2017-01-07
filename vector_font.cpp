/* Created by Darren Otgaar on 2017/01/05. http://www.github.com/otgaard/zap */

#include "vector_font.hpp"
#include <zap/maths/io.hpp>

/*
 * NOTE:  Each char is defined in local space of 30 x 50 pixels.  Upper case = Lower case
 */

using namespace zap::maths;
using namespace zap::engine;

constexpr static size_t INVALID_CHAR = size_t(-1);
constexpr static vec2f char_dims = { 30.f, 50.f };

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


vector_font::vector_font() : char_spacing(10), scale_(.5f), inv_scale_(2.f), vertex_count_(0) {
}

vector_font::~vector_font() = default;

bool vector_font::initialise() {
    if(!vbuf_.allocate() || !mesh_.allocate()) {
        LOG_ERR("Error initialising vector_font resources.");
        return false;
    }

    mesh_.set_stream(&vbuf_);
    mesh_.bind(); vbuf_.bind();

    vbuf_.initialise(10000);            // Storage for 10k lines for now

    return true;
}

size_t char_to_idx(char ch) {
    if(ch >= 48 && ch <= 57) return size_t(ch - 48 + 26);
    else if(ch >= 65 && ch <= 90) return size_t(ch - 65);
    else if(ch >= 97 && ch <= 122) return size_t(ch - 97);
    else return size_t(-1);
}

size_t vector_font::insert_string(const vec2f& P, const std::string& str) {
    const auto horz_spacing = vec2f(char_dims[0] + char_spacing, 0);
    auto lP = inv_scale_ * P;
    vbuf_.bind();
    if(vbuf_.map(buffer_access::BA_WRITE_ONLY)) {
        vec2i string_idx(vertex_count_, vertex_count_);
        for(auto& ch : str) {
            auto idx = char_to_idx(ch);
            if(idx == INVALID_CHAR) lP += horz_spacing;
            else {
                assert(vertex_count_ + chars[idx].size() < vbuf_.vertex_count() && "Buffer Overflow");
                std::transform(chars[idx].begin(), chars[idx].end(), vbuf_.begin()+vertex_count_, [&lP](const vec2f& cP) {
                    return vtx_p2c3_t(cP + lP, vec3b(255, 255, 255));
                });
                vertex_count_ += chars[idx].size();
                lP += horz_spacing;
            }
        }
        string_idx.y = vertex_count_;
        string_index_.push_back(string_idx);
        vbuf_.unmap();
    }
    vbuf_.release();
}

void vector_font::erase_string(size_t idx, bool compress) {
    assert(idx < string_index_.size() && "Invalid string specified to erase_string");
    if(compress) {
        auto si = string_index_[idx];
        vbuf_.bind();
        if(vbuf_.map(buffer_access::BA_WRITE_ONLY)) {
            std::copy(vbuf_.begin()+si.y, vbuf_.begin()+vertex_count_, vbuf_.begin()+si.x);
            vbuf_.unmap();
        }
        vbuf_.release();

        string_index_.erase(string_index_.begin() + idx);
        auto diff = si.y - si.x;
        std::transform(string_index_.begin()+idx, string_index_.end(), string_index_.begin()+idx, [&diff](const vec2i& v) {
            return v - vec2i(diff, diff);
        });
    } else {
        string_index_.erase(string_index_.begin() + idx);
    }
}

void vector_font::draw(const zap::renderer::camera& cam, program& shdr) {
    shdr.bind_uniform("PVM", cam.proj_view() * make_scale(scale_, scale_, 1.f));
    mesh_.bind();
    for(auto& si : string_index_) {
        mesh_.draw(primitive_type::PT_LINES, si.x, si.y - si.x);
    }
    mesh_.release();
}

AABB2i vector_font::string_AABB(const std::string& str) {
    return AABB2i(vec2i(0,0), scale_*str.length()*(char_dims.x+char_spacing)-char_spacing, scale_*char_dims.y);
}
