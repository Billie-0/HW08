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
	LevelDuration = 60.0f; // 전체 레벨 시간을 60초로 변경
	CurrentLevelIndex = 0;
	MaxLevels = 3;

	// Wave 시스템 초기화
	CurrentWave = 0;
	MaxWaves = 3;
	WaveDuration = 20.0f; // 각 웨이브당 20초
	AdditionalItemsPerWave = 10; // 웨이브당 추가할 아이템 개수
}

void ASpartaGameState::OnWaveStarted(int32 WaveNumber, const FString& WaveInfo)
{
	// C++에서 추가로 처리할 로직이 있다면 여기에 작성할 것.
	// 현재는 로그와 화면 메시지가 StartNextWave에서 처리되므로 비워둠.
}

void ASpartaGameState::ShowWaveNotification(int32 WaveNumber)
{
	// 기존 위젯이 있으면 제거
	if (WaveNotificationWidget)
	{
		WaveNotificationWidget->RemoveFromParent();
		WaveNotificationWidget = nullptr;
	}

	// 위젯 클래스가 설정되어 있는지 확인
	if (!WaveNotificationClass)
	{
		return;
	}

	// PlayerController 가져오기
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	// 위젯 생성
	WaveNotificationWidget = CreateWidget<UUserWidget>(PC, WaveNotificationClass);
	if (WaveNotificationWidget)
	{
		WaveNotificationWidget->AddToViewport(1000);

		// Wave 번호 설정
		if (UTextBlock* WaveText = Cast<UTextBlock>(WaveNotificationWidget->GetWidgetFromName(TEXT("WaveNumberText"))))
		{
			WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave %d Started!"), WaveNumber)));
		}

		// 새 아이템 정보 설정
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

		// 6초 후 자동 제거
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

	// C++ 함수와 블루프린트 이벤트 둘 다 호출
	OnWaveStarted(CurrentWave, WaveInfo);
	OnWaveStarted_BP(CurrentWave, WaveInfo);

	// Wave 알림 표시
	ShowWaveNotification(CurrentWave);

	// 다음 웨이브 타이머 설정 (마지막 웨이브가 아닐 때만)
	if (CurrentWave < MaxWaves)
	{
		GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &ASpartaGameState::StartNextWave, WaveDuration, false);
	}
}

void ASpartaGameState::SpawnAdditionalItemsForWave()
{
	// SpawnVolume 찾기
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	if (FoundVolumes.Num() == 0) return;

	ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
	if (!SpawnVolume) return;

	// 웨이브별 추가 아이템 스폰
	if (CurrentWave == 2 && SlowingItemBP)
	{
		// Wave 2: SlowingItem만 추가
		for (int32 i = 0; i < AdditionalItemsPerWave; i++)
		{
			SpawnVolume->SpawnSpecificItem(SlowingItemBP);
		}
	}
	else if (CurrentWave == 3)
	{
		// Wave 3: SlowingItem + BlindItem 추가
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
	CurrentWave = 0; // Wave 초기화

	// 현재 맵이 게임 레벨인지 확인
	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MenuLevel") || CurrentMapName.Contains("Menu"))
	{
		// 메뉴 레벨이면 Wave 시작하지 않음
		return;
	}

	// 기본 아이템들 스폰
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

	StartNextWave(); // 첫 번째 웨이브 시작 (레벨 시작 시)
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
	// 모든 타이머 정리
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
				// 시간 업데이트 (텍스트)
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
				}

				// 시간 업데이트 (Progress Bar)
				if (UProgressBar* TimeProgressBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("TimeProgressBar"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
					float TimePercent = RemainingTime / LevelDuration;
					TimeProgressBar->SetPercent(TimePercent);

					// 시간에 따라 색상 변경
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

				// 스코어 업데이트
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

				// 레벨 업데이트
				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level %d"), CurrentLevelIndex + 1)));
				}

				// Wave 정보 업데이트 (새로 추가)
				if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
				{
					WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d/%d"), CurrentWave, MaxWaves)));
				}
			}
		}
	}
}