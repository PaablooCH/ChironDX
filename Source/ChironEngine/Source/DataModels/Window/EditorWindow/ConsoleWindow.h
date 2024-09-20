#pragma once
#include "EditorWindow.h"

class ConsoleWindow : public EditorWindow
{
public:
    ConsoleWindow();
    ~ConsoleWindow() override;

private:
    void DrawWindowContent() override;

    void DrawOptionsMenu();
    void DrawMaxLengthSelection();
    void DrawConsole();

private:
    std::vector<Chiron::Log::LogLine> _consoleContents;
    std::map<LogSeverity, bool> _severityFilters;
    std::map<LogSeverity, ImVec4> _severityColors;
};
