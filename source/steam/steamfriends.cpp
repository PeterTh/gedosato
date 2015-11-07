// wrapper for ISteamFriends in isteamfriends.h
// generated using wrapper_gen_steam.rb

#include "steam/steamfriends.h"

#include "utils/win_utils.h"
#include "settings.h"

const char * hkISteamFriends::GetPersonaName() {
	SDLOG(20, "hkISteamFriends::GetPersonaName\n");
	return real->GetPersonaName();
}

SteamAPICall_t hkISteamFriends::SetPersonaName(const char * pchPersonaName) {
	SDLOG(20, "hkISteamFriends::SetPersonaName\n");
	return real->SetPersonaName(pchPersonaName);
}

EPersonaState hkISteamFriends::GetPersonaState() {
	SDLOG(20, "hkISteamFriends::GetPersonaState\n");
	return real->GetPersonaState();
}

int hkISteamFriends::GetFriendCount(int iFriendFlags) {
	SDLOG(20, "hkISteamFriends::GetFriendCount\n");
	if(Settings::get().getSteamFriendsLimit() >= 0) return Settings::get().getSteamFriendsLimit();
	return real->GetFriendCount(iFriendFlags);
}

CSteamID hkISteamFriends::GetFriendByIndex(int iFriend, int iFriendFlags) {
	SDLOG(20, "hkISteamFriends::GetFriendByIndex\n");
	return real->GetFriendByIndex(iFriend, iFriendFlags);
}

EFriendRelationship hkISteamFriends::GetFriendRelationship(CSteamID steamIDFriend) {
	SDLOG(20, "hkISteamFriends::GetFriendRelationship\n");
	return real->GetFriendRelationship(steamIDFriend);
}

EPersonaState hkISteamFriends::GetFriendPersonaState(CSteamID steamIDFriend) {
	SDLOG(20, "hkISteamFriends::GetFriendPersonaState\n");
	return real->GetFriendPersonaState(steamIDFriend);
}

const char * hkISteamFriends::GetFriendPersonaName(CSteamID steamIDFriend) {
	SDLOG(20, "hkISteamFriends::GetFriendPersonaName\n");
	return real->GetFriendPersonaName(steamIDFriend);
}

bool hkISteamFriends::GetFriendGamePlayed(CSteamID steamIDFriend, FriendGameInfo_t * pFriendGameInfo) {
	SDLOG(20, "hkISteamFriends::GetFriendGamePlayed\n");
	return real->GetFriendGamePlayed(steamIDFriend, pFriendGameInfo);
}

const char * hkISteamFriends::GetFriendPersonaNameHistory(CSteamID steamIDFriend, int iPersonaName) {
	SDLOG(20, "hkISteamFriends::GetFriendPersonaNameHistory\n");
	return real->GetFriendPersonaNameHistory(steamIDFriend, iPersonaName);
}

int hkISteamFriends::GetFriendSteamLevel(CSteamID steamIDFriend) {
	SDLOG(20, "hkISteamFriends::GetFriendSteamLevel\n");
	return real->GetFriendSteamLevel(steamIDFriend);
}

const char * hkISteamFriends::GetPlayerNickname(CSteamID steamIDPlayer) {
	SDLOG(20, "hkISteamFriends::GetPlayerNickname\n");
	return real->GetPlayerNickname(steamIDPlayer);
}

int hkISteamFriends::GetFriendsGroupCount() {
	SDLOG(20, "hkISteamFriends::GetFriendsGroupCount\n");
	return real->GetFriendsGroupCount();
}

FriendsGroupID_t hkISteamFriends::GetFriendsGroupIDByIndex(int iFG) {
	SDLOG(20, "hkISteamFriends::GetFriendsGroupIDByIndex\n");
	return real->GetFriendsGroupIDByIndex(iFG);
}

const char * hkISteamFriends::GetFriendsGroupName(FriendsGroupID_t friendsGroupID) {
	SDLOG(20, "hkISteamFriends::GetFriendsGroupName\n");
	return real->GetFriendsGroupName(friendsGroupID);
}

int hkISteamFriends::GetFriendsGroupMembersCount(FriendsGroupID_t friendsGroupID) {
	SDLOG(20, "hkISteamFriends::GetFriendsGroupMembersCount\n");
	return real->GetFriendsGroupMembersCount(friendsGroupID);
}

void hkISteamFriends::GetFriendsGroupMembersList(FriendsGroupID_t friendsGroupID, CSteamID * pOutSteamIDMembers, int nMembersCount) {
	SDLOG(20, "hkISteamFriends::GetFriendsGroupMembersList\n");
	return real->GetFriendsGroupMembersList(friendsGroupID, pOutSteamIDMembers, nMembersCount);
}

bool hkISteamFriends::HasFriend(CSteamID steamIDFriend, int iFriendFlags) {
	SDLOG(20, "hkISteamFriends::HasFriend\n");
	return real->HasFriend(steamIDFriend, iFriendFlags);
}

int hkISteamFriends::GetClanCount() {
	SDLOG(20, "hkISteamFriends::GetClanCount\n");
	return real->GetClanCount();
}

CSteamID hkISteamFriends::GetClanByIndex(int iClan) {
	SDLOG(20, "hkISteamFriends::GetClanByIndex\n");
	return real->GetClanByIndex(iClan);
}

const char * hkISteamFriends::GetClanName(CSteamID steamIDClan) {
	SDLOG(20, "hkISteamFriends::GetClanName\n");
	return real->GetClanName(steamIDClan);
}

const char * hkISteamFriends::GetClanTag(CSteamID steamIDClan) {
	SDLOG(20, "hkISteamFriends::GetClanTag\n");
	return real->GetClanTag(steamIDClan);
}

bool hkISteamFriends::GetClanActivityCounts(CSteamID steamIDClan, int * pnOnline, int * pnInGame, int * pnChatting) {
	SDLOG(20, "hkISteamFriends::GetClanActivityCounts\n");
	return real->GetClanActivityCounts(steamIDClan, pnOnline, pnInGame, pnChatting);
}

SteamAPICall_t hkISteamFriends::DownloadClanActivityCounts(CSteamID * psteamIDClans, int cClansToRequest) {
	SDLOG(20, "hkISteamFriends::DownloadClanActivityCounts\n");
	return real->DownloadClanActivityCounts(psteamIDClans, cClansToRequest);
}

int hkISteamFriends::GetFriendCountFromSource(CSteamID steamIDSource) {
	SDLOG(20, "hkISteamFriends::GetFriendCountFromSource\n");
	return real->GetFriendCountFromSource(steamIDSource);
}

CSteamID hkISteamFriends::GetFriendFromSourceByIndex(CSteamID steamIDSource, int iFriend) {
	SDLOG(20, "hkISteamFriends::GetFriendFromSourceByIndex\n");
	return real->GetFriendFromSourceByIndex(steamIDSource, iFriend);
}

bool hkISteamFriends::IsUserInSource(CSteamID steamIDUser, CSteamID steamIDSource) {
	SDLOG(20, "hkISteamFriends::IsUserInSource\n");
	return real->IsUserInSource(steamIDUser, steamIDSource);
}

void hkISteamFriends::SetInGameVoiceSpeaking(CSteamID steamIDUser, bool bSpeaking) {
	SDLOG(20, "hkISteamFriends::SetInGameVoiceSpeaking\n");
	return real->SetInGameVoiceSpeaking(steamIDUser, bSpeaking);
}

void hkISteamFriends::ActivateGameOverlay(const char * pchDialog) {
	SDLOG(20, "hkISteamFriends::ActivateGameOverlay\n");
	return real->ActivateGameOverlay(pchDialog);
}

void hkISteamFriends::ActivateGameOverlayToUser(const char * pchDialog, CSteamID steamID) {
	SDLOG(20, "hkISteamFriends::ActivateGameOverlayToUser\n");
	return real->ActivateGameOverlayToUser(pchDialog, steamID);
}

void hkISteamFriends::ActivateGameOverlayToWebPage(const char * pchURL) {
	SDLOG(20, "hkISteamFriends::ActivateGameOverlayToWebPage\n");
	return real->ActivateGameOverlayToWebPage(pchURL);
}

void hkISteamFriends::ActivateGameOverlayToStore(AppId_t nAppID, EOverlayToStoreFlag eFlag) {
	SDLOG(20, "hkISteamFriends::ActivateGameOverlayToStore\n");
	return real->ActivateGameOverlayToStore(nAppID, eFlag);
}

void hkISteamFriends::SetPlayedWith(CSteamID steamIDUserPlayedWith) {
	SDLOG(20, "hkISteamFriends::SetPlayedWith\n");
	return real->SetPlayedWith(steamIDUserPlayedWith);
}

void hkISteamFriends::ActivateGameOverlayInviteDialog(CSteamID steamIDLobby) {
	SDLOG(20, "hkISteamFriends::ActivateGameOverlayInviteDialog\n");
	return real->ActivateGameOverlayInviteDialog(steamIDLobby);
}

int hkISteamFriends::GetSmallFriendAvatar(CSteamID steamIDFriend) {
	SDLOG(20, "hkISteamFriends::GetSmallFriendAvatar\n");
	return real->GetSmallFriendAvatar(steamIDFriend);
}

int hkISteamFriends::GetMediumFriendAvatar(CSteamID steamIDFriend) {
	SDLOG(20, "hkISteamFriends::GetMediumFriendAvatar\n");
	return real->GetMediumFriendAvatar(steamIDFriend);
}

int hkISteamFriends::GetLargeFriendAvatar(CSteamID steamIDFriend) {
	SDLOG(20, "hkISteamFriends::GetLargeFriendAvatar\n");
	return real->GetLargeFriendAvatar(steamIDFriend);
}

bool hkISteamFriends::RequestUserInformation(CSteamID steamIDUser, bool bRequireNameOnly) {
	SDLOG(20, "hkISteamFriends::RequestUserInformation\n");
	return real->RequestUserInformation(steamIDUser, bRequireNameOnly);
}

SteamAPICall_t hkISteamFriends::RequestClanOfficerList(CSteamID steamIDClan) {
	SDLOG(20, "hkISteamFriends::RequestClanOfficerList\n");
	return real->RequestClanOfficerList(steamIDClan);
}

CSteamID hkISteamFriends::GetClanOwner(CSteamID steamIDClan) {
	SDLOG(20, "hkISteamFriends::GetClanOwner\n");
	return real->GetClanOwner(steamIDClan);
}

int hkISteamFriends::GetClanOfficerCount(CSteamID steamIDClan) {
	SDLOG(20, "hkISteamFriends::GetClanOfficerCount\n");
	return real->GetClanOfficerCount(steamIDClan);
}

CSteamID hkISteamFriends::GetClanOfficerByIndex(CSteamID steamIDClan, int iOfficer) {
	SDLOG(20, "hkISteamFriends::GetClanOfficerByIndex\n");
	return real->GetClanOfficerByIndex(steamIDClan, iOfficer);
}

uint32 hkISteamFriends::GetUserRestrictions() {
	SDLOG(20, "hkISteamFriends::GetUserRestrictions\n");
	return real->GetUserRestrictions();
}

bool hkISteamFriends::SetRichPresence(const char * pchKey, const char * pchValue) {
	SDLOG(20, "hkISteamFriends::SetRichPresence\n");
	return real->SetRichPresence(pchKey, pchValue);
}

void hkISteamFriends::ClearRichPresence() {
	SDLOG(20, "hkISteamFriends::ClearRichPresence\n");
	return real->ClearRichPresence();
}

const char * hkISteamFriends::GetFriendRichPresence(CSteamID steamIDFriend, const char * pchKey) {
	SDLOG(20, "hkISteamFriends::GetFriendRichPresence\n");
	return real->GetFriendRichPresence(steamIDFriend, pchKey);
}

int hkISteamFriends::GetFriendRichPresenceKeyCount(CSteamID steamIDFriend) {
	SDLOG(20, "hkISteamFriends::GetFriendRichPresenceKeyCount\n");
	return real->GetFriendRichPresenceKeyCount(steamIDFriend);
}

const char * hkISteamFriends::GetFriendRichPresenceKeyByIndex(CSteamID steamIDFriend, int iKey) {
	SDLOG(20, "hkISteamFriends::GetFriendRichPresenceKeyByIndex\n");
	return real->GetFriendRichPresenceKeyByIndex(steamIDFriend, iKey);
}

void hkISteamFriends::RequestFriendRichPresence(CSteamID steamIDFriend) {
	SDLOG(20, "hkISteamFriends::RequestFriendRichPresence\n");
	return real->RequestFriendRichPresence(steamIDFriend);
}

bool hkISteamFriends::InviteUserToGame(CSteamID steamIDFriend, const char * pchConnectString) {
	SDLOG(20, "hkISteamFriends::InviteUserToGame\n");
	return real->InviteUserToGame(steamIDFriend, pchConnectString);
}

int hkISteamFriends::GetCoplayFriendCount() {
	SDLOG(20, "hkISteamFriends::GetCoplayFriendCount\n");
	return real->GetCoplayFriendCount();
}

CSteamID hkISteamFriends::GetCoplayFriend(int iCoplayFriend) {
	SDLOG(20, "hkISteamFriends::GetCoplayFriend\n");
	return real->GetCoplayFriend(iCoplayFriend);
}

int hkISteamFriends::GetFriendCoplayTime(CSteamID steamIDFriend) {
	SDLOG(20, "hkISteamFriends::GetFriendCoplayTime\n");
	return real->GetFriendCoplayTime(steamIDFriend);
}

AppId_t hkISteamFriends::GetFriendCoplayGame(CSteamID steamIDFriend) {
	SDLOG(20, "hkISteamFriends::GetFriendCoplayGame\n");
	return real->GetFriendCoplayGame(steamIDFriend);
}

SteamAPICall_t hkISteamFriends::JoinClanChatRoom(CSteamID steamIDClan) {
	SDLOG(20, "hkISteamFriends::JoinClanChatRoom\n");
	return real->JoinClanChatRoom(steamIDClan);
}

bool hkISteamFriends::LeaveClanChatRoom(CSteamID steamIDClan) {
	SDLOG(20, "hkISteamFriends::LeaveClanChatRoom\n");
	return real->LeaveClanChatRoom(steamIDClan);
}

int hkISteamFriends::GetClanChatMemberCount(CSteamID steamIDClan) {
	SDLOG(20, "hkISteamFriends::GetClanChatMemberCount\n");
	return real->GetClanChatMemberCount(steamIDClan);
}

CSteamID hkISteamFriends::GetChatMemberByIndex(CSteamID steamIDClan, int iUser) {
	SDLOG(20, "hkISteamFriends::GetChatMemberByIndex\n");
	return real->GetChatMemberByIndex(steamIDClan, iUser);
}

bool hkISteamFriends::SendClanChatMessage(CSteamID steamIDClanChat, const char * pchText) {
	SDLOG(20, "hkISteamFriends::SendClanChatMessage\n");
	return real->SendClanChatMessage(steamIDClanChat, pchText);
}

int hkISteamFriends::GetClanChatMessage(CSteamID steamIDClanChat, int iMessage, void * prgchText, int cchTextMax, EChatEntryType * peChatEntryType, CSteamID * psteamidChatter) {
	SDLOG(20, "hkISteamFriends::GetClanChatMessage\n");
	return real->GetClanChatMessage(steamIDClanChat, iMessage, prgchText, cchTextMax, peChatEntryType, psteamidChatter);
}

bool hkISteamFriends::IsClanChatAdmin(CSteamID steamIDClanChat, CSteamID steamIDUser) {
	SDLOG(20, "hkISteamFriends::IsClanChatAdmin\n");
	return real->IsClanChatAdmin(steamIDClanChat, steamIDUser);
}

bool hkISteamFriends::IsClanChatWindowOpenInSteam(CSteamID steamIDClanChat) {
	SDLOG(20, "hkISteamFriends::IsClanChatWindowOpenInSteam\n");
	return real->IsClanChatWindowOpenInSteam(steamIDClanChat);
}

bool hkISteamFriends::OpenClanChatWindowInSteam(CSteamID steamIDClanChat) {
	SDLOG(20, "hkISteamFriends::OpenClanChatWindowInSteam\n");
	return real->OpenClanChatWindowInSteam(steamIDClanChat);
}

bool hkISteamFriends::CloseClanChatWindowInSteam(CSteamID steamIDClanChat) {
	SDLOG(20, "hkISteamFriends::CloseClanChatWindowInSteam\n");
	return real->CloseClanChatWindowInSteam(steamIDClanChat);
}

bool hkISteamFriends::SetListenForFriendsMessages(bool bInterceptEnabled) {
	SDLOG(20, "hkISteamFriends::SetListenForFriendsMessages\n");
	return real->SetListenForFriendsMessages(bInterceptEnabled);
}

bool hkISteamFriends::ReplyToFriendMessage(CSteamID steamIDFriend, const char * pchMsgToSend) {
	SDLOG(20, "hkISteamFriends::ReplyToFriendMessage\n");
	return real->ReplyToFriendMessage(steamIDFriend, pchMsgToSend);
}

int hkISteamFriends::GetFriendMessage(CSteamID steamIDFriend, int iMessageID, void * pvData, int cubData, EChatEntryType * peChatEntryType) {
	SDLOG(20, "hkISteamFriends::GetFriendMessage\n");
	return real->GetFriendMessage(steamIDFriend, iMessageID, pvData, cubData, peChatEntryType);
}

SteamAPICall_t hkISteamFriends::GetFollowerCount(CSteamID steamID) {
	SDLOG(20, "hkISteamFriends::GetFollowerCount\n");
	return real->GetFollowerCount(steamID);
}

SteamAPICall_t hkISteamFriends::IsFollowing(CSteamID steamID) {
	SDLOG(20, "hkISteamFriends::IsFollowing\n");
	return real->IsFollowing(steamID);
}

SteamAPICall_t hkISteamFriends::EnumerateFollowingList(uint32 unStartIndex) {
	SDLOG(20, "hkISteamFriends::EnumerateFollowingList\n");
	return real->EnumerateFollowingList(unStartIndex);
}
