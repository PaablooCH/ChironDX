#pragma once
#include "Window.h"

class AboutWindow : public Window
{
public:
    AboutWindow();
    ~AboutWindow() override;

    void Draw(const std::shared_ptr<CommandList>& commandList = nullptr) override;

private:
    bool _first;
};
