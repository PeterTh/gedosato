// wrapper for ISteamFriends in isteamfriends.h
// generated using wrapper_gen_steam.rb

#include "steam\isteamfriends.h"

struct hkISteamFriends : public ISteamFriends {
	ISteamFriends *real;

public:
	// original interface
	virtual const char *GetPersonaName();
	virtual SteamAPICall_t SetPersonaName(const char *pchPersonaName);
	virtual EPersonaState GetPersonaState();
	virtual int GetFriendCount(int iFriendFlags);
	virtual CSteamID GetFriendByIndex(int iFriend, int iFriendFlags);
	virtual EFriendRelationship GetFriendRelationship(CSteamID steamIDFriend);
	virtual EPersonaState GetFriendPersonaState(CSteamID steamIDFriend);
	virtual const char *GetFriendPersonaName(CSteamID steamIDFriend);
	virtual bool GetFriendGamePlayed(CSteamID steamIDFriend, FriendGameInfo_t *pFriendGameInfo);
	virtual const char *GetFriendPersonaNameHistory(CSteamID steamIDFriend, int iPersonaName);
	virtual int GetFriendSteamLevel(CSteamID steamIDFriend);
	virtual const char *GetPlayerNickname(CSteamID steamIDPlayer);
	virtual int GetFriendsGroupCount();
	virtual FriendsGroupID_t GetFriendsGroupIDByIndex(int iFG);
	virtual const char *GetFriendsGroupName(FriendsGroupID_t friendsGroupID);
	virtual int GetFriendsGroupMembersCount(FriendsGroupID_t friendsGroupID);
	virtual void GetFriendsGroupMembersList(FriendsGroupID_t friendsGroupID, CSteamID *pOutSteamIDMembers, int nMembersCount);
	virtual bool HasFriend(CSteamID steamIDFriend, int iFriendFlags);
	virtual int GetClanCount();
	virtual CSteamID GetClanByIndex(int iClan);
	virtual const char *GetClanName(CSteamID steamIDClan);
	virtual const char *GetClanTag(CSteamID steamIDClan);
	virtual bool GetClanActivityCounts(CSteamID steamIDClan, int *pnOnline, int *pnInGame, int *pnChatting);
	virtual SteamAPICall_t DownloadClanActivityCounts(CSteamID *psteamIDClans, int cClansToRequest);
	virtual int GetFriendCountFromSource(CSteamID steamIDSource);
	virtual CSteamID GetFriendFromSourceByIndex(CSteamID steamIDSource, int iFriend);
	virtual bool IsUserInSource(CSteamID steamIDUser, CSteamID steamIDSource);
	virtual void SetInGameVoiceSpeaking(CSteamID steamIDUser, bool bSpeaking);
	virtual void ActivateGameOverlay(const char *pchDialog);
	virtual void ActivateGameOverlayToUser(const char *pchDialog, CSteamID steamID);
	virtual void ActivateGameOverlayToWebPage(const char *pchURL);
	virtual void ActivateGameOverlayToStore(AppId_t nAppID, EOverlayToStoreFlag eFlag);
	virtual void SetPlayedWith(CSteamID steamIDUserPlayedWith);
	virtual void ActivateGameOverlayInviteDialog(CSteamID steamIDLobby);
	virtual int GetSmallFriendAvatar(CSteamID steamIDFriend);
	virtual int GetMediumFriendAvatar(CSteamID steamIDFriend);
	virtual int GetLargeFriendAvatar(CSteamID steamIDFriend);
	virtual bool RequestUserInformation(CSteamID steamIDUser, bool bRequireNameOnly);
	virtual SteamAPICall_t RequestClanOfficerList(CSteamID steamIDClan);
	virtual CSteamID GetClanOwner(CSteamID steamIDClan);
	virtual int GetClanOfficerCount(CSteamID steamIDClan);
	virtual CSteamID GetClanOfficerByIndex(CSteamID steamIDClan, int iOfficer);
	virtual uint32 GetUserRestrictions();
	virtual bool SetRichPresence(const char *pchKey, const char *pchValue);
	virtual void ClearRichPresence();
	virtual const char *GetFriendRichPresence(CSteamID steamIDFriend, const char *pchKey);
	virtual int GetFriendRichPresenceKeyCount(CSteamID steamIDFriend);
	virtual const char *GetFriendRichPresenceKeyByIndex(CSteamID steamIDFriend, int iKey);
	virtual void RequestFriendRichPresence(CSteamID steamIDFriend);
	virtual bool InviteUserToGame(CSteamID steamIDFriend, const char *pchConnectString);
	virtual int GetCoplayFriendCount();
	virtual CSteamID GetCoplayFriend(int iCoplayFriend);
	virtual int GetFriendCoplayTime(CSteamID steamIDFriend);
	virtual AppId_t GetFriendCoplayGame(CSteamID steamIDFriend);
	virtual SteamAPICall_t JoinClanChatRoom(CSteamID steamIDClan);
	virtual bool LeaveClanChatRoom(CSteamID steamIDClan);
	virtual int GetClanChatMemberCount(CSteamID steamIDClan);
	virtual CSteamID GetChatMemberByIndex(CSteamID steamIDClan, int iUser);
	virtual bool SendClanChatMessage(CSteamID steamIDClanChat, const char *pchText);
	virtual int GetClanChatMessage(CSteamID steamIDClanChat, int iMessage, void *prgchText, int cchTextMax, EChatEntryType *peChatEntryType, CSteamID *psteamidChatter);
	virtual bool IsClanChatAdmin(CSteamID steamIDClanChat, CSteamID steamIDUser);
	virtual bool IsClanChatWindowOpenInSteam(CSteamID steamIDClanChat);
	virtual bool OpenClanChatWindowInSteam(CSteamID steamIDClanChat);
	virtual bool CloseClanChatWindowInSteam(CSteamID steamIDClanChat);
	virtual bool SetListenForFriendsMessages(bool bInterceptEnabled);
	virtual bool ReplyToFriendMessage(CSteamID steamIDFriend, const char *pchMsgToSend);
	virtual int GetFriendMessage(CSteamID steamIDFriend, int iMessageID, void *pvData, int cubData, EChatEntryType *peChatEntryType);
	virtual SteamAPICall_t GetFollowerCount(CSteamID steamID);
	virtual SteamAPICall_t IsFollowing(CSteamID steamID);
	virtual SteamAPICall_t EnumerateFollowingList(uint32 unStartIndex);
};
