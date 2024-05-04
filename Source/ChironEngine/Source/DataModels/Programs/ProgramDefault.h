#pragma once
#include "Program.h"

class ProgramDefault : public Program
{
public:
	ProgramDefault() = delete;
	ProgramDefault(const std::string& name);
	~ProgramDefault() override;

protected:
	void InitRootSignature() override;
	void InitPipelineState() override;
};