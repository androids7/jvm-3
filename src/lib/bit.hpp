#pragma once

#include <array>
#include <cstdint>

#define SYSTEM_ENDIANNESS jvm::Endianness::LITTLE

namespace jvm {

	enum class Endianness : uint8_t {
		BIG,
		LITTLE
	};

	/**
	 * @brief Stores a number and its endianness information,
	 * providing utilities for bit access and endianness conversion
	 * @tparam T underlying data type
	 */
	template<typename T>
	class Data {
	private:
		Endianness m_endianness;
	public:
		union {
			T number;
			std::array<uint8_t, sizeof(T)> bytes;
		} value;

		explicit Data(Endianness = SYSTEM_ENDIANNESS);

		explicit Data(T number, Endianness = SYSTEM_ENDIANNESS);

		Data<T> &toEndianness(Endianness new_endianness);

		Data<T> &operator=(T number);

		Data<T> &operator=(const Data<T> &d);

		bool operator== (const Data<T> &d);

		int operator[] (int index);

		template<typename U>
		U as_type();

		void Print();

		void Print(std::string &s);

	};

	template<typename T>
	std::ostream& operator<< (std::ostream& os, Data<T>& data);

	typedef Data<uint8_t>  Byte;
	typedef Data<uint16_t> HalfWord;
	typedef Data<uint32_t> Word;
}

#include "../bit.cpp" // Templates require this
