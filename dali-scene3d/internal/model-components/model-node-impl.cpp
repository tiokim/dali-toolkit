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

// CLASS HEADER
#include <dali-scene3d/internal/model-components/model-node-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/model-components/model-primitive-impl.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
namespace
{
/**
 * Creates control through type registry
 */
BaseHandle Create()
{
  return Scene3D::ModelNode::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Scene3D::ModelNode, Dali::CustomActor, Create);
DALI_TYPE_REGISTRATION_END()
} // unnamed namespace

Dali::Scene3D::ModelNode ModelNode::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<ModelNode> nodeImpl = new ModelNode();

  // Pass ownership to handle
  Scene3D::ModelNode handle(*nodeImpl);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  nodeImpl->Initialize();

  return handle;
}

ModelNode::ModelNode()
: CustomActorImpl(ActorFlags::DISABLE_SIZE_NEGOTIATION)
{
}

ModelNode::~ModelNode()
{
}

void ModelNode::Initialize()
{
  OnInitialize();
}

void ModelNode::OnInitialize()
{
}

void ModelNode::OnSceneConnection(int depth)
{
}

void ModelNode::OnSceneDisconnection()
{
}

void ModelNode::OnChildAdd(Actor& child)
{
}

void ModelNode::OnChildRemove(Actor& child)
{
}

void ModelNode::OnPropertySet(Property::Index index, const Property::Value& propertyValue)
{
}

void ModelNode::OnSizeSet(const Vector3& targetSize)
{
}

void ModelNode::OnSizeAnimation(Animation& animation, const Vector3& targetSize)
{
  // @todo size negotiate background to new size, animate as well?
}

void ModelNode::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
}

void ModelNode::OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension)
{
}

Vector3 ModelNode::GetNaturalSize()
{
  return Vector3::ZERO;
}

float ModelNode::CalculateChildSize(const Dali::Actor& child, Dimension::Type dimension)
{
  return 0.0f;
}

float ModelNode::GetHeightForWidth(float width)
{
  return 0.0f;
}

float ModelNode::GetWidthForHeight(float height)
{
  return 0.0f;
}

bool ModelNode::RelayoutDependentOnChildren(Dimension::Type dimension)
{
  return false;
}

void ModelNode::OnCalculateRelayoutSize(Dimension::Type dimension)
{
}

void ModelNode::OnLayoutNegotiated(float size, Dimension::Type dimension)
{
}

ModelNode& GetImplementation(Dali::Scene3D::ModelNode& handle)
{
  CustomActorImpl& customInterface = handle.GetImplementation();
  ModelNode&       impl            = dynamic_cast<Internal::ModelNode&>(customInterface);
  return impl;
}

const ModelNode& GetImplementation(const Dali::Scene3D::ModelNode& handle)
{
  const CustomActorImpl& customInterface = handle.GetImplementation();
  // downcast to control
  const ModelNode& impl = dynamic_cast<const Internal::ModelNode&>(customInterface);
  return impl;
}

// Public Method

uint32_t ModelNode::GetModelPrimitiveCount() const
{
  return static_cast<uint32_t>(mModelPrimitiveContainer.size());
}

void ModelNode::AddModelPrimitive(Dali::Scene3D::ModelPrimitive modelPrimitive)
{
  for(auto&& primitive : mModelPrimitiveContainer)
  {
    if(primitive == modelPrimitive)
    {
      return;
    }
  }

  mModelPrimitiveContainer.push_back(modelPrimitive);

  Actor self = Self();
  GetImplementation(modelPrimitive).AddPrimitiveObserver(this);
  if(mDiffuseTexture && mSpecularTexture)
  {
    GetImplementation(modelPrimitive).SetImageBasedLightTexture(mDiffuseTexture, mSpecularTexture, mIblScaleFactor, mSpecularMipmapLevels);
  }

  Dali::Renderer renderer = GetImplementation(modelPrimitive).GetRenderer();
  if(renderer)
  {
    uint32_t rendererCount = self.GetRendererCount();
    bool     exist         = false;
    for(uint32_t i = 0; i < rendererCount; ++i)
    {
      if(renderer == self.GetRendererAt(i))
      {
        exist = true;
        break;
      }
    }
    if(!exist)
    {
      self.AddRenderer(renderer);
    }
  }
}

void ModelNode::RemoveModelPrimitive(Dali::Scene3D::ModelPrimitive modelPrimitive)
{
  uint32_t primitiveCount = GetModelPrimitiveCount();
  for(uint32_t i = 0; i < primitiveCount; ++i)
  {
    if(mModelPrimitiveContainer[i] != modelPrimitive)
    {
      continue;
    }
    RemoveModelPrimitive(i);
    break;
  }
}

void ModelNode::RemoveModelPrimitive(uint32_t index)
{
  if(index >= mModelPrimitiveContainer.size())
  {
    return;
  }

  Actor self = Self();
  GetImplementation(mModelPrimitiveContainer[index]).RemovePrimitiveObserver(this);

  Dali::Renderer renderer = GetImplementation(mModelPrimitiveContainer[index]).GetRenderer();
  if(renderer)
  {
    self.RemoveRenderer(renderer);
  }

  mModelPrimitiveContainer.erase(mModelPrimitiveContainer.begin() + index);
}

Dali::Scene3D::ModelPrimitive ModelNode::GetModelPrimitive(uint32_t index) const
{
  if(index < mModelPrimitiveContainer.size())
  {
    return mModelPrimitiveContainer[index];
  }
  return Scene3D::ModelPrimitive();
}

Scene3D::ModelNode ModelNode::FindChildModelNodeByName(std::string_view nodeName)
{
  Actor childActor = Self().FindChildByName(nodeName);
  return Scene3D::ModelNode::DownCast(childActor);
}

void ModelNode::SetImageBasedLightTexture(Dali::Texture diffuseTexture, Dali::Texture specularTexture, float iblScaleFactor, uint32_t specularMipmapLevels)
{
  mDiffuseTexture       = diffuseTexture;
  mSpecularTexture      = specularTexture;
  mIblScaleFactor       = iblScaleFactor;
  mSpecularMipmapLevels = specularMipmapLevels;
  for(auto&& primitive : mModelPrimitiveContainer)
  {
    GetImplementation(primitive).SetImageBasedLightTexture(diffuseTexture, specularTexture, iblScaleFactor, specularMipmapLevels);
  }
}

void ModelNode::SetImageBasedLightScaleFactor(float iblScaleFactor)
{
  mIblScaleFactor = iblScaleFactor;
  for(auto&& primitive : mModelPrimitiveContainer)
  {
    GetImplementation(primitive).SetImageBasedLightScaleFactor(iblScaleFactor);
  }
}

void ModelNode::SetBlendShapeData(Scene3D::Loader::BlendShapes::BlendShapeData& data, Scene3D::ModelPrimitive primitive)
{
  GetImplementation(primitive).SetBlendShapeData(data);
}

void ModelNode::SetBoneMatrix(const Matrix& inverseMatrix, Scene3D::ModelPrimitive primitive, Scene3D::Loader::Index& boneIndex)
{
  Dali::Scene3D::Loader::Skinning::BoneData boneData;
  boneData.primitive = primitive;
  boneData.boneIndex = boneIndex;
  char propertyNameBuffer[32];
  snprintf(propertyNameBuffer, sizeof(propertyNameBuffer), "%s[%d]", Dali::Scene3D::Loader::Skinning::BONE_UNIFORM_NAME, boneIndex);
  boneData.propertyName  = propertyNameBuffer;
  boneData.inverseMatrix = inverseMatrix;
  mBoneDataContainer.push_back(std::move(boneData));

  UpdateBoneMatrix(primitive);
}

void ModelNode::OnRendererCreated(Renderer renderer)
{
  Self().AddRenderer(renderer);
}

void ModelNode::UpdateBoneMatrix(Scene3D::ModelPrimitive primitive)
{
  for(auto&& boneData : mBoneDataContainer)
  {
    if(boneData.primitive != primitive)
    {
      continue;
    }

    Dali::Renderer renderer = GetImplementation(primitive).GetRenderer();
    if(!renderer)
    {
      continue;
    }

    Dali::Shader shader = renderer.GetShader();
    if(!shader)
    {
      continue;
    }

    if(boneData.constraint)
    {
      boneData.constraint.Remove();
      boneData.constraint.Reset();
    }

    if(shader.GetPropertyIndex(boneData.propertyName) == Property::INVALID_INDEX)
    {
      auto propBoneXform = shader.RegisterProperty(boneData.propertyName, Matrix{false});

      Matrix inverseMatrix = boneData.inverseMatrix;
      // Constrain bone matrix to joint transform.
      boneData.constraint = Constraint::New<Matrix>(shader, propBoneXform, [inverseMatrix](Matrix& output, const PropertyInputContainer& inputs)
                                                    { Matrix::Multiply(output, inverseMatrix, inputs[0]->GetMatrix()); });

      Actor joint = Self();
      boneData.constraint.AddSource(Source{joint, Actor::Property::WORLD_MATRIX});
      boneData.constraint.ApplyPost();
    }
    break;
  }
}

} // namespace Internal

} // namespace Scene3D

} // namespace Dali