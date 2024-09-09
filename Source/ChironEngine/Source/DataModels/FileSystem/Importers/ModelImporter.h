#pragma once
#include "Importer.h"

class Model;

class ModelImporter : public Importer<Model>
{
public: 
	ModelImporter();
	~ModelImporter() override;

	void Import(const char* filePath, std::shared_ptr<Model> model) override;
};

