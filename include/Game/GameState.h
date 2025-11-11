#pragma once
#include <memory>
#include <unordered_map>
#include <Game/Input.h>

// Forward declare class
class GameStateManager;

// Forward declare application context
struct ApplicationContext;

/**
 * \class IGameState
 * \brief Virtual class containing events to be called by the \ref GameStateManager
 */
class IGameState{
protected:
    GameStateManager& m_ctx;
public:
    explicit IGameState(GameStateManager& ctx) : m_ctx(ctx) {}
    virtual void EnterState(){}
    virtual void CheckSwitchStates(){}
    virtual void OnUpdate(float dt){}
    virtual void OnFixedUpdate(){}
    virtual void OnLateUpdate(float dt){}
    virtual void ExitState(){}

    virtual void OnRender(){}
    virtual void OnPhysics(){}
};

/**
 * \class IStateFactory Small class for abstracted creation of our states on game initialization
 */
class IStateFactory{
protected:
    GameStateManager& m_ctx;
public:
    explicit IStateFactory(GameStateManager& ctx) : m_ctx(ctx){}

};


/**
 * \class GameStateManager Managing context that allows for a clear game state
 * \brief Calls the current state's lifecycle functions, and allows for clear state initialization/exiting
 */
class GameStateManager{
private:
    std::unique_ptr<IStateFactory> m_stateFactory;

public:
    std::shared_ptr<IGameState> initialState;
    std::shared_ptr<IGameState> currentState;
    std::unordered_map<std::string, std::shared_ptr<IGameState>> states;
    std::shared_ptr<InputManager> inputManager;
    ApplicationContext& ctx;

    void Init();

    void SwitchStates(std::shared_ptr<IGameState> nextState);
    void Update(float dt);
    void FixedUpdate();
    void LateUpdate(float dt);
    void OnRender();
    void OnPhysics();

    explicit GameStateManager(ApplicationContext& ctx);
};