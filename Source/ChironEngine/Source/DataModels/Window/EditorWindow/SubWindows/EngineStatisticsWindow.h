#pragma once
#include "SubWindow.h"

class EngineStatisticsWindow : public SubWindow
{
public:
    EngineStatisticsWindow();
    ~EngineStatisticsWindow() override;

private:
    void DrawWindowContent(const std::shared_ptr<CommandList>& commandList = nullptr) override;

private:
    std::vector<float> _fpsValues;
    std::vector<float> _timeValues;

    int _fpsIt;
    int _timeIt;
};