#pragma once

#include <SimpleMath.h>

struct CameraShader
{
    DirectX::SimpleMath::Matrix view;
    DirectX::SimpleMath::Matrix proj;
    DirectX::SimpleMath::Vector3 pos;
};