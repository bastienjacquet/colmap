// COLMAP - Structure-from-Motion.
// Copyright (C) 2016  Johannes L. Schoenberger <jsch at inf.ethz.ch>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "base/warp.h"

#include "util/logging.h"

namespace colmap {

void WarpImageBetweenCameras(const Camera& source_camera,
                             const Camera& target_camera,
                             const Bitmap& source_image, Bitmap* target_image) {
  CHECK_EQ(source_camera.Width(), source_image.Width());
  CHECK_EQ(source_camera.Height(), source_image.Height());
  CHECK_NOTNULL(target_image);

  target_image->Allocate(static_cast<int>(target_camera.Width()),
                         static_cast<int>(target_camera.Height()),
                         source_image.IsRGB());

  Eigen::Vector2d image_point;
  for (int y = 0; y < target_image->Height(); ++y) {
    image_point.y() = y + 0.5;
    for (int x = 0; x < target_image->Width(); ++x) {
      image_point.x() = x + 0.5;
      // Camera models assume that the upper left pixel center is (0.5, 0.5).
      const Eigen::Vector2d world_point =
          target_camera.ImageToWorld(image_point);
      const Eigen::Vector2d source_point =
          source_camera.WorldToImage(world_point);

      BitmapColor<float> color;
      if (source_image.InterpolateBilinear(source_point.x() - 0.5,
                                           source_point.y() - 0.5, &color)) {
        target_image->SetPixel(x, y, color.Cast<uint8_t>());
      } else {
        target_image->SetPixel(x, y, BitmapColor<uint8_t>(0, 0, 0));
      }
    }
  }
}

}  // namespace colmap
