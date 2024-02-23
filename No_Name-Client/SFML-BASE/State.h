#pragma once
//#include "Button.h"
#include "SFML_ENGINE/WindowManager.h"
#include "SFML_ENGINE/RessourcesManager.h"

class State
{
protected:
    std::list<std::unique_ptr<State>>* m_stackState;
    WindowManager& m_windowManager;
    bool m_isReady;
    bool m_initIsStarted;
    bool m_needToBeDeleted;
public:
    State(WindowManager& _window, std::list<std::unique_ptr<State>>* stackState);
    bool getIsReady() { return m_isReady; }
    bool getNeedToBeDeleted() { return m_needToBeDeleted; }
    bool& getIsStarted() { return m_initIsStarted; }
    void virtual init() = 0;
    void virtual update() = 0;
    void virtual render() = 0;
    void virtual pushState(char data) = 0;
};

typedef std::list<std::unique_ptr<State>> StateList;

/*Exemple de cr�ation d'une classe state*/
/*class Test : public State
{
public:
    Test(WindowManager& _window, StateStack* stackState);
private:
    void virtual init();
    void virtual update();
    void virtual render();
    void virtual pushState(char data);
};*/