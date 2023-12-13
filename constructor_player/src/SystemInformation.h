#ifndef SYSTEMINFORMATION_H
#define SYSTEMINFORMATION_H

#include <GL/glut.h>

#ifndef WIN32
    #include <sys/utsname.h>
#endif

#ifdef WIN32
#include <intrin.h>



typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
LPFN_ISWOW64PROCESS fnIsWow64Process;
BOOL IsWow64()
{
    BOOL bIsWow64 = FALSE;
    fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(GetModuleHandle(TEXT("kernel32")),"IsWow64Process");
    if(NULL != fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
        {
            //handle error
        }
    }
    return bIsWow64;
}

#endif

void PrintAllInfo(int argc, char **argv)
{
    #ifndef WIN32
	//OS info

		struct utsname uts;
        if(uname(&uts) < 0)
        {
             std::cout  << std::endl;
             std::cout << "uname call failed" << std::endl;
             std::cout  << std::endl;
        }
        else
        {
            std::cout  << std::endl;
            std::cout << "sysname: " << uts.sysname << std::endl;
            std::cout << "nodename: " << uts.nodename << std::endl;
            std::cout << "release: " << uts.release << std::endl;
            std::cout << "version: " << uts.version << std::endl;
            std::cout << "machine: " << uts.machine << std::endl;
            std::cout  << std::endl;
        }
    #endif

	
	bool isWin32=true;
	bool isWin64=false;
	#if _WIN64
		isWin32 =  false;
		isWin64 = true;
	#endif

	#ifdef WIN32
		{
			OSVERSIONINFO osvi;

			ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

			GetVersionEx(&osvi);

			std::cout  << std::endl;
			std::cout << "OS MajorVersion: " << osvi.dwMajorVersion << std::endl;
			std::cout << "OS MinorVersion: " << osvi.dwMinorVersion << std::endl;
		}

		if (	isWin64 == true)
		{
			std::cout << "Is 64 bit exe" << std::endl;
		}
		else
		{
			std::cout << "Is 32 bit exe" << std::endl;
		}

		if (IsWow64()) 
		{
			std::cout << "IsWow64" << std::endl;
		}
		else
		{
			std::cout << "Is NOT Wow64" << std::endl;
		}
		std::cout  << std::endl;
	#endif

	//OpenGL Version
	{
		char *version = NULL;
		char *vendor = NULL;
		char *renderer = NULL;
		char *extensions = NULL;
		GLuint idWindow = 0;
		//int   glutVersion;
		 
		glutInit(&argc, argv);
		glutInitWindowSize(1,1);
		glutInitDisplayMode(GLUT_RGBA);
		idWindow = glutCreateWindow("test");
		glutHideWindow();
		 
		//glutVersion = glutGet(0x01FC);
		version =     (char*)glGetString(GL_VERSION);
		vendor =      (char*)glGetString(GL_VENDOR);
		renderer =    (char*)glGetString(GL_RENDERER);
		extensions =  (char*)glGetString(GL_EXTENSIONS);
		 
		//printf("VERSION=%s\nVENDOR=%s\nRENDERER=%s\nEXTENSIONS=%s\n", version,vendor,renderer,extensions);
		printf("VERSION=%s\nVENDOR=%s\nRENDERER=%s\n", version,vendor,renderer);
 
		glutDestroyWindow(idWindow);

		std::cout  << std::endl;
	}

	//CPU/RAM info
	//For Linux-specific methods, you can access /proc/meminfo (and /proc/cpuinfo, if you can't be bothered to parse cpuid responses).
	#ifdef WIN32
	{
		int CPUInfo[4] = {-1};
		unsigned   nExIds, i =  0;
		char CPUBrandString[0x40];
		// Get the information associated with each extended ID.
		__cpuid(CPUInfo, 0x80000000);
		nExIds = CPUInfo[0];
		for (i=0x80000000; i<=nExIds; ++i)
		{
			__cpuid(CPUInfo, i);
			// Interpret CPU brand string
			if  (i == 0x80000002)
				memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
			else if  (i == 0x80000003)
				memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
			else if  (i == 0x80000004)
				memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
		}
		//string includes manufacturer, model and clockspeed
		std::cout << "CPU Type: " << CPUBrandString << std::endl;


		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);
		std::cout << "Number of Cores: " << sysInfo.dwNumberOfProcessors << std::endl;

		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof (statex);
		GlobalMemoryStatusEx(&statex);
		std::cout << "Total System Memory: " << (statex.ullTotalPhys/1024)/1024 << "MB" << std::endl;
		std::cout << std::endl;
	}
	#endif


    #ifndef WIN32

        //printf("\033[1;32m[ OK ]\033[0m Initializing...\n");
        //printf("\033[1m[INFO]\033[0m CPU name: ");
        system("grep -i --color 'model name' /proc/cpuinfo | uniq | sed -e 's/model name//' |    sed -e 's/: //' | sed -e 's/          //' | sed -e 's/@.*//'");
        printf("\n");
        //printf("\033[1m[INFO]\033[0m CPU speed: ");
        system("grep -i --color 'model name' /proc/cpuinfo | uniq | sed -e 's/.*@//'");
        printf("\n");

    #endif

}

#endif
