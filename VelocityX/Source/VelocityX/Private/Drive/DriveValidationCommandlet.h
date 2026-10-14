#pragma once
#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "DriveValidationCommandlet.generated.h"

UCLASS()
class VELOCITYX_API UDriveValidationCommandlet : public UCommandlet
{
 GENERATED_BODY()
public:
 UDriveValidationCommandlet();
 virtual int32 Main(const FString& Params) override;
};
