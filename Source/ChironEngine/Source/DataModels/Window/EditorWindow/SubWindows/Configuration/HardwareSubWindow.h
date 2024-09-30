#pragma once
#include "../SubWindow.h"

class HardwareSubWindow : public SubWindow
{
public:
    HardwareSubWindow();
    ~HardwareSubWindow() override;

private:
    void DrawWindowContent(const std::shared_ptr<CommandList>& commandList = nullptr) override;

private:
    std::string _cpuCore;
    std::string _vendorName;
    std::string _gpuName;
    std::string _ram;
};

