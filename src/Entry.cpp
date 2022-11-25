#include "CommonIncludes.hpp"
#include "gta/Include.hpp"
#include "memory/Include.hpp"

void entryPoint()
{
	static auto sec = mem::scanForPtr("48 8B ? ? ? ? ? 33 F6 E9 ? ? ? ? 55 48 8D ? ? ? ? ? 48 87 2C 24 C3 48 8B 45 50 0F B6 00").add(3).relative().as<rage::atSingleton<rage::RageSecurity>*>();
	
	while (gta::g_Running)
	{
		if (auto s = sec->getInstance(); sec->isValid())
		{
			s->m_interval = std::numeric_limits<uint32_t>::max();
		}

		std::this_thread::yield();
	}
}

BOOL APIENTRY DllMain(const HMODULE hModule, const DWORD reasonForCall, [[maybe_unused]] LPCVOID lpReserved)
{
	if (reasonForCall == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);

		if (const auto handle = CreateThread(nullptr, 0, [](LPVOID mod) -> DWORD
		{
			try
			{
				entryPoint();
			}
			catch (...)
			{
				MessageBoxA(nullptr, "An exception occured while disabling sig scanner!", "gta-antisig", MB_OK);
			}

			FreeLibraryAndExitThread(HMODULE(mod), EXIT_SUCCESS);

		}, hModule, 0, nullptr); handle != nullptr)
		{
			CloseHandle(handle);
		}
	}

	return TRUE;
}