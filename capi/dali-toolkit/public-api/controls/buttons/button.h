#ifndef __DALI_TOOLKIT_BUTTON_H__
#define __DALI_TOOLKIT_BUTTON_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class CustomActor;
}

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class Button;
}

/**
 * Button is a base class for different kind of buttons.
 * This class provides the dimmed property and the clicked signal.
 *
 * A ClickedSignal() is emitted when the button is touched and the touch
 * point doesn't leave the boundary of the button.
 *
 * When the \e dimmed property is set to \e true, no signal is emitted.
 */
class Button : public Control
{
public:

  // Signal Names
  static const char* const SIGNAL_CLICKED;

public:

  /**
   * Create an uninitialized Button. Only derived versions can be instantiated.
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  Button();

  /**
   * Copy constructor.
   */
  Button( const Button& button );

  /**
   * Assignment operator.
   */
  Button& operator=( const Button& button );

  /**
   * Downcast an Object handle to Button. If handle points to a Button the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a Button or an uninitialized handle
   */
  static Button DownCast( BaseHandle handle );

  /**
   * Virtual destructor.
   * Dali::Object derived classes typically do not contain member data.
   */
  virtual ~Button();

  /**
   * Sets the button as \e dimmed.
   *
   * No signals are emitted when the \e dimmed property is set.
   *
   * @param[in] dimmed property.
   */
  void SetDimmed( bool dimmed );

  /**
   * @return \e true if the button is \e dimmed.
   */
  bool IsDimmed() const;

  /**
   * Sets the animation time.
   * @param [in] animationTime The animation time in seconds.
   */
  void SetAnimationTime( float animationTime );

  /**
   * Retrieves button's animation time.
   * @return The animation time in seconds.
   */
  float GetAnimationTime() const;

public: //Signals

  // Button Clicked

  typedef SignalV2< bool ( Button ) > ClickedSignalV2;

  /**
   * Signal emitted when the button is touched and the touch point doesn't leave the boundary of the button.
   */
  ClickedSignalV2& ClickedSignal();

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  Button( Internal::Button& implementation );

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  Button( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_BUTTON_H__
