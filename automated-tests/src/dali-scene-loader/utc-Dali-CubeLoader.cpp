/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

// Enable debug log for test coverage
#define DEBUG_ENABLED 1

#include "dali-scene-loader/public-api/cube-loader.h"
#include <dali-test-suite-utils.h>
#include <string_view>

#include <fstream>

using namespace Dali;
using namespace Dali::SceneLoader;

int UtcDaliCubeLoaderFailNonexistent(void)
{
  CubeData data;
  DALI_TEST_CHECK(!LoadCubeData("non-existent.jpg", data));
  END_TEST;
}

int UtcDaliCubeLoaderFailInvalid1(void)
{
  CubeData data;
  DALI_TEST_CHECK(!LoadCubeData(TEST_RESOURCE_DIR "/gallery-small-1.jpg", data)); // Wrong sized image
  END_TEST;
}

int UtcDaliCubeLoaderSuccess01(void)
{
  CubeData cubeData;
  auto path = TEST_RESOURCE_DIR "/forest_diffuse_cubemap.png";  // cross horizontal
  DALI_TEST_CHECK(LoadCubeData(path, cubeData));

  DALI_TEST_EQUAL(6u, cubeData.data.size());
  for (auto& face: cubeData.data)
  {
    uint32_t size = 512;
    DALI_TEST_EQUAL(size, face[0].GetWidth());
    DALI_TEST_EQUAL(size, face[0].GetHeight());
    DALI_TEST_EQUAL(Pixel::Format::RGBA8888, face[0].GetPixelFormat());
  }

  END_TEST;
}

int UtcDaliCubeLoaderSuccess02(void)
{
  CubeData cubeData;
  auto path = TEST_RESOURCE_DIR "/forest_diffuse_cubemap_cross_vertical.png"; // cross vertical
  DALI_TEST_CHECK(LoadCubeData(path, cubeData));

  DALI_TEST_EQUAL(6u, cubeData.data.size());
  for (auto& face: cubeData.data)
  {
    uint32_t size = 256;
    DALI_TEST_EQUAL(size, face[0].GetWidth());
    DALI_TEST_EQUAL(size, face[0].GetHeight());
    DALI_TEST_EQUAL(Pixel::Format::RGBA8888, face[0].GetPixelFormat());
  }

  END_TEST;
}

int UtcDaliCubeLoaderSuccess03(void)
{
  CubeData cubeData;
  auto path = TEST_RESOURCE_DIR "/cubemap_array_horizontal.png"; // array horizontal
  DALI_TEST_CHECK(LoadCubeData(path, cubeData));

  DALI_TEST_EQUAL(6u, cubeData.data.size());
  for (auto& face: cubeData.data)
  {
    uint32_t size = 100;
    DALI_TEST_EQUAL(size, face[0].GetWidth());
    DALI_TEST_EQUAL(size, face[0].GetHeight());
    DALI_TEST_EQUAL(Pixel::Format::RGB888, face[0].GetPixelFormat());
  }

  END_TEST;
}

int UtcDaliCubeLoaderSuccess04(void)
{
  CubeData cubeData;
  auto path = TEST_RESOURCE_DIR "/cubemap_array_vertical.png"; // array horizontal
  DALI_TEST_CHECK(LoadCubeData(path, cubeData));

  DALI_TEST_EQUAL(6u, cubeData.data.size());
  for (auto& face: cubeData.data)
  {
    uint32_t size = 100;
    DALI_TEST_EQUAL(size, face[0].GetWidth());
    DALI_TEST_EQUAL(size, face[0].GetHeight());
    DALI_TEST_EQUAL(Pixel::Format::RGB888, face[0].GetPixelFormat());
  }

  END_TEST;
}

int UtcDaliCubeLoaderCubeDataCreateTexture(void)
{
  CubeData cubeData;
  auto path = TEST_RESOURCE_DIR "/forest_diffuse_cubemap.png";
  DALI_TEST_CHECK(LoadCubeData(path, cubeData));

  TestApplication app;
  auto texture = cubeData.CreateTexture();

  DALI_TEST_CHECK(texture);
  DALI_TEST_EQUAL(512u, texture.GetWidth());
  DALI_TEST_EQUAL(512u, texture.GetHeight());

  END_TEST;
}
