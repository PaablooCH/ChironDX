#pragma once

template<typename U>

class Importer
{
public:
	Importer() = default;
	virtual ~Importer() {}

	// Reads a path that indicates a file to import to engine
	virtual void Import(const char* filePath, std::shared_ptr<U> resource) = 0;
};

