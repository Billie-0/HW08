#include "SlowingItem.h"
#include "SpartaCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

ASlowingItem::ASlowingItem()
{
	SlowDuration = 5.f;
	SlowRate = 0.5f;
	ItemType = "Slowing";
}

void ASlowingItem::ActivateItem(AActor* Activator)
{
    ACharacter* Character = Cast<ACharacter>(Activator);
    if (Character)
    {
        UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
        if (MoveComp)
        {
            float OriginalSpeed = MoveComp->MaxWalkSpeed;
            float SlowedSpeed = OriginalSpeed * SlowRate;
            MoveComp->MaxWalkSpeed *= SlowRate;

            if (ASpartaCharacter* SpartaChar = Cast<ASpartaCharacter>(Character))
            {
                SpartaChar->bIsSlowed = true;
                SpartaChar->SlowedSpeed = SlowedSpeed;
            }

            // 일정 시간 후 원래 속도로 복구
            FTimerHandle TimerHandle;
            FTimerDelegate RestoreSpeed;
            RestoreSpeed.BindLambda([=]()
            {
                if (IsValid(Character) && Character->GetCharacterMovement())
                {
                    Character->GetCharacterMovement()->MaxWalkSpeed = OriginalSpeed;

                    // Sprint 상태 복구
                    if (ASpartaCharacter* SpartaChar = Cast<ASpartaCharacter>(Character))
                    {
                        SpartaChar->bIsSlowed = false;
                    }
                }
            });

            GetWorld()->GetTimerManager().SetTimer(TimerHandle, RestoreSpeed, SlowDuration, false);
        }

        DestroyItem();
    }
}