#include <windows.h>
#include <winioctl.h>
#include <stdio.h>

int main()
{
    TCHAR drive[] = TEXT("\\\\.\\PhysicalDrive0"); // 第一个物理硬盘的路径
    HANDLE hDrive = CreateFile(drive, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDrive == INVALID_HANDLE_VALUE) {
        printf("Failed to open drive, error code: %d\n", GetLastError());
        return 1;
    }

    STORAGE_PROPERTY_QUERY query;
    ZeroMemory(&query, sizeof(query));
    query.PropertyId = StorageDeviceProperty;
    query.QueryType = PropertyStandardQuery;

    STORAGE_DESCRIPTOR_HEADER header;
    ZeroMemory(&header, sizeof(header));
    DWORD bytesReturned;
    if (!DeviceIoControl(hDrive, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(query), &header, sizeof(header), &bytesReturned, NULL)) {
        printf("Failed to query drive property, error code: %d\n", GetLastError());
        CloseHandle(hDrive);
        return 1;
    }

    char* buffer = new char[header.Size];
    if (!DeviceIoControl(hDrive, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(query), buffer, header.Size, &bytesReturned, NULL)) {
        printf("Failed to query drive property, error code: %d\n", GetLastError());
        CloseHandle(hDrive);
        delete[] buffer;
        return 1;
    }

    STORAGE_DEVICE_DESCRIPTOR* deviceDescriptor = (STORAGE_DEVICE_DESCRIPTOR*)buffer;
    char* serialNumber = buffer + deviceDescriptor->SerialNumberOffset;
    printf("Serial number: %s\n", serialNumber);

    CloseHandle(hDrive);
    delete[] buffer;

    return 0;
}
