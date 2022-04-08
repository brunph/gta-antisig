#pragma once
#include "Memory.hpp"

namespace mem
{
	class MemoryPatch
	{
	private:
		mem::Ptr m_ptr;

		void* m_originalData = nullptr;
		std::vector<int8_t> m_patch = {};

		bool m_isApplied = {};

	public:
		MemoryPatch() = default;
		~MemoryPatch();

		template <size_t Size>
		MemoryPatch& init(Ptr ptr, std::array<int, Size> patchArray)
		{
			m_ptr = ptr;

			// initialize the vector with enough space for the int array
			m_patch.resize(patchArray.size());
			// convert everything to a uint vector where the datatype is much smaller
			std::transform(patchArray.begin(), patchArray.end(), m_patch.begin(), [](int x) { return static_cast<int8_t>(x); });

			return *this;
		}

		[[nodiscard]] bool isPatchApplied()
		{
			return m_isApplied;
		}

		MemoryPatch& setApplied(const bool applied)
		{
			m_isApplied = applied;

			return *this;
		}

		MemoryPatch& applyPatch();
		MemoryPatch& undoPatch();
	};
}