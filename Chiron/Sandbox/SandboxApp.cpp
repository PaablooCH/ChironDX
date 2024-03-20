#include <ChironEngine.h>

class Sandbox : public Chiron::Application
{
public:
    Sandbox()
    {

    }

    ~Sandbox()
    {

    }
};

Chiron::Application* Chiron::CreateApplication()
{
    return new Sandbox();
}