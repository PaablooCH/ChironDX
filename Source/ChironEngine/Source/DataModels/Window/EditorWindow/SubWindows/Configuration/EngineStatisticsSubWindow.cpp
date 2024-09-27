#include "Pch.h"
#include "EngineStatisticsSubWindow.h"

#include "Application.h"

namespace
{
    int maxFps = 50;
    int maxTime = 50;
}

EngineStatisticsSubWindow::EngineStatisticsSubWindow() : SubWindow("Engine"), _fpsIt(0), _timeIt(0)
{
    _fpsValues.resize(maxFps);
    _timeValues.resize(maxTime);
}

EngineStatisticsSubWindow::~EngineStatisticsSubWindow()
{
}

void EngineStatisticsSubWindow::DrawWindowContent(const std::shared_ptr<CommandList>& commandList)
{
    auto availableX = ImGui::GetContentRegionAvail().x;

    auto& maxFrameRate = App->GetMaxFrameRate();
    const char* label = "Max FPS:";
    ImVec2 labelSize = ImGui::CalcTextSize(label);
    float sliderWidth = availableX - labelSize.x - ImGui::GetStyle().ItemInnerSpacing.x - 5;
    ImGui::TextUnformatted(label);
    ImGui::SameLine();
    ImGui::PushItemWidth(sliderWidth);
    ImGui::SliderInt("##maxfps", &maxFrameRate, 1, 120, "%d", ImGuiSliderFlags_AlwaysClamp);
    ImGui::PopItemWidth();
    
    auto deltaTime = App->GetDeltaTime();
    
    float fps = 1.f / deltaTime;
    _fpsValues[_fpsIt] = std::max(fps, 0.0001f);
    char currentFramerate[25]{};
    sprintf_s(currentFramerate, 25, "Framerate %.1f", _fpsValues[_fpsIt]);
    ImGui::PlotHistogram("##fps", _fpsValues.data(), static_cast<int>(_fpsValues.size()), 0, currentFramerate, 0.f, 160.f, ImVec2(availableX, 160));

    float time = deltaTime * 1000.f;
    _timeValues[_timeIt] = std::min(std::max(time, 0.0001f), 1000.f);
    char currentTime[25]{};
    sprintf_s(currentTime, 25, "Latency %.1f", _timeValues[_timeIt]);
    ImGui::PlotLines("##latency", _timeValues.data(), static_cast<int>(_timeValues.size()), 0, currentTime, 0.f, 40.0f, ImVec2(availableX, 100));
    
    if (_fpsIt < maxFps - 1)
    {
        _fpsIt++;
    }
    else
    {
        _fpsValues.erase(_fpsValues.begin());
        _fpsValues.push_back(0);
    }
    if (_timeIt < maxTime - 1)
    {
        _timeIt++;
    }
    else
    {
        _timeValues.erase(_timeValues.begin());
        _timeValues.push_back(0);
    }
}
