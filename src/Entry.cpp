#include "CommonIncludes.hpp"
#include "gta/Include.hpp"
#include "memory/Include.hpp"

void patchBonusString()
{
	static auto ptr = mem::scanForPtr("62 6F 6E 75 73 00");

	if (!ptr)
	{
		util::log("didn't find bonus string, already written perhaps?");
		return;
	}

	char str[] = "naahh";

	ptr.write(str, 6);
}

void entryPoint()
{
	static auto tunables = mem::scanForPtr("48 8B 0D ? ? ? ? BF 02 00 00 00 44 8B C7").add(3).relative().as<rage::atSingleton<rage::CTunables>*>();
	
	patchBonusString();

	while (gta::g_Running)
	{
		if (tunables->isValid())
		{
			if (const auto ptr = tunables->getInstance(); ptr)
			{
				ptr->m_bCount = 0;
			}
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