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

#include "dali-scene3d/public-api/algorithm/navigation-mesh.h"
#include "dali-scene3d/public-api/algorithm/path-finder.h"
#include "dali-scene3d/public-api/loader/navigation-mesh-factory.h"
#include <dali-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Scene3D::Algorithm;
using namespace Dali::Scene3D::Loader;

bool CompareResults( const std::vector<uint32_t>& nodes, const WayPointList& waypoints )
{
  if(nodes.size() != waypoints.size())
  {
    return false;
  }
  for(auto i = 0u; i < nodes.size(); ++i )
  {
    if(nodes[i] != waypoints[i].GetNavigationMeshFaceIndex())
    {
      return false;
    }
  }
  return true;
}

int UtcDaliPathFinderNewP(void)
{
  auto navmesh = NavigationMeshFactory::CreateFromFile( "resources/navmesh-test.bin");

  auto pathfinder = PathFinder::New( *navmesh, PathFinderAlgorithm::DEFAULT );

  DALI_TEST_CHECK(navmesh);
  DALI_TEST_CHECK(pathfinder);

  END_TEST;
}

int UtcDaliPathFinderNewFail(void)
{
  auto navmesh = NavigationMeshFactory::CreateFromFile( "resources/navmesh-test.bin");

  auto pathfinder = PathFinder::New( *navmesh, static_cast<PathFinderAlgorithm>(-1) );

  DALI_TEST_CHECK(navmesh);
  DALI_TEST_CHECK(!pathfinder);

  END_TEST;
}

void printWaypointForPython( WayPointList& waypoints)
{
  tet_printf( "size: %d\n", waypoints.size());
  tet_printf( "[");
  for(auto& wp : waypoints)
  {
    auto index = wp.GetNavigationMeshFaceIndex();
    tet_printf("%d, ", index);
  }
  tet_printf( "]");
}

int UtcDaliPathFinderDjikstraFindPath0(void)
{
  auto navmesh = NavigationMeshFactory::CreateFromFile( "resources/navmesh-test.bin");

  auto pathfinder = PathFinder::New( *navmesh, PathFinderAlgorithm::DJIKSTRA_SHORTEST_PATH );

  DALI_TEST_CHECK(navmesh);
  DALI_TEST_CHECK(pathfinder);

  {
    auto waypoints = pathfinder->FindPath(18, 139);
    DALI_TEST_NOT_EQUALS(int(waypoints.size()), 0, 0, TEST_LOCATION);

    // Results are verified in the Blender
    std::vector<uint32_t> expectedResults =
      {18, 97, 106, 82, 50, 139};

    DALI_TEST_EQUALS(CompareResults(expectedResults, waypoints), true, TEST_LOCATION);
  }
  //printWaypointForPython(waypoints);

  {
    // Top floor middle to the tree

    auto waypoints = pathfinder->FindPath(18, 157);
    DALI_TEST_NOT_EQUALS(int(waypoints.size()), 0, 0, TEST_LOCATION);

    //printWaypointForPython(waypoints);

    // Results are verified in the Blender
    std::vector<uint32_t> expectedResults =
      {18, 97, 106, 82, 50, 6, 89, 33, 157};

    DALI_TEST_EQUALS(CompareResults(expectedResults, waypoints), true, TEST_LOCATION);

  }

  END_TEST;
}

int UtcDaliPathFinderDjikstraFindPath1(void)
{
  auto navmesh = NavigationMeshFactory::CreateFromFile( "resources/navmesh-test.bin");
  // All coordinates in navmesh local space
  navmesh->SetSceneTransform( Matrix(Matrix::IDENTITY));

  auto pathfinder = PathFinder::New( *navmesh, PathFinderAlgorithm::DJIKSTRA_SHORTEST_PATH );

  DALI_TEST_CHECK(navmesh);
  DALI_TEST_CHECK(pathfinder);

  {
    Vector3 from(-6.0767, -1.7268, 0.1438); // ground floor
    Vector3 to(-6.0767, -1.7268, 4.287); // first floor

    auto waypoints = pathfinder->FindPath(from, to);
    DALI_TEST_NOT_EQUALS(int(waypoints.size()), 0, 0, TEST_LOCATION);

    // Results are verified in the Blender
    std::vector<uint32_t> expectedResults =
     {154, 58, 85, 106, 128, 132, 137};

    DALI_TEST_EQUALS(CompareResults(expectedResults, waypoints), true, TEST_LOCATION);

    // Verify last and first points by finding floor points
    {
      Vector3  verifyPos = Vector3::ZERO;
      uint32_t verifyIndex  = NavigationMesh::NULL_FACE;
      auto     result = navmesh->FindFloor(from, verifyPos, verifyIndex);

      DALI_TEST_EQUALS(result, true, TEST_LOCATION);
      DALI_TEST_EQUALS(verifyPos, waypoints[0].GetScenePosition(), TEST_LOCATION);
      DALI_TEST_EQUALS(verifyIndex, waypoints[0].GetNavigationMeshFaceIndex(), TEST_LOCATION);

      // Verified with Blender
      Vector2 local(1.064201f, -0.273200f);
      DALI_TEST_EQUALS(local, waypoints[0].GetFaceLocalSpacePosition(), TEST_LOCATION);
    }

    {
      Vector3  verifyPos = Vector3::ZERO;
      uint32_t verifyIndex  = NavigationMesh::NULL_FACE;
      auto     result = navmesh->FindFloor(to, verifyPos, verifyIndex);

      DALI_TEST_EQUALS(result, true, TEST_LOCATION);
      DALI_TEST_EQUALS(verifyPos, waypoints.back().GetScenePosition(), TEST_LOCATION);
      DALI_TEST_EQUALS(verifyIndex, waypoints.back().GetNavigationMeshFaceIndex(), TEST_LOCATION);

      // Verified with Blender
      Vector2 local(0.165907f, 0.142597f);
      DALI_TEST_EQUALS(local, waypoints.back().GetFaceLocalSpacePosition(), TEST_LOCATION);
    }
  }

  END_TEST;
}