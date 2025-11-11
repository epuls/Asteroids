#include <iostream>
#include <cmath>

#include <Game/Player.hpp>
#include <FileIO/LoadObj.hpp>
#include <ApplicationSettings.hpp>




void Player::ThrustForward(float dt) {
    if (!m_updateThrust) return;


    const float theta = shipState.rotation;// * (M_PI / 180);
    // For x forward
    //float fx = std::cos(theta);
    //float fy = std::sin(theta);

    // For y forward
    float fx = -std::sin(theta);
    float fy =  std::cos(theta);

    // Acceleration along forward
    float ax = fx * shipState.thrust;
    float ay = fy * shipState.thrust;

    // Integrate velocity and position
    shipState.velocity[0] += ax * dt;
    shipState.velocity[1] += ay * dt;

    // Point along y forward
    float spawnDist = -shipState.playerScale * 0.8f;

    float sx = fx * spawnDist + shipState.translation[0];
    float sy = fy * spawnDist + shipState.translation[1];

    thrustParticles.EmitAt(sx, sy, -fx * bulletData.speed, -fy * bulletData.speed);

}

// Resource helper

void Player::Init() {
    shipTex = m_ctx.assetManager->GetAssetHandleUPtr("ShipTex");
    shipModel = m_ctx.assetManager->GetAssetHandleUPtr("ShipMesh");
    impactThud = m_ctx.assetManager->GetAssetHandleUPtr("ImpactThud");
    impactBoing = m_ctx.assetManager->GetAssetHandleUPtr("ImpactBoing");
    impactThunder = m_ctx.assetManager->GetAssetHandleUPtr("ImpactThunder");
    crunch = m_ctx.assetManager->GetAssetHandleUPtr("Crunch");
    shatter = m_ctx.assetManager->GetAssetHandleUPtr("Shatter0");
    shoot = m_ctx.assetManager->GetAssetHandleUPtr("Shoot0");



    shipShader.texture0 = m_ctx.assetManager->GetTexture(*shipTex);
    shipShader.Create();
    shipShader.SetUniformLocations();

    m_ctx.assetManager->GetMesh(*shipModel).upload();
    glUseProgram(0);


    bulletParticles.particleShader = std::make_unique<DefaultRoundParticle>();
    bulletParticles.particleShader->tint[0] = 0.9;
    bulletParticles.particleShader->tint[1] = 0.99;
    bulletParticles.particleShader->tint[2] = 1.0;
    bulletParticles.particleShader->tint[3] = 1;
    bulletParticles.particleShader->Create();
    bulletParticles.particleShader->SetUniformLocations();
    bulletParticles.GetParticleData().shaderProgram = bulletParticles.particleShader->GetProgram();


    auto bulletEmit = EmissionParams{
            .emitType = EmissionParams::EmitType::BURST,
            .cooldown = bulletData.cooldown,
            .numContinuousParticles = 1,
            .numBurstParticles = 1,
            .spawnPoint{0,0},
            //.radius = 0.0001f,
            .radius = 0.01f,
            .sizeMin = 0.01,
            .sizeMax = 0.01,
            .minVel = 0.0,
            .maxVel = 0.0,
            .minLife = bulletData.lifetime,
            .maxLife = bulletData.lifetime,
            .drag = 1.0,
            .sizeDecay = 1.0
    };

    bulletParticles.SetEmissionParams(bulletEmit);
    bulletParticles.Init();
    glUseProgram(0);

    thrustParticles.particleShader = std::make_unique<DefaultRoundParticle>();
    thrustParticles.particleShader->Create();
    thrustParticles.GetParticleData().shaderProgram = thrustParticles.particleShader->GetProgram();

    auto onThrustEmit = EmissionParams{
            .emitType = EmissionParams::EmitType::BURST,
            .velocityDirection = EmissionParams::VelocityDirection::PARTICLE_RELATIVE,
            //.numParticlesPerEmit = 50000,
            .numBurstParticles = 50,
            .spawnPoint{0,0},
            .radius = 0.03f,
            .sizeMin = 0.001,
            .sizeMax = 0.020,
            .minVel = 0.0,
            .maxVel = 0.0,
            .minVelAngular = 0,
            .maxVelAngular = 0,
            .minLife = 0.01f,
            .maxLife = 0.15f,
            .drag = 1,
            .sizeDecay = 0.99
    };

    thrustParticles.SetEmissionParams(onThrustEmit);
    thrustParticles.Init();
    glUseProgram(0);

}

void Player::PhysicsLoop(float dt) {
    RotateLeft(dt);
    RotateRight(dt);
    ThrustForward(dt);
    SpawnBullets(dt);
    MoveShip(dt);

    shipState.currentDamageCooldown -= dt;


}

void Player::RenderLoop(float dt) {

    //bulletParticles.Update(dt);

    shipShader.Use();
    shipShader.SetRotation(shipState.rotation);
    shipShader.SetTranslation(shipState.translation[0], shipState.translation[1]);
    shipShader.SetScale(shipState.playerScale);
    m_ctx.assetManager->GetMesh(*shipModel).draw();

    const float dim = 2;
    DrawShipOffset(+dim, 0);
    DrawShipOffset(-dim, 0);
    DrawShipOffset(0, +dim);
    DrawShipOffset(0, -dim);

    // Potential VFX below drawing wireframe over the ship

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //shipMesh.draw();
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    //thrustParticles.Update(dt);


}

void Player::MoveShip(float dt) {
    shipState.translation[0] += shipState.velocity[0] * dt;
    shipState.translation[1] += shipState.velocity[1] * dt;
    shipState.rotation += (shipState.angularVelocity * dt);

    // Apply drag to rotation if not actively rotating
    if(!m_updateRightRot && !m_updateLeftRot)
        shipState.angularVelocity *= shipState.angularVelocityDecay * dt;

    auto wrap_axis = [](float p, float radius) {
        const float min = -1.0f - radius;
        const float max =  1.0f + radius;
        const float W   =  max - min; // 2 + 2*radius
        // wrap for negatives:
        return min + std::fmod(std::fmod(p - min, W) + W, W);
    };

    shipState.translation[0] = wrap_axis(shipState.translation[0], 0.0000001);
    shipState.translation[1] = wrap_axis(shipState.translation[1], 0.0000001);


    if (m_followCam){
        IShader::camPos[0] = shipState.translation[0];
        IShader::camPos[1] = shipState.translation[1];
    } else {
        IShader::camPos[0] = 0.0f;
        IShader::camPos[1] = 0.0f;
    }



}


void Player::RotateLeft(float dt) {
    if (!m_updateLeftRot) return;
    shipState.angularVelocity += shipState.rotationSpeed * dt;
    shipState.angularVelocity = std::clamp(shipState.angularVelocity, -shipState.maxAngularVelocity, shipState.maxAngularVelocity);
}

void Player::RotateRight(float dt) {
    if (!m_updateRightRot) return;
    shipState.angularVelocity -= shipState.rotationSpeed * dt;
    shipState.angularVelocity = std::clamp(shipState.angularVelocity, -shipState.maxAngularVelocity, shipState.maxAngularVelocity);

}

void Player::DrawShipOffset(float x, float y) {

    shipShader.SetRotation(shipState.rotation);
    shipShader.SetTranslation(shipState.translation[0] + x, shipState.translation[1] + y);
    m_ctx.assetManager->GetMesh(*shipModel).draw();
}

void Player::OnUpArrow(KeyState state) {
    switch(state){
        case(PRESSED):
            m_updateThrust = true;
            break;
        case(HELD):
            m_updateThrust = true;
            break;
        case(EMPTY):
            m_updateThrust = false;
            break;
        case(RELEASED):
            m_updateThrust = false;
    }
}

void Player::OnLeftArrow(KeyState state) {
    switch(state){
        case(PRESSED):
            m_updateLeftRot = true;
            break;
        case(HELD):
            m_updateLeftRot = true;
            break;
        default:
            m_updateLeftRot = false;
            break;
    }
}

void Player::OnRightArrow(KeyState state) {
    switch(state){
        case(PRESSED):
            m_updateRightRot = true;
            break;
        case(HELD):
            m_updateRightRot = true;
            break;
        default:
            m_updateRightRot = false;
            break;
    }
}

void Player::OnSpaceButton(KeyState state) {
    switch(state){
        case(PRESSED):
            m_spawnBullets = true;
            break;
        case(HELD):
            m_spawnBullets = true;
            break;
        default:
            m_spawnBullets = false;
            break;
    }
}

void Player::OnGButton(KeyState state) {
    if (state == PRESSED){
        m_followCam = !m_followCam;
    }
}

void Player::SpawnBullets(float dt) {
    if (!m_spawnBullets) return;

    const float theta = shipState.rotation;

    // For y forward
    float fx = -std::sin(theta);
    float fy =  std::cos(theta);

    // Point along y forward
    float spawnDist = shipState.playerScale;

    float sx = fx * spawnDist + shipState.translation[0];
    float sy = fy * spawnDist + shipState.translation[1];
    if (bulletParticles.GetCanEmit()){
        m_ctx.assetManager->GetAudioClip(*shoot).PlayWithJitter(0.8, 0.5f, 0.05f);
    }

    bulletParticles.EmitAt(sx, sy, fx * bulletData.speed + (shipState.velocity[0] * dt), fy * bulletData.speed + (shipState.velocity[1] * dt));
}


void Player::Damage(float dirX, float dirY, float impactVelocity) {
    if (shipState.currentDamageCooldown > 0) {
        shipState.velocity[0] = dirX * impactVelocity;
        shipState.velocity[1] = dirY * impactVelocity;
        return;
    }
    shipState.currentDamageCooldown = shipState.damageCooldown;
    shipState.health -= 1;
    shipState.velocity[0] = dirX * impactVelocity;
    shipState.velocity[1] = dirY * impactVelocity;


    if (shipState.health <= 0){
        m_ctx.assetManager->GetAudioClip(*impactThud).PlayWithJitter(0.7, 0.3f, 0.2f);
        m_ctx.assetManager->GetAudioClip(*impactThunder).PlayWithJitter(0.8, 0.3f, 0.15f);
        m_ctx.assetManager->GetAudioClip(*crunch).PlayWithJitter(0.6, 0.1f, 0.1f);
        m_ctx.assetManager->GetAudioClip(*shatter).PlayWithJitter(1.3, 0.1f, 0.1f);
    } else {
        m_ctx.assetManager->GetAudioClip(*impactBoing).PlayWithJitter(1.0, 0.3f, 0.1f);
        m_ctx.assetManager->GetAudioClip(*crunch).PlayWithJitter(0.6, 0.1f, 0.1f);
        m_ctx.assetManager->GetAudioClip(*shatter).PlayWithJitter(1.3, 0.1f, 0.1f);
    }
}

void Player::Shutdown() {
    ShipState cleanState{};
    shipState = cleanState;
    bulletParticles.Shutdown();
    thrustParticles.Shutdown();

    m_updateThrust = false;
    m_updateLeftRot = false;
    m_updateRightRot = false;
    m_spawnBullets = false;
}

void Player::OnParticleRender(float dt) {
    bulletParticles.Update(dt);
    thrustParticles.Update(dt);
}



