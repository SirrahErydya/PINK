/**
 * @file   ImageProcessingLib/rotate.h
 * @date   Oct 31, 2018
 * @author Bernd Doser, HITS gGmbH
 */

#pragma once

#include "UtilitiesLib/Interpolation.h"
#include "UtilitiesLib/pink_exception.h"

namespace pink {

template <typename T>
void rotate_bilinear(T const* src, T *dst, int src_height, int src_width, int dst_height, int dst_width, float alpha)
{
    const float cos_alpha = cos(alpha);
    const float sin_alpha = sin(alpha);

    // Center of src image
    const float src_center_x = (src_width - 1) * 0.5;
    const float src_center_y = (src_height - 1) * 0.5;

    // Center of dst image
    const float dst_center_x = (dst_width - 1) * 0.5;
    const float dst_center_y = (dst_height - 1) * 0.5;

    for (int dst_x = 0; dst_x < dst_width; ++dst_x) {
        for (int dst_y = 0; dst_y < dst_height; ++dst_y) {

            float dst_position_x = static_cast<float>(dst_x) - dst_center_x;
            float dst_position_y = static_cast<float>(dst_y) - dst_center_y;

            float src_position_x = dst_position_x * cos_alpha - dst_position_y * sin_alpha + src_center_x;
            float src_position_y = dst_position_x * sin_alpha + dst_position_y * cos_alpha + src_center_y;

            if (src_position_x < 0.0 or src_position_x > src_width - 1 or src_position_y < 0.0 or src_position_y > src_height - 1)
            {
                dst[dst_x * dst_height + dst_y] = 0.0;
            }
            else
            {
                int src_x = src_position_x;
                int src_y = src_position_y;

                int src_x_plus_1 = src_x + 1;
                int src_y_plus_1 = src_y + 1;

                float rx = src_position_x - src_x;
                float ry = src_position_y - src_y;

                float cx = 1.0 - rx;
                float cy = 1.0 - ry;

                dst[dst_x * dst_height + dst_y] = cx * cy * src[src_x * src_height + src_y]
                                                + cx * ry * src[src_x * src_height + src_y_plus_1]
                                                + rx * cy * src[src_x_plus_1 * src_height + src_y]
                                                + rx * ry * src[src_x_plus_1 * src_height + src_y_plus_1];
            }
        }
    }
}

template <typename T>
void rotate(T const* src, T *dst, int src_height, int src_width, int dst_height, int dst_width, float alpha, Interpolation interpolation)
{
    if (interpolation == Interpolation::BILINEAR)
        rotate_bilinear(src, dst, src_height, src_width, dst_height, dst_width, alpha);
    else {
        throw pink::exception("rotate: unknown interpolation\n");
    }
}

} // namespace pink
