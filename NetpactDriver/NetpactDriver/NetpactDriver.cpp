#include <ntddk.h>

#define NETPACT_PREFIX "Netpact: "


void NetpactUnload(PDRIVER_OBJECT driverObject);

extern "C"
NTSTATUS DriverEntry(PDRIVER_OBJECT driverObject, PUNICODE_STRING)
{
	PDEVICE_OBJECT deviceObject;
	UNICODE_STRING devName, win32Name;

	driverObject->DriverUnload = NetpactUnload;

	RtlInitUnicodeString(&devName, L"\\Device\\Netpact");

	NTSTATUS status = IoCreateDevice(driverObject, 
									0, 
									&devName, 
									FILE_DEVICE_UNKNOWN, 
									0, 
									FALSE, 
									&deviceObject);
	if (status != STATUS_SUCCESS)
	{
		KdPrint((NETPACT_PREFIX "Error creating device..."));
		return status;
	}

	RtlInitUnicodeString(&win32Name, L"\\??\\Netpact");
	IoCreateSymbolicLink(&win32Name, &devName);

	return STATUS_SUCCESS;
}

void NetpactUnload(PDRIVER_OBJECT driverObject)
{
	UNICODE_STRING win32Name;
	RtlInitUnicodeString(&win32Name, L"\\??\\Netpact");

	IoDeleteSymbolicLink(&win32Name);
	IoDeleteDevice(driverObject->DeviceObject);
}