#ifndef DALI_SCENE3D_LOADER_BUFFER_DEFINITION_H
#define DALI_SCENE3D_LOADER_BUFFER_DEFINITION_H
/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>

// EXTERNAL INCLUDES
#include <fstream>
#include <memory>
#include <vector>

namespace Dali
{
namespace Scene3D
{
namespace Loader
{
/**
 * @brief Defines a buffer that is loaded from input uri.
 * The buffer can contain 3D resource data such as mesh, animation, and texture.
 */
struct DALI_SCENE3D_API BufferDefinition
{
  using Vector = std::vector<BufferDefinition>;

  BufferDefinition();
  ~BufferDefinition();

  BufferDefinition(const BufferDefinition& other)            = default;
  BufferDefinition& operator=(const BufferDefinition& other) = default;

  BufferDefinition(BufferDefinition&& other);
  BufferDefinition& operator=(BufferDefinition&&) = default;

  /**
   * @brief Retrieves data stream of this buffer.
   * @return iostream of this buffer
   */
  std::iostream& GetBufferStream();

  /**
   * @brief Retrieves uri of this buffer
   * @return uri of the buffer
   */
  std::string GetUri();

  /**
   * @brief Checks whether the buffer is available or not.
   * It is available, if the buffer is successfully loaded from file or base64 stream.
   * @return True if it is available.
   */
  bool IsAvailable();

private:
  /**
   * @brief Loads buffer from file or encoded stream.
   */
  void LoadBuffer();

public: // DATA
  std::string mResourcePath;
  std::string mUri;
  uint32_t    mByteLength{0};
  std::string mName;

private:
  struct Impl;
  std::unique_ptr<Impl> mImpl;

  bool mIsEmbedded{false};
};

} // namespace Loader
} // namespace Scene3D
} // namespace Dali

#endif // DALI_SCENE3D_LOADER_BUFFER_DEFINITION_H