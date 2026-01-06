// packet_hook.cpp - Packet Interception Hook Implementation
//
// HookedSendPacket intercepts packets BEFORE encryption (plain text).
// For injection, we call the original function - game encrypts automatically.

#include "packet_hook.h"
#include "../libs/minhook/include/MinHook.h"
#include <sstream>
#include <iomanip>
#include <mutex>

static bool s_isInitialized = false;
static char s_lastError[256] = "";
static uint32_t s_totalPackets = 0;
static uint32_t s_characterId = 0;
static LPVOID s_originalSendPacket = nullptr;
static void* s_networkPtr = nullptr;
static std::vector<PacketLogEntry> s_packetHistory;
static std::mutex s_historyMutex;

typedef int(__fastcall* SendPacketFunc)(void* thisPtr, void* edx, void* data, int len);

static int __fastcall HookedSendPacket(void* thisPtr, void* edx, void* data, int len)
{
    if (thisPtr != nullptr) 
        s_networkPtr = thisPtr;

    if (data != nullptr && len >= 4) 
    {
        std::lock_guard<std::mutex> lock(s_historyMutex);

        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        PacketLogEntry entry;
        entry.length = *reinterpret_cast<const uint16_t*>(&bytes[0]);
        entry.type = *reinterpret_cast<const uint16_t*>(&bytes[2]);
        entry.hexData = PacketHook_BufferToHex(bytes, len);
        entry.hexPreview = PacketHook_BufferToHex(bytes, len > 48 ? 48 : len);

        if (s_packetHistory.size() >= MAX_PACKET_HISTORY) 
            s_packetHistory.erase(s_packetHistory.begin());
        
        s_packetHistory.push_back(entry);
        s_totalPackets++;

        // Extract character ID from MsgAction (type MSG_ACTION, offset 8)
        if (entry.type == MSG_ACTION && len >= 12) 
        {
            uint32_t id = *reinterpret_cast<const uint32_t*>(&bytes[8]);
            if (id > 1000000 && id < 2000000000) 
                s_characterId = id;
        }
    }

    return reinterpret_cast<SendPacketFunc>(s_originalSendPacket)(thisPtr, edx, data, len);
}

bool PacketHook_Init()
{
    if (s_isInitialized) 
        return true;

    LPVOID target = reinterpret_cast<LPVOID>(SEND_PACKET_ADDRESS);

    if (MH_CreateHook(target, reinterpret_cast<LPVOID>(&HookedSendPacket), &s_originalSendPacket) != MH_OK) 
    {
        snprintf(s_lastError, sizeof(s_lastError), "Failed to create hook");
        return false;
    }

    if (MH_EnableHook(target) != MH_OK) 
    {
        snprintf(s_lastError, sizeof(s_lastError), "Failed to enable hook");
        return false;
    }

    s_isInitialized = true;
    snprintf(s_lastError, sizeof(s_lastError), "Hook installed successfully");
    return true;
}

void PacketHook_Shutdown()
{
    if (!s_isInitialized) 
        return;

    LPVOID target = reinterpret_cast<LPVOID>(SEND_PACKET_ADDRESS);
    MH_DisableHook(target);
    MH_RemoveHook(target);
    s_isInitialized = false;
}

bool PacketHook_CanInject()
{
    return s_isInitialized && s_networkPtr != nullptr;
}

bool PacketHook_Inject(const void* data, int len)
{
    if (!s_isInitialized) 
    {
        snprintf(s_lastError, sizeof(s_lastError), "Hook not initialized");
        return false;
    }

    if (s_networkPtr == nullptr) 
    {
        snprintf(s_lastError, sizeof(s_lastError), "Network not ready (do something in game first)");
        return false;
    }

    if (data == nullptr || len < 4) 
    {
        snprintf(s_lastError, sizeof(s_lastError), "Invalid packet");
        return false;
    }

    int result = reinterpret_cast<SendPacketFunc>(s_originalSendPacket)(s_networkPtr, nullptr, const_cast<void*>(data), len);

    if (result > 0) 
    {
        snprintf(s_lastError, sizeof(s_lastError), "Injected %d bytes", len);
        return true;
    }

    snprintf(s_lastError, sizeof(s_lastError), "Injection failed");
    return false;
}

std::vector<PacketLogEntry> PacketHook_GetHistory()
{
    std::lock_guard<std::mutex> lock(s_historyMutex);
    return s_packetHistory;
}

void PacketHook_ClearHistory()
{
    std::lock_guard<std::mutex> lock(s_historyMutex);
    s_packetHistory.clear();
}

const char* PacketHook_GetLastError()
{
    return s_lastError;
}

uint32_t PacketHook_GetTotalCount()
{
    return s_totalPackets;
}

uint32_t PacketHook_GetCharacterId()
{
    return s_characterId;
}

const char* PacketHook_GetTypeName(uint16_t type)
{
    switch (type) 
    {
        case MSG_REGISTER:        return "MsgRegister";
        case MSG_TALK:            return "MsgTalk";
        case MSG_WALK_OLD:        return "MsgWalkOld";
        case MSG_USER_INFO:       return "MsgUserInfo";
        case MSG_ITEM_INFO:       return "MsgItemInfo";
        case MSG_ITEM:            return "MsgItem";
        case MSG_ACTION_OLD:      return "MsgActionOld";
        case MSG_TICK:            return "MsgTick";
        case MSG_PLAYER_OLD:      return "MsgPlayerOld";
        case MSG_NAME:            return "MsgName";
        case MSG_WEATHER:         return "MsgWeather";
        case MSG_USER_ATTRIB_OLD: return "MsgUserAttribOld";
        case MSG_FRIEND:          return "MsgFriend";
        case MSG_INTERACT:        return "MsgInteract";
        case MSG_TEAM:            return "MsgTeam";
        case MSG_ALLOT:           return "MsgAllot";
        case MSG_WEAPON_SKILL_OLD: return "MsgWeaponSkillOld";
        case MSG_TEAM_MEMBER:     return "MsgTeamMember";
        case MSG_GEM_EMBED:       return "MsgGemEmbed";
        case MSG_DATA:            return "MsgData";
        case MSG_PING:            return "MsgPing";
        case MSG_WEAPON_SKILL:    return "MsgWeaponSkill";
        case MSG_LOAD_MAP:        return "MsgLoadMap";
        case MSG_CONNECT:         return "MsgConnect";
        case MSG_CONNECT_EX:      return "MsgConnectEx";
        case MSG_MAP_ITEM:        return "MsgMapItem";
        case MSG_MAGIC_INFO:      return "MsgMagicInfo";
        case MSG_MAGIC_EFFECT:     return "MsgMagicEffect";
        case MSG_SYNDICATE:       return "MsgSyndicate";
        case MSG_NPC_INFO_EX:     return "MsgNpcInfoEx";
        case MSG_VIP_USER_HANDLE: return "MsgVipUserHandle";
        case MSG_NPC_INFO:        return "MsgNpcInfo";
        case MSG_NPC:             return "MsgNpc";
        case MSG_TASK_DIALOG:     return "MsgTaskDialog";
        case MSG_NOBILITY:         return "MsgNobility";
        case MSG_TRANSPORTOR:      return "MsgTransportor";
        case MSG_WALK:             return "MsgWalk";
        case MSG_ACTION:           return "MsgAction";
        case MSG_PLAYER:           return "MsgPlayer";
        case MSG_USER_ATTRIB:      return "MsgUserAttrib";
        default:                   return "Unknown";
    }
}

std::string PacketHook_BufferToHex(const uint8_t* buf, int len)
{
    std::stringstream ss;
    for (int i = 0; i < len; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(buf[i]);
        if (i < len - 1) 
            ss << " ";
    }
    return ss.str();
}
