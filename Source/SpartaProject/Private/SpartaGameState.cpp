#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "SlowingItem.h"
#include "BlindItem.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"

ASpartaGameState::ASpartaGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 60.0f; // ��ü ���� �ð��� 60�ʷ� ����
	CurrentLevelIndex = 0;
	MaxLevels = 3;

	// Wave �ý��� �ʱ�ȭ
	CurrentWave = 0;
	MaxWaves = 3;
	WaveDuration = 20.0f; // �� ���̺�� 20��
	AdditionalItemsPerWave = 10; // ���̺�� �߰��� ������ ����
}

void ASpartaGameState::OnWaveStarted(int32 WaveNumber, const FString& WaveInfo)
{
	// C++���� �߰��� ó���� ������ �ִٸ� ���⿡ �ۼ��� ��.
	// ����� �α׿� ȭ�� �޽����� StartNextWave���� ó���ǹǷ� �����.
}

void ASpartaGameState::ShowWaveNotification(int32 WaveNumber)
{
	// ���� ������ ������ ����
	if (WaveNotificationWidget)
	{
		WaveNotificationWidget->RemoveFromParent();
		WaveNotificationWidget = nullptr;
	}

	// ���� Ŭ������ �����Ǿ� �ִ��� Ȯ��
	if (!WaveNotificationClass)
	{
		return;
	}

	// PlayerController ��������
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	// ���� ����
	WaveNotificationWidget = CreateWidget<UUserWidget>(PC, WaveNotificationClass);
	if (WaveNotificationWidget)
	{
		WaveNotificationWidget->AddToViewport(1000);

		// Wave ��ȣ ����
		if (UTextBlock* WaveText = Cast<UTextBlock>(WaveNotificationWidget->GetWidgetFromName(TEXT("WaveNumberText"))))
		{
			WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave %d Started!"), WaveNumber)));
		}

		// �� ������ ���� ����
		if (UTextBlock* NewItemText = Cast<UTextBlock>(WaveNotificationWidget->GetWidgetFromName(TEXT("NewItemText"))))
		{
			FString NewItemInfo;
			switch (WaveNumber)
			{
			case 2:
				NewItemInfo = TEXT("New Item: SlowingItem");
				break;
			case 3:
				NewItemInfo = TEXT("New Item: BlindItem");
				break;
			default:
				NewItemInfo = TEXT("");
				break;
			}
			NewItemText->SetText(FText::FromString(NewItemInfo));
		}

		// 6�� �� �ڵ� ����
		FTimerHandle HideTimer;
		GetWorldTimerManager().SetTimer(HideTimer, this, &ASpartaGameState::HideWaveNotification, 6.0f, false);
	}
}

void ASpartaGameState::HideWaveNotification()
{
	if (WaveNotificationWidget)
	{
		WaveNotificationWidget->RemoveFromParent();
		WaveNotificationWidget = nullptr;
	}
}

void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();

	StartLevel();

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASpartaGameState::UpdateHUD,
		0.1f,
		true
	);
}

int32 ASpartaGameState::GetScore() const
{
	return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			SpartaGameInstance->AddToScore(Amount);
		}
	}
}

void ASpartaGameState::StartNextWave()
{
	CurrentWave++;

	FString WaveInfo;

	switch (CurrentWave)
	{
	case 1:
		WaveInfo = TEXT("Wave 1 Started! Basic items available.");
		break;

	case 2:
		WaveInfo = TEXT("Wave 2 Started! SlowingItem added!");
		SpawnAdditionalItemsForWave();
		break;

	case 3:
		WaveInfo = TEXT("Wave 3 Started! BlindItem added!");
		SpawnAdditionalItemsForWave();
		break;

	default:
		WaveInfo = FString::Printf(TEXT("Wave %d Started!"), CurrentWave);
		break;
	}

	// C++ �Լ��� �������Ʈ �̺�Ʈ �� �� ȣ��
	OnWaveStarted(CurrentWave, WaveInfo);
	OnWaveStarted_BP(CurrentWave, WaveInfo);

	// Wave �˸� ǥ��
	ShowWaveNotification(CurrentWave);

	// ���� ���̺� Ÿ�̸� ���� (������ ���̺갡 �ƴ� ����)
	if (CurrentWave < MaxWaves)
	{
		GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &ASpartaGameState::StartNextWave, WaveDuration, false);
	}
}

void ASpartaGameState::SpawnAdditionalItemsForWave()
{
	// SpawnVolume ã��
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	if (FoundVolumes.Num() == 0) return;

	ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
	if (!SpawnVolume) return;

	// ���̺꺰 �߰� ������ ����
	if (CurrentWave == 2 && SlowingItemBP)
	{
		// Wave 2: SlowingItem�� �߰�
		for (int32 i = 0; i < AdditionalItemsPerWave; i++)
		{
			SpawnVolume->SpawnSpecificItem(SlowingItemBP);
		}
	}
	else if (CurrentWave == 3)
	{
		// Wave 3: SlowingItem + BlindItem �߰�
		if (SlowingItemBP)
		{
			for (int32 i = 0; i < AdditionalItemsPerWave / 2; i++)
			{
				SpawnVolume->SpawnSpecificItem(SlowingItemBP);
			}
		}
		if (BlindItemBP)
		{
			for (int32 i = 0; i < AdditionalItemsPerWave / 2; i++)
			{
				SpawnVolume->SpawnSpecificItem(BlindItemBP);
			}
		}
	}
}

void ASpartaGameState::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD();
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		}
	}

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	CurrentWave = 0; // Wave �ʱ�ȭ

	// ���� ���� ���� �������� Ȯ��
	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MenuLevel") || CurrentMapName.Contains("Menu"))
	{
		// �޴� �����̸� Wave �������� ����
		return;
	}

	// �⺻ �����۵� ����
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	const int32 ItemToSpawn = 40;

	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}

	GetWorldTimerManager().SetTimer(LevelTimerHandle, this, &ASpartaGameState::OnLevelTimeUp, LevelDuration, false);

	StartNextWave(); // ù ��° ���̺� ���� (���� ���� ��)
}

void ASpartaGameState::OnLevelTimeUp()
{
	EndLevel();
}

void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;

	UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"),
		CollectedCoinCount,
		SpawnedCoinCount)

		if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
		{
			EndLevel();
		}
}

void ASpartaGameState::EndLevel()
{
	// ��� Ÿ�̸� ����
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			AddScore(Score);
			CurrentLevelIndex++;
			SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;
		}
	}

	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}

	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
}

void ASpartaGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true);
			SpartaPlayerController->ShowMainMenu(true);
		}
	}
}

void ASpartaGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController);
		if (SpartaPlayerController)
		{
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				// �ð� ������Ʈ (�ؽ�Ʈ)
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
				}

				// �ð� ������Ʈ (Progress Bar)
				if (UProgressBar* TimeProgressBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("TimeProgressBar"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
					float TimePercent = RemainingTime / LevelDuration;
					TimeProgressBar->SetPercent(TimePercent);

					// �ð��� ���� ���� ����
					FLinearColor BarColor;
					if (TimePercent > 0.6f)
					{
						BarColor = FLinearColor::Green;
					}
					else if (TimePercent > 0.3f)
					{
						BarColor = FLinearColor::Yellow;
					}
					else
					{
						BarColor = FLinearColor::Red;
					}
					TimeProgressBar->SetFillColorAndOpacity(BarColor);
				}

				// ���ھ� ������Ʈ
				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
						if (SpartaGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
						}
					}
				}

				// ���� ������Ʈ
				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level %d"), CurrentLevelIndex + 1)));
				}

				// Wave ���� ������Ʈ (���� �߰�)
				if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
				{
					WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d/%d"), CurrentWave, MaxWaves)));
				}
			}
		}
	}
}