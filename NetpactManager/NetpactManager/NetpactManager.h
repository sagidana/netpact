#pragma once
#include <windows.h>
#include <stdio.h>

class NetpactManager 
{
private:
	PCWSTR _DriverName;
	PCWSTR _DriverPath;
	BOOL _IsDriverInstalled;
	BOOL _IsDriverLoaded;

	BOOL installDriver();
	BOOL uninstallDriver();
public:
	NetpactManager();
	BOOL LoadNetpactDriver();
	BOOL UnloadNetpactDriver();
	~NetpactManager();
};