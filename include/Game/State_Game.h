#pragma once
#include <Game/GameState.h>
#include <Game/Input.h>
#include <Game/Player.hpp>
#include <Game/Asteroids.h>
#include <Rendering/RenderText.hpp>
#include <Assets/AssetHandle.h>


class State_Game : public IGameState{
public:
    explicit State_Game(GameStateManager& ctx) : IGameState(ctx){}
    // State transitions
    void EnterState() override;
    void ExitState() override;
    void CheckSwitchStates() override;

    // State event functions
    void OnUpdate(float dt) override;
    void OnFixedUpdate() override;
    void OnPhysics() override;
    void OnRender() override;
    void OnLateUpdate(float dt) override;

    // State-specific objects
    Player player{m_ctx.ctx};
    Asteroids asteroids{m_ctx.ctx};
    int score = 0;

    AssetHandle lifeTexture;





private:
    void AddInputEvents();
    void RemoveInputEvents();
    void SetScoreText(int& val);

    void BulletAsteroidCollision(ParticleSystem &pd, Asteroids& asteroids);
    void PlayerAsteroidCollision(Player &p, std::vector<Asteroids::AsteroidData> &data);

    UIRectShader scoreShader{};
    UIQuad scoreQuad{};
    std::unique_ptr<RenderText> scoreRenderText;

    UIRectShader livesShaders[3]{};
    UIQuad livesQuads[3]{};

    UIRectShader fullscreenQuadShader{};
    UIQuad fullscreenQuad;

    // Camera Mode
    enum class CameraFollowType{
        GEOCENTRIC,
        EGOCENTRIC
    };

    CameraFollowType m_cameraFollowType = CameraFollowType::EGOCENTRIC;
};