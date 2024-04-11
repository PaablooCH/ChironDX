#pragma once

#include "Pch.h"
#include "Application.h"

enum class MAIN_STATES
{
	MAIN_CREATION,
	MAIN_INIT,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

std::unique_ptr<Application> App;

#ifdef ENGINE
std::unique_ptr<ChironLog> logContext = std::make_unique<ChironLog>();
#endif // ENGINE

int main(int argc, char** argv)
{
	int mainReturn = EXIT_FAILURE;
	MAIN_STATES state = MAIN_STATES::MAIN_CREATION;

	while (state != MAIN_STATES::MAIN_EXIT)
	{
		switch (state)
		{
		case MAIN_STATES::MAIN_CREATION:
#ifdef ENGINE
			ChironLog::Init();
#endif
			LOG_INFO("Application Creation --------------");
			App = std::make_unique<Application>();
			state = MAIN_STATES::MAIN_INIT;
			break;

		case MAIN_STATES::MAIN_INIT:
			LOG_INFO("Application Init --------------");
			if (!App->Init())
			{
				LOG_ERROR("Application Init exits with error -----");
				state = MAIN_STATES::MAIN_EXIT;
			}
			else
			{
				state = MAIN_STATES::MAIN_START;
			}

			break;

		case MAIN_STATES::MAIN_START:

			LOG_INFO("Application Start --------------");
			if (!App->Start())
			{
				LOG_ERROR("Application Start exits with error -----");
				state = MAIN_STATES::MAIN_EXIT;
			}
			else
			{
				state = MAIN_STATES::MAIN_UPDATE;
			}

			break;

		case MAIN_STATES::MAIN_UPDATE:
		{
			UpdateStatus updateReturn = App->Update();

			if (updateReturn == UpdateStatus::UPDATE_ERROR)
			{
				LOG_ERROR("Application Update exits with error -----");
				state = MAIN_STATES::MAIN_EXIT;
			}

			if (updateReturn == UpdateStatus::UPDATE_STOP)
			{
				state = MAIN_STATES::MAIN_FINISH;
			}
		}
		break;

		case MAIN_STATES::MAIN_FINISH:

			LOG_INFO("Application CleanUp --------------");
			if (!App->CleanUp())
			{
				LOG_ERROR("Application CleanUp exits with error -----");
			}
			else
			{
				mainReturn = EXIT_SUCCESS;
			}

			state = MAIN_STATES::MAIN_EXIT;

			break;
		}
	}

	LOG_INFO("Bye :)\n");

	return mainReturn;
}