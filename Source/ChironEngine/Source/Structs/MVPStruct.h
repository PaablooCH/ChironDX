#pragma once

#include <SimpleMath.h>

struct ModelViewProjection
{
	DirectX::SimpleMath::Matrix model;
	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix proj;
};