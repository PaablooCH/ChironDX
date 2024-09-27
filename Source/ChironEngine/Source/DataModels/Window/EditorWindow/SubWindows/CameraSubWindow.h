#pragma once
#include "SubWindow.h"

class CameraSubWindow : public SubWindow
{
public:
    CameraSubWindow();
    ~CameraSubWindow() override;

private:
    void DrawWindowContent(const std::shared_ptr<CommandList>& commandList = nullptr) override;
};

