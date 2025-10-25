#pragma once

#include <qfont.h>

inline QFont set_font_size(int size, bool bold, QFont font)
{
	font.setPointSize(size);
	font.setBold(bold);
	return font;
}
