#pragma once

#include <unordered_map>

#include <ApplicationSettings.hpp>
#include <ApplicationContext.hpp>

#include <Rendering/OffscreenFBO.hpp>
#include <Rendering/ParticleSystem.h>
#include <Rendering/Shader.hpp>
#include <Rendering/HardcodedShaders.hpp>
#include <Rendering/Mesh.h>

#include <Game/Input.h>
#include <Game/Player.hpp>
#include <Game/Asteroids.h>
#include <Game/GameState.h>
#include <Game/State_MainMenu.h>



/**
 * \class Application
 * \brief Core class driving the application, contains all of our managers and is the root manager
 * of lifecycle functions
 * \details Handles application level tasks: Rendering state-agnostic objects, input, settings,
 * lightweight resource management
 */
class Application{
private:
public:
    Application();

    // Shared context, includes all of our systems
    ApplicationContext Context{};

    // Rendering
    static OffscreenFBO particleFBO;
    static void BindParticleFBO();
    static void UnbindParticleFBO();

    // Assets
    std::shared_ptr<AssetHandle> bgAstTex;

    // Exposed for calling in SDL callback funcs
    bool Init();
    void Loop(double dt);
    bool Shutdown();


private:

    void InitRendering();
    void InitBGParticles();


    // Static BG stars
    ParticleSystem m_bgStarParticlesWhite = ParticleSystem(true, true);
    ParticleSystem m_bgStarParticlesBlue = ParticleSystem(true, true);
    ParticleSystem m_bgStarParticlesGreen = ParticleSystem(true, true);

    // Dynamic BG particles
    ParticleSystem m_bgAsteroids = ParticleSystem(true, false);
    ParticleSystem m_bgShootingStars = ParticleSystem(false, false);


    double m_bgShootingStarTimer = 0;

};