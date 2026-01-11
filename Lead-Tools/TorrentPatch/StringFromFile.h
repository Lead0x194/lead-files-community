#pragma once

bool EL_StringFromFileW( std::string& self, MA_LPCWSTR filePathz, Earth::EA_CStatus* pcStatus = NULL );
bool EL_StringFromDiskFileW( std::string& self, MA_LPCWSTR filePathz, Earth::EA_CStatus* pcStatus = NULL );
bool EL_StringFromZipFileW( std::string& self, MA_LPCWSTR filePathz, Earth::EA_CStatus* pcStatus = NULL );
