#pragma once

#include <SimpleMath.h>

struct InverseViewProj
{
    DirectX::SimpleMath::Matrix invView;
    DirectX::SimpleMath::Matrix invProj;
};