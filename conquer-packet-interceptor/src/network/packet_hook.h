#pragma once
// packet_hook.h - Packet Interception Hook
// 
// SendPacket hook intercepts packets BEFORE encryption (plain text).
// Found via IDA Pro XREF tracing from Winsock send() → encryption wrapper → SendPacket
// Address: 0x007414F0 (Conquer Online version 6609)

#include "GameProtocol.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <cstdint>
#include <vector>
#include <string>

// SendPacket function address (found via reverse engineering)
constexpr uintptr_t SEND_PACKET_ADDRESS = 0x007414F0;
constexpr size_t MAX_PACKET_HISTORY = 100;

// Captured packet entry structure
struct PacketLogEntry {
    std::string hexData;     // Full packet in hexadecimal
    std::string hexPreview;  // Preview (first 48 bytes)
    uint16_t type;           // Packet type (offset 2-3)
    uint16_t length;         // Packet length (offset 0-1)
};

// Public API
bool PacketHook_Init();
void PacketHook_Shutdown();
bool PacketHook_CanInject();
bool PacketHook_Inject(const void* data, int len);
std::vector<PacketLogEntry> PacketHook_GetHistory();
void PacketHook_ClearHistory();
const char* PacketHook_GetLastError();
uint32_t PacketHook_GetTotalCount();
uint32_t PacketHook_GetCharacterId();
const char* PacketHook_GetTypeName(uint16_t type);
std::string PacketHook_BufferToHex(const uint8_t* buf, int len);
