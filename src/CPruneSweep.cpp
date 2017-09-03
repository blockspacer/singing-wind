#include "CPruneSweep.h"
#include <algorithm>

struct cmp_func {
    size_t sort_axis;
    bool operator() (const PSBox &a, const PSBox &b) {
        return a.mins[sort_axis] <= b.mins[sort_axis];
    }
};

inline bool overlap(const PSBox &a, const PSBox &b) {
    for (size_t i = 0 ; i < 2 ; ++i) {
        if (a.maxs[i] < b.mins[i]) return false;
        if (a.mins[i] > b.maxs[i]) return false;
    }
    return true;
}

void PruneSweeper::prune_and_sweep() {
    std::sort(m_boxes.begin(), m_boxes.end(), cmp_func{sort_axis});
    m_pairs.clear();

    glm::tvec2<float> means{0, 0};
    glm::tvec2<float> sqrd_means{0, 0};

    for (unsigned int i = 0 ; i < m_boxes.size() ; ++i) {
        const auto &box = m_boxes[i];
        glm::tvec2<float> p_mn = 0.5f * (box.mins + box.maxs);

        means += p_mn;
        sqrd_means += p_mn * p_mn;

        for (unsigned int j = i+1 ; j < m_boxes.size() ; ++j) {
            if (m_boxes[j].mins[sort_axis] > box.maxs[sort_axis]) {
                break;
            }
            if (overlap(box, m_boxes[j])) {
                m_pairs.emplace_back(box.entity, m_boxes[j].entity);
            }
        }
    }

    glm::tvec2<float> var = sqrd_means - means * means / (float)m_boxes.size();
    sort_axis = 0;
    if (var.y > var.x) {
        sort_axis = 1;
    }
}
