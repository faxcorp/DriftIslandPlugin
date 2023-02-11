// f4x 2023

#include "DriftIslandBlueprintFunctionLibrary.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Algo/Reverse.h"
#include "MeshVertexPainter/MeshVertexPainter.h"
#include "Components.h"
#include "StaticMeshResources.h"
#include "Rendering/PositionVertexBuffer.h"
#include "Engine/StaticMesh.h"

void UDriftIslandBlueprintFunctionLibrary::DestroyAllComponentsExcept(AActor *Actor, TArray<USceneComponent *> ComponentsToKeep)
{
    TArray<USceneComponent *> Components;
    Actor->GetComponents<USceneComponent>(Components);
    for (USceneComponent *Component : Components)
    {
        if (!ComponentsToKeep.Contains(Component))
        {

            Component->UnregisterComponent();
            Component->DestroyComponent();
        }
    }
}

TArray<AActor *> UDriftIslandBlueprintFunctionLibrary::FilterActorsByName(const TArray<AActor *> &Actors, FString NameToMatch)
{
    // return null if the array is empty
    if (Actors.Num() == 0)
    {
        return TArray<AActor *>();
    }

    TArray<AActor *> FilteredActors;
    
    for (AActor *Actor : Actors)
    {
        if (Actor->GetName().Contains(NameToMatch))
        {
            FilteredActors.Add(Actor);
        }
    }
    return FilteredActors;
}

void UDriftIslandBlueprintFunctionLibrary::ReplaceSplinePoints(USplineComponent *SplineToCopy, USplineComponent *SplineToClear, bool bMatchWorldLocation)
{
    SplineToClear->ClearSplinePoints(true);

    int32 NumberOfPoints = SplineToCopy->GetNumberOfSplinePoints();
    for (int i = 0; i < NumberOfPoints; i++)
    {
        FVector Location = SplineToCopy->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
        FVector Tangent = SplineToCopy->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
        SplineToClear->AddSplinePoint(Location, ESplineCoordinateSpace::Local, true);
        SplineToClear->SetTangentAtSplinePoint(i, Tangent, ESplineCoordinateSpace::Local, true);
    }

    if (bMatchWorldLocation)
    {
        SplineToClear->SetWorldLocation(SplineToCopy->GetComponentLocation());
    }
}

float UDriftIslandBlueprintFunctionLibrary::GetDistanceAlongSplineAtWorldLocation(const USplineComponent *InSpline, const FVector InWorldLocation)

{

    const float InputKey = InSpline->FindInputKeyClosestToWorldLocation(InWorldLocation);

    return InSpline->GetDistanceAlongSplineAtSplineInputKey(InputKey);
}

AActor *UDriftIslandBlueprintFunctionLibrary::FindNearestActorToLocation(const TArray<AActor *> &Actors, const FVector &Location)
{
    AActor *NearestActor = nullptr;
    float NearestDistance = 99999999.0f;
    for (AActor *Actor : Actors)
    {
        float Distance = FVector::Distance(Location, Actor->GetActorLocation());
        if (Distance < NearestDistance)
        {
            NearestDistance = Distance;
            NearestActor = Actor;
        }
    }
    return NearestActor;
}

void UDriftIslandBlueprintFunctionLibrary::ReverseSplinesToLookTowardsRoad(const TArray<AActor *> &SplinesToReverse, const TArray<AActor *> &SplinesToLookAt, bool ReverseAll)
{

    if (SplinesToReverse.Num() == 0 || SplinesToLookAt.Num() == 0)
    {
        return;
    }

    else
    {

        {

            for (AActor *SplineToReverse : SplinesToReverse)
            {
                USplineComponent *CurrSpline = SplineToReverse->FindComponentByClass<USplineComponent>();

                int NumPoints = CurrSpline->GetNumberOfSplinePoints();
                int HalfNumPoints = NumPoints / 2;

                FVector PointLocation = CurrSpline->GetLocationAtSplinePoint(HalfNumPoints, ESplineCoordinateSpace::World);

                FVector PointRightVector = CurrSpline->GetRightVectorAtSplinePoint(HalfNumPoints, ESplineCoordinateSpace::World);

                TArray<AActor *> FiveClosestSplines;
                TArray<AActor *> SplinesToLookAtCopy = SplinesToLookAt;
                for (int i = 0; i < 5; i++)
                {

                    AActor *NearestActor = FindNearestActorToLocation(SplinesToLookAtCopy, PointLocation);

                    FiveClosestSplines.Add(NearestActor);

                    SplinesToLookAtCopy.Remove(NearestActor);
                }

                FVector ClosestWorldLocation;
                float ClosestDistance = 99999999.0f;
                for (AActor *Actor : FiveClosestSplines)
                {

                    USplineComponent *CurrSplineClosest = Actor->FindComponentByClass<USplineComponent>();

                    FVector ClosestLocation = CurrSplineClosest->FindLocationClosestToWorldLocation(PointLocation, ESplineCoordinateSpace::World);

                    float Distance = FVector::Distance(PointLocation, ClosestLocation);

                    if (Distance < ClosestDistance)
                    {
                        ClosestDistance = Distance;
                        ClosestWorldLocation = ClosestLocation;
                    }
                }

                float Dot = FVector::DotProduct(PointRightVector, (ClosestWorldLocation - PointLocation).GetSafeNormal());

                if (Dot < 0 || ReverseAll)

                {

                    CurrSpline->ComponentTags.Add("Reversed");
                }
                else
                {

                    CurrSpline->ComponentTags.RemoveAll([&](FName Tag)
                                                        { return Tag.ToString().Contains("Reversed"); });
                }
            }
        }
    }
}

// a function that takes positions array as an input, distance as an input, location as an input and ouputs a bool if the location is within the distance of any of the positions

bool UDriftIslandBlueprintFunctionLibrary::IsWithinDistanceOfPositions(const TArray<FVector> &Positions, float Distance, const FVector &Location)
{
    // check if the arrays are empty
    if (Positions.Num() == 0)
    {
        return false;
    }
    for (FVector Position : Positions)
    {
        if (FVector::Distance(Location, Position) < Distance)
        {
            return true;
        }
    }
    return false;
}