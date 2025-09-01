#pragma once

#include "rapidjson/document.h"

namespace Chiron::Loader
{
    void LoadScene(std::variant<std::string, std::reference_wrapper<rapidjson::Document>>&& scenePathOrDocument,
        std::function<void(void)>&& callback,
        bool mantainCurrentScene = false);

    bool LoadInProcess();
}
