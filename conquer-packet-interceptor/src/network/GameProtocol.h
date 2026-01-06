#pragma once
// GameProtocol.h - Conquer Online Packet Definitions
// 
// Packet structures mapped from server source code (C# -> C++).
// All packets use #pragma pack(push, 1) for correct network alignment.
// Byte order: Little Endian (x86/x64 standard)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <cstdint>
#include <cstring>

#pragma pack(push, 1)

// Packet Type IDs
enum PacketType : uint16_t 
{
    MSG_REGISTER            = 1001,
    MSG_TALK                = 1004,
    MSG_WALK_OLD            = 1005,
    MSG_USER_INFO           = 1006,
    MSG_ITEM_INFO           = 1008,
    MSG_ITEM                = 1009,
    MSG_ACTION_OLD          = 1010,
    MSG_TICK                = 1012,
    MSG_PLAYER_OLD          = 1014,
    MSG_NAME                = 1015,
    MSG_WEATHER             = 1016,
    MSG_USER_ATTRIB_OLD     = 1017,
    MSG_FRIEND              = 1019,
    MSG_INTERACT            = 1022,
    MSG_TEAM                = 1023,
    MSG_ALLOT               = 1024,
    MSG_WEAPON_SKILL_OLD    = 1025,
    MSG_TEAM_MEMBER         = 1026,
    MSG_GEM_EMBED           = 1027,
    MSG_DATA                = 1033,
    MSG_PING                = 1037,
    MSG_WEAPON_SKILL        = 1040,
    MSG_LOAD_MAP            = 1044,
    MSG_CONNECT             = 1052,
    MSG_CONNECT_EX          = 1055,
    MSG_MAP_ITEM            = 1101,
    MSG_MAGIC_INFO          = 1103,
    MSG_MAGIC_EFFECT        = 1105,
    MSG_SYNDICATE           = 1107,
    MSG_NPC_INFO_EX         = 1109,
    MSG_VIP_USER_HANDLE     = 1128,
    MSG_NPC_INFO            = 2030,
    MSG_NPC                 = 2031,
    MSG_TASK_DIALOG         = 2032,
    MSG_NOBILITY            = 2064,
    MSG_TRANSPORTOR         = 2400,
    MSG_WALK                = 10005,
    MSG_ACTION              = 10010,
    MSG_PLAYER              = 10014,
    MSG_USER_ATTRIB         = 10017
};

// ActionType - Used by MsgAction
enum ActionType : uint16_t {
    ACTION_LOGIN_SPAWN              = 74,
    ACTION_LOGIN_INVENTORY          = 75,
    ACTION_LOGIN_RELATIONSHIPS      = 76,
    ACTION_LOGIN_PROFICIENCIES      = 77,
    ACTION_LOGIN_SPELLS             = 78,
    ACTION_CHARACTER_DIRECTION      = 79,
    ACTION_CHARACTER_EMOTE          = 81,
    ACTION_MAP_PORTAL               = 85,
    ACTION_MAP_TELEPORT             = 86,
    ACTION_CHARACTER_LEVEL_UP       = 92,
    ACTION_SPELL_ABORT_XP           = 93,
    ACTION_CHARACTER_REVIVE         = 94,
    ACTION_CHARACTER_DELETE         = 95,
    ACTION_CHARACTER_PK_MODE        = 96,
    ACTION_LOGIN_GUILD              = 97,
    ACTION_MAP_MINE                 = 99,
    ACTION_MAP_TEAM_LEADER_STAR     = 101,
    ACTION_MAP_QUERY                = 102,
    ACTION_ABORT_MAGIC              = 103,
    ACTION_MAP_ARGB                 = 104,
    ACTION_MAP_TEAM_MEMBER_STAR     = 106,
    ACTION_KICKBACK                 = 108,
    ACTION_SPELL_REMOVE             = 109,
    ACTION_CHARACTER_OBSERVATION    = 117,
    ACTION_SPELL_ABORT_TRANSFORM    = 118,
    ACTION_SPELL_ABORT_FLIGHT       = 120,
    ACTION_MAP_GOLD                 = 121,
    ACTION_RELATIONSHIPS_ENEMY      = 123,
    ACTION_CLIENT_DIALOG            = 126,
    ACTION_MAP_EFFECT               = 134,
    ACTION_REMOVE_ENTITY            = 135,
    ACTION_MAP_JUMP                 = 137,
    ACTION_CHARACTER_DEAD           = 145,
    ACTION_SYNC_SCREEN              = 146,
    ACTION_RELATIONSHIPS_FRIEND     = 148,
    ACTION_CHARACTER_AVATAR         = 151,
    ACTION_QUERY_TRADE_BUDDY        = 152,
    ACTION_AWAY                     = 161,
    ACTION_PATH_FINDING             = 162,
    ACTION_LOGIN_COMPLETE           = 251,
    ACTION_UPGRADE_MAGIC_SKILL      = 252,
    ACTION_UPGRADE_WEAPON_SKILL     = 253,
    ACTION_USER_ATTRIB_INFO         = 408
};

// ItemActionType - Used by MsgItem
enum ItemActionType : uint16_t {
    ITEM_SHOP_PURCHASE          = 1,
    ITEM_SHOP_SELL              = 2,
    ITEM_INVENTORY_REMOVE       = 3,
    ITEM_INVENTORY_EQUIP        = 4,
    ITEM_EQUIPMENT_WEAR         = 5,
    ITEM_EQUIPMENT_REMOVE       = 6,
    ITEM_EQUIPMENT_SPLIT        = 7,
    ITEM_EQUIPMENT_COMBINE      = 8,
    ITEM_BANK_QUERY             = 9,
    ITEM_BANK_DEPOSIT           = 10,
    ITEM_BANK_WITHDRAW          = 11,
    ITEM_EQUIPMENT_REPAIR       = 14,
    ITEM_EQUIPMENT_REPAIR_ALL   = 15,
    ITEM_EQUIPMENT_IMPROVE      = 19,
    ITEM_EQUIPMENT_LEVEL_UP     = 20,
    ITEM_BOOTH_QUERY            = 21,
    ITEM_BOOTH_SELL             = 22,
    ITEM_BOOTH_REMOVE           = 23,
    ITEM_BOOTH_PURCHASE         = 24,
    ITEM_EQUIPMENT_AMOUNT       = 25,
    ITEM_FIREWORKS              = 26,
    ITEM_CLIENT_PING            = 27,
    ITEM_EQUIPMENT_ENCHANT      = 28,
    ITEM_INVENTORY_DROP_ITEM    = 37,
    ITEM_INVENTORY_DROP_SILVER  = 38,
    ITEM_GEM_COMPOSE            = 39,
    ITEM_ACTIVATE_ACCESSORY     = 41,
    ITEM_SOCKET_EQUIPMENT       = 43,
    ITEM_MAIN_EQUIPMENT         = 44,
    ITEM_ALTERNATIVE_EQUIPMENT  = 45,
    ITEM_MERGE_ITEMS            = 48,
    ITEM_SPLIT_ITEMS            = 49,
    ITEM_REQUEST_ITEM_TOOLTIP   = 52
};

// MsgInteractType - Used by MsgInteract
enum MsgInteractType : uint32_t {
    INTERACT_NONE               = 0,
    INTERACT_STEAL              = 1,
    INTERACT_ATTACK             = 2,
    INTERACT_HEAL               = 3,
    INTERACT_POISON             = 4,
    INTERACT_ASSASSINATE        = 5,
    INTERACT_FREEZE             = 6,
    INTERACT_UNFREEZE           = 7,
    INTERACT_COURT              = 8,
    INTERACT_MARRY              = 9,
    INTERACT_DIVORCE            = 10,
    INTERACT_PRESENT_MONEY      = 11,
    INTERACT_PRESENT_ITEM       = 12,
    INTERACT_SEND_FLOWERS       = 13,
    INTERACT_KILL               = 14,
    INTERACT_JOIN_GUILD         = 15,
    INTERACT_QUERY_INFO         = 19,
    INTERACT_RUSH_ATTACK        = 20,
    INTERACT_ABORT_MAGIC        = 22,
    INTERACT_REFLECT_WEAPON     = 23,
    INTERACT_MAGIC_ATTACK       = 24,
    INTERACT_SHOOT              = 28,
    INTERACT_QUARRY             = 29,
    INTERACT_CHOP               = 30,
    INTERACT_HUSTLE             = 31,
    INTERACT_SOUL               = 32,
    INTERACT_INCREASE_JAR       = 36,
    INTERACT_COUNTER_KILL       = 43,
    INTERACT_COUNTER_KILL_SWITCH = 44,
    INTERACT_FATAL_STRIKE       = 45,
    INTERACT_COUPLE_ACTION_REQUEST = 46,
    INTERACT_DASH_WHIRL         = 53,
    INTERACT_AZURE_DMG          = 55
};

// MsgAction (ID: 10010) - General player actions
struct MsgAction 
{
    uint16_t Length;
    uint16_t Type;
    int32_t  Unknown;
    uint32_t Identity;          // Player ID
    uint32_t Command;
    uint32_t Argument;
    uint32_t Timestamp;
    uint16_t Action;            // ActionType
    uint16_t Direction;
    uint16_t X;
    uint16_t Y;
    uint32_t Map;
    uint32_t MapColor;
    uint8_t  Sprint;
    
    uint16_t GetCommandX() const { return static_cast<uint16_t>(Command & 0xFFFF); }
    uint16_t GetCommandY() const { return static_cast<uint16_t>(Command >> 16); }

    void SetCommandXY(uint16_t x, uint16_t y) 
    { 
        Command = (static_cast<uint32_t>(y) << 16) | x; 
    }
    
    uint16_t GetArgumentX() const { return static_cast<uint16_t>(Argument & 0xFFFF); }
    uint16_t GetArgumentY() const { return static_cast<uint16_t>(Argument >> 16); }

    void SetArgumentXY(uint16_t x, uint16_t y) 
    { 
        Argument = (static_cast<uint32_t>(y) << 16) | x; 
    }
    
    void Init(uint32_t playerId, ActionType actionType) 
    {
        memset(this, 0, sizeof(MsgAction));
        Length = sizeof(MsgAction);
        Type = MSG_ACTION;
        Identity = playerId;
        Action = static_cast<uint16_t>(actionType);
        Timestamp = GetTickCount();
        Unknown = static_cast<int32_t>(Timestamp);
    }
};
static_assert(sizeof(MsgAction) == 41, "MsgAction size mismatch");

// MsgItem (ID: 1009) - Item operations
struct MsgItem 
{
    uint16_t Length;
    uint16_t Type;
    int32_t  Padding;
    uint32_t Identity;          // Item ID
    uint32_t Command;
    uint32_t Data;
    uint16_t Action;            // ItemActionType
    uint32_t Timestamp;
    uint32_t Argument;
    uint32_t Argument2;
    uint8_t  MoneyType;
    uint32_t Headgear;
    uint32_t Necklace;
    uint32_t Armor;
    uint32_t RightHand;
    uint32_t LeftHand;
    uint32_t Ring;
    uint32_t Talisman;
    uint32_t Boots;
    uint32_t Garment;
    uint32_t RightAccessory;
    uint32_t LeftAccessory;
    uint32_t MountArmor;
    uint32_t Crop;
    uint32_t Wings;
    
    void InitUseItem(uint32_t itemId, uint32_t slot = 0) 
    {
        memset(this, 0, sizeof(MsgItem));
        Length = sizeof(MsgItem);
        Type = MSG_ITEM;
        Identity = itemId;
        Command = slot;
        Action = ITEM_INVENTORY_EQUIP;
        Timestamp = GetTickCount();
    }
    
    void InitPing() 
    {
        memset(this, 0, sizeof(MsgItem));
        Length = sizeof(MsgItem);
        Type = MSG_ITEM;
        Action = ITEM_CLIENT_PING;
        Timestamp = GetTickCount();
    }
};
static_assert(sizeof(MsgItem) == 91, "MsgItem size mismatch");

// MsgInteract (ID: 1022) - Interactions (attack, magic, etc)
struct MsgInteract 
{
    uint16_t Length;
    uint16_t Type;
    int32_t  Timestamp;
    int32_t  Padding;
    uint32_t SenderIdentity;
    uint32_t TargetIdentity;
    uint16_t PosX;
    uint16_t PosY;
    uint32_t Action;            // MsgInteractType
    int32_t  Data;
    int32_t  Command;
    int32_t  Effect;
    int32_t  EffectValue;
    
    uint16_t GetMagicType() const { return static_cast<uint16_t>(Data & 0xFFFF); }
    uint16_t GetMagicLevel() const { return static_cast<uint16_t>(Data >> 16); }

    void SetMagic(uint16_t type, uint16_t level) 
    {
        Data = (static_cast<int32_t>(level) << 16) | type;
    }
    
    void InitAttack(uint32_t sender, uint32_t target, uint16_t x, uint16_t y) 
    {
        memset(this, 0, sizeof(MsgInteract));
        Length = sizeof(MsgInteract);
        Type = MSG_INTERACT;
        Timestamp = GetTickCount();
        SenderIdentity = sender;
        TargetIdentity = target;
        PosX = x;
        PosY = y;
        Action = INTERACT_ATTACK;
    }
};
static_assert(sizeof(MsgInteract) == 44, "MsgInteract size mismatch");

// MsgVipUserHandle (ID: 1128) - VIP teleport
struct MsgVipUserHandle 
{
    uint16_t Length;
    uint16_t Type;
    int32_t  Mode;
    int32_t  Location;
    int32_t  Countdown;
    
    void InitSelfTeleport(int32_t locationId) 
    {
        memset(this, 0, sizeof(MsgVipUserHandle));
        Length = sizeof(MsgVipUserHandle) + 1;
        Type = MSG_VIP_USER_HANDLE;
        Mode = 0;
        Location = locationId;
        Countdown = 0;
    }
};

// MsgTeam (ID: 1023) - Party/Team system
struct MsgTeam 
{
    uint16_t Length;
    uint16_t Type;
    uint32_t Action;
    uint32_t Identity;
    
    void Init(uint32_t action, uint32_t targetId = 0) 
    {
        Length = sizeof(MsgTeam);
        Type = MSG_TEAM;
        Action = action;
        Identity = targetId;
    }
};

enum TeamAction : uint32_t {
    TEAM_CREATE          = 0,
    TEAM_REQUEST_JOIN    = 1,
    TEAM_LEAVE           = 2,
    TEAM_ACCEPT_INVITE   = 3,
    TEAM_REQUEST_INVITE  = 4,
    TEAM_ACCEPT_JOIN     = 5,
    TEAM_DISMISS         = 6,
    TEAM_KICK            = 7,
    TEAM_FORBID          = 8,
    TEAM_REMOVE_FORBID   = 9,
    TEAM_CLOSE_MONEY     = 10,
    TEAM_OPEN_MONEY      = 11,
    TEAM_CLOSE_ITEM      = 12,
    TEAM_OPEN_ITEM       = 13,
    TEAM_LEADER          = 15
};

// MsgAllot (ID: 1024) - Attribute point distribution
struct MsgAllot 
{
    uint16_t Length;
    uint16_t Type;
    int32_t  Timestamp;
    int32_t  Identity;
    int32_t  Force;
    int32_t  Speed;
    int32_t  Health;
    int32_t  Soul;
    
    void Init(int32_t str, int32_t agi, int32_t vit, int32_t spi) 
    {
        memset(this, 0, sizeof(MsgAllot));
        Length = sizeof(MsgAllot);
        Type = MSG_ALLOT;
        Timestamp = GetTickCount();
        Force = str;
        Speed = agi;
        Health = vit;
        Soul = spi;
    }
};

// MsgGemEmbed (ID: 1027) - Gem socketing
struct MsgGemEmbed 
{
    uint16_t Length;
    uint16_t Type;
    uint32_t Timestamp;
    uint32_t Identity;
    uint32_t MainIdentity;
    uint32_t MinorIdentity;
    uint16_t Position;
    uint16_t Action;
    
    void InitEmbed(uint32_t equipId, uint32_t gemId, uint16_t pos) 
    {
        memset(this, 0, sizeof(MsgGemEmbed));
        Length = sizeof(MsgGemEmbed);
        Type = MSG_GEM_EMBED;
        Timestamp = GetTickCount();
        MainIdentity = equipId;
        MinorIdentity = gemId;
        Position = pos;
        Action = 0;
    }
};

// MsgFriend (ID: 1019) - Friend system
struct MsgFriend 
{
    uint16_t Length;
    uint16_t Type;
    uint32_t Identity;
    uint8_t  Action;
    bool     Online;
    int16_t  Padding;
    int32_t  Nobility;
    int32_t  Gender;
    char     Name[16];
    
    void Init(uint32_t targetId, uint8_t action) 
    {
        memset(this, 0, sizeof(MsgFriend));
        Length = sizeof(MsgFriend);
        Type = MSG_FRIEND;
        Identity = targetId;
        Action = action;
    }
};
static_assert(sizeof(MsgFriend) == 36, "MsgFriend size mismatch");

enum FriendAction : uint8_t {
    FRIEND_REQUEST       = 10,
    FRIEND_NEW           = 11,
    FRIEND_SET_ONLINE    = 12,
    FRIEND_SET_OFFLINE   = 13,
    FRIEND_REMOVE        = 14,
    FRIEND_ADD           = 15,
    ENEMY_SET_ONLINE     = 16,
    ENEMY_SET_OFFLINE    = 17,
    ENEMY_REMOVE         = 18,
    ENEMY_ADD            = 19
};

// MsgMapItem (ID: 1101) - Ground items
struct MsgMapItem 
{
    uint16_t Length;
    uint16_t Type;
    int32_t  Timestamp;
    uint32_t Identity;
    uint32_t ItemType;
    uint16_t X;
    uint16_t Y;
    uint16_t Color;
    uint16_t Action;
    int32_t  Composition;
    uint32_t OwnerIdentity;
    int32_t  Unknown;
    
    void InitPickup(uint32_t itemId) 
    {
        memset(this, 0, sizeof(MsgMapItem));
        Length = sizeof(MsgMapItem);
        Type = MSG_MAP_ITEM;
        Timestamp = GetTickCount();
        Identity = itemId;
        Action = 3; // MAP_ITEM_PICK
    }
};
static_assert(sizeof(MsgMapItem) == 36, "MsgMapItem size mismatch");

// MsgSyndicate (ID: 1107) - Guild operations
struct MsgSyndicate 
{
    uint16_t Length;
    uint16_t Type;
    uint32_t Mode;
    uint32_t Identity;
    int32_t  ConditionLevel;
    int32_t  ConditionMetempsychosis;
    int32_t  ConditionProfession;
    
    void Init(uint32_t mode, uint32_t targetId = 0) 
    {
        memset(this, 0, sizeof(MsgSyndicate));
        Length = sizeof(MsgSyndicate);
        Type = MSG_SYNDICATE;
        Mode = mode;
        Identity = targetId;
    }
};

enum SyndicateRequest : uint32_t {
    SYNDICATE_JOIN_REQUEST      = 1,
    SYNDICATE_INVITE_REQUEST    = 2,
    SYNDICATE_QUIT              = 3,
    SYNDICATE_QUERY             = 6,
    SYNDICATE_ALLY              = 7,
    SYNDICATE_UNALLY            = 8,
    SYNDICATE_ENEMY             = 9,
    SYNDICATE_UNENEMY           = 10,
    SYNDICATE_DONATE_SILVERS    = 11,
    SYNDICATE_REFRESH           = 12,
    SYNDICATE_DISBAND           = 19,
    SYNDICATE_DONATE_CPS        = 20,
    SYNDICATE_SET_REQUIREMENTS  = 24,
    SYNDICATE_BULLETIN          = 27,
    SYNDICATE_PROMOTION         = 28,
    SYNDICATE_ACCEPT_REQUEST    = 29,
    SYNDICATE_DISCHARGE         = 30,
    SYNDICATE_RESIGN            = 32,
    SYNDICATE_DISCHARGE2        = 33,
    SYNDICATE_PAID_PROMOTION    = 34,
    SYNDICATE_DISCHARGE_PAID    = 36,
    SYNDICATE_PROMOTION_LIST    = 37,
    SYNDICATE_DENY_JOIN_REQUEST = 48
};

#pragma pack(pop)

// Helper functions for creating packets
namespace PacketBuilder {
    inline MsgAction CreateJumpPacket(uint32_t playerId, uint16_t destX, 
                                       uint16_t destY, uint16_t currentX,
                                       uint16_t currentY, uint16_t direction = 0) 
    {
        MsgAction msg;
        msg.Init(playerId, ACTION_MAP_JUMP);
        msg.SetCommandXY(destX, destY);
        msg.SetArgumentXY(currentX, currentY);
        msg.Direction = direction;
        msg.X = currentX;
        msg.Y = currentY;
        return msg;
    }
    
    inline MsgInteract CreateAttackPacket(uint32_t sender, uint32_t target,
                                           uint16_t x, uint16_t y) 
    {
        MsgInteract msg;
        msg.InitAttack(sender, target, x, y);
        return msg;
    }
    
    inline MsgItem CreateUseItemPacket(uint32_t itemId) 
    {
        MsgItem msg;
        msg.InitUseItem(itemId);
        return msg;
    }
    
    inline MsgItem CreatePingPacket() 
    {
        MsgItem msg;
        msg.InitPing();
        return msg;
    }
    
    inline MsgVipUserHandle CreateVipTeleportPacket(int32_t locationId) 
    {
        MsgVipUserHandle msg;
        msg.InitSelfTeleport(locationId);
        return msg;
    }
}
