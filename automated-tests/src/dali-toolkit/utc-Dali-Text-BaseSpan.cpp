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

#include <stdlib.h>
#include <unistd.h>
#include <iostream>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/text/spans/foreground-color-span.h>

using namespace Dali;
using namespace Toolkit;

int UtcDaliToolkitTextBaseSpanDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextBaseSpanDownCast");

  BaseHandle baseHandel = Text::ForegroundColorSpan::New(Color::GREEN);
  DALI_TEST_CHECK(baseHandel);

  Text::BaseSpan baseSpan = Text::BaseSpan::DownCast(baseHandel);
  DALI_TEST_CHECK(baseSpan);

  END_TEST;
}
