#pragma once

#include <Rendering/Mesh.h>
#include <Rendering/HardcodedShaders.hpp>
#include <Game/Input.h>
#include <Rendering/ParticleSystem.h>
#include <Assets/AssetHandle.h>
#include <ApplicationContext.hpp>

/**
 * \class Player
 * Core class that handles the player ship during the \ref State_Game class.
 */
class Player{
public:

    PlayerShipShader shipShader{};

    // ASSETS
    std::unique_ptr<AssetHandle> shipTex;
    std::unique_ptr<AssetHandle> shipModel;

    // Audio assets
    std::unique_ptr<AssetHandle> shoot;
    std::unique_ptr<AssetHandle> impactThud;
    std::unique_ptr<AssetHandle> impactBoing;
    std::unique_ptr<AssetHandle> impactThunder;

    std::unique_ptr<AssetHandle> crunch;
    std::unique_ptr<AssetHandle> shatter;



    ParticleSystem bulletParticles = ParticleSystem(false, false);
    ParticleSystem thrustParticles = ParticleSystem(false, false);


    struct ShipState{
        unsigned int health = 3;
        float damageCooldown = 0.5;
        float currentDamageCooldown = -1;
        float pivot[2] = {0,0};
        float translation[2] = {0.0,0};
        float camTranslation[2] = {0,0};
        float rotation = 0; //RADIANS
        float velocity[2] = {0,0};
        float thrust = 1;
        float rotationSpeed = 30;
        float angularVelocity = 0;
        float maxAngularVelocity = 5;
        float angularVelocityDecay = 0.99f;
        float playerScale = 0.1;
    };

    struct BulletData{
        float cooldown = 0.3f;
        float lifetime = 5.0f;
        float speed = 0.01f;
    };

    ShipState shipState;
    BulletData bulletData;

    // Should use templates for these probably
    void OnUpArrow(KeyState state);
    void OnLeftArrow(KeyState state);
    void OnRightArrow(KeyState state);
    void OnSpaceButton(KeyState state);
    void OnGButton(KeyState state);


    void ThrustForward(float dt);
    void RotateLeft(float dt);
    void RotateRight(float dt);
    void SpawnBullets(float dt);

    void DrawShipOffset(float x, float y);
    void MoveShip(float dt);

    void Init();
    void PhysicsLoop(float dt);
    void RenderLoop(float dt);
    void OnParticleRender(float dt);
    void Shutdown();

    void Damage(float dirX, float dirY, float impactVelocity);

    explicit Player(ApplicationContext& ctx) : m_ctx(ctx){}

private:
    ApplicationContext& m_ctx;
    bool m_modelLoaded = false;
    bool m_followCam = false;

    bool m_updateThrust{};
    bool m_updateLeftRot{};
    bool m_updateRightRot{};
    bool m_spawnBullets{};


};