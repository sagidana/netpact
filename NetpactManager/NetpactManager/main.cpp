#include <Windows.h>
#include "NetpactManager.h"


int main()
{
	NetpactManager netpactManager;

	netpactManager.LoadNetpactDriver();
	


	netpactManager.UnloadNetpactDriver();

	return 0;
}	