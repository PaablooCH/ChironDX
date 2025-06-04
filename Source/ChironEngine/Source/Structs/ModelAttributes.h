#pragma once

#include <SimpleMath.h>

struct ModelAttributes
{
    DirectX::SimpleMath::Matrix model;
    int uvCorrector;
    int hasAlbedo;
};