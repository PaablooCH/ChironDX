#pragma once
#include "Importer.h"

class TextureAsset;

class TextureImporter : public Importer<TextureAsset>
{
public:
	TextureImporter();
	~TextureImporter() override;

	void Import(const char* filePath, const std::shared_ptr<TextureAsset>& texture) override;

private:
	int CalculateMipLevels(int width, int height);
};