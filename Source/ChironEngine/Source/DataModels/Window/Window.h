#pragma once
class Window
{
public:
    virtual ~Window();

    virtual void Draw(bool& enabled) = 0;

    // ------------- GETTERS ----------------------

    inline const std::string& GetName() const;

protected:
    Window(const std::string& name);

protected:
    std::string _name;
};

inline const std::string& Window::GetName() const
{
    return _name;
}
