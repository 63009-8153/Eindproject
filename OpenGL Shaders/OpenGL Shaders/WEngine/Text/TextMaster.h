#pragma once

#include "../../Header.h"

#include "../Utils/loader.h"
#include "FontType.h"
#include "GUIText.h"

#include "FontRenderer.h"

class TextMaster
{
public:
	TextMaster();
	~TextMaster();

	static void init(Loader *theLoader);
	static void loadText(GUIText &text);
	static void removeText(GUIText *text, int pos);
	static void cleanUp();

private:
	static Loader *loader;
	static std::map<FontType, std::vector<GUIText>> texts;
	static FontRenderer renderer;
};

