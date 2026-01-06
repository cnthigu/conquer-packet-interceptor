// packet_viewer.cpp - ImGui Interface for Packet Visualization

#include "packet_viewer.h"
#include "../network/packet_hook.h"
#include "../network/GameProtocol.h"
#include "../dx9/dx9_hook.h"
#include "../imgui/imgui.h"
#include <cstdio>
#include <cstring>
#include <cctype>
#include <vector>

static char s_lastInjectStatus[128] = "";
static int s_lastInjectIndex = -1;
static int s_selectedFilter = 0;

struct PacketFilter {
    const char* name;
    uint16_t type;
};

static const PacketFilter PACKET_FILTERS[] = 
{
    { "All", 0 },
    { "MsgRegister", MSG_REGISTER },
    { "MsgTalk", MSG_TALK },
    { "MsgWalkOld", MSG_WALK_OLD },
    { "MsgUserInfo", MSG_USER_INFO },
    { "MsgItemInfo", MSG_ITEM_INFO },
    { "MsgItem", MSG_ITEM },
    { "MsgActionOld", MSG_ACTION_OLD },
    { "MsgTick", MSG_TICK },
    { "MsgPlayerOld", MSG_PLAYER_OLD },
    { "MsgName", MSG_NAME },
    { "MsgWeather", MSG_WEATHER },
    { "MsgUserAttribOld", MSG_USER_ATTRIB_OLD },
    { "MsgFriend", MSG_FRIEND },
    { "MsgInteract", MSG_INTERACT },
    { "MsgTeam", MSG_TEAM },
    { "MsgAllot", MSG_ALLOT },
    { "MsgWeaponSkillOld", MSG_WEAPON_SKILL_OLD },
    { "MsgTeamMember", MSG_TEAM_MEMBER },
    { "MsgGemEmbed", MSG_GEM_EMBED },
    { "MsgData", MSG_DATA },
    { "MsgPing", MSG_PING },
    { "MsgWeaponSkill", MSG_WEAPON_SKILL },
    { "MsgLoadMap", MSG_LOAD_MAP },
    { "MsgConnect", MSG_CONNECT },
    { "MsgConnectEx", MSG_CONNECT_EX },
    { "MsgMapItem", MSG_MAP_ITEM },
    { "MsgMagicInfo", MSG_MAGIC_INFO },
    { "MsgMagicEffect", MSG_MAGIC_EFFECT },
    { "MsgSyndicate", MSG_SYNDICATE },
    { "MsgNpcInfoEx", MSG_NPC_INFO_EX },
    { "MsgVipUserHandle", MSG_VIP_USER_HANDLE },
    { "MsgNpcInfo", MSG_NPC_INFO },
    { "MsgNpc", MSG_NPC },
    { "MsgTaskDialog", MSG_TASK_DIALOG },
    { "MsgNobility", MSG_NOBILITY },
    { "MsgTransportor", MSG_TRANSPORTOR },
    { "MsgWalk", MSG_WALK },
    { "MsgAction", MSG_ACTION },
    { "MsgPlayer", MSG_PLAYER },
    { "MsgUserAttrib", MSG_USER_ATTRIB }
};

static const int FILTER_COUNT = sizeof(PACKET_FILTERS) / sizeof(PACKET_FILTERS[0]);

static const char* GetFilterName(void*, int index)
{
    return PACKET_FILTERS[index].name;
}

static bool PassesFilter(uint16_t packetType)
{
    return s_selectedFilter == 0 || packetType == PACKET_FILTERS[s_selectedFilter].type;
}

static std::vector<uint8_t> HexStringToBytes(const std::string& hexStr)
{
    std::vector<uint8_t> bytes;
    const char* ptr = hexStr.c_str();

    while (*ptr) 
    {
        while (*ptr && (isspace(*ptr) || *ptr == ',' || *ptr == '-' || *ptr == ':')) 
            ptr++;
        if (!*ptr) break;

        char hexByte[3] = { 0 };
        if (isxdigit(ptr[0])) 
        {
            hexByte[0] = ptr[0];
            ptr++;
            if (isxdigit(ptr[0])) 
            {
                hexByte[1] = ptr[0];
                ptr++;
            } else {
                hexByte[1] = hexByte[0];
                hexByte[0] = '0';
            }
            bytes.push_back(static_cast<uint8_t>(strtol(hexByte, nullptr, 16)));
        } else {
            ptr++;
        }
    }
    return bytes;
}

static void CopyTextToClipboard(const char* text)
{
    if (!OpenClipboard(nullptr)) 
        return;

    EmptyClipboard();

    size_t len = strlen(text) + 1;
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
    if (hMem) {
        char* pMem = static_cast<char*>(GlobalLock(hMem));
        if (pMem) {
            memcpy(pMem, text, len);
            GlobalUnlock(hMem);
            SetClipboardData(CF_TEXT, hMem);
        }
    }
    CloseClipboard();
}

static void TryInjectPacket(const std::string& hexData, int historyIndex)
{
    std::vector<uint8_t> bytes = HexStringToBytes(hexData);

    if (bytes.size() < 4) 
    {
        snprintf(s_lastInjectStatus, sizeof(s_lastInjectStatus), "Invalid packet!");
        return;
    }

    if (!PacketHook_CanInject()) 
    {
        snprintf(s_lastInjectStatus, sizeof(s_lastInjectStatus), "Not ready! Do something in game first.");
        return;
    }

    uint16_t pktType = *reinterpret_cast<uint16_t*>(&bytes[2]);

    if (PacketHook_Inject(bytes.data(), static_cast<int>(bytes.size()))) 
    {
        snprintf(s_lastInjectStatus, sizeof(s_lastInjectStatus),
            "OK! %s (%zu bytes)", PacketHook_GetTypeName(pktType), bytes.size());
        s_lastInjectIndex = historyIndex;
    } 
    else 
    {
        snprintf(s_lastInjectStatus, sizeof(s_lastInjectStatus),
            "FAILED: %s", PacketHook_GetLastError());
    }
}

static void RenderPacketStatusBar()
{
    if (PacketHook_CanInject()) 
    {
        ImGui::Text("READY");
    } 
    else 
    {
        ImGui::Text("Waiting...");
    }

    ImGui::SameLine();
    ImGui::Text("| Packets: %u", PacketHook_GetTotalCount());

    if (PacketHook_GetCharacterId() != 0) 
    {
        ImGui::SameLine();
        ImGui::Text("| ID: %u", PacketHook_GetCharacterId());
    }
}

static void RenderPacketControls(const std::vector<PacketLogEntry>& history)
{
    if (ImGui::Button("Clear")) 
    {
        PacketHook_ClearHistory();
        s_lastInjectStatus[0] = '\0';
        s_lastInjectIndex = -1;
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(150);
    ImGui::Combo("##Filter", &s_selectedFilter, GetFilterName, nullptr, FILTER_COUNT);

    if (!history.empty()) 
    {
        ImGui::SameLine();
        if (ImGui::Button("Use Last")) 
        {
            for (int i = static_cast<int>(history.size()) - 1; i >= 0; i--) 
            {
                if (PassesFilter(history[i].type)) {
                    TryInjectPacket(history[i].hexData, i);
                    break;
                }
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Copy Last"))
        {
            for (int i = static_cast<int>(history.size()) - 1; i >= 0; i--) 
            {
                if (PassesFilter(history[i].type)) 
                {
                    CopyTextToClipboard(history[i].hexData.c_str());
                    break;
                }
            }
        }
    }

    ImGui::SameLine();
    ImGui::TextDisabled("(%zu packets)", history.size());
}

static void RenderPacketList(const std::vector<PacketLogEntry>& history)
{
    ImGui::BeginChild("PacketLog", ImVec2(0, 0), true);

    int displayed = 0;
    const int MAX_DISPLAY = 50;

    for (int i = static_cast<int>(history.size()) - 1; i >= 0 && displayed < MAX_DISPLAY; i--) 
    {
        const PacketLogEntry& entry = history[i];

        if (!PassesFilter(entry.type)) 
            continue;
        displayed++;

        ImGui::PushID(i);

        if (ImGui::SmallButton("Use")) 
        {
            TryInjectPacket(entry.hexData, i);
        }
        ImGui::SameLine();

        if (ImGui::SmallButton("Copy")) 
        {
            CopyTextToClipboard(entry.hexData.c_str());
        }
        ImGui::SameLine();

        if (i == s_lastInjectIndex) 
        {
            ImGui::Text(">>");
            ImGui::SameLine();
        }

        ImGui::Text("%s", PacketHook_GetTypeName(entry.type));
        ImGui::SameLine();
        ImGui::Text("(%d) Len=%d", entry.type, entry.length);
        ImGui::Text("  %s%s", entry.hexPreview.c_str(), entry.length > 48 ? "..." : "");

        ImGui::PopID();
    }

    ImGui::EndChild();
}

static void RenderPacketSnifferTab()
{
    if (!PacketHook_CanInject()) 
    {
        PacketHook_Init();
    }

    RenderPacketStatusBar();
    ImGui::Separator();

    if (s_lastInjectStatus[0] != '\0')
    {
        ImGui::Text(">> %s", s_lastInjectStatus);
        ImGui::Separator();
    }

    std::vector<PacketLogEntry> history = PacketHook_GetHistory();
    RenderPacketControls(history);
    RenderPacketList(history);
}

static void RenderAboutTab()
{
    ImGui::Text("conquer-packet-interceptor v1.0 by Carniato");

    ImGui::Separator();
    ImGui::Text("github.com/cnthigu");

    ImGui::Separator();
    ImGui::Text("If you find this project useful, please consider");
    ImGui::Text("giving it a star on GitHub. It helps a lot!");
}

void RenderImGuiInterface()
{
    if (!IsImGuiWindowOpen()) 
        return;

    ImGui::SetNextWindowSize(ImVec2(520, 380), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);

    ImGui::Begin("Packet Interceptor by Carniato", nullptr, ImGuiWindowFlags_None);

    if (ImGui::BeginTabBar("MainTabs", ImGuiTabBarFlags_None)) 
    {
        if (ImGui::BeginTabItem("Packets")) 
        {
            RenderPacketSnifferTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("About")) 
        {
            RenderAboutTab();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}
