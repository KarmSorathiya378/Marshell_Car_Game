// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VELOCITYXWorldGeneration.generated.h"

/**
 * Procedural world and highway generation actor
 */
UCLASS()
class VELOCITYX_API AVELOCITYXWorldGeneration : public AActor
{
	GENERATED_BODY()

public:
	AVELOCITYXWorldGeneration();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "World Generation")
	void GenerateWorld();

	UFUNCTION(BlueprintCallable, Category = "World Generation")
	void GenerateHighway();

	UFUNCTION(BlueprintCallable, Category = "World Generation")
	void GenerateHighwaySegment(int32 SegmentIndex);

	UFUNCTION(BlueprintCallable, Category = "World Generation")
	void GenerateRoadMarkings(FVector Start, FVector End);

	UFUNCTION(BlueprintCallable, Category = "World Generation")
	void GenerateBarrier(FVector Start, FVector End, bool bLeft);

	UFUNCTION(BlueprintCallable, Category = "World Generation")
	void GenerateTerrain();

	UFUNCTION(BlueprintCallable, Category = "World Generation")
	void PlaceEnvironment();

	UFUNCTION(BlueprintCallable, Category = "World Generation")
	void PlaceTree();

	UFUNCTION(BlueprintCallable, Category = "World Generation")
	void PlaceRock();

	UFUNCTION(BlueprintCallable, Category = "World Generation")
	void PlaceBush();

	UFUNCTION(BlueprintCallable, Category = "World Generation")
	void UpdateDynamicElements(float DeltaTime);

	// Highway parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highway")
	float HighwayWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highway")
	float HighwaySegmentLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highway")
	int32 NumHighwaySegments;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Highway")
	float TotalHighwayLength;

	// Environment parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float TerrainHeightVariance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float TreeDensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float RockDensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float BushDensity;

	// Performance optimization
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
	float LODBias;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
	float FoliageCullingDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
	int32 ActiveChunks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
	float ChunkSize;
};
