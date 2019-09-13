/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
 */

#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <dali-toolkit-test-suite-utils.h>
#include <toolkit-timer.h>
#include <toolkit-event-thread-callback.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/animated-vector-image-visual-actions-devel.h>
#include <dali-toolkit/devel-api/visuals/animated-vector-image-visual-signals-devel.h>
#include "dummy-control.h"

using namespace Dali;
using namespace Dali::Toolkit;

void dali_animated_vector_image_visual_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_animated_vector_image_visual_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

const char* TEST_VECTOR_IMAGE_FILE_NAME =  TEST_RESOURCE_DIR  "/insta_camera.json";

bool gAnimationFinishedSignalFired = false;

void VisualEventSignal( Control control, Dali::Property::Index visualIndex, Dali::Property::Index signalId )
{
  if( visualIndex == DummyControl::Property::TEST_VISUAL && signalId == DevelAnimatedVectorImageVisual::Signal::ANIMATION_FINISHED )
  {
    gAnimationFinishedSignalFired = true;
  }
}

}

int UtcDaliVisualFactoryGetAnimatedVectorImageVisual01(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetAnimatedVectorImageVisual01: Request animated vector image visual with a json url" );

  VisualFactory factory = VisualFactory::Get();
  Visual::Base visual = factory.CreateVisual( TEST_VECTOR_IMAGE_FILE_NAME, ImageDimensions() );
  DALI_TEST_CHECK( visual );

  DummyControl actor = DummyControl::New( true );
  DummyControlImpl& dummyImpl = static_cast< DummyControlImpl& >( actor.GetImplementation() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );
  actor.SetSize( 200.0f, 200.0f );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  // renderer is added to actor
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  Renderer renderer = actor.GetRendererAt( 0u );
  DALI_TEST_CHECK( renderer );

  // Test SetOffStage().
  actor.Unparent();
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliVisualFactoryGetAnimatedVectorImageVisual02(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetAnimatedVectorImageVisual02: Request animated vector image visual with a Property::Map" );

  Property::Map propertyMap;
  propertyMap.Add( Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE )
             .Add( ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME  );

  Visual::Base visual = VisualFactory::Get().CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  DummyControl actor = DummyControl::New( true );
  DummyControlImpl& dummyImpl = static_cast< DummyControlImpl& >( actor.GetImplementation() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );
  actor.SetSize( 200.0f, 200.0f );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  // renderer is added to actor
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  Renderer renderer = actor.GetRendererAt( 0u );
  DALI_TEST_CHECK( renderer );

  actor.Unparent( );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliVisualFactoryGetAnimatedVectorImageVisual03(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetAnimatedVectorImageVisual03: Request animated vector image visual with a Property::Map" );

  int startFrame = 1, endFrame = 3;
  Property::Array playRange;
  playRange.PushBack( startFrame );
  playRange.PushBack( endFrame );

  Property::Map propertyMap;
  propertyMap.Add( Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE )
             .Add( ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME  )
             .Add( DevelImageVisual::Property::LOOP_COUNT, 3  )
             .Add( DevelImageVisual::Property::PLAY_RANGE, playRange  );

  Visual::Base visual = VisualFactory::Get().CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  DummyControl actor = DummyControl::New( true );
  DummyControlImpl& dummyImpl = static_cast< DummyControlImpl& >( actor.GetImplementation() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );
  actor.SetSize( 200.0f, 200.0f );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  // renderer is added to actor
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  Renderer renderer = actor.GetRendererAt( 0u );
  DALI_TEST_CHECK( renderer );

  actor.Unparent( );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliVisualFactoryGetAnimatedVectorImageVisual04(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetAnimatedVectorImageVisual04: Request animated vector image visual with a Property::Map" );

  int startFrame = 1, endFrame = 3;
  Property::Array playRange;
  playRange.PushBack( startFrame );
  playRange.PushBack( endFrame );

  Property::Map propertyMap;
  propertyMap.Add( "visualType", DevelVisual::ANIMATED_VECTOR_IMAGE )
             .Add( "url", TEST_VECTOR_IMAGE_FILE_NAME )
             .Add( "loopCount", 3 )
             .Add( "playRange", playRange )
             .Add( "stopBehavior", DevelImageVisual::StopBehavior::FIRST_FRAME )
             .Add( "loopingMode", DevelImageVisual::LoopingMode::AUTO_REVERSE );

  Visual::Base visual = VisualFactory::Get().CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  DummyControl actor = DummyControl::New( true );
  DummyControlImpl& dummyImpl = static_cast< DummyControlImpl& >( actor.GetImplementation() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );
  actor.SetSize( 200.0f, 200.0f );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  // renderer is added to actor
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  Renderer renderer = actor.GetRendererAt( 0u );
  DALI_TEST_CHECK( renderer );

  Property::Map resultMap;
  visual.CreatePropertyMap( resultMap );

  // check the property values from the returned map from a visual
  Property::Value* value = resultMap.Find( ImageVisual::Property::URL, Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get< std::string >() == TEST_VECTOR_IMAGE_FILE_NAME );

  value = resultMap.Find( DevelImageVisual::Property::LOOP_COUNT, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get< int >() == 3 );

  value = resultMap.Find( DevelImageVisual::Property::PLAY_RANGE, Property::ARRAY );
  DALI_TEST_CHECK( value );

  Property::Array* result = value->GetArray();
  DALI_TEST_CHECK( result );

  DALI_TEST_CHECK( result->GetElementAt( 0 ).Get< int >() == startFrame );
  DALI_TEST_CHECK( result->GetElementAt( 1 ).Get< int >() == endFrame );

  value = resultMap.Find( DevelImageVisual::Property::STOP_BEHAVIOR, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get< int >() == DevelImageVisual::StopBehavior::FIRST_FRAME );

  value = resultMap.Find( DevelImageVisual::Property::LOOPING_MODE, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get< int >() == DevelImageVisual::LoopingMode::AUTO_REVERSE );

  actor.Unparent( );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualGetPropertyMap01(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliAnimatedVectorImageVisualGetPropertyMap01" );

  int startFrame = 1, endFrame = 3;
  Property::Array playRange;
  playRange.PushBack( startFrame );
  playRange.PushBack( endFrame );

  Property::Map propertyMap;
  propertyMap.Add( Toolkit::Visual::Property::TYPE,  DevelVisual::ANIMATED_VECTOR_IMAGE )
             .Add( ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME )
             .Add( DevelImageVisual::Property::LOOP_COUNT, 3 )
             .Add( DevelImageVisual::Property::PLAY_RANGE, playRange );

  // request AnimatedVectorImageVisual with a property map
  VisualFactory factory = VisualFactory::Get();
  Visual::Base visual = factory.CreateVisual( propertyMap );

  Property::Map resultMap;
  visual.CreatePropertyMap( resultMap );

  // check the property values from the returned map from a visual
  Property::Value* value = resultMap.Find( Toolkit::Visual::Property::TYPE, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get< int >() == DevelVisual::ANIMATED_VECTOR_IMAGE );

  value = resultMap.Find( ImageVisual::Property::URL, Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get< std::string >() == TEST_VECTOR_IMAGE_FILE_NAME );

  value = resultMap.Find( DevelImageVisual::Property::LOOP_COUNT, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get< int >() == 3 );

  value = resultMap.Find( DevelImageVisual::Property::PLAY_RANGE, Property::ARRAY );
  DALI_TEST_CHECK( value );

  Property::Array* result = value->GetArray();
  DALI_TEST_CHECK( result );

  DALI_TEST_CHECK( result->GetElementAt( 0 ).Get< int >() == startFrame );
  DALI_TEST_CHECK( result->GetElementAt( 1 ).Get< int >() == endFrame );

  value = resultMap.Find( DevelImageVisual::Property::STOP_BEHAVIOR, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get< int >() == DevelImageVisual::StopBehavior::CURRENT_FRAME );

  value = resultMap.Find( DevelImageVisual::Property::LOOPING_MODE, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get< int >() == DevelImageVisual::LoopingMode::RESTART );

  // request AnimatedVectorImageVisual with an URL
  Visual::Base visual2 = factory.CreateVisual( TEST_VECTOR_IMAGE_FILE_NAME, ImageDimensions() );

  resultMap.Clear();
  visual2.CreatePropertyMap( resultMap );

  // check the property values from the returned map from a visual
  value = resultMap.Find( Toolkit::Visual::Property::TYPE, Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get< int >() == DevelVisual::ANIMATED_VECTOR_IMAGE );

  value = resultMap.Find( ImageVisual::Property::URL, Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get< std::string >() == TEST_VECTOR_IMAGE_FILE_NAME );

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualPlayback(void)
{
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliAnimatedVectorImageVisualPlayback" );

  {
    // request AnimatedVectorImageVisual with a property map
    VisualFactory factory = VisualFactory::Get();
    Visual::Base visual = factory.CreateVisual(
      Property::Map()
      .Add( Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE )
      .Add( ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME ) );

    DummyControl dummyControl = DummyControl::New( true );
    Impl::DummyControl& dummyImpl = static_cast< Impl::DummyControl& >( dummyControl.GetImplementation() );
    dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );
    dummyControl.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

    Property::Map attributes;
    tet_infoline( "Test Play action" );
    DevelControl::DoAction( dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes );

    Stage::GetCurrent().Add( dummyControl );
    application.SendNotification();
    application.Render( 16 );

    std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );    // wait for one animation loop (16fps, 5frames, need 80ms)

    Property::Map map = dummyControl.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
    Property::Value* value = map.Find( DevelImageVisual::Property::PLAY_STATE );
    DALI_TEST_CHECK( value->Get< int >() == DevelImageVisual::PlayState::PLAYING );

    tet_infoline( "Test Pause action" );
    DevelControl::DoAction( dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PAUSE, attributes );

    application.SendNotification();
    application.Render(16);

    std::this_thread::sleep_for( std::chrono::milliseconds( 20 ) );    // wait for next rasterize thread run

    map = dummyControl.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
    value = map.Find( DevelImageVisual::Property::PLAY_STATE );
    DALI_TEST_CHECK( value->Get< int >() == DevelImageVisual::PlayState::PAUSED );

    tet_infoline( "Test Play action" );
    DevelControl::DoAction( dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes );

    application.SendNotification();
    application.Render(16);

    map = dummyControl.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
    value = map.Find( DevelImageVisual::Property::PLAY_STATE );
    DALI_TEST_CHECK( value->Get< int >() == DevelImageVisual::PlayState::PLAYING );

    tet_infoline( "Test Stop action" );
    DevelControl::DoAction( dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::STOP, attributes );

    application.SendNotification();
    application.Render(16);

    map = dummyControl.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
    value = map.Find( DevelImageVisual::Property::PLAY_STATE );
    DALI_TEST_CHECK( value->Get< int >() == DevelImageVisual::PlayState::STOPPED );

    tet_infoline( "Test Stop action again" );
    DevelControl::DoAction( dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::STOP, attributes );

    application.SendNotification();
    application.Render(16);

    map = dummyControl.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
    value = map.Find( DevelImageVisual::Property::PLAY_STATE );
    DALI_TEST_CHECK( value->Get< int >() == DevelImageVisual::PlayState::STOPPED );

    tet_infoline( "Test Play action" );
    DevelControl::DoAction( dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes );

    application.SendNotification();
    application.Render(16);

    map = dummyControl.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
    value = map.Find( DevelImageVisual::Property::PLAY_STATE );
    DALI_TEST_CHECK( value->Get< int >() == DevelImageVisual::PlayState::PLAYING );

    tet_infoline( "Off stage" );
    dummyControl.Unparent();

    application.SendNotification();
    application.Render(16);

    map = dummyControl.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
    value = map.Find( DevelImageVisual::Property::PLAY_STATE );
    DALI_TEST_CHECK( value->Get< int >() == DevelImageVisual::PlayState::PAUSED );

    tet_infoline( "On stage again" );
    Stage::GetCurrent().Add( dummyControl );

    application.SendNotification();
    application.Render(16);

    map = dummyControl.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
    value = map.Find( DevelImageVisual::Property::PLAY_STATE );
    DALI_TEST_CHECK( value->Get< int >() == DevelImageVisual::PlayState::PAUSED );

    tet_infoline( "Test Play action" );
    DevelControl::DoAction( dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes );

    application.SendNotification();
    application.Render(16);

    map = dummyControl.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
    value = map.Find( DevelImageVisual::Property::PLAY_STATE );
    DALI_TEST_CHECK( value->Get< int >() == DevelImageVisual::PlayState::PLAYING );

    // Change Size
    Vector3 newSize( 100.0f, 100.0f, 0.0f );
    dummyControl.SetSize( newSize );

    application.SendNotification();
    application.Render(16);

    // Size should be changed
    Vector3 naturalSize = dummyControl.GetNaturalSize();
    DALI_TEST_CHECK( naturalSize == newSize );

    dummyControl.Unparent();
  }

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualCustomShader(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliAnimatedVectorImageVisualCustomShader Test custom shader" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map properties;
  Property::Map shader;
  const std::string vertexShader = "Foobar";
  const std::string fragmentShader = "Foobar sampler2D Foobar";
  shader[Visual::Shader::Property::FRAGMENT_SHADER] = fragmentShader;
  shader[Visual::Shader::Property::VERTEX_SHADER] = vertexShader;

  properties[Visual::Property::TYPE] = Visual::IMAGE;
  properties[Visual::Property::SHADER] = shader;
  properties[ImageVisual::Property::URL] = TEST_VECTOR_IMAGE_FILE_NAME;

  Visual::Base visual = factory.CreateVisual( properties );

  // trigger creation through setting on stage
  DummyControl dummy = DummyControl::New( true );
  Impl::DummyControl& dummyImpl = static_cast< Impl::DummyControl& >( dummy.GetImplementation() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

  dummy.SetSize( 200.f, 200.f );
  dummy.SetParentOrigin( ParentOrigin::CENTER );
  Stage::GetCurrent().Add( dummy );

  application.SendNotification();
  application.Render();

  Renderer renderer = dummy.GetRendererAt( 0 );
  Shader shader2 = renderer.GetShader();
  Property::Value value = shader2.GetProperty( Shader::Property::PROGRAM );
  Property::Map* map = value.GetMap();
  DALI_TEST_CHECK( map );

  std::string resultFragmentShader, resultVertexShader;
  Property::Value* fragment = map->Find( "fragment" ); // fragment key name from shader-impl.cpp
  fragment->Get( resultFragmentShader );
  DALI_TEST_CHECK( resultFragmentShader.find( fragmentShader ) != std::string::npos );

  Property::Value* vertex = map->Find( "vertex" ); // vertex key name from shader-impl.cpp
  vertex->Get( resultVertexShader );
  DALI_TEST_CHECK( resultVertexShader.find( vertexShader ) != std::string::npos );

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualNaturalSize(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliAnimatedVectorImageVisualNaturalSize" );

  VisualFactory factory = VisualFactory::Get();
  Visual::Base visual = factory.CreateVisual( TEST_VECTOR_IMAGE_FILE_NAME, ImageDimensions() );
  DALI_TEST_CHECK( visual );

  DummyControl actor = DummyControl::New( true );
  DummyControlImpl& dummyImpl = static_cast< DummyControlImpl& >( actor.GetImplementation() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

  Vector2 controlSize( 20.f, 30.f );
  Vector2 naturalSize;

  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  visual.GetNaturalSize( naturalSize );

  DALI_TEST_EQUALS( naturalSize, Vector2( 100.0f, 100.0f ), TEST_LOCATION );    // 100x100 is the content default size.

  actor.SetSize( controlSize );

  application.SendNotification();
  application.Render();

  visual.GetNaturalSize( naturalSize );

  DALI_TEST_EQUALS( naturalSize, controlSize, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualLoopCount(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliAnimatedVectorImageVisualLoopCount" );

  Property::Map propertyMap;
  propertyMap.Add( Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE )
             .Add( ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME  )
             .Add( DevelImageVisual::Property::LOOP_COUNT, 3  );

  Visual::Base visual = VisualFactory::Get().CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  DummyControl actor = DummyControl::New( true );
  DummyControlImpl& dummyImpl = static_cast< DummyControlImpl& >( actor.GetImplementation() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

  Vector2 controlSize( 20.f, 30.f );
  actor.SetSize( controlSize );

  Stage::GetCurrent().Add( actor );

  Property::Map attributes;
  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes );

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - animation finished
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  // renderer is added to actor
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  Renderer renderer = actor.GetRendererAt( 0u );
  DALI_TEST_CHECK( renderer );

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualPlayRange(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliAnimatedVectorImageVisualPlayRange" );

  int startFrame = 3, endFrame = 1;
  Property::Array array;
  array.PushBack( startFrame );
  array.PushBack( endFrame );

  Property::Map propertyMap;
  propertyMap.Add( Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE )
             .Add( ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME  )
             .Add( DevelImageVisual::Property::PLAY_RANGE, array  );

  Visual::Base visual = VisualFactory::Get().CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  DummyControl actor = DummyControl::New( true );
  DummyControlImpl& dummyImpl = static_cast< DummyControlImpl& >( actor.GetImplementation() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

  Vector2 controlSize( 20.f, 30.f );
  actor.SetSize( controlSize );

  Stage::GetCurrent().Add( actor );

  Property::Map attributes;
  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes );

  application.SendNotification();
  application.Render();

  // renderer is added to actor
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  Renderer renderer = actor.GetRendererAt( 0u );
  DALI_TEST_CHECK( renderer );

  Property::Map map = actor.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
  Property::Value* value = map.Find( DevelImageVisual::Property::PLAY_RANGE );

  int resultStartFrame, resultEndFrame;
  Property::Array* result = value->GetArray();
  result->GetElementAt( 0 ).Get( resultStartFrame );
  result->GetElementAt( 1 ).Get( resultEndFrame );

  DALI_TEST_EQUALS( startFrame, resultStartFrame, TEST_LOCATION );
  DALI_TEST_EQUALS( endFrame, resultEndFrame, TEST_LOCATION );

  // Set invalid play range
  array.Clear();
  array.PushBack( -1 );
  array.PushBack( 100 );

  attributes.Clear();
  attributes.Add( DevelImageVisual::Property::PLAY_RANGE, array );
  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::UPDATE_PROPERTY, attributes );

  application.SendNotification();
  application.Render();

  map = actor.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
  value = map.Find( DevelImageVisual::Property::PLAY_RANGE );

  result = value->GetArray();
  result->GetElementAt( 0 ).Get( resultStartFrame );
  result->GetElementAt( 1 ).Get( resultEndFrame );

  DALI_TEST_EQUALS( startFrame, resultStartFrame, TEST_LOCATION );  // Should not be changed
  DALI_TEST_EQUALS( endFrame, resultEndFrame, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualAnimationFinishedSignal(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliAnimatedVectorImageVisualAnimationFinishedSignal" );

  Property::Map propertyMap;
  propertyMap.Add( Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE )
             .Add( ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME  )
             .Add( DevelImageVisual::Property::LOOP_COUNT, 3  );

  Visual::Base visual = VisualFactory::Get().CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  DummyControl actor = DummyControl::New( true );
  DummyControlImpl& dummyImpl = static_cast< DummyControlImpl& >( actor.GetImplementation() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

  DevelControl::VisualEventSignal( actor ).Connect( &VisualEventSignal );

  Vector2 controlSize( 20.f, 30.f );
  actor.SetSize( controlSize );

  Stage::GetCurrent().Add( actor );

  Property::Map attributes;
  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes );

  application.SendNotification();
  application.Render();

  // Wait for animation finish
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  Property::Map map = actor.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
  Property::Value* value = map.Find( DevelImageVisual::Property::PLAY_STATE );
  DALI_TEST_CHECK( value->Get< int >() == DevelImageVisual::PlayState::STOPPED );

  DALI_TEST_EQUALS( gAnimationFinishedSignalFired, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualJumpTo(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliAnimatedVectorImageVisualJumpTo" );

  Property::Map propertyMap;
  propertyMap.Add( Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE )
             .Add( ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME  );

  Visual::Base visual = VisualFactory::Get().CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  DummyControl actor = DummyControl::New( true );
  DummyControlImpl& dummyImpl = static_cast< DummyControlImpl& >( actor.GetImplementation() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

  Vector2 controlSize( 20.f, 30.f );
  actor.SetSize( controlSize );

  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::JUMP_TO, 2 );

  application.SendNotification();
  application.Render();

  Property::Map map = actor.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
  Property::Value* value = map.Find( DevelImageVisual::Property::CURRENT_FRAME_NUMBER );
  DALI_TEST_EQUALS( value->Get< int >(), 2, TEST_LOCATION );

  Property::Array array;
  array.PushBack( 0 );
  array.PushBack( 2 );

  Property::Map attributes;
  attributes.Add( DevelImageVisual::Property::PLAY_RANGE, array );
  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::UPDATE_PROPERTY, attributes );

  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::JUMP_TO, 3 );

  application.SendNotification();
  application.Render();

  map = actor.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
  value = map.Find( DevelImageVisual::Property::CURRENT_FRAME_NUMBER );
  DALI_TEST_EQUALS( value->Get< int >(), 2, TEST_LOCATION );

  // Change play range
  attributes.Clear();
  array.Clear();

  array.PushBack( 0 );
  array.PushBack( 4 );

  attributes.Add( DevelImageVisual::Property::PLAY_RANGE, array );
  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::UPDATE_PROPERTY, attributes );

  attributes.Clear();
  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes );

  application.SendNotification();
  application.Render();

  // Stop and jump to 3
  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::STOP, attributes );

  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::JUMP_TO, 3 );

  application.SendNotification();
  application.Render();

  map = actor.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
  value = map.Find( DevelImageVisual::Property::CURRENT_FRAME_NUMBER );
  DALI_TEST_EQUALS( value->Get< int >(), 3, TEST_LOCATION );

  // Jump to the same position
  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::JUMP_TO, 3 );

  application.SendNotification();
  application.Render();

  map = actor.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
  value = map.Find( DevelImageVisual::Property::CURRENT_FRAME_NUMBER );
  DALI_TEST_EQUALS( value->Get< int >(), 3, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualUpdateProperty(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliAnimatedVectorImageVisualJumpToCurrentProgress" );

  int startFrame = 1, endFrame = 3;
  Property::Array playRange;
  playRange.PushBack( startFrame );
  playRange.PushBack( endFrame );

  Property::Map propertyMap;
  propertyMap.Add( Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE )
             .Add( ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME  )
             .Add( DevelImageVisual::Property::LOOP_COUNT, 3  )
             .Add( DevelImageVisual::Property::PLAY_RANGE, playRange  );

  Visual::Base visual = VisualFactory::Get().CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  DummyControl actor = DummyControl::New( true );
  DummyControlImpl& dummyImpl = static_cast< DummyControlImpl& >( actor.GetImplementation() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

  Vector2 controlSize( 20.f, 30.f );
  actor.SetSize( controlSize );

  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  Property::Map map = actor.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
  Property::Value* value = map.Find( DevelImageVisual::Property::LOOP_COUNT );
  DALI_TEST_EQUALS( value->Get< int >(), 3, TEST_LOCATION );

  value = map.Find( DevelImageVisual::Property::PLAY_RANGE, Property::ARRAY );
  DALI_TEST_CHECK( value );

  Property::Array* result = value->GetArray();
  DALI_TEST_CHECK( result );

  DALI_TEST_CHECK( result->GetElementAt( 0 ).Get< int >() == startFrame );
  DALI_TEST_CHECK( result->GetElementAt( 1 ).Get< int >() == endFrame );

  playRange.Clear();
  playRange.PushBack( 0 );
  playRange.PushBack( 2 );

  Property::Map attributes;
  attributes.Add( DevelImageVisual::Property::PLAY_RANGE, playRange );
  attributes.Add( DevelImageVisual::Property::LOOP_COUNT, 5 );

  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::UPDATE_PROPERTY, attributes );

  application.SendNotification();
  application.Render();

  map = actor.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
  value = map.Find( DevelImageVisual::Property::LOOP_COUNT );
  DALI_TEST_EQUALS( value->Get< int >(), 5, TEST_LOCATION );

  value = map.Find( DevelImageVisual::Property::PLAY_RANGE );
  result = value->GetArray();
  DALI_TEST_CHECK( result );

  DALI_TEST_CHECK( result->GetElementAt( 0 ).Get< int >() == 0 );
  DALI_TEST_CHECK( result->GetElementAt( 1 ).Get< int >() == 2 );

  attributes.Clear();

  playRange.Clear();
  playRange.PushBack( startFrame );
  playRange.PushBack( endFrame );

  attributes.Add( DevelImageVisual::Property::PLAY_RANGE, playRange );

  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::UPDATE_PROPERTY, attributes );

  application.SendNotification();
  application.Render();

  map = actor.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
  value = map.Find( DevelImageVisual::Property::PLAY_RANGE );

  result = value->GetArray();
  DALI_TEST_CHECK( result );

  DALI_TEST_CHECK( result->GetElementAt( 0 ).Get< int >() == startFrame );
  DALI_TEST_CHECK( result->GetElementAt( 1 ).Get< int >() == endFrame );

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualStopBehavior(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliAnimatedVectorImageVisualStopBehavior" );

  Property::Map propertyMap;
  propertyMap.Add( Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE )
             .Add( ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME )
             .Add( DevelImageVisual::Property::LOOP_COUNT, 3 )
             .Add( DevelImageVisual::Property::STOP_BEHAVIOR, DevelImageVisual::StopBehavior::FIRST_FRAME );

  Visual::Base visual = VisualFactory::Get().CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  DummyControl actor = DummyControl::New( true );
  DummyControlImpl& dummyImpl = static_cast< DummyControlImpl& >( actor.GetImplementation() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

  Vector2 controlSize( 20.f, 30.f );
  actor.SetSize( controlSize );

  Stage::GetCurrent().Add( actor );

  Property::Map attributes;
  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes );

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - animation finished
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  Property::Map map = actor.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
  Property::Value* value = map.Find( DevelImageVisual::Property::CURRENT_FRAME_NUMBER );
  DALI_TEST_EQUALS( value->Get< int >(), 0, TEST_LOCATION );  // Should be the first frame

  // Change stop behavior
  attributes.Add( DevelImageVisual::Property::STOP_BEHAVIOR, DevelImageVisual::StopBehavior::LAST_FRAME );

  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::UPDATE_PROPERTY, attributes );

  attributes.Clear();

  // Play again
  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes );

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - animation finished
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  map = actor.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );

  Property::Value* value1 = map.Find( DevelImageVisual::Property::TOTAL_FRAME_NUMBER );
  int totalFrameNumber = value1->Get< int >();

  value = map.Find( DevelImageVisual::Property::CURRENT_FRAME_NUMBER );
  DALI_TEST_EQUALS( value->Get< int >(), totalFrameNumber - 1, TEST_LOCATION );  // Should be the last frame

  // Change stop behavior
  attributes.Add( DevelImageVisual::Property::STOP_BEHAVIOR, DevelImageVisual::StopBehavior::CURRENT_FRAME );
  attributes.Add( DevelImageVisual::Property::LOOP_COUNT, -1 );

  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::UPDATE_PROPERTY, attributes );

  attributes.Clear();

  // Play again
  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes );

  application.SendNotification();
  application.Render();

  // Pause
  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PAUSE, attributes );

  map = actor.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
  value = map.Find( DevelImageVisual::Property::CURRENT_FRAME_NUMBER );
  int currentFrameNumber = value->Get< int >();

  // Stop
  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::STOP, attributes );

  application.SendNotification();
  application.Render( 16 );

  map = actor.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
  value = map.Find( DevelImageVisual::Property::CURRENT_FRAME_NUMBER );
  DALI_TEST_EQUALS( value->Get< int >(), currentFrameNumber, TEST_LOCATION );  // Should be same with currentFrameNumber

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualLoopingMode(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliAnimatedVectorImageVisualLoopingMode" );

  Property::Map propertyMap;
  propertyMap.Add( Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE )
             .Add( ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME )
             .Add( DevelImageVisual::Property::LOOP_COUNT, 3 )
             .Add( DevelImageVisual::Property::STOP_BEHAVIOR, DevelImageVisual::StopBehavior::LAST_FRAME )
             .Add( DevelImageVisual::Property::LOOPING_MODE, DevelImageVisual::LoopingMode::AUTO_REVERSE );

  Visual::Base visual = VisualFactory::Get().CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  DummyControl actor = DummyControl::New( true );
  DummyControlImpl& dummyImpl = static_cast< DummyControlImpl& >( actor.GetImplementation() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

  Vector2 controlSize( 20.f, 30.f );
  actor.SetSize( controlSize );

  Stage::GetCurrent().Add( actor );

  Property::Map attributes;
  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes );

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - animation finished
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  Property::Map map = actor.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
  Property::Value* value = map.Find( DevelImageVisual::Property::CURRENT_FRAME_NUMBER );
  DALI_TEST_EQUALS( value->Get< int >(), 0, TEST_LOCATION );  // Should be the first frame because of auto reverse

  // Change stop behavior
  attributes.Add( DevelImageVisual::Property::STOP_BEHAVIOR, DevelImageVisual::StopBehavior::CURRENT_FRAME );

  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::UPDATE_PROPERTY, attributes );

  // Play again
  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes );

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - animation finished
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  map = actor.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
  value = map.Find( DevelImageVisual::Property::CURRENT_FRAME_NUMBER );
  DALI_TEST_EQUALS( value->Get< int >(), 0, TEST_LOCATION );  // Should be the first frame

  // Change looping mode
  attributes.Add( DevelImageVisual::Property::LOOPING_MODE, DevelImageVisual::LoopingMode::RESTART );

  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::UPDATE_PROPERTY, attributes );

  // Play again
  DevelControl::DoAction( actor, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedVectorImageVisual::Action::PLAY, attributes );

  application.SendNotification();
  application.Render();

  // Trigger count is 1 - animation finished
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  Property::Value* value1 = map.Find( DevelImageVisual::Property::TOTAL_FRAME_NUMBER );
  int totalFrameNumber = value1->Get< int >();

  map = actor.GetProperty< Property::Map >( DummyControl::Property::TEST_VISUAL );
  value = map.Find( DevelImageVisual::Property::CURRENT_FRAME_NUMBER );
  DALI_TEST_EQUALS( value->Get< int >(), totalFrameNumber - 1, TEST_LOCATION );  // Should be the last frame

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualPropertyNotification(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliAnimatedVectorImageVisualPropertyNotification" );

  Property::Map propertyMap;
  propertyMap.Add( Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE )
             .Add( ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME  );

  Visual::Base visual = VisualFactory::Get().CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  DummyControl actor = DummyControl::New( true );
  DummyControlImpl& dummyImpl = static_cast< DummyControlImpl& >( actor.GetImplementation() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

  Vector2 controlSize( 20.f, 30.f );
  Vector3 controlScale( 2.0f, 2.0f, 1.0f );
  actor.SetSize( controlSize );
  actor.SetScale( controlScale );

  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  application.SendNotification();
  application.Render();

  Renderer renderer = actor.GetRendererAt( 0u );
  DALI_TEST_CHECK( renderer );

  auto textureSet = renderer.GetTextures();
  auto texture = textureSet.GetTexture(0);

  DALI_TEST_EQUALS( controlSize.width * controlScale.width, texture.GetWidth(), TEST_LOCATION );
  DALI_TEST_EQUALS( controlSize.height * controlScale.height, texture.GetHeight(), TEST_LOCATION );

  // Change scale and size
  controlSize = Vector2( 50.f, 40.f );
  controlScale= Vector3( 0.5f, 0.5f, 1.0f );
  actor.SetSize( controlSize );
  actor.SetScale( controlScale );

  application.SendNotification();
  application.Render();

  application.SendNotification();
  application.Render();

  renderer = actor.GetRendererAt( 0u );
  DALI_TEST_CHECK( renderer );

  textureSet = renderer.GetTextures();
  texture = textureSet.GetTexture(0);

  DALI_TEST_EQUALS( controlSize.width * controlScale.width, texture.GetWidth(), TEST_LOCATION );
  DALI_TEST_EQUALS( controlSize.height * controlScale.height, texture.GetHeight(), TEST_LOCATION );

  END_TEST;
}
