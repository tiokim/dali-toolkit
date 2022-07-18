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

// CLASS HEADER
#include "model-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <filesystem>

// INTERNAL INCLUDES
#include <dali-scene-loader/public-api/animation-definition.h>
#include <dali-scene-loader/public-api/camera-parameters.h>
#include <dali-scene-loader/public-api/dli-loader.h>
#include <dali-scene-loader/public-api/gltf2-loader.h>
#include <dali-scene-loader/public-api/light-parameters.h>
#include <dali-scene-loader/public-api/load-result.h>
#include <dali-scene-loader/public-api/node-definition.h>
#include <dali-scene-loader/public-api/scene-definition.h>
#include <dali-scene-loader/public-api/shader-definition-factory.h>
#include <dali-scene-loader/public-api/controls/model-view/model-view.h>

using namespace Dali;

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
namespace
{
BaseHandle Create()
{
  return Scene3D::ModelView::New(std::string());
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Scene3D::ModelView, Toolkit::Control, Create);
DALI_TYPE_REGISTRATION_END()

static constexpr uint32_t OFFSET_FOR_DIFFUSE_CUBE_TEXTURE  = 2u;
static constexpr uint32_t OFFSET_FOR_SPECULAR_CUBE_TEXTURE = 1u;

static constexpr Vector3 Y_DIRECTION(1.0f, -1.0f, 1.0f);

static constexpr std::string_view KTX_EXTENSION  = ".ktx";
static constexpr std::string_view OBJ_EXTENSION  = ".obj";
static constexpr std::string_view GLTF_EXTENSION = ".gltf";
static constexpr std::string_view DLI_EXTENSION  = ".dli";

struct BoundingVolume
{
  void Init()
  {
    pointMin = Vector3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    pointMax = Vector3(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
  }

  void ConsiderNewPointInVolume(const Vector3& position)
  {
    pointMin.x = std::min(position.x, pointMin.x);
    pointMin.y = std::min(position.y, pointMin.y);
    pointMin.z = std::min(position.z, pointMin.z);

    pointMax.x = std::max(position.x, pointMax.x);
    pointMax.y = std::max(position.y, pointMax.y);
    pointMax.z = std::max(position.z, pointMax.z);
  }

  Vector3 CalculateSize()
  {
    return pointMax - pointMin;
  }

  Vector3 CalculatePivot()
  {
    Vector3 pivot = pointMin / (pointMin - pointMax);
    for(uint32_t i = 0; i < 3; ++i)
    {
      // To avoid divid by zero
      if(pointMin[i] == pointMax[i])
      {
        pivot[i] = 0.5f;
      }
    }
    return pivot;
  }

  Vector3 pointMin;
  Vector3 pointMax;
};

Texture LoadCubeMap(const std::string& cubeMapPath)
{
  std::filesystem::path modelPath(cubeMapPath);
  std::string           extension = modelPath.extension();
  std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

  Texture cubeTexture;
  if(extension == KTX_EXTENSION)
  {
    SceneLoader::CubeData cubeData;
    if(SceneLoader::LoadCubeMapData(cubeMapPath, cubeData))
    {
      cubeTexture = cubeData.CreateTexture();
    }
    else
    {
      DALI_LOG_ERROR("Fail to load cube map, %s\n", cubeMapPath.c_str());
    }
  }

  return cubeTexture;
}

void ConfigureBlendShapeShaders(
  SceneLoader::ResourceBundle& resources, const SceneLoader::SceneDefinition& scene, Actor root, std::vector<SceneLoader::BlendshapeShaderConfigurationRequest>&& requests)
{
  std::vector<std::string> errors;
  auto                     onError = [&errors](const std::string& msg) { errors.push_back(msg); };
  if(!scene.ConfigureBlendshapeShaders(resources, root, std::move(requests), onError))
  {
    SceneLoader::ExceptionFlinger flinger(ASSERT_LOCATION);
    for(auto& msg : errors)
    {
      flinger << msg << '\n';
    }
  }
}

void AddModelTreeToAABB(BoundingVolume& AABB, const SceneLoader::SceneDefinition& scene, const Dali::SceneLoader::Customization::Choices& choices, Dali::SceneLoader::Index iNode, Dali::SceneLoader::NodeDefinition::CreateParams& nodeParams, Matrix parentMatrix)
{
  static constexpr uint32_t BOX_POINT_COUNT             = 8;
  static uint32_t           BBIndex[BOX_POINT_COUNT][3] = {{0, 0, 0}, {0, 1, 0}, {1, 0, 0}, {1, 1, 0}, {0, 0, 1}, {0, 1, 1}, {1, 0, 1}, {1, 1, 1}};

  Matrix nodeMatrix;
  const SceneLoader::NodeDefinition* node        = scene.GetNode(iNode);
  Matrix                             localMatrix = node->GetLocalSpace();
  Matrix::Multiply(nodeMatrix, localMatrix, parentMatrix);

  Vector3 volume[2];
  if(node->GetExtents(nodeParams.mResources, volume[0], volume[1]))
  {
    for(uint32_t i = 0; i < BOX_POINT_COUNT; ++i)
    {
      Vector4 position       = Vector4(volume[BBIndex[i][0]].x, volume[BBIndex[i][1]].y, volume[BBIndex[i][2]].z, 1.0f);
      Vector4 objectPosition = nodeMatrix * position;
      objectPosition /= objectPosition.w;

      AABB.ConsiderNewPointInVolume(Vector3(objectPosition));
    }
  }

  if(node->mCustomization)
  {
    if(!node->mChildren.empty())
    {
      auto                     choice = choices.Get(node->mCustomization->mTag);
      Dali::SceneLoader::Index i      = std::min(choice != Dali::SceneLoader::Customization::NONE ? choice : 0, static_cast<Dali::SceneLoader::Index>(node->mChildren.size() - 1));

      AddModelTreeToAABB(AABB, scene, choices, node->mChildren[i], nodeParams, nodeMatrix);
    }
  }
  else
  {
    for(auto i : node->mChildren)
    {
      AddModelTreeToAABB(AABB, scene, choices, i, nodeParams, nodeMatrix);
    }
  }
}

} // anonymous namespace

ModelView::ModelView(const std::string& modelPath, const std::string& resourcePath)
: Control(ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT)),
  mModelPath(modelPath),
  mResourcePath(resourcePath),
  mModelLayer(),
  mModelRoot(),
  mNaturalSize(Vector3::ZERO),
  mModelPivot(AnchorPoint::CENTER),
  mIblScaleFactor(1.0f),
  mFitSize(false),
  mFitCenter(false)
{
}

ModelView::~ModelView()
{
}

Dali::Scene3D::ModelView ModelView::New(const std::string& modelPath, const std::string& resourcePath)
{
  ModelView* impl = new ModelView(modelPath, resourcePath);

  Dali::Scene3D::ModelView handle = Dali::Scene3D::ModelView(*impl);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

const Actor ModelView::GetModelRoot()
{
  return mModelRoot;
}

void ModelView::FitSize(bool fit)
{
  mFitSize = fit;
  ScaleModel();
}

void ModelView::FitCenter(bool fit)
{
  mFitCenter = fit;
  FitModelPosition();
}

void ModelView::SetImageBasedLightSource(const std::string& diffuse, const std::string& specular, float scaleFactor)
{
  Texture diffuseTexture = LoadCubeMap(diffuse);
  if(diffuseTexture)
  {
    Texture specularTexture = LoadCubeMap(specular);
    if(specularTexture)
    {
      mDiffuseTexture  = diffuseTexture;
      mSpecularTexture = specularTexture;
      mIblScaleFactor  = scaleFactor;

      SetImageBasedLight(mModelRoot);
    }
  }
}

uint32_t ModelView::GetAnimationCount()
{
  return mAnimations.size();
}

Dali::Animation ModelView::GetAnimation(uint32_t index)
{
  Dali::Animation animation;
  if(mAnimations.size() > index)
  {
    animation = mAnimations[index].second;
  }
  return animation;
}

Dali::Animation ModelView::GetAnimation(const std::string& name)
{
  Dali::Animation animation;
  if(!name.empty())
  {
    for(auto&& animationData : mAnimations)
    {
      if(animationData.first == name)
      {
        animation = animationData.second;
        break;
      }
    }
  }
  return animation;
}

///////////////////////////////////////////////////////////
//
// Private methods
//

void ModelView::OnSceneConnection(int depth)
{
  if(!mModelRoot)
  {
    LoadModel();
  }

  Control::OnSceneConnection(depth);
}

void ModelView::OnInitialize()
{
  Actor self  = Self();
  mModelLayer = Layer::New();
  mModelLayer.SetProperty(Layer::Property::BEHAVIOR, Layer::LAYER_3D);
  mModelLayer.SetProperty(Layer::Property::DEPTH_TEST, true);
  mModelLayer.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  mModelLayer.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  mModelLayer.SetResizePolicy(ResizePolicy::FILL_TO_PARENT,
                              Dimension::ALL_DIMENSIONS);

  // Models in glTF and dli are defined as right hand coordinate system.
  // DALi uses left hand coordinate system. Scaling negative is for change winding order.
  mModelLayer.SetProperty(Dali::Actor::Property::SCALE_Y, -1.0f);
  self.Add(mModelLayer);
}

Vector3 ModelView::GetNaturalSize()
{
  if(!mModelRoot)
  {
    LoadModel();
  }

  return mNaturalSize;
}

float ModelView::GetHeightForWidth(float width)
{
  Extents padding;
  padding = Self().GetProperty<Extents>(Toolkit::Control::Property::PADDING);
  return Control::GetHeightForWidth(width) + padding.top + padding.bottom;
}

float ModelView::GetWidthForHeight(float height)
{
  Extents padding;
  padding = Self().GetProperty<Extents>(Toolkit::Control::Property::PADDING);
  return Control::GetWidthForHeight(height) + padding.start + padding.end;
}

void ModelView::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  Control::OnRelayout(size, container);
  ScaleModel();
}

void ModelView::LoadModel()
{
  std::filesystem::path modelPath(mModelPath);
  if(mResourcePath.empty())
  {
    mResourcePath = std::string(modelPath.parent_path()) + "/";
  }
  std::string extension = modelPath.extension();
  std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

  SceneLoader::ResourceBundle::PathProvider pathProvider = [&](SceneLoader::ResourceType::Value type) {
    return mResourcePath;
  };

  SceneLoader::ResourceBundle                        resources;
  SceneLoader::SceneDefinition                       scene;
  std::vector<SceneLoader::AnimationGroupDefinition> animGroups;
  std::vector<SceneLoader::CameraParameters>         cameraParameters;
  std::vector<SceneLoader::LightParameters>          lights;

  std::vector<Dali::SceneLoader::AnimationDefinition> animations;
  animations.clear();

  SceneLoader::LoadResult output{resources, scene, animations, animGroups, cameraParameters, lights};

  if(extension == DLI_EXTENSION)
  {
    SceneLoader::DliLoader              loader;
    SceneLoader::DliLoader::InputParams input{
      pathProvider(SceneLoader::ResourceType::Mesh),
      nullptr,
      {},
      {},
      nullptr,
      {}};
    SceneLoader::DliLoader::LoadParams loadParams{input, output};
    if(!loader.LoadScene(mModelPath, loadParams))
    {
      SceneLoader::ExceptionFlinger(ASSERT_LOCATION) << "Failed to load scene from '" << mModelPath << "': " << loader.GetParseError();
    }
  }
  else if(extension == GLTF_EXTENSION)
  {
    SceneLoader::ShaderDefinitionFactory sdf;
    sdf.SetResources(resources);
    SceneLoader::LoadGltfScene(mModelPath, sdf, output);

    resources.mEnvironmentMaps.push_back({});
  }
  else
  {
    DALI_LOG_ERROR("Unsupported model type.\n");
  }

  SceneLoader::Transforms                   xforms{SceneLoader::MatrixStack{}, SceneLoader::ViewProjection{}};
  SceneLoader::NodeDefinition::CreateParams nodeParams{resources, xforms, {}, {}, {}};
  SceneLoader::Customization::Choices       choices;

  mModelRoot = Actor::New();

  BoundingVolume AABB;
  for(auto iRoot : scene.GetRoots())
  {
    auto resourceRefs = resources.CreateRefCounter();
    scene.CountResourceRefs(iRoot, choices, resourceRefs);
    resources.CountEnvironmentReferences(resourceRefs);

    resources.LoadResources(resourceRefs, pathProvider);

    // glTF Mesh is defined in right hand coordinate system, with positive Y for Up direction.
    // Because DALi uses left hand system, Y direciton will be flipped for environment map sampling.
    for(auto&& env : resources.mEnvironmentMaps)
    {
      env.first.mYDirection = Y_DIRECTION;
    }

    if(auto actor = scene.CreateNodes(iRoot, choices, nodeParams))
    {
      scene.ConfigureSkeletonJoints(iRoot, resources.mSkeletons, actor);
      scene.ConfigureSkinningShaders(resources, actor, std::move(nodeParams.mSkinnables));
      ConfigureBlendShapeShaders(resources, scene, actor, std::move(nodeParams.mBlendshapeRequests));

      scene.ApplyConstraints(actor, std::move(nodeParams.mConstrainables));

      mModelRoot.Add(actor);
    }

    AddModelTreeToAABB(AABB, scene, choices, iRoot, nodeParams, Matrix::IDENTITY);
  }

  if(!animations.empty())
  {
    auto getActor = [&](const std::string& name) {
      return mModelRoot.FindChildByName(name);
    };

    mAnimations.clear();
    for(auto&& animation : animations)
    {
      Dali::Animation anim = animation.ReAnimate(getActor);

      mAnimations.push_back({animation.mName, anim});
    }
  }

  SetImageBasedLight(mModelRoot);

  mNaturalSize = AABB.CalculateSize();
  mModelPivot = AABB.CalculatePivot();
  mModelRoot.SetProperty(Dali::Actor::Property::SIZE, mNaturalSize);

  FitModelPosition();
  ScaleModel();

  mModelLayer.Add(mModelRoot);
}

void ModelView::ScaleModel()
{
  if(mModelRoot)
  {
    if(mFitSize)
    {
      Vector3 size = Self().GetProperty<Vector3>(Dali::Actor::Property::SIZE);
      if(size.x > 0.0f && size.y > 0.0f)
      {
        float scaleFactor = MAXFLOAT;
        scaleFactor       = std::min(size.x / mNaturalSize.x, scaleFactor);
        scaleFactor       = std::min(size.y / mNaturalSize.y, scaleFactor);
        mModelRoot.SetProperty(Dali::Actor::Property::SCALE, scaleFactor);
      }
      else
      {
        DALI_LOG_ERROR("ModelView size is wrong.");
      }
    }
    else
    {
      mModelRoot.SetProperty(Dali::Actor::Property::SCALE, 1.0f);
    }
  }
}

void ModelView::FitModelPosition()
{
  if(mModelRoot)
  {
    if(mFitCenter)
    {
      // Loaded model pivot is not the model center.
      mModelRoot.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      mModelRoot.SetProperty(Dali::Actor::Property::ANCHOR_POINT, Vector3::ONE - mModelPivot);
    }
    else
    {
      mModelRoot.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
      mModelRoot.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    }
  }
}

void ModelView::SetImageBasedLight(Actor node)
{
  if(!mDiffuseTexture || !mSpecularTexture || !node)
  {
    return;
  }

  uint32_t rendererCount = node.GetRendererCount();
  if(rendererCount)
  {
    node.RegisterProperty(SceneLoader::NodeDefinition::GetIblScaleFactorUniformName().data(), mIblScaleFactor);
  }

  for(uint32_t i = 0; i < rendererCount; ++i)
  {
    Dali::Renderer renderer = node.GetRendererAt(i);
    if(renderer)
    {
      Dali::TextureSet textures = renderer.GetTextures();
      if(textures)
      {
        uint32_t textureCount = textures.GetTextureCount();
        // EnvMap requires at least 2 texture, diffuse and specular
        if(textureCount > 2u)
        {
          textures.SetTexture(textureCount - OFFSET_FOR_DIFFUSE_CUBE_TEXTURE, mDiffuseTexture);
          textures.SetTexture(textureCount - OFFSET_FOR_SPECULAR_CUBE_TEXTURE, mSpecularTexture);
        }
      }
    }
  }

  uint32_t childrenCount = node.GetChildCount();
  for(uint32_t i = 0; i < childrenCount; ++i)
  {
    SetImageBasedLight(node.GetChildAt(i));
  }
}

} // namespace Internal
} // namespace Scene3D
} // namespace Dali
