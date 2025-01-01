// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

#include "MultiplayerSessionSubsystem.generated.h"

struct FBlueprintSessionResult;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionCreateComplete, bool, Success);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionUpdateComplete, bool, Success);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionDestroyComplete, bool, Success);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSessionsFindComplete, const TArray<FBlueprintSessionResult>&, SessionResults, bool, Success);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionJoinComplete, EOnJoinSessionCompleteResult::Type Result);

const int32 MAX_SESSIONS_SEARCH_RESULT = 50;
static const int32 MIN_PLAYERS_IN_SESSION = 2;
//See max for spectators
static const int32 MAX_PLAYERS_IN_SESSION = 2;



USTRUCT(BlueprintType)
struct RAZINGTEST_API FCustomSessionSettings
{
	GENERATED_BODY()
public:

	FCustomSessionSettings(FCustomSessionSettings&&) = default;
	FCustomSessionSettings(const FCustomSessionSettings&) = default;
	FCustomSessionSettings& operator=(FCustomSessionSettings&&) = default;
	FCustomSessionSettings& operator=(const FCustomSessionSettings&) = default;


	FCustomSessionSettings() {}

	FCustomSessionSettings(const int32& _MaxConnections,
		bool _IsPrivate,
		bool _IsLANMatch,
		const FString& _LevelToLaunch)
		: MaxConnections(_MaxConnections)
		, IsPrivate(_IsPrivate)
		, IsLANMatch(_IsLANMatch)
		, LevelToLaunch(_LevelToLaunch)
	{}
	UPROPERTY()
		int32 MaxConnections = 0;
	UPROPERTY()
		bool IsPrivate = false;
	UPROPERTY()
		bool IsLANMatch = false;
	UPROPERTY()
		FString LevelToLaunch;
};




UCLASS()
class RAZINGTEST_API UMultiplayerSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection);
	UMultiplayerSessionSubsystem();

	//On...SessionCompleteEvents are called when the action is resolved (Successful or not)

	UFUNCTION(BlueprintCallable)
	void CreateSession(int32 MaxConnections, bool IsPrivate, bool IsLANMatch, FName LevelToOpen);
	FOnSessionCreateComplete OnCreateSessionCompleteEvent;

	void UpdateSession(int32 MaxConnections, bool IsPrivate, bool IsLANMatch, const FString& LevelToLaunchName);
	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnSessionUpdateComplete OnUpdateSessionCompleteEvent;

	void DestroySession();
	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnSessionDestroyComplete OnDestroySessionCompleteEvent;

	UFUNCTION(BlueprintCallable)
	void FindSessions(bool IsLANQuery);
	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnSessionsFindComplete OnFindSessionsCompleteEvent;

	UFUNCTION(BlueprintCallable)
	void JoinGameSession(const FBlueprintSessionResult& SessionResult);
	FOnSessionJoinComplete OnJoinGameSessionCompleteEvent;

	const FCustomSessionSettings& GetCustomSessionSettings() { return *CustomSettings; }

	//Callable by widgets
	UFUNCTION(BlueprintCallable)
		void ServerTravel(const FString& LevelName, const FString& Options = "?Listen");

	TArray<FString> GetSessionPlayersID();
protected:
	void OnCreateSessionCompleted(FName SessionName, bool Success);
	//Linked to OnCreateSessionCompleteEvent
	UFUNCTION()
		void OnCreateSessionCompleteResult(bool Success);

	void OnUpdateSessionCompleted(FName SessionName, bool Success);

	void OnDestroySessionCompleted(FName SessionName, bool Success);
	//Linked to OnDestroySessionCompleteEvent
	UFUNCTION()
		void OnDestroySessionCompletedResult(bool Success);

	void OnFindSessionsCompleted(bool Success);

	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	//Linked to OnJoinGameSessionCompleteEvent
	void OnJoinSessionCompletedResult(EOnJoinSessionCompleteResult::Type Result);
	//Use ClientTravel for JoinSession -> JoinSession doesn't automatically open the current Server Level
	bool TryTravelToCurrentSession();



private:
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;

	//Will be used for more precised settings used for the game -> To put in GameMode later
	TSharedPtr<FCustomSessionSettings> CustomSettings;

#pragma region Unreal Sessions delegates
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;

	FOnUpdateSessionCompleteDelegate UpdateSessionCompleteDelegate;
	FDelegateHandle UpdateSessionCompleteDelegateHandle;

	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;

	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;

	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
#pragma endregion Unreal Sessions delegates
};
