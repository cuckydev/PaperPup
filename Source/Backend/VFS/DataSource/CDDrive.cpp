#include "Backend/VFS/DataSource/CDDrive.h"

#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>
#include <string>

#include "Types/Exceptions.h"

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

namespace PaperPup::VFS::DataSource
{

//
class CDDrive_Impl : public CDDrive
{
private:
	// Thread
	std::thread thread;
	std::atomic<bool> thread_quit;

	bool ThreadSleep(int ms)
	{
		auto end = std::chrono::steady_clock::now() + std::chrono::milliseconds(ms);
		while (std::chrono::steady_clock::now() < end)
		{
			if (thread_quit)
				return true;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		return thread_quit;
	}

	std::basic_string<TCHAR> root_directory;

	bool Scan()
	{
		while (1)
		{
			// Get all connected logical drives
			auto size = GetLogicalDriveStrings(0, nullptr);
			if (size == 0)
				throw Types::Win32Exception();

			std::basic_string<TCHAR> string(static_cast<size_t>(size), '0');
			if (GetLogicalDriveStrings(size, const_cast<TCHAR *>(string.data())) == 0)
				throw Types::Win32Exception();

			const TCHAR *p = string.data();
			while (*p != '\0')
			{
				// Drive must be a CDROM drive
				auto drive_type = GetDriveType(p);
				if (drive_type == DRIVE_CDROM)
				{
					// Get the volume name, must be "PLAYSTATION"
					TCHAR name_buffer[MAX_PATH + 1];
					TCHAR fs_buffer[MAX_PATH + 1];
					if (GetVolumeInformation(p, name_buffer, MAX_PATH + 1, nullptr, nullptr, nullptr, fs_buffer, MAX_PATH + 1) != 0)
					{
						if (!lstrcmp(name_buffer, TEXT("PLAYSTATION")))
						{
							std::basic_string<TCHAR> drive_name = std::basic_string<TCHAR>(TEXT("\\\\.\\")) + p;
							drive_name.pop_back();

							HANDLE hDisk = CreateFile(drive_name.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_READ_ATTRIBUTES, nullptr);
							if (hDisk == INVALID_HANDLE_VALUE)
								throw Types::Win32Exception();
						}
					}
				}

				// Skip to next name
				while (*p++ != '\0');
			}

			if (ThreadSleep(1000))
				return true;
		}

		// return thread_quit;
	}

	void Thread()
	{
		while (1)
		{
			if (Scan()) return;
		}
	}

public:
	CDDrive_Impl()
	{
		// thread = std::thread([this]()
		// 	{
		// 		Thread();
		// 	}
		// );
	}
	~CDDrive_Impl() override
	{
		// Stop thread
		// thread_quit = true;
		// thread.join();
	}
};

CDDrive &CDDrive::Instance()
{
	static CDDrive_Impl instance;
	return instance;
}

}
