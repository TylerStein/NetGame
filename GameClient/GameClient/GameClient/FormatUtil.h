#ifndef FORMAT_UTIL
#define FORMAT_UTIL

#include <string>
#include <sstream>

class FormatUtil
{
public:
	static void TextToHex(const std::string str, std::string& hex, bool capital = false) {
		hex.resize(str.size() * 2);
		const size_t a = capital ? 'A' - 1 : 'a' - 1;

		for (size_t i = 0, c = str[0] & 0xFF; i < hex.size(); c = str[i / 2] & 0xFF) {
			hex[i++] = (char)(c > 0x9F ? (c / 16 - 9) | a : c / 16 | '0');
			hex[i++] = (char)((c & 0xF) > 9 ? (c % 16 - 9) | a : c % 16 | '0');
		}
	}

	static void HexToText(const std::string hex, std::string& str) {
		str.resize((hex.size() + 1) / 2);

		for (size_t i = 0, j = 0; i < str.size(); i++, j++) {
			str[i] = (hex[j] & '@' ? hex[j] + 9 : hex[j]) << 4, j++;
			str[i] = (hex[j] & '@' ? hex[j] + 9 : hex[j]) & 0xF;
		}
	}

	static void HexToTextAlt(const std::string hex, std::string& str) {
		str.clear();
		for (size_t i = 0; i < hex.length(); i += 2) {
			std::string byte = hex.substr(i, 2);
			char chr = (char)(int)strtol(byte.c_str(), NULL, 16);
			str.push_back(chr);
		}

	}

	static void HexToInt32BE(const std::string hex, int32_t& number) {
		std::stringstream ss;
		ss << std::dec << hex;
		ss >> number;
	}

	static void Int32ToHexBE(const int32_t number, std::string& hex) {
		hex.clear();

		std::stringstream ss;
		ss << std::hex << number;
		
		std::string sub = ss.str();
		int leftover = 8 - sub.length();
		for (int i = 0; i < leftover; i++) hex.push_back('0');
		hex.append(sub);

		const char* thing = hex.c_str();
	}
private:
	FormatUtil() {
		//
	}

	~FormatUtil() {
		//
	}
};

#endif