#pragma once
#include <Game/GameState.h>
#include <Game/Input.h>
#include <Rendering/Quad.hpp>
#include <Rendering/HardcodedShaders.hpp>
#include <Rendering/Texture.h>
#include <Rendering/RenderText.hpp>


/**
 * \class State_MainMenu
 * \brief Handles the logic for the Main Menu prompting the player to press space to play
 */
class State_MainMenu : public IGameState{
public:
    explicit State_MainMenu(GameStateManager& ctx) : IGameState(ctx){}
    void EnterState() override;
    void ExitState() override;
    void CheckSwitchStates() override;
    void OnUpdate(float dt) override;
    void OnRender() override;

private:
    void OnSpaceButton(KeyState state);
    float m_stateTime = 0;
    bool m_startGame = false;

    UIRectShader uiShader{};
    UIQuad uiQuad{};

    std::unique_ptr<RenderText> renderText;


};