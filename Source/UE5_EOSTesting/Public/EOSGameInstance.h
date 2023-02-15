// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineExternalUIInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include "EOSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class UE5_EOSTESTING_API UEOSGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UEOSGameInstance();

	virtual void Init() override;

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void Login();
	void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	bool HostSession(FUniqueNetIdRepl UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers);
	TSharedPtr<class FOnlineSessionSettings> SessionSettings;

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void StartOnlineGame(FName SessionName, bool bIsLan, int MaxNumPlayers, FString MapName);

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void FindOnlineGames(bool bIsLan);

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void JoinOnlineGame();

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void DestroySessionAndLeaveGame();

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	void FindSessions(FUniqueNetIdRepl UserId, bool bIsLAN, bool bIsPresence);
	//void FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence);
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;
	void OnFindSessionsComplete(bool bWasSuccessful);

	bool JoinSession(FUniqueNetIdRepl UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable)
	void DestroySession();
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	// UFUNCTION(BlueprintCallable)
	void StartMatchmaking();
	void OnMatchmakingComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable)
	void GetAllFriends();
	void OnGetAllFriendsComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr);

	UFUNCTION(BlueprintCallable)
	void ShowInviteUI();
	UFUNCTION(BlueprintCallable)
	void ShowFriendsUI();

protected:
	class IOnlineSubsystem* OnlineSubSystem;

	bool bIsLoggedIn;
	FName MySessionName = FName("TestSession");

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	int NumPublicConnections = 5;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	bool bIsDedicated = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	bool bShouldAdvertise = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	bool bIsLANMatch = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	bool bAllowJoinInProgress = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	bool bAllowJoinViaPresence = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	bool bUsesPresence = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	bool bUseLobbiesIfAvailable = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString LobbyName = "Default";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString MapPath = FString("ThirdPersonMap");
};
