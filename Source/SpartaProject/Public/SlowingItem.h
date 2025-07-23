#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "SlowingItem.generated.h"

UCLASS()
class SPARTAPROJECT_API ASlowingItem : public ABaseItem
{
	GENERATED_BODY()

public:
	ASlowingItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slow")
	float SlowDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slow")
	float SlowRate;

	virtual void ActivateItem(AActor* Activator) override;
};
