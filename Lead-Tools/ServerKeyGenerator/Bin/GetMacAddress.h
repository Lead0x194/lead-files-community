#pragma once

#pragma comment(lib, "Iphlpapi.lib")

#include <Iphlpapi.h>
#include <string>

// adapter 중에 찾는 MAC address가 있는지 확인하고 ip리턴
// macAddress : AABBCCDDEEFF의 hex string 또는 AA-BB-CC-DD-EE-FF (대문자)
// 리턴값 : 해당 adapter에 할당된 ip address
static std::string GetIpAddressWithMacAddress(const std::string& macAddress)
{
	// adapter 정보를 최대 2개까지만 가져오자
	IP_ADAPTER_INFO AdapterInfo[2];       // Allocate information 

	// for up to 2 NICs
	DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer

	DWORD dwStatus = GetAdaptersInfo(      // Call GetAdapterInfo
		AdapterInfo,                 // [out] buffer to receive data
		&dwBufLen);                  // [in] size of receive data buffer
	assert(dwStatus == ERROR_SUCCESS);  // Verify return value is 
	// valid, no buffer overflow

	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo; // Contains pointer to
	// current adapter info
	do {
		// get mac address string
		char buf[64], buf1[64];
		sprintf_s(buf, sizeof(buf), "%02X%02X%02X%02X%02X%02X",
			pAdapterInfo->Address[0], pAdapterInfo->Address[1], pAdapterInfo->Address[2], pAdapterInfo->Address[3], pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
		sprintf_s(buf1, sizeof(buf1), "%02X-%02X-%02X-%02X-%02X-%02X",
			pAdapterInfo->Address[0], pAdapterInfo->Address[1], pAdapterInfo->Address[2], pAdapterInfo->Address[3], pAdapterInfo->Address[4], pAdapterInfo->Address[5]);

		if (macAddress == buf || macAddress == buf1)
		{
			return pAdapterInfo->IpAddressList.IpAddress.String;
		}

		pAdapterInfo = pAdapterInfo->Next;    // Progress through 
		// linked list
	}
	while(pAdapterInfo);                    // Terminate if last adapter

	// not found
	return "";
}

// adapter 중에 찾는 ip address가 있는지 확인하고 mac리턴
// 리턴값(macAddress) : AABBCCDDEEFF의 hex string 또는 AA-BB-CC-DD-EE-FF (대문자)
static std::string GetMacAddressWithIpAddress(const char* ip)
{
	// adapter 정보를 최대 10개까지만 가져오자
	IP_ADAPTER_INFO AdapterInfo[10];       // Allocate information 

	// for up to 10 NICs
	DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer

	DWORD dwStatus = GetAdaptersInfo(      // Call GetAdapterInfo
		AdapterInfo,                 // [out] buffer to receive data
		&dwBufLen);                  // [in] size of receive data buffer
	assert(dwStatus == ERROR_SUCCESS);  // Verify return value is 
	// valid, no buffer overflow

	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo; // Contains pointer to
	// current adapter info
	do {
		if (strcmp(pAdapterInfo->IpAddressList.IpAddress.String, ip) == 0)
		{
			// get mac address string
			char buf[64];
			sprintf_s(buf, sizeof(buf), "%02X-%02X-%02X-%02X-%02X-%02X",
				pAdapterInfo->Address[0], pAdapterInfo->Address[1], pAdapterInfo->Address[2], pAdapterInfo->Address[3], pAdapterInfo->Address[4], pAdapterInfo->Address[5]);

			return buf;
		}

		pAdapterInfo = pAdapterInfo->Next;    // Progress through 
		// linked list
	}
	while(pAdapterInfo);                    // Terminate if last adapter

	// not found
	return "";
}
