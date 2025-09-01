#pragma once
#include "Program.h"

class GridProgram : public Program
{
public:
    GridProgram() = delete;
    GridProgram(const std::string& name);
    ~GridProgram() override;

protected:
    void InitRootSignature() override;
    void InitPipelineState() override;
};