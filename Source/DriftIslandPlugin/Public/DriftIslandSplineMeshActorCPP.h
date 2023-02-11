// f4x 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DriftIslandBlueprintFunctionLibrary.h"
#include "DriftIslandSplineMeshActorCPP.generated.h"

UCLASS()
class DRIFTISLANDPLUGIN_API ADriftIslandSplineMeshActorCPP : public AActor
{
    GENERATED_BODY()
public:
    ADriftIslandSplineMeshActorCPP();

    UFUNCTION(BlueprintCallable, Category = "DriftIsland")
    void CreateSplineMeshes(USplineComponent *InputSpline, AActor *ParentActor, TArray<UStaticMesh *> MeshesForSplines, TArray<UStaticMesh *> MeshesForPosts, int NumRandomPosts, int CullDistanceMin, int CullDistanceMax, float OffsetSplineTowardsRightVector, float TangentScale, float RandomRoll, float Overlap, float KnockOutWeight, bool StraightenPosts, bool OutputStaticMeshesOnSplines, bool SnapToGround, TArray<UStaticMeshComponent *> &OutStaticMeshesPosts, TArray<USplineMeshComponent *> &OutSplineMeshes);

    UFUNCTION(BlueprintCallable, Category = "DriftIsland")
    static USplineComponent *SplitSplineByDistance(TArray<USplineComponent *> &OutSplines, USplineComponent *Spline, float DistanceF = 250.0f, float SplineStart = 0.0f, float SplineEnd = 1.0f, bool SetPointsToLinear = false, bool NewSplineForEachSegment = false, float TangentScale = 1.0f, float RandomRoll = 0.0f, float RandomPositionRadius = 0.0f, AActor *ParentActor = nullptr);
};
