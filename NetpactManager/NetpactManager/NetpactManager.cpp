#include "NetpactManager.h"


/* 
------------------------- Public Methods -------------------------
*/

NetpactManager::NetpactManager()
{
	this->_DriverName = L"Netpact";
	this->_DriverPath = L"D:\\Netpact\\NetpactManager\\x64\\Debug\\NetpactDriver.sys";
	this->_IsDriverLoaded = FALSE;

	this->_IsDriverInstalled = NetpactManager::installDriver();

	if (this->_IsDriverInstalled)
		printf("Driver installed.\n");
}

NetpactManager::~NetpactManager()
{
	if (!this->_IsDriverInstalled)
		return;

	this->_IsDriverInstalled = !NetpactManager::uninstallDriver();

	if (!this->_IsDriverInstalled)
		printf("Driver uninstalled.\n");
}

BOOL NetpactManager::LoadNetpactDriver()
{
	if (!this->_IsDriverInstalled)
		return FALSE;
	if (this->_IsDriverLoaded)
		return TRUE;

	SC_HANDLE hScManager = OpenSCManager(NULL,
										NULL, 
										SC_MANAGER_ALL_ACCESS);
	
	if (hScManager == NULL)
	{
		printf("%s -> Error (%d): Unable to open SCManager.\n", __FUNCTION__, GetLastError());
		return FALSE;
	}

	SC_HANDLE hService = OpenService(	hScManager, 
										this->_DriverName,
										SERVICE_ALL_ACCESS);

	if (hService == NULL)
	{
		printf("%s -> Error (%d): Unable to open service.\n", __FUNCTION__, GetLastError());

		CloseServiceHandle(hScManager);
		return FALSE;
	}

	if (StartService(hService, 0, NULL) == NULL)
	{
		printf("%s -> Error (%d): Unable to start a service.\n", __FUNCTION__, GetLastError());
		
		CloseServiceHandle(hScManager);
		CloseServiceHandle(hService);

		return FALSE;
	}

	this->_IsDriverLoaded = TRUE;
	
	CloseServiceHandle(hScManager);
	CloseServiceHandle(hService);

	return TRUE;
}


BOOL NetpactManager::UnloadNetpactDriver()
{
	if (!this->_IsDriverInstalled)
		return TRUE;
	if (!this->_IsDriverLoaded)
		return TRUE;

	SC_HANDLE hScManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hScManager == NULL)
	{
		printf("%s -> Error (%d): Unable to open SCManager\n", __FUNCTION__, GetLastError());
		return FALSE;
	}

	SC_HANDLE hService = OpenService(hScManager,
									this->_DriverName,
									SERVICE_ALL_ACCESS);

	if (hService == NULL)
	{
		printf("%s -> Error(%d): Unable to open service.\n", __FUNCTION__, GetLastError());
		
		CloseServiceHandle(hScManager);
		return FALSE;
	}

	SERVICE_STATUS ss;
	if (ControlService(hService, SERVICE_CONTROL_STOP, &ss) == NULL)
	{
		printf("%s -> Error (%d): Unable to stop the service.\n", __FUNCTION__, GetLastError());

		CloseServiceHandle(hScManager);
		CloseServiceHandle(hService);

		return FALSE;
	}

	CloseServiceHandle(hScManager);
	CloseServiceHandle(hService);

	this->_IsDriverLoaded = FALSE;
	return TRUE;
}

/*
------------------------- Private Methods -------------------------
*/

BOOL NetpactManager::installDriver()
{
	SC_HANDLE hScManager = OpenSCManager(	NULL,
											NULL,
											SC_MANAGER_ALL_ACCESS);
	if (hScManager == NULL)
	{
		printf("%s -> Error (%d): Unable to open SCManager\n", __FUNCTION__, GetLastError());
		return FALSE;
	}

	SC_HANDLE hService = CreateService(		hScManager,				
											this->_DriverName,
											this->_DriverName,
											SERVICE_ALL_ACCESS,    
											SERVICE_KERNEL_DRIVER, 
											SERVICE_DEMAND_START,  
											SERVICE_ERROR_IGNORE,  
											this->_DriverPath,
											NULL,                  
											NULL,                  
											NULL,                  
											NULL,                  
											NULL);

	if (hService == NULL)
	{
		hService = OpenService(hScManager, this->_DriverName, SERVICE_ALL_ACCESS);
		
		if (hService == NULL)
		{
			printf("%s -> Error (%d): Unable to open service.\n", __FUNCTION__, GetLastError());

			CloseServiceHandle(hScManager);
			return FALSE;
		}
		else
			CloseServiceHandle(hService);
	}

	CloseServiceHandle(hScManager);
	return TRUE;
}

BOOL NetpactManager::uninstallDriver()
{
	if (!this->_IsDriverInstalled)
		return TRUE;
	
	if (this->_IsDriverLoaded)
		this->UnloadNetpactDriver();

	SC_HANDLE hScManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hScManager == NULL)
	{
		printf("%s -> Error (%d): Unable to open SCManager\n", __FUNCTION__, GetLastError());
		return FALSE;
	}

	SC_HANDLE hService = OpenService(hScManager,
		this->_DriverName,
		SERVICE_ALL_ACCESS);

	if (hService == NULL)
	{
		printf("%s -> Error (%d): Unable to open service.\n", __FUNCTION__, GetLastError());

		CloseServiceHandle(hScManager);
		return FALSE;
	}

	DeleteService(hService);
	
	CloseServiceHandle(hScManager);
	CloseServiceHandle(hService);

	return TRUE;
}

