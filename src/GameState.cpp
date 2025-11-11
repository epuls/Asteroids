#include <Game/GameState.h>
#include <Game/AsteroidStateFactory.hpp>
#include <ApplicationContext.hpp>

void GameStateManager::Init() {
    m_stateFactory = std::make_unique<AsteroidStateFactory>(*this);
    currentState = initialState;
    currentState->EnterState();
}

void GameStateManager::SwitchStates(std::shared_ptr<IGameState> nextState) {
    currentState->ExitState();
    currentState = nextState;
    currentState->EnterState();
}

void GameStateManager::Update(float dt) {
    currentState->CheckSwitchStates();
    currentState->OnUpdate(dt);
}

void GameStateManager::FixedUpdate() {
    currentState->OnFixedUpdate();
}

void GameStateManager::LateUpdate(float dt) {
    currentState->OnLateUpdate(dt);
}

void GameStateManager::OnRender() {
    currentState->OnRender();
}

void GameStateManager::OnPhysics() {
    currentState->OnPhysics();
}

GameStateManager::GameStateManager(ApplicationContext &ctx) : ctx(ctx) {
    inputManager = ctx.inputManager;
}


