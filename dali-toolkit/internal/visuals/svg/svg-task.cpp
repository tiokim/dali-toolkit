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
#include <dali-toolkit/internal/visuals/svg/svg-task.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/file-loader.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/svg/svg-visual.h>

#ifdef TRACE_ENABLED
#include <sstream>
#endif

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_IMAGE_PERFORMANCE_MARKER, false);
}

SvgTask::SvgTask(VectorImageRenderer vectorRenderer, CallbackBase* callback, AsyncTask::PriorityType priorityType)
: AsyncTask(callback, priorityType),
  mVectorRenderer(vectorRenderer),
  mHasSucceeded(false)
{
}

PixelData SvgTask::GetPixelData() const
{
  return PixelData();
}

bool SvgTask::HasSucceeded() const
{
  return mHasSucceeded;
}

VectorImageRenderer SvgTask::GetRenderer()
{
  return mVectorRenderer;
}

SvgLoadingTask::SvgLoadingTask(VectorImageRenderer vectorRenderer, const VisualUrl& url, EncodedImageBuffer encodedImageBuffer, float dpi, CallbackBase* callback)
: SvgTask(vectorRenderer, callback, url.GetProtocolType() == VisualUrl::ProtocolType::REMOTE ? AsyncTask::PriorityType::LOW : AsyncTask::PriorityType::HIGH),
  mImageUrl(url),
  mEncodedImageBuffer(encodedImageBuffer),
  mDpi(dpi)
{
}

SvgLoadingTask::~SvgLoadingTask()
{
}

void SvgLoadingTask::Process()
{
  if(mVectorRenderer.IsLoaded())
  {
    // Already loaded
    mHasSucceeded = true;
    return;
  }

#ifdef TRACE_ENABLED
  if(gTraceFilter && gTraceFilter->IsTraceEnabled())
  {
    std::ostringstream oss;
    oss << "[url:" << mImageUrl.GetEllipsedUrl() << "]";
    // DALI_TRACE_BEGIN(gTraceFilter, "DALI_SVG_LOADING_TASK"); ///< TODO : Open it if we can control trace log level
    DALI_LOG_RELEASE_INFO("BEGIN: DALI_SVG_LOADING_TASK %s", oss.str().c_str());
  }
#endif

  bool loadFailed = false;

  Dali::Vector<uint8_t> buffer;

  if(mEncodedImageBuffer)
  {
    // Copy raw buffer.
    // TODO : Can't we load svg without copy buffer in future?
    buffer = mEncodedImageBuffer.GetRawBuffer();

    // We don't need to hold image buffer anymore.
    mEncodedImageBuffer.Reset();
  }
  else if(mImageUrl.IsLocalResource())
  {
    if(!Dali::FileLoader::ReadFile(mImageUrl.GetUrl(), buffer))
    {
      DALI_LOG_ERROR("Failed to read file! [%s]\n", mImageUrl.GetUrl().c_str());
      loadFailed = true;
    }
  }
  else
  {
    if(!Dali::FileLoader::DownloadFileSynchronously(mImageUrl.GetUrl(), buffer))
    {
      DALI_LOG_ERROR("Failed to download file! [%s]\n", mImageUrl.GetUrl().c_str());
      loadFailed = true;
    }
  }

  if(!loadFailed)
  {
    buffer.Reserve(buffer.Count() + 1u);
    buffer.PushBack('\0');

    if(!mVectorRenderer.Load(buffer, mDpi))
    {
      DALI_LOG_ERROR("Failed to load data! [%s]\n", mImageUrl.GetUrl().c_str());
      loadFailed = true;
    }
  }

  mHasSucceeded = !loadFailed;
#ifdef TRACE_ENABLED
  if(gTraceFilter && gTraceFilter->IsTraceEnabled())
  {
    std::ostringstream oss;
    oss << "[success:" << mHasSucceeded << " ";
    oss << "url:" << mImageUrl.GetEllipsedUrl() << "]";
    // DALI_TRACE_END(gTraceFilter, "DALI_SVG_LOADING_TASK"); ///< TODO : Open it if we can control trace log level
    DALI_LOG_RELEASE_INFO("END: DALI_SVG_LOADING_TASK %s", oss.str().c_str());
  }
#endif
}

bool SvgLoadingTask::IsReady()
{
  return true;
}

SvgRasterizingTask::SvgRasterizingTask(VectorImageRenderer vectorRenderer, uint32_t width, uint32_t height, CallbackBase* callback)
: SvgTask(vectorRenderer, callback),
  mWidth(width),
  mHeight(height)
{
}

SvgRasterizingTask::~SvgRasterizingTask()
{
}

void SvgRasterizingTask::Process()
{
  if(!mVectorRenderer.IsLoaded())
  {
    DALI_LOG_ERROR("File is not loaded!\n");
    return;
  }

  DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_SVG_RASTERIZE_TASK", [&](std::ostringstream& oss) {
    oss << "[size:" << mWidth << "x" << mHeight << " ";
    oss << "url:" << mImageUrl.GetEllipsedUrl() << "]";
  });

  Devel::PixelBuffer pixelBuffer = mVectorRenderer.Rasterize(mWidth, mHeight);
  if(!pixelBuffer)
  {
    DALI_LOG_ERROR("Rasterize is failed!\n");
    DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_SVG_RASTERIZE_TASK", [&](std::ostringstream& oss) {
      oss << "[size:" << mWidth << "x" << mHeight << " ";
      oss << "url:" << mImageUrl.GetEllipsedUrl() << "]";
    });
    return;
  }

  mPixelData    = Devel::PixelBuffer::Convert(pixelBuffer);
  mHasSucceeded = true;

  DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_SVG_RASTERIZE_TASK", [&](std::ostringstream& oss) {
    oss << "[size:" << mWidth << "x" << mHeight << " ";
    oss << "url:" << mImageUrl.GetEllipsedUrl() << "]";
  });
}

bool SvgRasterizingTask::IsReady()
{
  return mVectorRenderer.IsLoaded();
}

PixelData SvgRasterizingTask::GetPixelData() const
{
  return mPixelData;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
