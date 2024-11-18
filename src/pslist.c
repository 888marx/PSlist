#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <psapi.h>
#include <string.h>

void PrintHelp();
void ListThreadDetails(DWORD processID);
void ListProcAndThreads(char* processName);

void PrintHelp() {j
    printf("Usage:\n");
    printf("  plist.exe                - List all processes\n");
    printf("  plist.exe <name>         - List details for the specified process name\n");
    printf("  plist.exe -d <PID>       - Show thread details for the specified process ID\n");
    printf("  plist.exe -h             - Show this help message\n");
}

void ListThreadDetails(DWORD processID) {
    HANDLE hThreadSnap;
    THREADENTRY32 te32;

    hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE) {
        printf("CreateToolhelp32Snapshot (of threads) failed.\n");
        return;
    }

    te32.dwSize = sizeof(THREADENTRY32);

    if (!Thread32First(hThreadSnap, &te32)) {
        printf("Thread32First failed.\n");
        CloseHandle(hThreadSnap);
        return;
    }

    printf("\nDetails for PID %lu:\n", processID);
    printf("%-25s %-20s %-20s %-20s %-20s\n", "Thread ID", "Base Priority", "User Time", "CSwitch", "State");

    do {
        if (te32.th32OwnerProcessID == processID) {
            HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, te32.th32ThreadID);
            FILETIME createTime, exitTime, kernelTime, userTime;
            if (hThread && GetThreadTimes(hThread, &createTime, &exitTime, &kernelTime, &userTime)) {
                ULARGE_INTEGER li;
                li.LowPart = userTime.dwLowDateTime;
                li.HighPart = userTime.dwHighDateTime;
                double userTimeInSeconds = li.QuadPart / 10000000.0;
                printf("%-25lu %-20d %-20.2f %-20s %-20s\n", te32.th32ThreadID, te32.tpBasePri, userTimeInSeconds, "N/A", "N/A");
            } else {
                printf("%-25lu %-20d %-20s %-20s %-20s\n", te32.th32ThreadID, te32.tpBasePri, "Error", "N/A", "N/A");
            }
            if (hThread) CloseHandle(hThread);
        }
    } while (Thread32Next(hThreadSnap, &te32));

    CloseHandle(hThreadSnap);
}

void ListProcAndThreads(char* processName) {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        printf("CreateToolhelp32Snapshot (of processes) failed.\n");
        return;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32)) {
        printf("Process32First failed.\n");
        CloseHandle(hProcessSnap);
        return;
    }

    printf("%-25s %-10s %-10s %-10s %-10s %-10s\n", "Name", "PID", "Pri", "Thd", "Hnd", "Priv");

    do {
        char processNameExe[256];
        if (processName != NULL) {
            strcpy(processNameExe, processName);
            if (!strstr(processName, ".exe")) {
                strcat(processNameExe, ".exe");
            }
        }

        if (processName != NULL && strcmp(pe32.szExeFile, processNameExe) != 0) {
            continue;
        }

        DWORD handleCount = 0;
        PROCESS_MEMORY_COUNTERS pmc = {0};
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
        if (hProcess != NULL) {
            GetProcessHandleCount(hProcess, &handleCount);
            GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc));
            CloseHandle(hProcess);
        }

        printf("%-25s %-10d %-10d %-10d %-10d %-10lu\n", pe32.szExeFile, pe32.th32ProcessID, pe32.pcPriClassBase, pe32.cntThreads, handleCount, pmc.PagefileUsage);

    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        if (strcmp(argv[1], "-h") == 0) {
            PrintHelp();
        } else {
            ListProcAndThreads(argv[1]);
        }
    } else if (argc == 3 && strcmp(argv[1], "-d") == 0) {
        DWORD pid = strtoul(argv[2], NULL, 10);
        if (pid > 0) {
            ListThreadDetails(pid);
        } else {
            printf("Invalid PID.\n");
        }
    } else {
        ListProcAndThreads(NULL);
    }
    return 0;
}
