// f4x 2023


#include "DriftIslandSplineMeshActorCPP.h"
#include "DriftIslandBlueprintFunctionLibrary.h"
#include "Components/SplineMeshComponent.h"
#include "MeshVertexPainter/MeshVertexPainter.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"



ADriftIslandSplineMeshActorCPP::ADriftIslandSplineMeshActorCPP()
{

	PrimaryActorTick.bCanEverTick = false;
}

void ADriftIslandSplineMeshActorCPP::CreateSplineMeshes(USplineComponent* InputSpline, AActor* ParentActor, TArray<UStaticMesh*> MeshesForSplines, TArray<UStaticMesh*> MeshesForPosts, int NumRandomPosts, int CullDistanceMin, int CullDistanceMax, float OffsetSplineTowardsRightVector, float TangentScale, float RandomRoll, float Overlap, float KnockOutWeight, bool StraightenPosts, bool OutputStaticMeshesOnSplines, bool SnapToGround, TArray<UStaticMeshComponent*>& OutStaticMeshesPosts, TArray<USplineMeshComponent*>& OutSplineMeshes)
{

	if (!InputSpline || InputSpline->GetNumberOfSplinePoints() < 2)
	{
		return;
	}

	if (MeshesForSplines.Num() != 0)
	{
		InputSpline->SetUnselectedSplineSegmentColor(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f));
		bool IsReversed = false;
		if (InputSpline->ComponentHasTag("Reversed"))

		{
			IsReversed = true;
			TArray<FVector> SplinePointsLocations;
			for (int i = 0; i < InputSpline->GetNumberOfSplinePoints(); i++)
			{
				SplinePointsLocations.Add(InputSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World));
			}
			TArray<FVector> SplinePointsUpVectors;
			for (int i = 0; i < InputSpline->GetNumberOfSplinePoints(); i++)
			{
				SplinePointsUpVectors.Add(InputSpline->GetUpVectorAtSplinePoint(i, ESplineCoordinateSpace::World));
			}
			USplineComponent* ReversedSpline = NewObject<USplineComponent>(ParentActor, "ReversedSpline");
			ReversedSpline->SetMobility(EComponentMobility::Movable);
			ReversedSpline->RegisterComponent();
			ReversedSpline->ClearSplinePoints(true);
			for (int ii = SplinePointsLocations.Num() - 1; ii >= 0; ii--)
			{
				ReversedSpline->AddSplinePoint(SplinePointsLocations[ii], ESplineCoordinateSpace::World, true);
			}
			for (int iii = SplinePointsUpVectors.Num() - 1; iii >= 0; iii--)
			{
				ReversedSpline->SetUpVectorAtSplinePoint(iii, SplinePointsUpVectors[iii], ESplineCoordinateSpace::World);
			}
			ReversedSpline->UpdateSpline();
			InputSpline = ReversedSpline;
			InputSpline->AddWorldOffset(FVector(0.0f, 0.0f, 1.0f));
			InputSpline->SetUnselectedSplineSegmentColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
		}

		for (int i = 0; i < InputSpline->GetNumberOfSplinePoints() - 1; i++)
		{
			float NormalizedTime = i / (float)(InputSpline->GetNumberOfSplinePoints() - 1);
			float NormalizedTimeNextPoint = FMath::Clamp(i + 1 / (float)(InputSpline->GetNumberOfSplinePoints() - 1), 0.0f, 1.0f);
			FVector StartRightVector = InputSpline->GetRightVectorAtTime(NormalizedTime, ESplineCoordinateSpace::World);
			FVector EndRightVector = InputSpline->GetRightVectorAtTime(NormalizedTimeNextPoint, ESplineCoordinateSpace::World);
			StartRightVector = FVector(StartRightVector.X, StartRightVector.Y, 0.0f).GetSafeNormal();
			EndRightVector = FVector(EndRightVector.X, EndRightVector.Y, 0.0f).GetSafeNormal();
			FVector StartPosition = InputSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
			FVector EndPosition = InputSpline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::World);
			StartPosition += StartRightVector * OffsetSplineTowardsRightVector;
			EndPosition += EndRightVector * OffsetSplineTowardsRightVector;
			if (SnapToGround)
			{
				FVector StartPositionTrace = InputSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
				FVector EndPositionTrace = InputSpline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::World);
				FVector StartRightVectorTrace = InputSpline->GetRightVectorAtSplinePoint(i, ESplineCoordinateSpace::World);
				FVector EndRightVectorTrace = InputSpline->GetRightVectorAtSplinePoint(i + 1, ESplineCoordinateSpace::World);
				StartRightVectorTrace = FVector(StartRightVectorTrace.X, StartRightVectorTrace.Y, 0.0f).GetSafeNormal();
				EndRightVectorTrace = FVector(EndRightVectorTrace.X, EndRightVectorTrace.Y, 0.0f).GetSafeNormal();
				StartPositionTrace += StartRightVectorTrace * OffsetSplineTowardsRightVector;
				EndPositionTrace += EndRightVectorTrace * OffsetSplineTowardsRightVector;
				FHitResult HitResult;

				if (UWorld* World = GetWorld())
				{
					if (World->LineTraceSingleByObjectType(HitResult, StartPositionTrace + FVector(0.0f, 0.0f, 10000.0f), StartPositionTrace - FVector(0.0f, 0.0f, 10000.0f), FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic), FCollisionQueryParams(FName("SnapToGround"), true, ParentActor)))
					{
						StartPosition = HitResult.ImpactPoint;
					}

					if (World->LineTraceSingleByObjectType(HitResult, EndPositionTrace + FVector(0.0f, 0.0f, 10000.0f), EndPositionTrace - FVector(0.0f, 0.0f, 10000.0f), FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic), FCollisionQueryParams(FName("SnapToGround"), true, ParentActor)))
					{
						EndPosition = HitResult.ImpactPoint;
					}
				}
			}
			float StartRoll = InputSpline->GetRollAtSplinePoint(i, ESplineCoordinateSpace::World);
			float EndRoll = InputSpline->GetRollAtSplinePoint(i + 1, ESplineCoordinateSpace::World);
			float RandomStartRoll = FMath::DegreesToRadians(FMath::RandRange(-1.0f, 1.0f)) * RandomRoll;
			float RandomEndRoll = FMath::DegreesToRadians(FMath::RandRange(-1.0f, 1.0f)) * RandomRoll;
			StartRoll += RandomStartRoll;
			EndRoll += RandomEndRoll;
			FVector StartTangent = InputSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::World) * TangentScale;
			FVector EndTangent = InputSpline->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::World) * TangentScale;
			FRotator StartRotation = StartTangent.Rotation();
			FRotator EndRotation = EndTangent.Rotation();
			float StartScale = 1.0f;
			float EndScale = 1.0f;
			int RandomNumber = FMath::RandRange(0, MeshesForSplines.Num() - 1);
			UStaticMesh* Mesh = MeshesForSplines[RandomNumber];
			if (OutputStaticMeshesOnSplines)

			{

				// if its the last point, dont do nothing
				if (i == InputSpline->GetNumberOfSplinePoints() - 1)
				{
					continue;
				}

				UStaticMeshComponent* StaticMesh = NewObject<UStaticMeshComponent>(ParentActor);
				StaticMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
				StaticMesh->SetupAttachment(ParentActor->GetRootComponent());
				ParentActor->AddInstanceComponent(StaticMesh);
				StaticMesh->SetStaticMesh(Mesh);
				StaticMesh->SetMobility(EComponentMobility::Movable);
				FVector Direction = (EndPosition - StartPosition).GetSafeNormal();
				FRotator Rotation = Direction.Rotation();
				StaticMesh->SetWorldRotation(Rotation);
				StaticMesh->SetWorldLocation(StartPosition - Direction * Overlap);
				StaticMesh->RegisterComponent();
				FBoxSphereBounds MeshBounds = Mesh->GetBounds();
				float MeshLength = MeshBounds.BoxExtent.X * 2 - Overlap;
				float Scale = (EndPosition - StartPosition).Size() / MeshLength;
				StaticMesh->SetWorldScale3D(FVector(Scale, 1.0f, 1.0f));
				OutStaticMeshesPosts.Add(StaticMesh);
			}

			else
			{
				USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(ParentActor);
				SplineMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
				SplineMesh->SetupAttachment(ParentActor->GetRootComponent());
				ParentActor->AddInstanceComponent(SplineMesh);
				SplineMesh->SetStaticMesh(Mesh);
				SplineMesh->SetMobility(EComponentMobility::Movable);
				FVector OverlapOffsetStart = FVector(0.0f, 0.0f, 0.0f);
				FVector OverlapOffsetEnd = FVector(0.0f, 0.0f, 0.0f);
				OverlapOffsetStart = StartTangent.GetSafeNormal() * -1 * Overlap;
				OverlapOffsetEnd = EndTangent.GetSafeNormal() * Overlap;
				StartPosition += OverlapOffsetStart;
				EndPosition += OverlapOffsetEnd;
				SplineMesh->SetStartAndEnd(StartPosition, StartTangent, EndPosition, EndTangent);
				SplineMesh->SetWorldLocation(FVector(0.0f, 0.0f, 0.0f));
				StartRoll = FMath::DegreesToRadians(StartRoll);
				EndRoll = FMath::DegreesToRadians(EndRoll);
				SplineMesh->SetStartRoll(StartRoll);
				SplineMesh->SetEndRoll(EndRoll);
				SplineMesh->SetupAttachment(ParentActor->GetRootComponent());
				ParentActor->AddInstanceComponent(SplineMesh);
				SplineMesh->RegisterComponent();

				OutSplineMeshes.Add(SplineMesh);

				FLinearColor StartColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
				FLinearColor EndColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

				if (i == InputSpline->GetNumberOfSplinePoints() - 2)
				{
					EndColor = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
				}
				else if (i == 0)
				{
					StartColor = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
				}

				FMeshVertexPainter::PaintVerticesLerpAlongAxis(SplineMesh, StartColor, EndColor, EVertexPaintAxis::X, false);
			}
		}
	}

	if (MeshesForPosts.Num() != 0)
	{

		for (int i = 0; i < InputSpline->GetNumberOfSplinePoints(); i++)
		{
			int RandomNumber = FMath::RandRange(0, MeshesForPosts.Num() - 1);
			UStaticMeshComponent* StaticMesh = NewObject<UStaticMeshComponent>(ParentActor);
			StaticMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
			StaticMesh->SetupAttachment(ParentActor->GetRootComponent());
			ParentActor->AddInstanceComponent(StaticMesh);
			StaticMesh->SetStaticMesh(MeshesForPosts[RandomNumber]);
			StaticMesh->SetMobility(EComponentMobility::Movable);
			FVector Loc = InputSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
			FVector RightVec = InputSpline->GetRightVectorAtSplinePoint(i, ESplineCoordinateSpace::World);
			RightVec = FVector(RightVec.X, RightVec.Y, 0.0f).GetSafeNormal();
			Loc += RightVec * OffsetSplineTowardsRightVector;
			StaticMesh->SetWorldLocation(Loc);
			FRotator Rot = InputSpline->GetRotationAtSplinePoint(i, ESplineCoordinateSpace::World);
			float RollAtSplinePoint = InputSpline->GetRollAtSplinePoint(i, ESplineCoordinateSpace::World);
			StaticMesh->SetWorldRotation(Rot);

			if (StraightenPosts)
			{
				StaticMesh->SetRelativeRotation(FRotator(0.0f, StaticMesh->GetRelativeRotation().Yaw, RollAtSplinePoint));
			}

			StaticMesh->SetupAttachment(ParentActor->GetRootComponent());
			ParentActor->AddInstanceComponent(StaticMesh);
			StaticMesh->RegisterComponent();
			OutStaticMeshesPosts.Add(StaticMesh);
		}

		for (int i2 = 0; i2 < NumRandomPosts; i2++)
		{
			float RandomKnockoutFloat = FMath::RandRange(0.0f, 1.0f);
			TArray<FVector> LocationsR;
			float RandomIndex = FMath::RandRange(0.0f, 1.0f);
			int RandomMeshIndex = FMath::RandRange(0, MeshesForPosts.Num() - 1);
			if (RandomKnockoutFloat > KnockOutWeight)
			{
				FVector LocR = InputSpline->GetLocationAtTime(RandomIndex, ESplineCoordinateSpace::Local);
				FVector Right = InputSpline->GetRightVectorAtTime(RandomIndex, ESplineCoordinateSpace::Local);
				Right = FVector(Right.X, Right.Y, 0.0f).GetSafeNormal();
				LocR += Right * OffsetSplineTowardsRightVector;
				float DistF = 100000.0f;

				for (int k = 0; k < LocationsR.Num(); k++)
				{

					DistF = FMath::Min(DistF, FVector::Dist(LocR, LocationsR[k]));
				}

				if (DistF > 10.0f)
				{
					UStaticMeshComponent* StaticMesh = NewObject<UStaticMeshComponent>(ParentActor);
					StaticMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
					StaticMesh->SetupAttachment(ParentActor->GetRootComponent());
					ParentActor->AddInstanceComponent(StaticMesh);
					StaticMesh->SetStaticMesh(MeshesForPosts[RandomMeshIndex]);
					StaticMesh->SetMobility(EComponentMobility::Movable);
					StaticMesh->SetRelativeLocation(LocR);
					StaticMesh->SetRelativeRotation(InputSpline->GetTangentAtTime(RandomIndex, ESplineCoordinateSpace::Local).Rotation());

					if (StraightenPosts)
					{
						StaticMesh->SetRelativeRotation(FRotator(0.0f, StaticMesh->GetRelativeRotation().Yaw, 0.0f));
					}

					StaticMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
					StaticMesh->SetupAttachment(ParentActor->GetRootComponent());
					ParentActor->AddInstanceComponent(StaticMesh);
					StaticMesh->RegisterComponent();
					OutStaticMeshesPosts.Add(StaticMesh);
					LocationsR.Add(LocR);
				}
			}
		}

		for (int i3 = 0; i3 < OutStaticMeshesPosts.Num(); i3++)
		{
			if (SnapToGround)
			{
				FVector PostLoc = OutStaticMeshesPosts[i3]->GetComponentLocation();
				FHitResult HitResult;
				if (UWorld* World = GetWorld())
				{
					if (World->LineTraceSingleByObjectType(HitResult, PostLoc + FVector(0.0f, 0.0f, 10000.0f), PostLoc - FVector(0.0f, 0.0f, 10000.0f), FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic), FCollisionQueryParams(FName("SnapToGround"), true, ParentActor)))
					{
						PostLoc = HitResult.ImpactPoint;
					}
				}
				OutStaticMeshesPosts[i3]->SetWorldLocation(PostLoc);
			}
		}

	}
}

USplineComponent* ADriftIslandSplineMeshActorCPP::SplitSplineByDistance(TArray<USplineComponent*>& OutSplines, USplineComponent* Spline, float DistanceF, float SplineStart, float SplineEnd, bool SetPointsToLinear, bool NewSplineForEachSegment, float TangentScale, float RandomRoll, float RandomPositionRadius, AActor* ParentActor)
{
	float SplineLength = Spline->GetSplineLength();

	SplineLength = SplineLength * (SplineEnd - SplineStart);
	int Divisions = SplineLength / DistanceF;

	if (Divisions < 3)
	{
		Divisions = 3;
	}

	float Distance = SplineLength / (Divisions - 1);

	if (Distance < 50.0f)
	{
		Distance = 50.0f;
	}

	USplineComponent* NewSpline = NewObject<USplineComponent>(ParentActor);
	NewSpline->CreationMethod = EComponentCreationMethod::UserConstructionScript;
	NewSpline->SetupAttachment(ParentActor->GetRootComponent());
	NewSpline->RegisterComponent();
	NewSpline->ClearSplinePoints();

	for (int i = 0; i < Divisions; i++)
	{
		float NormalizedDistance = (Distance * i) / SplineLength * (SplineEnd - SplineStart);
		FVector Position;
		Position = Spline->GetWorldLocationAtTime(NormalizedDistance + SplineStart, true);
		NewSpline->AddSplinePoint(Position, ESplineCoordinateSpace::World);

		float RandomRollF = FMath::DegreesToRadians(FMath::RandRange(-1.0f, 1.0f)) * RandomRoll;

		FVector UpVector = Spline->GetUpVectorAtTime(NormalizedDistance, ESplineCoordinateSpace::World);
		FVector RightVector = Spline->GetRightVectorAtTime(NormalizedDistance, ESplineCoordinateSpace::World);

		FVector LerpVector = FMath::Lerp(FVector::UpVector, FVector::RightVector, RandomRollF);

		int LastPointIndex = NewSpline->GetNumberOfSplinePoints() - 1;

		NewSpline->SetUpVectorAtSplinePoint(LastPointIndex, LerpVector, ESplineCoordinateSpace::World);
	}

	if (SetPointsToLinear)
	{
		for (int i = 0; i < NewSpline->GetNumberOfSplinePoints(); i++)
		{
			NewSpline->SetSplinePointType(i, ESplinePointType::Linear, true);
		}
	}

	for (int i = 0; i < NewSpline->GetNumberOfSplinePoints(); i++)
	{
		FVector Tangent = NewSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
		Tangent *= TangentScale;
		NewSpline->SetTangentAtSplinePoint(i, Tangent, ESplineCoordinateSpace::Local, true);
	}

	if (NewSplineForEachSegment)
	{

		for (int i = 0; i < NewSpline->GetNumberOfSplinePoints() - 1; i++)
		{
			USplineComponent* NewSpline2 = NewObject<USplineComponent>(ParentActor);
			NewSpline2->CreationMethod = EComponentCreationMethod::UserConstructionScript;
			NewSpline2->SetupAttachment(ParentActor->GetRootComponent());

			NewSpline2->RegisterComponent();
			NewSpline2->ClearSplinePoints();
			NewSpline2->AddSplinePoint(NewSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local), ESplineCoordinateSpace::Local);
			NewSpline2->AddSplinePoint(NewSpline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local), ESplineCoordinateSpace::Local);
			if (SetPointsToLinear)
			{
				NewSpline2->SetSplinePointType(0, ESplinePointType::Linear, true);
				NewSpline2->SetSplinePointType(1, ESplinePointType::Linear, true);
			}
			NewSpline2->UpdateSpline();

			OutSplines.Add(NewSpline2);
		}
		NewSpline->DestroyComponent();
	}

	for (int i = 0; i < NewSpline->GetNumberOfSplinePoints(); i++)
	{
		FVector RandomPosition = NewSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
		RandomPosition += FVector(FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f), 0.0f) * RandomPositionRadius;
		NewSpline->SetLocationAtSplinePoint(i, RandomPosition, ESplineCoordinateSpace::World, true);
	}

	NewSpline->UpdateSpline();

	NewSpline->ComponentTags = Spline->ComponentTags;

	return NewSpline;
}


