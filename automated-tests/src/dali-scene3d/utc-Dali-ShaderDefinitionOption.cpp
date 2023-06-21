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

#include <dali-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <string_view>
#include "dali-scene3d/public-api/loader/shader-definition-option.h"

using namespace Dali;
using namespace Dali::Toolkit;

void shader_definition_option_startup(void)
{
  test_return_value = TET_UNDEF;
}

void shader_definition_option_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliShaderDefinitionOptionInit(void)
{
  Scene3D::Loader::ShaderDefinitionOption option;
  DALI_TEST_EQUALS(option.GetOptionHash(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliShaderDefinitionOptionSetTransparency(void)
{
  Scene3D::Loader::ShaderDefinitionOption option;
  DALI_TEST_EQUALS(option.GetOptionHash(), 0u, TEST_LOCATION);

  option.SetTransparency();
  DALI_TEST_NOT_EQUALS(option.GetOptionHash(), static_cast<uint64_t>(0u), 0.1f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliShaderDefinitionOptionAddOption(void)
{
  Scene3D::Loader::ShaderDefinitionOption option;
  DALI_TEST_EQUALS(option.GetOptionHash(), 0u, TEST_LOCATION);

  Scene3D::Loader::ShaderDefinitionOption::Type types[19] = {
    Scene3D::Loader::ShaderDefinitionOption::Type::GLTF_CHANNELS,
    Scene3D::Loader::ShaderDefinitionOption::Type::THREE_TEXTURE,
    Scene3D::Loader::ShaderDefinitionOption::Type::BASE_COLOR_TEXTURE,
    Scene3D::Loader::ShaderDefinitionOption::Type::METALLIC_ROUGHNESS_TEXTURE,
    Scene3D::Loader::ShaderDefinitionOption::Type::NORMAL_TEXTURE,
    Scene3D::Loader::ShaderDefinitionOption::Type::OCCLUSION,
    Scene3D::Loader::ShaderDefinitionOption::Type::EMISSIVE,
    Scene3D::Loader::ShaderDefinitionOption::Type::ALPHA_TEST,
    Scene3D::Loader::ShaderDefinitionOption::Type::SUBSURFACE,
    Scene3D::Loader::ShaderDefinitionOption::Type::SPECULAR,
    Scene3D::Loader::ShaderDefinitionOption::Type::SPECULAR_COLOR,
    Scene3D::Loader::ShaderDefinitionOption::Type::SKINNING,
    Scene3D::Loader::ShaderDefinitionOption::Type::FLIP_UVS_VERTICAL,
    Scene3D::Loader::ShaderDefinitionOption::Type::COLOR_ATTRIBUTE,
    Scene3D::Loader::ShaderDefinitionOption::Type::VEC4_TANGENT,
    Scene3D::Loader::ShaderDefinitionOption::Type::MORPH_POSITION,
    Scene3D::Loader::ShaderDefinitionOption::Type::MORPH_NORMAL,
    Scene3D::Loader::ShaderDefinitionOption::Type::MORPH_TANGENT,
    Scene3D::Loader::ShaderDefinitionOption::Type::MORPH_VERSION_2_0};

  uint64_t hash = 0u;
  for(uint32_t i = 0; i < 19; ++i)
  {
    hash |= (1 << static_cast<uint32_t>(types[i]));
    option.AddOption(types[i]);
    DALI_TEST_EQUALS(option.GetOptionHash(), hash, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliShaderDefinitionOptionGetDefines(void)
{
  Scene3D::Loader::ShaderDefinitionOption option;
  DALI_TEST_EQUALS(option.GetOptionHash(), 0u, TEST_LOCATION);

  Scene3D::Loader::ShaderDefinitionOption::Type types[19] = {
    Scene3D::Loader::ShaderDefinitionOption::Type::GLTF_CHANNELS,
    Scene3D::Loader::ShaderDefinitionOption::Type::THREE_TEXTURE,
    Scene3D::Loader::ShaderDefinitionOption::Type::BASE_COLOR_TEXTURE,
    Scene3D::Loader::ShaderDefinitionOption::Type::METALLIC_ROUGHNESS_TEXTURE,
    Scene3D::Loader::ShaderDefinitionOption::Type::NORMAL_TEXTURE,
    Scene3D::Loader::ShaderDefinitionOption::Type::OCCLUSION,
    Scene3D::Loader::ShaderDefinitionOption::Type::EMISSIVE,
    Scene3D::Loader::ShaderDefinitionOption::Type::ALPHA_TEST,
    Scene3D::Loader::ShaderDefinitionOption::Type::SUBSURFACE,
    Scene3D::Loader::ShaderDefinitionOption::Type::SPECULAR,
    Scene3D::Loader::ShaderDefinitionOption::Type::SPECULAR_COLOR,
    Scene3D::Loader::ShaderDefinitionOption::Type::SKINNING,
    Scene3D::Loader::ShaderDefinitionOption::Type::FLIP_UVS_VERTICAL,
    Scene3D::Loader::ShaderDefinitionOption::Type::COLOR_ATTRIBUTE,
    Scene3D::Loader::ShaderDefinitionOption::Type::VEC4_TANGENT,
    Scene3D::Loader::ShaderDefinitionOption::Type::MORPH_POSITION,
    Scene3D::Loader::ShaderDefinitionOption::Type::MORPH_NORMAL,
    Scene3D::Loader::ShaderDefinitionOption::Type::MORPH_TANGENT,
    Scene3D::Loader::ShaderDefinitionOption::Type::MORPH_VERSION_2_0};

  uint64_t hash = 0u;
  for(uint32_t i = 0; i < 19; ++i)
  {
    hash |= (1 << static_cast<uint32_t>(types[i]));
    option.AddOption(types[i]);
    DALI_TEST_EQUALS(option.GetOptionHash(), hash, TEST_LOCATION);

    std::vector<std::string> defines;
    option.GetDefines(defines);
    DALI_TEST_EQUALS(defines.size(), i + 1, TEST_LOCATION);
    for(uint32_t j = 0; j < defines.size(); ++j)
    {
      DALI_TEST_EQUALS(defines[j], option.GetDefineKeyword(types[j]), TEST_LOCATION);
    }
  }

  END_TEST;
}