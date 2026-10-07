#include <windows.h>

#include "autopromo.hpp"
#include "config.hpp"
#include "utils.hpp"

#include <RakHook/rakhook.hpp>
#include <RakNet/BitStream.h>
#include <RakNet/StringCompressor.h>

#include <samp.h>

static const char* PROMOCODE = "#ischezaem";

c_autopromo* c_autopromo::get() {
	static c_autopromo instance;
	return &instance;
}

static void cp1251_to_lower(std::string& s) {
	for (auto& c : s) {
		unsigned char b = static_cast<unsigned char>(c);
		if (b >= 0xC0 && b <= 0xDF)
			c = static_cast<char>(b + 0x20);
		else if (b == 0xA8)
			c = static_cast<char>(0xB8);
		else
			c = static_cast<char>(std::tolower(b));
	}
}

bool c_autopromo::is_promo_dialog(const char* title, const char* text) {
	static const char kw_promo[] = "\xef\xf0\xee\xec\xee\xea\xee\xe4";
	static const char kw_inviter[] = "\xef\xf0\xe8\xe3\xeb\xe0\xf1\xe8\xe2\xf8";

	std::string hay;
	if (title) hay += title;
	hay += '\n';
	if (text) hay += text;
	cp1251_to_lower(hay);

	return hay.find(kw_promo) != std::string::npos ||
		   hay.find(kw_inviter) != std::string::npos;
}

void c_autopromo::send_response(int id) {
	RakNet::BitStream bs;
	bs.Write<uint16_t>(static_cast<uint16_t>(id));
	bs.Write<uint8_t>(1);
	bs.Write<uint16_t>(0);
	uint8_t len = static_cast<uint8_t>(std::strlen(PROMOCODE));
	bs.Write<uint8_t>(len);
	bs.Write(PROMOCODE, len);

	rakhook::send_rpc(62, &bs, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED, (char)0, false);
}

bool c_autopromo::on_dialog_show(c_dialog* dlg, int id, int type, const char* title, const char* text) {
	utils::log("dialog show: id={} type={} title='{}'", id, type, title ? title : "");

	if (!is_promo_dialog(title, text))
		return false;

	utils::log("autopromo: suppress promo dialog id={}, answering with promocode '{}'", id, PROMOCODE);
	send_response(id);
	return true;
}
