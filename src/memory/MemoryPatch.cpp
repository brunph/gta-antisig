#include "MemoryPatch.hpp"

namespace mem
{
	MemoryPatch::~MemoryPatch()
	{
		if (isPatchApplied()) undoPatch();

		VirtualFree(m_originalData, 0, MEM_RELEASE);
	}

	mem::MemoryPatch& MemoryPatch::applyPatch()
	{
		if (isPatchApplied())
			return *this;

		if (m_ptr.isNull())
			return *this;

		m_originalData = VirtualAlloc(nullptr, m_patch.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		if (!m_originalData)
		{
			throw std::exception("Patch allocation was invalid");
			return *this;
		}

		// don't like this too much tbh, but recasting it and using the memory lib should be fine
		mem::Ptr(m_originalData).write(m_ptr.as<void*>(), m_patch.size());

		if (m_ptr.write(m_patch.data(), m_patch.size()))
		{
			setApplied(true);
		}
		else
		{
			setApplied(false);
		}

		return *this;
	}

	mem::MemoryPatch& MemoryPatch::undoPatch()
	{
		if (!isPatchApplied())
			return *this;

		if (m_ptr.write(m_originalData, m_patch.size()))
		{
			setApplied(false);
		}

		return *this;
	}
}