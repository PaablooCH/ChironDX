#pragma once
#include "Importer.h"

class Texture;

class TextureImporter : public Importer<Texture>
{
public:
	TextureImporter();
	~TextureImporter() override;

	void Import(const char* filePath, std::shared_ptr<Texture> texture) override;

private:
	int CalculateMipLevels(int width, int height);
};