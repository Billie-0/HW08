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
				// ���콺 �Է����� ī�޶� ȸ�� ����
				PC->SetIgnoreLookInput(true);

				// ���� �ð� �� �ٽ� ȸ�� �����ϰ�
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [=]()
				{
					if (IsValid(PC))
					{
						PC->SetIgnoreLookInput(false);
					}
				}, 5.0f, false); // 5�� �� ����
			}
		}

		DestroyItem();
	}
}
