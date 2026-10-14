// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "VELOCITYXInputComponent.generated.h"

/**
 * UVELOCITYXInputComponent
 * Custom Enhanced Input Component for VELOCITY X, providing helper binding templates
 * and integration hooks for vehicles, UI menus, and telemetry.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VELOCITYX_API UVELOCITYXInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	UVELOCITYXInputComponent();

	/** Binds an action with a safe null check and logs registration */
	template<class UserClass, typename FuncType>
	void BindActionChecked(const class UInputAction* Action, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func)
	{
		if (Action)
		{
			BindAction(Action, TriggerEvent, Object, Func);
		}
	}
};
