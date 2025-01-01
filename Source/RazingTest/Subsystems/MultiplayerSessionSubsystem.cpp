// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "FindSessionsCallbackProxy.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"

void UMultiplayerSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

UMultiplayerSessionSubsystem::UMultiplayerSessionSubsystem()
{
	CreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionSubsystem::OnCreateSessionCompleted);
	OnCreateSessionCompleteEvent.AddDynamic(this, &UMultiplayerSessionSubsystem::OnCreateSessionCompleteResult);

	UpdateSessionCompleteDelegate = FOnUpdateSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionSubsystem::OnUpdateSessionCompleted);

	DestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionSubsystem::OnDestroySessionCompleted);
	OnDestroySessionCompleteEvent.AddDynamic(this, &UMultiplayerSessionSubsystem::OnDestroySessionCompletedResult);

	FindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMultiplayerSessionSubsystem::OnFindSessionsCompleted);

	JoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionSubsystem::OnJoinSessionCompleted);
	OnJoinGameSessionCompleteEvent.AddUObject(this, &UMultiplayerSessionSubsystem::OnJoinSessionCompletedResult);

}

#pragma region Create Session
void
UMultiplayerSessionSubsystem::CreateSession(int32 MaxConnections,bool IsPrivate,bool IsLANMatch, FName LevelToOpen)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		OnCreateSessionCompleteEvent.Broadcast(false);
		return;
	}
	CustomSettings = MakeShareable(new FCustomSessionSettings());
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->NumPrivateConnections = IsPrivate ? MaxConnections : 0;
	LastSessionSettings->NumPublicConnections = IsPrivate ? 0 : MaxConnections;
	LastSessionSettings->bAllowInvites = true;
	LastSessionSettings->bAllowJoinInProgress = !IsPrivate;
	LastSessionSettings->bAllowJoinViaPresence = !IsPrivate;
	LastSessionSettings->bAllowJoinViaPresenceFriendsOnly = !IsPrivate;
	LastSessionSettings->bIsDedicated = false;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bIsLANMatch = IsLANMatch;
	LastSessionSettings->bShouldAdvertise = true;

	LastSessionSettings->Set(SETTING_MAPNAME, LevelToOpen.ToString(), EOnlineDataAdvertisementType::DontAdvertise);
	CreateSessionCompleteDelegateHandle = sessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	if (!sessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
		OnCreateSessionCompleteEvent.Broadcast(false);
	}
	else
	{
		CustomSettings->MaxConnections = MaxConnections;
		CustomSettings->IsPrivate = IsPrivate;
		CustomSettings->IsLANMatch = IsLANMatch;
		CustomSettings->LevelToLaunch = LevelToOpen.ToString();
	}
}
void
UMultiplayerSessionSubsystem::OnCreateSessionCompleted(FName SessionName,bool Success)
{
	UE_LOG(LogTemp, Warning, TEXT("Try to create session.."));

	if (Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s  - Session successfully created"), *SessionName.ToString());
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("%s - Session failed to create"), *SessionName.ToString());

	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	OnCreateSessionCompleteEvent.Broadcast(Success);

}

void
UMultiplayerSessionSubsystem::OnCreateSessionCompleteResult(bool Success)
{
	if (Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session success to create"));
		FString levelName;
		LastSessionSettings->Get(SETTING_MAPNAME, levelName);
		UGameplayStatics::OpenLevel(this, FName(levelName), true, FString("?Listen"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Session failed to create"));
	}
}
#pragma endregion //CreateSession

#pragma region Update Session
void
UMultiplayerSessionSubsystem::UpdateSession(int32 MaxConnections,bool IsPrivate,bool IsLANMatch,const FString& LevelToLaunchName)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		OnUpdateSessionCompleteEvent.Broadcast(false);
		return;
	}

	TSharedPtr<FOnlineSessionSettings> updatedSessionSettings = MakeShareable(new FOnlineSessionSettings(*LastSessionSettings));



	updatedSessionSettings->NumPrivateConnections = IsPrivate ? MaxConnections : 0;
	updatedSessionSettings->NumPublicConnections = IsPrivate ? 0 : MaxConnections;
	updatedSessionSettings->bAllowJoinInProgress = !IsPrivate;
	updatedSessionSettings->bAllowJoinViaPresence = !IsPrivate;
	updatedSessionSettings->bAllowJoinViaPresenceFriendsOnly = !IsPrivate;
	updatedSessionSettings->bIsLANMatch = IsLANMatch;

	updatedSessionSettings->Set(SETTING_MAPNAME, LevelToLaunchName, EOnlineDataAdvertisementType::ViaOnlineService);


	UpdateSessionCompleteDelegateHandle = sessionInterface->AddOnUpdateSessionCompleteDelegate_Handle(UpdateSessionCompleteDelegate);

	if (!sessionInterface->UpdateSession(NAME_GameSession, *updatedSessionSettings))
	{
		sessionInterface->ClearOnUpdateSessionCompleteDelegate_Handle(UpdateSessionCompleteDelegateHandle);
		OnUpdateSessionCompleteEvent.Broadcast(false);
	}
	else
	{
		LastSessionSettings = std::move(updatedSessionSettings);
		CustomSettings->MaxConnections = MaxConnections;
		CustomSettings->IsPrivate = IsPrivate;
		CustomSettings->IsLANMatch = IsLANMatch;
		CustomSettings->LevelToLaunch = LevelToLaunchName;
	}
}

void
UMultiplayerSessionSubsystem::OnUpdateSessionCompleted(FName SessionName,bool Success)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnUpdateSessionCompleteDelegate_Handle(UpdateSessionCompleteDelegateHandle);
	}
	OnUpdateSessionCompleteEvent.Broadcast(Success);
}
#pragma endregion Update Session

#pragma region Destroy Session
void
UMultiplayerSessionSubsystem::DestroySession()
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		OnDestroySessionCompleteEvent.Broadcast(false);
		return;
	}

	DestroySessionCompleteDelegateHandle = sessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	if (!sessionInterface->DestroySession(NAME_GameSession))
	{
		sessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		OnDestroySessionCompleteEvent.Broadcast(false);
	}
}

void
UMultiplayerSessionSubsystem::OnDestroySessionCompleted(FName SessionName, bool Success)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}

	OnDestroySessionCompleteEvent.Broadcast(Success);
}

void
UMultiplayerSessionSubsystem::OnDestroySessionCompletedResult(bool Success)
{
	if (Success)
	{
		APlayerController* playerController = GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld());
		if(playerController)
			playerController->LocalTravel("LVL_Lobby");
	}
}

#pragma endregion Destroy Session

#pragma region Find Sessions
void
UMultiplayerSessionSubsystem::FindSessions(bool IsLANQuery)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		OnFindSessionsCompleteEvent.Broadcast(TArray<FBlueprintSessionResult>(), false);
		return;
	}

	FindSessionsCompleteDelegateHandle =
		sessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MAX_SESSIONS_SEARCH_RESULT;
	LastSessionSearch->bIsLanQuery = IsLANQuery;
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!sessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);

		OnFindSessionsCompleteEvent.Broadcast(TArray<FBlueprintSessionResult>(), false);
	}
}
void
UMultiplayerSessionSubsystem::OnFindSessionsCompleted(bool Success)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	if (LastSessionSearch->SearchResults.Num() <= 0)
	{
		OnFindSessionsCompleteEvent.Broadcast(TArray<FBlueprintSessionResult>(), Success);
		return;
	}
	//Modify cpp only results to BP compatible session results
	TArray<FBlueprintSessionResult> BPArrayResult;
	for (auto Result : LastSessionSearch->SearchResults)
	{
		FBlueprintSessionResult BPResult;
		BPResult.OnlineResult = Result;
		BPArrayResult.Add(BPResult);
	}
	OnFindSessionsCompleteEvent.Broadcast(BPArrayResult, Success);
}
#pragma endregion Find Session

#pragma region JoinSession

void
UMultiplayerSessionSubsystem::JoinGameSession(const FBlueprintSessionResult& SessionResult)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		OnJoinGameSessionCompleteEvent.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Silver, TEXT("sessionInterface invalid"));
		return;
	}

	JoinSessionCompleteDelegateHandle = sessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!sessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult.OnlineResult))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Silver, TEXT("Failed to join"));
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		OnJoinGameSessionCompleteEvent.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Silver, TEXT("JoinGameSession"));
	OnJoinGameSessionCompleteEvent.Broadcast(EOnJoinSessionCompleteResult::Success);
}


void
UMultiplayerSessionSubsystem::ServerTravel(const FString& LevelName, const FString& Options /* = "?Listen"*/)
{
	FString UrlToTravelTo = LevelName + Options;
	GetWorld()->ServerTravel(UrlToTravelTo);
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Silver, TEXT("Try Server Travel"));
}

TArray<FString>
UMultiplayerSessionSubsystem::GetSessionPlayersID()
{
	//Pas ça pour les Player Ids
	TArray<FString> Ids;
	for (auto MemberSetting : LastSessionSettings->MemberSettings)
	{
		Ids.AddUnique(MemberSetting.Key.Get().ToString());
	}
	return Ids;
}

void
UMultiplayerSessionSubsystem::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Silver, TEXT("OnJoinSessionCompleted"));
	OnJoinGameSessionCompleteEvent.Broadcast(Result);
}

void
UMultiplayerSessionSubsystem::OnJoinSessionCompletedResult(EOnJoinSessionCompleteResult::Type Result)
{
	switch (Result)
	{
		case EOnJoinSessionCompleteResult::Success:
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Silver, TEXT("OnJoinSession - Success"));
			UE_LOG(LogTemp, Warning, TEXT("OnJoinSession - Success"));
			TryTravelToCurrentSession();
		}
		break;
		case EOnJoinSessionCompleteResult::UnknownError:
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Silver, TEXT("OnJoinSession - UnknownError"));
			UE_LOG(LogTemp, Error, TEXT("OnJoinSession - UnknownError"));
		}
		break;
		case EOnJoinSessionCompleteResult::SessionIsFull:
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Silver, TEXT("OnJoinSession - SessionIsFull"));
			UE_LOG(LogTemp, Error, TEXT("OnJoinSession - SessionIsFull"));
		}
		break;
		case EOnJoinSessionCompleteResult::SessionDoesNotExist:
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Silver, TEXT("OnJoinSession - SessionDoesNotExist"));
			UE_LOG(LogTemp, Error, TEXT("OnJoinSession - SessionDoesNotExist"));
		}
		case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Silver, TEXT("OnJoinSession -  CouldNotRetrieveAddress of the session"));
			UE_LOG(LogTemp, Error, TEXT("OnJoinSession - CouldNotRetrieveAddress of the session"));
		}
		break;
	}
}

bool
UMultiplayerSessionSubsystem::TryTravelToCurrentSession()
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		return false;
	}

	FString connectString;
	if (!sessionInterface->GetResolvedConnectString(NAME_GameSession, connectString))
	{
		return false;
	}

	APlayerController* playerController = GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld());
	playerController->ClientTravel(connectString, TRAVEL_Absolute);
	return true;
}
#pragma endregion Join Session
