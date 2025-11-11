#include <Application.h>
#include <Rendering/Texture.h>

#include <valarray>
#include <memory>
#include "FileIO/LoadObj.hpp"

#define DRAW_DEBUG 1

Application::Application() {

}


bool Application::Init() {
    Context.assetManager->Init();

    try{
        bgAstTex = Context.assetManager->GetAssetHandle("BGAst0");

        InitRendering();

        Context.gameStateManager->Init();

        InitBGParticles();
    } catch (...){
        std::cout << "Failed to init app\n";
    }



    return true;
}


void Application::Loop(double dt) {
    m_totalTime += dt;
    m_fixedTickTime += dt;
    // Update (non-static) background particles
    m_bgStarParticlesWhite.Update(dt);
    m_bgStarParticlesBlue.Update(dt);
    m_bgStarParticlesGreen.Update(dt);

    m_bgAsteroids.Update(dt);

    // Spawn/Emit background shooting stars
    m_bgShootingStarTimer += dt;
    if (m_bgShootingStarTimer >= (double)3){
        m_bgShootingStarTimer = 0;
        m_bgShootingStars.Emit();
    }

    m_bgShootingStars.Update(dt);
    Context.gameStateManager->Update(dt);

    Context.gameStateManager->OnRender();

    if (m_fixedTickTime >= 1/60){
        Context.gameStateManager->FixedUpdate();

        Context.gameStateManager->OnPhysics();
        m_fixedTickTime = 0;

    }

    Context.gameStateManager->LateUpdate(dt);


}

bool Application::Shutdown() {
    try{
        m_bgStarParticlesWhite.Shutdown();
        m_bgStarParticlesBlue.Shutdown();
        m_bgStarParticlesGreen.Shutdown();
        m_bgAsteroids.Shutdown();
        m_bgShootingStars.Shutdown();
    } catch (...){
        std::cout << "failed to properly shut down application\n";
        return false;
    }

    return true;
}

void Application::InitBGParticles() {
    // Set up static bg stars
    m_bgStarParticlesWhite.particleShader = std::make_unique<DefaultRoundParticle>();
    m_bgStarParticlesWhite.particleShader->followCam = false;
    m_bgStarParticlesWhite.particleShader->Create();
    m_bgStarParticlesWhite.GetParticleData().shaderProgram = m_bgStarParticlesWhite.particleShader->GetProgram();

    auto bgStarEmit = EmissionParams{
            .emitType = EmissionParams::EmitType::BURST,
            .numContinuousParticles = 0,
            .numBurstParticles = 33,
            .spawnPoint{0,0},
            .radius = 3.0f,
            .sizeMin = 0.001,
            .sizeMax = 0.005,
            .minVel = 0.0,
            .maxVel = 0.0,
            .minLife = 1.0f,
            .maxLife = 1.0f,
            .drag = 1,
            .sizeDecay = 1
    };

    m_bgStarParticlesWhite.SetEmissionParams(bgStarEmit);
    m_bgStarParticlesWhite.Init();

    // ----------------------------

    // Blue BG
    m_bgStarParticlesBlue.particleShader = std::make_unique<DefaultRoundParticle>();
    m_bgStarParticlesBlue.particleShader->followCam = false;

    m_bgStarParticlesBlue.particleShader->Create();
    m_bgStarParticlesBlue.particleShader->tint[0] = 0.8;
    m_bgStarParticlesBlue.particleShader->tint[1] = 0.95;
    m_bgStarParticlesBlue.particleShader->tint[2] = 1.0;
    m_bgStarParticlesBlue.particleShader->tint[3] = 1.0;
    m_bgStarParticlesBlue.GetParticleData().shaderProgram = m_bgStarParticlesBlue.particleShader->GetProgram();
    m_bgStarParticlesBlue.SetEmissionParams(bgStarEmit);
    m_bgStarParticlesBlue.Init();

    // ----------------------------

    // ----------------------------

    // Green BG
    m_bgStarParticlesGreen.particleShader = std::make_unique<DefaultRoundParticle>();
    m_bgStarParticlesGreen.particleShader->followCam = false;
    m_bgStarParticlesGreen.particleShader->Create();
    m_bgStarParticlesGreen.particleShader->tint[0] = 0.8;
    m_bgStarParticlesGreen.particleShader->tint[1] = 1.0;
    m_bgStarParticlesGreen.particleShader->tint[2] = 0.8;
    m_bgStarParticlesGreen.particleShader->tint[3] = 1.0;
    m_bgStarParticlesGreen.GetParticleData().shaderProgram = m_bgStarParticlesGreen.particleShader->GetProgram();
    m_bgStarParticlesGreen.SetEmissionParams(bgStarEmit);
    m_bgStarParticlesGreen.Init();

    // ----------------------------



    m_bgShootingStars.particleShader = std::make_unique<DefaultRoundParticle>();
    m_bgShootingStars.particleShader->followCam = false;
    m_bgShootingStars.particleShader->Create();
    m_bgShootingStars.GetParticleData().shaderProgram = m_bgShootingStars.particleShader->GetProgram();

    auto bgShootingStarEmit = EmissionParams{
            .emitType = EmissionParams::EmitType::CONTINUOUS,
            .numContinuousParticles = 1,
            .numBurstParticles = 3,
            .spawnPoint{0,0},
            .radius = 3.0f,
            .sizeMin = 0.001,
            .sizeMax = 0.005,
            .minVel = -0.002,
            .maxVel = 0.002,
            .minLife = 1.0,
            .maxLife = 3.0,
            .drag = 0.999,
            .sizeDecay = 0.999
    };

    m_bgShootingStars.SetEmissionParams(bgShootingStarEmit);
    m_bgShootingStars.Init();



    // Set up bg asteroids
    m_bgAsteroids.particleShader = std::make_unique<TexturedQuadParticle>();

    //m_bgAsteroidTex = TextureFromFile(bgAstFileName, bgAstTexDir);
    m_bgAsteroids.particleShader->texture0 = Context.assetManager->GetTexture(bgAstTex);
    m_bgAsteroids.particleShader->followCam = false;
    m_bgAsteroids.particleShader->Create(); //ast.Use();
    m_bgAsteroids.particleShader->SetUniformLocations();

    m_bgAsteroids.GetParticleData().shaderProgram = m_bgAsteroids.particleShader->GetProgram();

    auto bgdAstEmit = EmissionParams{
            .numContinuousParticles = 15,
            .numBurstParticles = 15,
            .radius = 3.0f,
            .sizeMin = 0.02,
            .sizeMax = 0.05,
            .minVel = -0.00005,
            .maxVel = 0.00005,
            .minLife = 999999999.0,//Hacky for now, probably add an "inf" lifetime option
            .maxLife = 999999999.0,//Hacky for now, probably add an "inf" lifetime option
            .drag = 1.0,
            .sizeDecay = 1.0
    };
    m_bgAsteroids.SetEmissionParams(bgdAstEmit);
    //m_bgAsteroids.SetParticleShader(ast);
    m_bgAsteroids.Init();
}



void Application::InitRendering() {
    // Set ortho projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, 01, 1, 0, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_CULL_FACE);
}


void Application::BindParticleFBO() {
    // Bind offscreen and set viewport to its size
    glBindFramebuffer(GL_FRAMEBUFFER, particleFBO.fbo);
    glViewport(0, 0, particleFBO.w, particleFBO.h);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}



void Application::UnbindParticleFBO() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



}





