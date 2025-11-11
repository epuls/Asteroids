#pragma once

#include <Rendering/Mesh.h>
#include <Rendering/ParticleSystem.h>
#include <Rendering/HardcodedShaders.hpp>
#include <vector>

#include <Assets/AssetHandle.h>
#include "ApplicationContext.hpp"


class Asteroids{
public:
    PlayerShipShader astShader{};

    // Dust
    ParticleSystem onDieParticles = ParticleSystem(false,false, 1000000);
    ParticleSystem onDieMediumParticles = ParticleSystem(false,false, 1000);
    ParticleSystem onDieCrescentParticles = ParticleSystem(false,false, 1000);

    // Resources
    AssetHandle astModel;
    AssetHandle astTex;
    AssetHandle destroyMediumTex;
    AssetHandle destroyCrescentTex;

    AssetHandle shatter;
    AssetHandle impactBeat;
    AssetHandle impactShort;
    AssetHandle impactThud;
    AssetHandle crunch;


    void Init();
    void PhysicsLoop(float dt);
    void RenderLoop(float dt);
    void OnParticleRender(float dt);
    void Shutdown();

    struct AsteroidSettings{
        unsigned int maxAsteroids = 5;
        float radius = 1.0;
        float minVel = -0.01;
        float maxVel = 0.01;
        float minAngularVelocity = -0.3;
        float maxAngularVelocity = 0.3;
        float respawnTimer = 2.0f;
        float camTranslation[2] = {0,0};
    };

    struct AsteroidData{
        float translationX = 0;
        float translationY = 0;
        float rotation = 0;
        float rotationX = 0;
        float velocityX = 0;
        float velocityY = 0;
        float angularVelocity = 0;
        float angularVelocityX = 0;
        float health = 3;
        float radius = 0.15;
        unsigned int lastCollisionID = 9999;
        float lastCollisionIDCooldown = 0.001f;
        float currentIDCooldown = 0;
        bool alive = true;
        bool freeCollision = true;
        float respawnTimeRemaining = 0;

        inline void SetCollisionID(unsigned int id){
            lastCollisionID = id;
            currentIDCooldown = lastCollisionIDCooldown;
            freeCollision = false;
        }
    };

    AsteroidSettings asteroidSettings;

    void inline SetCameraTranslation(float x, float y){
        asteroidSettings.camTranslation[0] = x;
        asteroidSettings.camTranslation[1] = y;
    }

    std::vector<AsteroidData> asteroidDatas;

    void Damage(AsteroidData& a);

    // Initial spawn
    void SpawnAsteroid();
    void RespawnAsteroids(float dt);
    void DrawAsteroidOffset(AsteroidData a, float x, float y);
    void MoveAsteroids(float dt);

    explicit Asteroids(ApplicationContext& ctx) : m_ctx(ctx){}

private:
    ApplicationContext& m_ctx;


};