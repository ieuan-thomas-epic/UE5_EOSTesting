// Fill out your copyright notice in the Description page of Project Settings.


#include "EOSGameInstance.h"


UEOSGameInstance::UEOSGameInstance()
{
	bIsLoggedIn = false;
	OnlineSubSystem = IOnlineSubsystem::Get();
}

void UEOSGameInstance::Login()
{
	if (OnlineSubSystem)
	{
		if (IOnlineIdentityPtr Identity = OnlineSubSystem->GetIdentityInterface())
		{
			//FOnlineAccountCredentials Credentials;
			///*Credentials.Id = FString();
			//Credentials.Token = FString();
			//Credentials.Type = FString("accountportal");*/

			UE_LOG(LogTemp, Warning, TEXT("Trying to log in"));
			Identity->OnLoginCompleteDelegates->AddUObject(this, &UEOSGameInstance::OnLoginComplete);
			Identity->AutoLogin(0);
			// Identity->Login(0, Credentials);
		}
	}
}

void UEOSGameInstance::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	UE_LOG(LogTemp, Warning, TEXT("LoggedIn: %d"), bWasSuccessful)
	bIsLoggedIn = bWasSuccessful;
	if (OnlineSubSystem)
	{
		if (IOnlineIdentityPtr Identity = OnlineSubSystem->GetIdentityInterface())
		{
			Identity->ClearOnLoginCompleteDelegates(0, this);
		}
	}
}

void UEOSGameInstance::Init()
{
	Super::Init();

	/*OnlineSubSystem = IOnlineSubsystem::Get();
	Login();*/
}

bool UEOSGameInstance::HostSession(FUniqueNetIdRepl UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
{
	if (bIsLoggedIn)
	{
		if (OnlineSubSystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubSystem->GetSessionInterface())
			{
				SessionSettings = MakeShareable(new FOnlineSessionSettings());
				SessionSettings->bIsLANMatch = bIsLAN;
				SessionSettings->bUsesPresence = bIsPresence;
				SessionSettings->NumPublicConnections = MaxNumPlayers;
				SessionSettings->NumPrivateConnections = 0;
				SessionSettings->bIsDedicated = false;
				SessionSettings->bAllowInvites = true;
				SessionSettings->bAllowJoinInProgress = true;
				SessionSettings->bShouldAdvertise = true;
				SessionSettings->bUseLobbiesIfAvailable = true;
				SessionSettings->bAllowJoinViaPresence = true;
				SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

				SessionSettings->Set(SEARCH_KEYWORDS, LobbyName, EOnlineDataAdvertisementType::ViaOnlineService);
				SessionSettings->Set(SETTING_MAPNAME, MapPath, EOnlineDataAdvertisementType::ViaOnlineService);

				SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnCreateSessionComplete);
				return SessionPtr->CreateSession(*UserId, SessionName, *SessionSettings);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot create session: Not Logged In"));
	}
	return false;
}

void UEOSGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Success: %d"), bWasSuccessful);
	if (OnlineSubSystem)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubSystem->GetSessionInterface())
		{
			SessionPtr->ClearOnCreateSessionCompleteDelegates(this);
			if (bWasSuccessful)
			{
				SessionPtr->OnStartSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnStartOnlineGameComplete);
				SessionPtr->StartSession(SessionName);
			}
		}
	}
}
void UEOSGameInstance::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Session Success: %d"), bWasSuccessful);
	if (OnlineSubSystem)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubSystem->GetSessionInterface())
		{
			SessionPtr->ClearOnStartSessionCompleteDelegates(this);
		}
	}

	// If the start was successful, we can open a NewMap if we want. Make sure to use "listen" as a parameter!
	if (bWasSuccessful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName(*MapPath), true, "listen");
	}
}

void UEOSGameInstance::FindSessions(FUniqueNetIdRepl UserId, bool bIsLAN, bool bIsPresence)
{
	if (bIsLoggedIn)
	{
		if (OnlineSubSystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubSystem->GetSessionInterface())
			{
				SessionSearch = MakeShareable(new FOnlineSessionSearch());
				SessionSearch->QuerySettings.Set(SEARCH_KEYWORDS, LobbyName, EOnlineComparisonOp::Equals);
				SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
				SessionSearch->bIsLanQuery = bIsLAN;
				SessionSearch->MaxSearchResults = 5000;
				// SessionSearch->PingBucketSize = 50;

				// We only want to set this Query Setting if "bIsPresence" is true
				if (bIsPresence)
				{
					SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
				}

				SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnFindSessionsComplete);
				SessionPtr->FindSessions(*UserId, SessionSearch.ToSharedRef());
			}
		}
		else
		{
			OnFindSessionsComplete(false);
		}
	}
}
void UEOSGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Success: %d"), bWasSuccessful);
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found %d lobbies"), SessionSearch->SearchResults.Num());
		if (OnlineSubSystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubSystem->GetSessionInterface())
			{
				SessionPtr->ClearOnFindSessionsCompleteDelegates(this);

				if (SessionSearch->SearchResults.Num())
				{
					/*SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnJoinSessionComplete);
					SessionPtr->JoinSession(0, MySessionName, SessionSearch->SearchResults[0]);*/
				}
			}
		}
	}
}

bool UEOSGameInstance::JoinSession(FUniqueNetIdRepl UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult)
{
	// Return bool
	bool bSuccessful = false;

	if (bIsLoggedIn)
	{
		if (OnlineSubSystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubSystem->GetSessionInterface())
			{
				// Set the Handle again
				SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnJoinSessionComplete);

				// Call the "JoinSession" Function with the passed "SearchResult". The "SessionSearch->SearchResults" can be used to get such a
				// "FOnlineSessionSearchResult" and pass it. Pretty straight forward!
				bSuccessful = SessionPtr->JoinSession(*UserId, SessionName, SearchResult);
			}
		}
	}

	return bSuccessful;
}

void UEOSGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (OnlineSubSystem)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubSystem->GetSessionInterface())
		{
			{
				// Clear the Delegate again
				SessionPtr->ClearOnJoinSessionCompleteDelegates(this);

				// Get the first local PlayerController, so we can call "ClientTravel" to get to the Server Map
				// This is something the Blueprint Node "Join Session" does automatically!
				APlayerController* const PlayerController = GetFirstLocalPlayerController();

				// We need a FString to use ClientTravel and we can let the SessionInterface contruct such a
				// String for us by giving him the SessionName and an empty String. We want to do this, because
				// Every OnlineSubsystem uses different TravelURLs
				FString TravelURL;
				
				if (PlayerController && SessionPtr->GetResolvedConnectString(SessionName, TravelURL))
				{
					UE_LOG(LogTemp, Warning, TEXT("TravelURL: %s"), *TravelURL);

					// Finally call the ClienTravel. If you want, you could print the TravelURL to see
					// how it really looks like
					PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
				}
			}
		}
	}
}

void UEOSGameInstance::StartMatchmaking()
{
	// deactivated for the moment as it's not supported on EOSplus (yet?), remove false to make work
	if (bIsLoggedIn && false)
	{
		if (OnlineSubSystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubSystem->GetSessionInterface())
			{
				UE_LOG(LogTemp, Warning, TEXT("Matchmaking started"));
				FOnlineSessionSettings NewSessionSettings;
				TSharedRef<FOnlineSessionSearch> MySearchSettings = MakeShared<FOnlineSessionSearch>();

				// Set search criteria here
				MySearchSettings->bIsLanQuery = false;
				MySearchSettings->MaxSearchResults = 10;
				MySearchSettings->PingBucketSize = 50;

				TArray<FUniqueNetIdRef> MyLocalPlayers;

				// Get the local player's unique id
				ULocalPlayer* LocalPlayer = GetFirstGamePlayer();
				FUniqueNetIdRepl LocalPlayerId = LocalPlayer->GetPreferredUniqueNetId();

				// Create a shared reference to the local player's unique id
				TSharedRef<FUniqueNetIdString> LocalPlayerIdRef = MakeShared<FUniqueNetIdString>(LocalPlayerId.ToString());

				// Add the shared reference to the LocalPlayers array
				MyLocalPlayers.Add(LocalPlayerIdRef);

				//SessionPtr->OnMatchmakingCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnMatchmakingComplete);
				SessionPtr->StartMatchmaking(MyLocalPlayers, MySessionName, NewSessionSettings, MySearchSettings);
			}
		}
	}
}

void UEOSGameInstance::OnMatchmakingComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Log, TEXT("Matchmaking complete: %s, success: %d"), *SessionName.ToString(), bWasSuccessful);
}

void UEOSGameInstance::DestroySession()
{
	if (bIsLoggedIn)
	{
		if (OnlineSubSystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubSystem->GetSessionInterface())
			{
				SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnDestroySessionComplete);
				SessionPtr->DestroySession(MySessionName);
			}
		}
	}
}

void UEOSGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Session destroyed: %d"), bWasSuccessful)
		if (OnlineSubSystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubSystem->GetSessionInterface())
			{
				SessionPtr->ClearOnDestroySessionCompleteDelegates(this);
				// If it was successful, we just load another level (could be a MainMenu!)
				if (bWasSuccessful)
				{
					UGameplayStatics::OpenLevel(GetWorld(), "OliverLobby", true);
				}
			}
		}
}

void UEOSGameInstance::StartOnlineGame(FName SessionName, bool bIsLan, int MaxNumPlayers, FString MapName)
{
	// Creating a local player where we can get the UserID from
	ULocalPlayer* const Player = GetFirstGamePlayer();
	MySessionName = SessionName;
	MapPath = MapName;
	// Call our custom HostSession function. GameSessionName is a GameInstance variable
	HostSession(Player->GetPreferredUniqueNetId(), MySessionName, bIsLan, true, MaxNumPlayers);
}

void UEOSGameInstance::FindOnlineGames(bool bIsLan)
{
	ULocalPlayer* const Player = GetFirstGamePlayer();

	FindSessions(Player->GetPreferredUniqueNetId(), bIsLan, true);
}

void UEOSGameInstance::JoinOnlineGame()
{
	ULocalPlayer* const Player = GetFirstGamePlayer();

	// Just a SearchResult where we can save the one we want to use, for the case we find more than one!
	FOnlineSessionSearchResult SearchResult;

	// If the Array is not empty, we can go through it
	if (SessionSearch->SearchResults.Num() > 0)
	{
		for (int32 i = 0; i < SessionSearch->SearchResults.Num(); i++)
		{
			// To avoid something crazy, we filter sessions from ourself
			if (SessionSearch->SearchResults[i].Session.OwningUserId != Player->GetPreferredUniqueNetId())
			{
				SearchResult = SessionSearch->SearchResults[i];

				// Once we found sounce a Session that is not ours, just join it. Instead of using a for loop, you could
				// use a widget where you click on and have a reference for the GameSession it represents which you can use
				// here
				JoinSession(Player->GetPreferredUniqueNetId(), MySessionName, SearchResult);
				break;
			}
		}
	}
}

void UEOSGameInstance::DestroySessionAndLeaveGame()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubSystem->GetSessionInterface())
		{
			SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnDestroySessionComplete);
			SessionPtr->DestroySession(MySessionName);
		}
	}
}

void UEOSGameInstance::GetAllFriends()
{
	if (bIsLoggedIn)
	{
		if (OnlineSubSystem)
		{
			if (IOnlineFriendsPtr FriendsPtr = OnlineSubSystem->GetFriendsInterface())
			{
				FriendsPtr->ReadFriendsList(0, FString(""), FOnReadFriendsListComplete::CreateUObject(this, &UEOSGameInstance::OnGetAllFriendsComplete));
			}
		}
	}
}

void UEOSGameInstance::OnGetAllFriendsComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr)
{
	UE_LOG(LogTemp, Warning, TEXT("Found friends"));
	if (OnlineSubSystem)
	{
		if (IOnlineFriendsPtr FriendsPtr = OnlineSubSystem->GetFriendsInterface())
		{
			TArray<TSharedRef<FOnlineFriend>> FriendsList;
			if (FriendsPtr->GetFriendsList(0, ListName, FriendsList))
			{
				for (TSharedRef<FOnlineFriend> Friend : FriendsList)
				{
					FString FriendName = Friend.Get().GetRealName();
					UE_LOG(LogTemp, Warning, TEXT("Friend: %s"), *FriendName)
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Found no friends :'("))
			}
		}
	}
}

void UEOSGameInstance::ShowInviteUI()
{
	if (bIsLoggedIn)
	{
		if (OnlineSubSystem)
		{
			if (IOnlineExternalUIPtr UIPtr = OnlineSubSystem->GetExternalUIInterface())
			{
				UIPtr->ShowInviteUI(0, MySessionName);
			}
		}
	}
}
void UEOSGameInstance::ShowFriendsUI()
{
	if (bIsLoggedIn)
	{
		if (OnlineSubSystem)
		{
			if (IOnlineExternalUIPtr UIPtr = OnlineSubSystem->GetExternalUIInterface())
			{
				UIPtr->ShowFriendsUI(0);
			}
		}
	}
}
