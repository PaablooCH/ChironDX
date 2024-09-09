#pragma once

class Texture;
class IndexBuffer;
class VertexBuffer;

class Model
{
public:
	Model();
	~Model();

private:
	std::unique_ptr<Texture> _texture;
	std::unique_ptr<VertexBuffer> _vertex;
	std::unique_ptr<IndexBuffer> _index;
};

