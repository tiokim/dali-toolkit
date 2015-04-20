#ifndef __DALI_TOOLKIT_ATLAS_MANAGER_H__
#define __DALI_TOOLKIT_ATLAS_MANAGER_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <stdint.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/geometry/mesh-data.h>
#include <dali/public-api/images/atlas.h>
#include <dali/public-api/images/buffer-image.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{

class AtlasManager;

} // namespace Internal

/**
 * AtlasManager
 * ------------
 *
 * Creates and manages additions and removals of images from Texture Atlases
 *
 * The AtlasManager will match pixeltype and optimal block use to determine
 * the appropriate atlas to upload an image to.
 *
 * A policy can be specified to determine the action the AtlasManager will carry
 * out, should it not be able to add an image. This can return an error, or create
 * a new atlas of pre-determined dimensions to accomodate the new image.
 *
 * Images are referenced by an ImageId once they have been successfully uploaded.
 *
 * Once an image has been successfully uploaded, Geometry can be generated by passing
 * the ImageId to the GenerateMeshData method and geometry can be consolidated via
 * the StitchMesh method.
 *
 * Images are reference counted once mesh data has been generated. An image is removed
 * from the Atlas via the Remove( ImageId ) method. This unreferences the image and only
 * physically removes it from the atlas once all references have been freed.
 *
 * If the AddPolicy is set to generate and error if an image can't be uploaded, then it
 * is the applications responsibility to deal with the situation. An error will be indicated
 * with an ImageId of 0.
 *
 * Examples using the AtlasManager
 *
 * Create or obtain the AtlasManager
 * @code
 *
 * AtlasManager manager = AtlasManager::Get();
 *
 * @endcode
 *
 * Set the AtlasManager AddPolicy
 *
 * @code
 *
 * // Tell the atlas manager to create a new atlas, if it needs to
 * manager.SetAddPolicy( FAIL_ON_ADD_CREATES );
 *
 * // Tell the atlas manager to return an error, if it can't add an image
 * manager.SetAddPolicy( FAIL_ON_ADD_FAILS );
 *
 * @endcode
 *
 * Simple add and removal of BufferImage to and from an atlas
 *
 * @code
 *
 * // Structure returned by AtlasManager operations
 * AtlasSlot slot;
 *
 * // Add image to an atlas ( will be created if none suitable exists )
 * manager.Add( bitmapImage, slot );
 *
 * // slot.mImage returns the imageId for the bitmap, slot.mAtlas indicates the atlasId
 * // that the image was added to. The imageId is used to communicate with the AtlasManager
 * uint32_t imageId = slot.mImage;
 * if ( !imageId )
 * {
 *  // Addition has failed.....
 * }
 * ...
 * ...
 * // Done with image, so remove from atlas, if not being used elsewhere
 * manager.Remove( imageId );
 *
 * @endcode
 *
 * Create a Specific Atlas for adding BufferImages to
 *
 * @code
 *
 * // Create an RGB888 atlas of 2048x2848, with a blocksize of 128x128
 * uint32_t atlas = manager.CreateAtlas( 2048u, 2048u, 128u, 128u, Pixel::RGB888 );
 *
 * // Add an image to a preferred atlas ( note not specifying atlas may still result
 * // in the bitmap being added to the atlas above )
 * manager.Add( bitmapImage, slot, atlas );
 *
 * @endcode
 *
 * Create Geometry for a previously added image
 *
 * @code
 *
 * // Top left corner of geometry to be generated
 * Vector2 position( 1.0f, 1.0f );
 *
 * // Geometry will end up here!
 * MeshData meshData;
 * manager.GenerateMeshData( imageId, position, meshData );
 *
 * @endcode
 *
 * Generating Geometry from multiple images in the same atlas
 *
 * @code
 *
 * MeshData firstMesh;
 * MeshData secondMesh;
 * manager.GenerateMeshData( imageid_1, position_1, firstMesh );
 * manager.GenerateMeshData( imageid_2, position_2, secondMesh );
 *
 * // Combine the two meshes. Passing MESH_OPTIMIZE as an optional third parameter will remove duplicate vertices
 * manager.StitchMesh( first, second );
 *
 * @endcode
 *
 */

class AtlasManager : public BaseHandle
{
public:

  typedef uint32_t SizeType;
  typedef SizeType AtlasId;
  typedef SizeType ImageId;
  static const bool MESH_OPTIMIZE = true;

  struct AtlasSize
  {
    SizeType mWidth;              // width of the atlas in pixels
    SizeType mHeight;             // height of the atlas in pixels
    SizeType mBlockWidth;         // width of a block in pixels
    SizeType mBlockHeight;        // height of a block in pixels
  };

  /**
   * Metrics structures to describe Atlas Manager state
   *
   */
  struct AtlasMetricsEntry
  {
    AtlasSize mSize;                 // size of atlas and blocks
    SizeType mBlocksUsed;            // number of blocks used in the atlas
    SizeType mTotalBlocks;           // total blocks used by atlas
    Pixel::Format mPixelFormat;      // pixel format of the atlas
  };

  struct Metrics
  {
    SizeType mAtlasCount;                               // number of atlases
    SizeType mTextureMemoryUsed;                        // texture memory used by atlases
    Dali::Vector< AtlasMetricsEntry > mAtlasMetrics;    // container of atlas information
  };

  /**
   * Create an AtlasManager handle; this can be initialised with AtlasManager::New()
   * Calling member functions with an uninitialised handle is not allowed.
   */
  AtlasManager();

  /**
   * @brief Get new instance of AtlasManager object.
   *
   * @return A handle to the AtlasManager control.
   */
  static AtlasManager New();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~AtlasManager();

  /**
   * Policy on failing to add an image
   */
  enum AddFailPolicy
  {
    FAIL_ON_ADD_FAILS,
    FAIL_ON_ADD_CREATES
  };

  /**
   * @brief Container to hold result of placing texture into atlas
   */
  struct AtlasSlot
  {
    ImageId mImageId;                           // Id of stored Image
    AtlasId mAtlasId;                           // Id of Atlas containing this slot
  };

  typedef Dali::Vector< AtlasManager::AtlasSlot > slotContainer;

  /**
   * @brief Create a blank atlas of specific dimensions and pixel format with a certain block size
   *
   * @param[in] size desired atlas dimensions
   * @param[in] pixelformat format of a pixel in atlas
   *
   * @return atlas Id
   */
  AtlasId CreateAtlas( const AtlasSize& size, Pixel::Format pixelformat = Pixel::RGBA8888 );

  /**
   * @brief Set the policy on failure to add an image to an atlas
   *
   * @param policy policy to carry out if add fails
   */
  void SetAddPolicy( AddFailPolicy policy );

  /**
   * @brief Attempts to add an image to the most suitable atlas
   *
   * @details Add Policy may dictate that a new atlas is created if it can't presently be placed.
   *          If an add is made before an atlas is created under this policy,
   *          then a default size atlas will be created
   *
   * @param[in] image reference to a bitmapimage
   * @param[out] slot result of add operation
   * @param[in] atlas optional preferred atlas
   */
  void Add( const BufferImage& image,
            AtlasSlot& slot,
            AtlasId atlas = 0 );

  /**
   * @brief Remove previously added bitmapimage from atlas
   *
   * @param[in] id ImageId returned in the AtlasSlot from the add operation
   *
   * @return if true then image has been removed from the atlas
   */
  bool Remove( ImageId id );

  /**
   * @brief Generate mesh data for a previously added image
   *
   * @param[in] id Image Id returned in the AtlasSlot from the add operation
   * @param[in] position position of the resulting mesh in model space
   * @param[out] mesh Mesh Data Object to populate with mesh data
   */
  void GenerateMeshData( ImageId id,
                         const Vector2& position,
                         MeshData& mesh );

  /**
   * @brief Append second mesh to the first mesh
   *
   * @param[in] first First mesh
   * @param[in] second Second mesh
   * @param[in] optimize should we optimize vertex data
   */
  void StitchMesh( MeshData& first,
                   const MeshData& second,
                   bool optimize = false );

  /**
   * @brief Combine two meshes, outputting the result into a new mesh
   *
   * @param[in] first First mesh
   * @param[in] second Second mesh
   * @param[in] optimize should we optimize vertex data
   * @param[out] out resulting mesh
   */
  void StitchMesh( const MeshData& first,
                   const MeshData& second,
                   MeshData& out,
                   bool optimize = false );

  /**
   * @brief Get the BufferImage containing an atlas
   *
   * @param[in] atlas AtlasId returned when atlas was created
   *
   * @return Atlas Handle
   */
  Dali::Atlas GetAtlasContainer( AtlasId atlas ) const;

  /**
   * @brief Get the Id of the atlas containing an image
   *
   * @param[in] id ImageId
   *
   * @return Atlas Id
   */
  AtlasId GetAtlas( ImageId id );
  /**
   * @brief Get the current size of an atlas
   *
   * @param[in] atlas AtlasId
   *
   * @return AtlasSize structure for the atlas
   */
  const AtlasSize& GetAtlasSize( AtlasId atlas );

  /**
   * @brief Get the number of blocks available in an atlas
   *
   * @param[in] atlas AtlasId
   *
   * @return Number of blocks free in this atlas
   */
  SizeType GetFreeBlocks( AtlasId atlas );

  /**
   * @brief Sets the pixel area of any new atlas and also the individual block size
   *
   * @param[in] size Atlas size structure
   *
   * @param blockSize pixel area in atlas for a block
   */
  void SetNewAtlasSize( const AtlasSize& size );

  /**
   * @brief Get the number of atlases created
   *
   * @return number of atlases
   */
  SizeType GetAtlasCount() const;

  /**
   * @brief Get the pixel format used by an atlas
   *
   * @param[in] atlas AtlasId
   *
   * @return Pixel format used by this atlas
   */
  Pixel::Format GetPixelFormat( AtlasId atlas );

  /**
   * @brief Fill in a metrics structure showing current status of this Atlas Manager
   *
   * @param[in] metrics metrics structure to be filled
   */
  void GetMetrics( Metrics& metrics );

private:

  explicit DALI_INTERNAL AtlasManager(Internal::AtlasManager *impl);

};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_ATLAS_MANAGER_H__