#pragma once
#include "../CommonIncludes.hpp"

namespace mem
{
	class Ptr
	{
	private:
		void* m_handle = nullptr;
	public:
		// base constructor
		Ptr() = default;

		// extra type constructors
		Ptr(const std::nullptr_t null) : m_handle(null) {}
		Ptr(void* p) : m_handle(p) {}
		Ptr(const uintptr_t p) : m_handle(reinterpret_cast<void*>(p)) {}

		Ptr(const Ptr& copy) = default;

		// some operators
		operator void* ()
		{
			return m_handle;
		}
		
		operator bool ()
		{
			return !isNull();
		}

		[[nodiscard]] bool isNull() const
		{
			return m_handle == nullptr;
		}

		template <typename T>
		[[nodiscard]] typename std::enable_if<std::is_pointer<T>::value, T>::type as() const
		{
			return reinterpret_cast<T>(this->m_handle);
		}

		template <typename T>
		[[nodiscard]] typename std::enable_if<std::is_lvalue_reference<T>::value, T>::type as() const
		{
			return *this->as<typename std::remove_reference<T>::type*>();
		}

		template <typename T>
		std::enable_if_t<std::is_array_v<T>, T&> as() const
		{
			return this->as<T&>();
		}

		template <typename T>
		[[nodiscard]] std::enable_if_t<std::is_same_v<T, std::uintptr_t>, T> as() const
		{
			return reinterpret_cast<std::uintptr_t>(this->as<void*>());
		}

		template <typename T>
		[[nodiscard]] std::enable_if_t<std::is_same_v<T, std::intptr_t>, T> as() const
		{
			return reinterpret_cast<std::intptr_t>(this->as<void*>());
		}

		template <typename T>
		[[nodiscard]] std::enable_if_t<std::is_integral_v<T>, Ptr> add(const T offset) const
		{
			return this->as<std::intptr_t>() + offset;
		}

		template <typename T>
		std::enable_if_t<std::is_integral_v<T>, Ptr> sub(const T offset) const
		{
			return this->as<std::intptr_t>() - offset;
		}

		[[nodiscard]] Ptr relative(std::optional<int32_t> rel = std::nullopt) const
		{
			if (rel.has_value())
				return this->add(rel.value()).add(this->as<int&>());
			return this->add(4).add(this->as<int&>());
		}
		
		bool write(void* data, size_t size) const
		{
			uint32_t oldProtect = 0;

			if (VirtualProtect(this->as<void*>(), size, PAGE_EXECUTE_READWRITE, reinterpret_cast<DWORD*>(&oldProtect)) == TRUE)
			{
				std::memcpy(this->as<void*>(), data, size);

				VirtualProtect(this->as<void*>(), size, oldProtect, nullptr);

				// winapi docs said this is good to do
				FlushInstructionCache(GetCurrentProcess(), nullptr, 0);

				return true;
			}

			return false;
		}
	};

	template <typename T>
	Ptr findSignature(T const& signatureBytes, std::optional<std::string_view> moduleName = std::nullopt)
	{
		uint8_t* foundBytes = nullptr;

		if (auto* const module = GetModuleHandleA(moduleName.has_value() ? moduleName.value().data() : nullptr); module)
		{
			const auto imageStart = reinterpret_cast<uint8_t*>(module);
			const auto imageEnd = imageStart + (reinterpret_cast<PIMAGE_NT_HEADERS>(uint64_t(imageStart) + reinterpret_cast<PIMAGE_DOS_HEADER>(imageStart)->e_lfanew))->OptionalHeader.SizeOfImage;

			const auto result = std::search(imageStart, imageEnd, signatureBytes.cbegin(), signatureBytes.cend(), [](const uint8_t left, const uint8_t right) -> bool
			{
				return right == 0ui8 || left == right;
			});

			foundBytes = (result != imageEnd) ? result : nullptr;
		}

		return { foundBytes };
	}

	inline std::vector<uint8_t> prepSignature(const std::string& signature)
	{
		std::vector<uint8_t> signatureBytes{ };

		std::vector<std::string> signatureChunks{ };
		std::string currentChunk{ };

		std::istringstream stringStream{ signature };

		while (std::getline(stringStream, currentChunk, ' '))
			signatureChunks.push_back(currentChunk);

		std::transform(signatureChunks.cbegin(), signatureChunks.cend(), std::back_inserter(signatureBytes),
			[](const std::string& val) -> uint8_t
		{
			return val.find('?') != std::string::npos ? 0ui8 : static_cast<uint8_t>(std::stoi(val, nullptr, 16));
		}
		);
		return signatureBytes;
	}

	inline Ptr scanForPtr(const std::string& signature, const std::optional<std::string_view> moduleName = std::nullopt)
	{
		return findSignature(prepSignature(signature), moduleName);
	}
}