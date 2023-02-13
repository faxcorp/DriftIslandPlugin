CPP Spline Mesh Actor Function Nodes:

- CreateSplineMeshes is a BlueprintCallable function in the "DriftIsland" category of Unreal Engine. It takes in several inputs including a USplineComponent pointer InputSpline, an AActor pointer ParentActor, two arrays of UStaticMesh pointers MeshesForSplines and MeshesForPosts, and several integers and floats for various parameters. The function outputs two arrays of pointers OutStaticMeshesPosts and OutSplineMeshes of UStaticMeshComponent and USplineMeshComponent respectively.
This function creates spline meshes along a spline component InputSpline and a parent actor ParentActor. It uses the MeshesForSplines array for the spline meshes and MeshesForPosts array for the static meshes of the posts. The number of random posts is specified by NumRandomPosts. The CullDistanceMin and CullDistanceMax determine the minimum and maximum distances at which the meshes will be culled (not rendered).

- The OffsetSplineTowardsRightVector, TangentScale, RandomRoll, and Overlap determine the position and orientation of the spline meshes. The KnockOutWeight determines the amount by which the spline meshes will be offset. If StraightenPosts is true, the posts will be straightened, otherwise they will follow the spline. If OutputStaticMeshesOnSplines is true, the static meshes will be placed along the spline, otherwise they will be placed on the posts. If SnapToGround is true, the static meshes will be snapped to the ground.

- SplitSplineByDistance is a BlueprintCallable static function in the "DriftIsland" category of Unreal Engine. It takes in several inputs including a reference to an array of USplineComponent pointers OutSplines, a USplineComponent pointer Spline, several floats for various parameters, and an AActor pointer ParentActor. The function returns a USplineComponent pointer.
This function splits a spline component Spline into several segments, specified by the distance DistanceF. The starting and ending points of the spline can be set using SplineStart and SplineEnd. If SetPointsToLinear is true, the points of the spline will be set to linear, otherwise they will be set to curved. If NewSplineForEachSegment is true, a new spline will be created for each segment, otherwise a single spline will be created for all segments. The TangentScale, RandomRoll, and RandomPositionRadius determine the position and orientation of the spline. The parent actor of the spline can be set using the ParentActor pointer. The resulting splines are stored in the OutSplines array.


Blueprint Library Function Nodes: 

- DestroyAllComponentsExcept: This function is a blueprint callable function that belongs to the "DriftIsland" category. It has a display name of "Destroy All Components Except". This function takes an actor and a list of scene components as inputs and destroys all of the components belonging to the actor except for the components specified in the list.

- FilterActorsByName: This function is a blueprint pure function that belongs to the "DriftIsland" category. It has a display name of "Filter Actors By Name". This function takes a list of actors and a name to match as inputs and returns a filtered list of actors whose names match the specified name.

- ReplaceSplinePoints: This function is a blueprint callable function that belongs to the "DriftIsland" category. It has a display name of "Replace Spline Points". This function takes two spline components and a boolean value as inputs. It replaces the points of the second spline component with the points of the first spline component and can optionally match the world location.

- GetDistanceAlongSplineAtWorldLocation: This function is a blueprint callable function that belongs to the "DriftIsland" category. It has a display name of "Get Distance Along Spline At World Location". This function takes a spline component and a world location as inputs and returns the distance along the spline at the specified world location.

- FindNearestActorToLocation: This function is a blueprint callable function that belongs to the "DriftIsland" category. It has a display name of "Find Nearest Actor To Location". This function takes a list of actors and a location as inputs and returns the actor closest to the specified location.

- ReverseSplinesToLookTowardsRoad: This function is a blueprint callable function that belongs to the "DriftIsland" category. It has a display name of "Reverse Splines To Look Towards Road". This function takes two lists of splines as inputs and reverses the direction of the splines in the first list so that they face towards the splines in the second list. The function also has a boolean input that determines whether to reverse all splines in the first list.

- IsWithinDistanceOfPositions: This function is a blueprint callable function that belongs to the "DriftIsland" category. It has a display name of "Is Within Distance Of Positions". This function takes a list of positions, a distance, and a location as inputs and returns a boolean value indicating whether the specified location is within the specified distance of any of the positions in the list.


=========================================
=========================================
Tutorial



To see how to use the blueprints, open up example map from the plugins content folder

![image](https://user-images.githubusercontent.com/37246339/218529202-2f69b2cf-755e-4da0-866d-8542fbb8354f.png)

The splines to generate stuff from are placed into the level (name is important as the procedural spawners will use the name to find it)

![image](https://user-images.githubusercontent.com/37246339/218529422-c2ee1b73-51a1-4c26-9b5c-e191c297f219.png)

BP_IslandMultiSplineMeshMakerCPP is the actor that you place into the level that has information about the spawner

![image](https://user-images.githubusercontent.com/37246339/218529613-e51e8ccd-3775-407c-b02e-a80b41fff518.png)

IslandManager actor handles the spawning (has to keep track of splines used so multiple objects dont get generated on top of each other)

![image](https://user-images.githubusercontent.com/37246339/218529846-e9a33a11-e75b-4955-8998-d05769fe59af.png)


