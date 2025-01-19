#pragma once

#include <qfont.h>

inline QFont setFontSize(int size, bool bold, QFont font)
{
	font.setPointSize(size);
	font.setBold(bold);
	return font;
}
