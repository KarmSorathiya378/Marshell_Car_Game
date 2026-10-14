// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Vehicle/BaseCar.h"
#include "TrackSupercar.generated.h"

/**
 * TrackSupercar - Circuit-focused precision racing machine
 * Engineered for maximum cornering grip, massive downforce, stiff suspension, and lap-record pace.
 */
UCLASS()
class VELOCITYX_API ATrackSupercar : public ABaseCar
{
	GENERATED_BODY()

public:
	ATrackSupercar();

	// --- Track Specific Features ---
	UFUNCTION(BlueprintCallable, Category = "Track Car")
	void SetDRSActive(bool bActive);

	UFUNCTION(BlueprintPure, Category = "Track Car")
	bool IsDRSActive() const { return bDRSActive; }

protected:
	virtual void SetupCarDefaults() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Car")
	bool bDRSActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Car")
	float BaseDownforce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Car")
	float BaseDrag;
};
