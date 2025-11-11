#include <Game/Asteroids.h>
#include <FileIO/LoadObj.hpp>
#include <random>
#include <ApplicationSettings.hpp>


void Asteroids::Init(){
    astTex = m_ctx.assetManager->GetAssetHandle("AstTex");
    astModel = m_ctx.assetManager->GetAssetHandle("Asteroid");

    destroyMediumTex = m_ctx.assetManager->GetAssetHandle("DestroyMedium");
    destroyCrescentTex = m_ctx.assetManager->GetAssetHandle("DestroyCrescent");

    shatter = m_ctx.assetManager->GetAssetHandle("Shatter0");
    impactBeat = m_ctx.assetManager->GetAssetHandle("ImpactBeat");
    impactShort = m_ctx.assetManager->GetAssetHandle("ImpactShort");
    impactThud = m_ctx.assetManager->GetAssetHandle("ImpactThud");
    crunch = m_ctx.assetManager->GetAssetHandle("Crunch");

    astShader.texture0 = m_ctx.assetManager->GetTexture(astTex);
    astShader.Create();
    astShader.Use();
    astShader.SetUniformLocations();

    m_ctx.assetManager->GetMesh(astModel).upload();
    SpawnAsteroid();

    onDieParticles.particleShader = std::make_unique<DefaultRoundParticle>();
    onDieParticles.particleShader->Create();

    onDieParticles.particleShader->tint[0] = 0.95;
    onDieParticles.particleShader->tint[1] = 0.60;
    onDieParticles.particleShader->tint[2] = 0.50;
    onDieParticles.particleShader->tint[3] = 0.80;

    onDieParticles.particleShader->SetUniformLocations();
    onDieParticles.GetParticleData().shaderProgram = onDieParticles.particleShader->GetProgram();

    auto onDieEmit = EmissionParams{
            .emitType = EmissionParams::EmitType::BURST,
            //.numParticlesPerEmit = 50000,
            .numBurstParticles = 250,
            .spawnPoint{0,0},
            .radius = 0.1f,
            .sizeMin = 0.001,
            .sizeMax = 0.005,
            .minVel = -0.003,
            .maxVel = 0.003,
            .minLife = 0.25f,
            .maxLife = 2.0f,
            .drag = 1,
            .sizeDecay = 0.999
    };

    onDieParticles.SetEmissionParams(onDieEmit);
    onDieParticles.Init();
    glUseProgram(0);

    // -----------

    onDieMediumParticles.particleShader = std::make_unique<TexturedQuadParticle>();
    onDieMediumParticles.particleShader->texture0 = m_ctx.assetManager->GetTexture(destroyMediumTex);
    onDieMediumParticles.particleShader->Create();
    onDieMediumParticles.particleShader->SetUniformLocations();
    onDieMediumParticles.GetParticleData().shaderProgram = onDieMediumParticles.particleShader->GetProgram();


    auto onDieMediumEmit = EmissionParams{
            .emitType = EmissionParams::EmitType::BURST,
            //.numParticlesPerEmit = 50000,
            .numBurstParticles = 100,
            .spawnPoint{0,0},
            .radius = 0.1f,
            .sizeMin = 0.02,
            .sizeMax = 0.06,
            .minVel = -0.002,
            .maxVel = 0.002,
            .minVelAngular = -10,
            .maxVelAngular = 10,
            .minLife = 1.0f,
            .maxLife = 5.0f,
            .drag = 1,
            .sizeDecay = 0.999
    };

    onDieMediumParticles.SetEmissionParams(onDieMediumEmit);
    onDieMediumParticles.Init();
    glUseProgram(0);


    // -----------

    onDieCrescentParticles.particleShader = std::make_unique<TexturedQuadParticle>();
    glUseProgram(onDieCrescentParticles.particleShader->GetProgram());
    onDieCrescentParticles.particleShader->texture0 = m_ctx.assetManager->GetTexture(destroyCrescentTex);
    onDieCrescentParticles.particleShader->Create();
    onDieCrescentParticles.particleShader->SetUniformLocations();
    onDieCrescentParticles.GetParticleData().shaderProgram = onDieCrescentParticles.particleShader->GetProgram();


    auto onDieCrescentEmit = EmissionParams{
            .emitType = EmissionParams::EmitType::BURST,
            .velocityDirection = EmissionParams::VelocityDirection::AWAY_FROM_SPAWN_POINT,
            //.numParticlesPerEmit = 50000,
            .numBurstParticles = 15,
            .spawnPoint{0,0},
            .radius = 0.09f,
            .sizeMin = 0.1,
            .sizeMax = 0.2,
            .minVel = 0.03,
            .maxVel = 0.06,
            .minVelAngular = -10,
            .maxVelAngular = 10,
            .minLife = 0.05f,
            .maxLife = 0.2f,
            .drag = 1,
            .sizeDecay = 0.95
    };

    onDieCrescentParticles.SetEmissionParams(onDieCrescentEmit);
    onDieCrescentParticles.Init();
}

void Asteroids::PhysicsLoop(float dt) {
    MoveAsteroids(dt);


    for(auto& ast : asteroidDatas){
        ast.currentIDCooldown -= dt;
        if (ast.currentIDCooldown <= 0) {
            ast.lastCollisionID = 99999;
            ast.freeCollision = true;
            ast.currentIDCooldown = ast.lastCollisionIDCooldown;
        }
    }

    RespawnAsteroids(dt);
}

void Asteroids::RenderLoop(float dt) {
    astShader.Use();

    for(auto& a : asteroidDatas){
        if (!a.alive) continue;
        astShader.SetTranslation(a.translationX, a.translationY);
        astShader.SetRotation(a.rotation);
        astShader.SetRotationX(a.rotation);
        astShader.SetScale(a.radius);
        astShader.SetCamPos();
        m_ctx.assetManager->GetMesh(astModel).draw();

        const float dim = 2;
        DrawAsteroidOffset(a, +dim, 0);
        DrawAsteroidOffset(a, -dim, 0);
        DrawAsteroidOffset(a, 0, +dim);
        DrawAsteroidOffset(a, 0, -dim);

        DrawAsteroidOffset(a, +dim, +dim);
        DrawAsteroidOffset(a, -dim, -dim);
        DrawAsteroidOffset(a, -dim, +dim);
        DrawAsteroidOffset(a, +dim, -dim);
    }

}

void Asteroids::SpawnAsteroid() {

    std::random_device astRandDevice;
    std::default_random_engine astRandEngine(astRandDevice());

    std::uniform_real_distribution<float> spawnRand(-asteroidSettings.radius, asteroidSettings.radius);
    std::uniform_real_distribution<float> velRand(asteroidSettings.minVel, asteroidSettings.maxVel);
    std::uniform_real_distribution<float> angularVelRand(asteroidSettings.minAngularVelocity, asteroidSettings.maxAngularVelocity);

    asteroidDatas.clear();
    asteroidDatas.reserve(asteroidSettings.maxAsteroids);
    for (int i = 0; i < asteroidSettings.maxAsteroids; i++){
        AsteroidData asteroidD = {
                .translationX = spawnRand(astRandEngine),
                .translationY = spawnRand(astRandEngine),
                .velocityX = velRand(astRandEngine),
                .velocityY = velRand(astRandEngine),
                .angularVelocity = angularVelRand(astRandEngine),
                .angularVelocityX = angularVelRand(astRandEngine),
                .alive = true
        };

        //cheap spawn protection, rework later
        if (asteroidD.translationX < 0.25 && asteroidD.translationY < 0.25){
            asteroidD.translationX = spawnRand(astRandEngine);
            asteroidD.translationY = spawnRand(astRandEngine);
            //std::cout << "moving center-spawned asteroid\n";
        }

        asteroidDatas.emplace_back(asteroidD);
    }
}

void Asteroids::DrawAsteroidOffset(AsteroidData a, float x, float y) {
    astShader.SetTranslation(a.translationX + x, a.translationY + y);
    astShader.SetRotation(a.rotation);
    m_ctx.assetManager->GetMesh(astModel).draw();
}

void Asteroids::MoveAsteroids(float dt) {

    for(auto& ast : asteroidDatas){
        ast.translationX += ast.velocityX * dt;
        ast.translationY += ast.velocityY * dt;
        ast.rotation += (ast.angularVelocity * dt);
        ast.rotationX += (ast.angularVelocityX * dt);
        //std::cout<<shipState.rotation<<"\n";
        //shipState.angularVelocity *= shipState.angularVelocityDecay * dt;

        auto wrap_axis = [](float p, float radius) {
            const float min = -1.0f - radius;
            const float max =  1.0f + radius;
            const float W   =  max - min; // 2 + 2*radius
            // wrap for negatives:
            return min + std::fmod(std::fmod(p - min, W) + W, W);
        };



        ast.translationX = wrap_axis(ast.translationX, 0.0001);
        ast.translationY = wrap_axis(ast.translationY, 0.0001);
    }


}

void Asteroids::Damage(Asteroids::AsteroidData &a) {
    a.health -= 1;
    if (a.health <= 0){
        onDieParticles.EmitAt(a.translationX, a.translationY, a.velocityX * 0.005, a.velocityY * 0.005);
        onDieMediumParticles.EmitAt(a.translationX, a.translationY, a.velocityX * 0.005, a.velocityY * 0.005);
        onDieCrescentParticles.EmitAt(a.translationX, a.translationY, a.velocityX * 0.005, a.velocityY * 0.005);
        a.alive = false;
        a.respawnTimeRemaining = asteroidSettings.respawnTimer;
        m_ctx.assetManager->GetAudioClip(shatter).PlayWithJitter(0.9, 0.3, 0.2);
        m_ctx.assetManager->GetAudioClip(impactBeat).PlayWithJitter(0.9, 0.1, 0.01);
        m_ctx.assetManager->GetAudioClip(impactShort).PlayWithJitter(0.9, 0.3, 0.1);

    } else {
        m_ctx.assetManager->GetAudioClip(impactThud).PlayWithJitter(1.0, 0.2, 0.1);
        m_ctx.assetManager->GetAudioClip(crunch).PlayWithJitter(1.0, 0.2, 0.05);
    }
}

void Asteroids::Shutdown() {
    onDieParticles.Shutdown();
    onDieMediumParticles.Shutdown();
    onDieCrescentParticles.Shutdown();
}

void Asteroids::RespawnAsteroids(float dt) {
    std::random_device astRandDevice;
    std::default_random_engine astRandEngine(astRandDevice());

    std::uniform_real_distribution<float> spawnRand(-asteroidSettings.radius, asteroidSettings.radius);
    std::uniform_real_distribution<float> velRand(asteroidSettings.minVel, asteroidSettings.maxVel);
    std::uniform_real_distribution<float> angularVelRand(asteroidSettings.minAngularVelocity, asteroidSettings.maxAngularVelocity);

    for (auto& ast : asteroidDatas){
        if (ast.alive) { continue; }
        ast.respawnTimeRemaining -= dt;
        if (ast.respawnTimeRemaining > 0) { continue; }
        ast.alive = true;
        ast.health = 3;
        ast.lastCollisionID = 9999;
        ast.freeCollision = true;
        ast.translationX = spawnRand(astRandEngine);
        ast.translationY = spawnRand(astRandEngine);
        ast.velocityX = velRand(astRandEngine);
        ast.velocityY = velRand(astRandEngine);
        ast.angularVelocity = angularVelRand(astRandEngine);
        ast.angularVelocityX = angularVelRand(astRandEngine);
    }

    //std::cout << "alive: " << aliveCount << "\n";
}

void Asteroids::OnParticleRender(float dt) {
    onDieParticles.Update(dt);
    onDieMediumParticles.Update(dt);
    onDieCrescentParticles.Update(dt);
}
