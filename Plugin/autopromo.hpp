#pragma once

#include <string>

class c_dialog;

class c_autopromo
{
public:
	static c_autopromo* get();

	bool on_dialog_show(c_dialog* dlg, int id, int type, const char* title, const char* text);

private:
	bool is_promo_dialog(const char* title, const char* text);
	void send_response(int id);

	c_autopromo() = default;
	~c_autopromo() = default;
};
