#pragma once
#include "Program.h"

class DefaultProgram : public Program
{
public:
	DefaultProgram() = delete;
	DefaultProgram(const std::string& name);
	~DefaultProgram() override;

protected:
	void InitRootSignature() override;
	void InitPipelineState() override;
};