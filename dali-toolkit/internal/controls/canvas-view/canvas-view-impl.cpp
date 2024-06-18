/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include "canvas-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/rendering/texture-devel.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
BaseHandle Create()
{
  return BaseHandle();
}
// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Toolkit::CanvasView, Toolkit::Control, Create);
DALI_PROPERTY_REGISTRATION(Toolkit, CanvasView, "viewBox", VECTOR2, VIEW_BOX)
DALI_PROPERTY_REGISTRATION(Toolkit, CanvasView, "synchronousLoading", BOOLEAN, SYNCHRONOUS_LOADING)
DALI_PROPERTY_REGISTRATION(Toolkit, CanvasView, "rasterizationRequestManually", BOOLEAN, RASTERIZATION_REQUEST_MANUALLY)
DALI_TYPE_REGISTRATION_END()
} // anonymous namespace

using namespace Dali;

CanvasView::CanvasView(const Vector2& viewBox)
: Control(ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT)),
  mCanvasRenderer(CanvasRenderer::New(viewBox)),
  mTexture(),
  mTextureSet(),
  mSize(viewBox),
  mIsSynchronous(true),
  mManualRasterization(false),
  mProcessorRegistered(false)
{
}

CanvasView::~CanvasView()
{
  if(Adaptor::IsAvailable() && mProcessorRegistered)
  {
    if(mRasterizingTask)
    {
      Dali::AsyncTaskManager::Get().RemoveTask(mRasterizingTask);
      mRasterizingTask.Reset();
    }
    Adaptor::Get().UnregisterProcessorOnce(*this, true);
  }
}

Toolkit::CanvasView CanvasView::New(const Vector2& viewBox)
{
  CanvasView* impl = new CanvasView(viewBox);

  Toolkit::CanvasView handle = Toolkit::CanvasView(*impl);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

/////////////////////////////////////////////////////////////

void CanvasView::OnInitialize()
{
  // CanvasView can relayout in the OnImageReady, alternative to a signal would be to have a upcall from the Control to CanvasView
  Dali::Toolkit::Control handle(GetOwner());

  Self().SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Accessibility::Role::IMAGE);

  // Request rasterization once at very first time.
  RequestRasterization();
}

void CanvasView::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  if(!mCanvasRenderer ||
     !mCanvasRenderer.SetSize(size))
  {
    return;
  }
  mSize = size;
}

void CanvasView::OnSizeSet(const Vector3& targetSize)
{
  Control::OnSizeSet(targetSize);

  if(!mCanvasRenderer ||
     !mCanvasRenderer.SetSize(Vector2(targetSize)))
  {
    return;
  }
  mSize.width  = targetSize.width;
  mSize.height = targetSize.height;
}

void CanvasView::SetProperty(BaseObject* object, Property::Index propertyIndex, const Property::Value& value)
{
  Toolkit::CanvasView canvasView = Toolkit::CanvasView::DownCast(Dali::BaseHandle(object));
  if(canvasView)
  {
    CanvasView& canvasViewImpl(GetImpl(canvasView));

    switch(propertyIndex)
    {
      case Toolkit::CanvasView::Property::VIEW_BOX:
      {
        Vector2 valueVector2;
        if(value.Get(valueVector2))
        {
          canvasViewImpl.SetViewBox(valueVector2);
        }
        break;
      }
      case Toolkit::CanvasView::Property::SYNCHRONOUS_LOADING:
      {
        bool isSynchronous;
        if(value.Get(isSynchronous))
        {
          canvasViewImpl.SetSynchronous(isSynchronous);
        }
        break;
      }
      case Toolkit::CanvasView::Property::RASTERIZATION_REQUEST_MANUALLY:
      {
        bool isRasterizationManually;
        if(value.Get(isRasterizationManually))
        {
          canvasViewImpl.SetRasterizationRequestManually(isRasterizationManually);
        }
        break;
      }
    }
  }
}

Property::Value CanvasView::GetProperty(BaseObject* object, Property::Index propertyIndex)
{
  Property::Value value;

  Toolkit::CanvasView canvasView = Toolkit::CanvasView::DownCast(Dali::BaseHandle(object));

  if(canvasView)
  {
    CanvasView& canvasViewImpl(GetImpl(canvasView));

    switch(propertyIndex)
    {
      case Toolkit::CanvasView::Property::VIEW_BOX:
      {
        value = canvasViewImpl.GetViewBox();
        break;
      }
      case Toolkit::CanvasView::Property::SYNCHRONOUS_LOADING:
      {
        value = canvasViewImpl.IsSynchronous();
        break;
      }
      case Toolkit::CanvasView::Property::RASTERIZATION_REQUEST_MANUALLY:
      {
        value = canvasViewImpl.IsRasterizationRequestManually();
        break;
      }
    }
  }
  return value;
}

void CanvasView::Process(bool postProcessor)
{
  mProcessorRegistered = false;

  if(mCanvasRenderer && mCanvasRenderer.IsCanvasChanged() && mSize.width > 0 && mSize.height > 0)
  {
    AddRasterizationTask();
  }

  // If we are not doing manual rasterization, register processor once again.
  // TODO : Could we reqest it only if IsCanvasChagned() is true?
  if(!mManualRasterization)
  {
    RequestRasterization();
  }
}

void CanvasView::AddRasterizationTask()
{
  if(mCanvasRenderer && mCanvasRenderer.Commit())
  {
    if(mIsSynchronous)
    {
      CanvasRendererRasterizingTaskPtr rasterizingTask = new CanvasRendererRasterizingTask(mCanvasRenderer, MakeCallback(this, &CanvasView::ApplyRasterizedImage));
      rasterizingTask->Process();
      ApplyRasterizedImage(rasterizingTask);
      rasterizingTask.Reset(); // We don't need it anymore.
    }
    else
    {
      if(!mRasterizingTask)
      {
        mRasterizingTask = new CanvasRendererRasterizingTask(mCanvasRenderer, MakeCallback(this, &CanvasView::ApplyRasterizedImage));
        AsyncTaskManager::Get().AddTask(mRasterizingTask);
      }
    }
  }
}

void CanvasView::ApplyRasterizedImage(CanvasRendererRasterizingTaskPtr task)
{
  if(task->IsRasterized())
  {
    Texture rasterizedTexture = task->GetRasterizedTexture();
    if(rasterizedTexture && rasterizedTexture.GetWidth() != 0 && rasterizedTexture.GetHeight() != 0)
    {
      if(!mTextureSet)
      {
        std::string fragmentShader = SHADER_CANVAS_VIEW_FRAG.data();
        DevelTexture::ApplyNativeFragmentShader(rasterizedTexture, fragmentShader);

        mTextureSet       = TextureSet::New();
        Geometry geometry = VisualFactoryCache::CreateQuadGeometry();
        Shader   shader   = Shader::New(SHADER_CANVAS_VIEW_VERT, fragmentShader, Shader::Hint::NONE, "CANVAS_VIEW");
        Renderer renderer = Renderer::New(geometry, shader);

        renderer.SetTextures(mTextureSet);
        renderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true);
        Self().AddRenderer(renderer);
      }
      mTextureSet.SetTexture(0, rasterizedTexture);
    }
  }

  if(task == mRasterizingTask)
  {
    mRasterizingTask.Reset(); // We don't need it anymore
  }

  //If there are accumulated changes to CanvasRenderer during Rasterize, Rasterize once again.
  if(!mIsSynchronous && !mManualRasterization && mCanvasRenderer && mCanvasRenderer.IsCanvasChanged())
  {
    AddRasterizationTask();
  }
}

bool CanvasView::AddDrawable(Dali::CanvasRenderer::Drawable& drawable)
{
  if(mCanvasRenderer && mCanvasRenderer.AddDrawable(drawable))
  {
    return true;
  }
  return false;
}

bool CanvasView::RemoveDrawable(Dali::CanvasRenderer::Drawable& drawable)
{
  if(mCanvasRenderer && mCanvasRenderer.RemoveDrawable(drawable))
  {
    return true;
  }
  return false;
}

bool CanvasView::RemoveAllDrawables()
{
  if(mCanvasRenderer && mCanvasRenderer.RemoveAllDrawables())
  {
    return true;
  }
  return false;
}

void CanvasView::RequestRasterization()
{
  if(!mProcessorRegistered && Adaptor::IsAvailable())
  {
    mProcessorRegistered = true;
    Adaptor::Get().RegisterProcessorOnce(*this, true);
  }
}

bool CanvasView::SetViewBox(const Vector2& viewBox)
{
  if(mCanvasRenderer && mCanvasRenderer.SetViewBox(viewBox))
  {
    return true;
  }
  return false;
}

const Vector2& CanvasView::GetViewBox()
{
  if(mCanvasRenderer)
  {
    return mCanvasRenderer.GetViewBox();
  }
  return Vector2::ZERO;
}

void CanvasView::SetSynchronous(const bool isSynchronous)
{
  mIsSynchronous = isSynchronous;
}

const bool CanvasView::IsSynchronous() const
{
  return mIsSynchronous;
}

void CanvasView::SetRasterizationRequestManually(const bool isRasterizationManually)
{
  if(mManualRasterization != isRasterizationManually)
  {
    mManualRasterization = isRasterizationManually;
    if(!mManualRasterization)
    {
      RequestRasterization();
    }
  }
}

const bool CanvasView::IsRasterizationRequestManually() const
{
  return mManualRasterization;
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
