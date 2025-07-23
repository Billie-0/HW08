#include "BlindItem.h"
#include "SpartaCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

void ABlindItem::ActivateItem(AActor* Activator)
{
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator))
		{
			APlayerController* PC = Cast<APlayerController>(PlayerCharacter->GetController());
			if (PC)
			{
				// 마우스 입력으로 카메라 회전 제한
				PC->SetIgnoreLookInput(true);

				// 일정 시간 후 다시 회전 가능하게
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [=]()
				{
					if (IsValid(PC))
					{
						PC->SetIgnoreLookInput(false);
					}
				}, 5.0f, false); // 5초 후 해제
			}
		}

		DestroyItem();
	}
}
