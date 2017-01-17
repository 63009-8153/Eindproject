#include "TextMaster.h"



TextMaster::TextMaster()
{
}


TextMaster::~TextMaster()
{
}

void TextMaster::init(Loader * theLoader)
{
	//renderer = FontRenderer();
	//loader = theLoader;
}

void TextMaster::loadText(GUIText &text)
{
	/*FontType *font = text.getFont();
	TextMeshData data = font->loadText(text);

	GLuint vao = loader->loadToVAO(data.getVertexPositions(), data.getVertexCount(), data.getTextureCoords(), data.getTextureCount());
	text.setMeshInfo(vao, data.getVertexCount());

	std::vector<GUIText> textBatch = texts.find(*font)->second;

	if (texts.find(*font) == texts.end()) {
		texts.insert(std::pair<FontType, std::vector<GUIText>>(*font, std::vector<GUIText>()));
	}

	texts.find(*font)->second.push_back(text);*/
}

void TextMaster::removeText(GUIText *text, int pos)
{
	///texts.find(*text->getFont())->second.erase(texts.find(*text->getFont())->second.begin() + pos);
}

void TextMaster::cleanUp()
{
}
