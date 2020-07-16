#include <sysinfoapi.h>
#include <winioctl.h>
#include <ioapiset.h>
#include <fileapi.h>
#include  <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdio.h>
#include <Winnls.h>
#pragma comment(lib,"Kernel32.lib")


BOOL checkCPUTemperature();
BOOL language();
BOOL checkthreatbook();
BOOL checkMemory(INT memory);
BOOL checkUptime(DWORD msTime);
BOOL checkoffice(INT aNum);


