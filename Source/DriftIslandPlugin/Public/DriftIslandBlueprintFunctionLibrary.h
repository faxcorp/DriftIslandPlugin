// f4x 2023

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Components/SplineComponent.h"
#include "CoreMinimal.h"
#include "DriftIslandBlueprintFunctionLibrary.generated.h"

UCLASS()
class UDriftIslandBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "DriftIsland", meta = (DisplayName = "Destroy All Components Except"))
    static void DestroyAllComponentsExcept(AActor *Actor, TArray<USceneComponent *> ComponentsToKeep);

    UFUNCTION(BlueprintPure, Category = "DriftIsland", meta = (DisplayName = "Filter Actors By Name"))
    static TArray<AActor *> FilterActorsByName(const TArray<AActor *> &Actors, FString NameToMatch);

    UFUNCTION(BlueprintCallable, Category = "DriftIsland", meta = (DisplayName = "Replace Spline Points"))
    static void ReplaceSplinePoints(USplineComponent *SplineToCopy, USplineComponent *SplineToClear, bool bMatchWorldLocation);

    UFUNCTION(BlueprintCallable, Category = "DriftIsland", meta = (DisplayName = "Get Distance Along Spline At World Location"))
    static float GetDistanceAlongSplineAtWorldLocation(const USplineComponent *InSpline, const FVector InWorldLocation);

    UFUNCTION(BlueprintCallable, Category = "DriftIsland", meta = (DisplayName = "Find Nearest Actor To Location"))
    static AActor *FindNearestActorToLocation(const TArray<AActor *> &Actors, const FVector &Location);

    UFUNCTION(BlueprintCallable, Category = "DriftIsland", meta = (DisplayName = "Reverse Splines To Look Towards Road"))
    static void ReverseSplinesToLookTowardsRoad(const TArray<AActor *> &SplinesToReverse, const TArray<AActor *> &SplinesToLookAt, bool ReverseAll);

    UFUNCTION(BlueprintCallable, Category = "DriftIsland", meta = (DisplayName = "Is Within Distance Of Positions"))
    static bool IsWithinDistanceOfPositions(const TArray<FVector> &Positions, float Distance, const FVector &Location);
};
