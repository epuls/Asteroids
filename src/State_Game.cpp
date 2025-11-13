#include <Game/State_Game.h>
#include <iostream>
#include <valarray>
#include "ApplicationSettings.hpp"
#include <Application.h>

#define DRAW_DEBUG 1

void State_Game::SetScoreText(int& val) {
    auto fontStr = ApplicationSettings::DIRECTORY + "/resources/fonts/MomoTrustDisplay-Regular.ttf";
    auto txt = "Score: " + std::to_string(val);
    scoreRenderText = std::make_unique<RenderText>(fontStr.c_str(), txt.c_str(), 100);
}

void State_Game::EnterState() {
    player.Init();
    asteroids.Init();
    AddInputEvents();
    lifeTexture = m_ctx.ctx.assetManager->GetAssetHandleSharedPtr("Life");


    scoreShader.Create(); scoreShader.SetUniformLocations();
    scoreQuad.program = scoreShader.GetProgram();
    scoreQuad.InitBuffers();

    SetScoreText(score);

    scoreShader.Use();
    scoreShader.SetScale(0.35, scoreRenderText->YScaleToPreserveAspect(0.35));
    scoreShader.SetTranslation(0.0, 0.9);
    scoreShader.SetOpacity(0.9);

    glUseProgram(0);

    float OFFSET = 0.10;
    float SCALE = 0.10;

    for (int i = 0; i < 3; i++){
        livesShaders[i].Create();
        livesQuads[i].program = livesShaders[i].GetProgram();
        livesQuads[i].InitBuffers();
        livesShaders[i].Use();
        livesShaders[i].SetScale(SCALE, SCALE);
        livesShaders[i].SetTranslation((-1 + SCALE) + (OFFSET * i), 0.9);
        livesShaders[i].SetOpacity(0.9);
    }

    glUseProgram(0);

    fullscreenQuadShader.Create();
    fullscreenQuad.program = fullscreenQuadShader.GetProgram();
    fullscreenQuadShader.SetTextureID(Application::particleFBO.color);
    fullscreenQuad.InitBuffers();
    fullscreenQuadShader.Use();
    fullscreenQuadShader.SetScale(2, 2);
    fullscreenQuadShader.flipUV = false; fullscreenQuadShader.SetFlipUV();

}

void State_Game::ExitState() {
    RemoveInputEvents();
    player.Shutdown();
    asteroids.Shutdown();
    scoreQuad.cleanup();
    score = 0;

}

void State_Game::CheckSwitchStates() {
    if (player.shipState.health <= 0){
        m_ctx.SwitchStates(m_ctx.states.at("MainMenu"));
    }
}

void State_Game::OnUpdate(float dt) {
    asteroids.PhysicsLoop(dt);
    asteroids.RenderLoop(dt);

    player.PhysicsLoop(dt);
    player.RenderLoop(dt);

    asteroids.SetCameraTranslation(player.shipState.camTranslation[0], player.shipState.camTranslation[1]);
}

void State_Game::OnFixedUpdate() {

}

void State_Game::OnPhysics(){
    BulletAsteroidCollision(player.bulletParticles, asteroids);
    PlayerAsteroidCollision(player, asteroids.asteroidDatas);
}

void State_Game::OnRender() {
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    scoreShader.Use();
    glBindTexture(GL_TEXTURE_2D, scoreRenderText->id);
    scoreQuad.draw();
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, m_ctx.ctx.assetManager->GetTexture(*lifeTexture));
    for (int i = 0; i < player.shipState.health; i++){
        livesShaders[i].Use();
        livesQuads[i].draw();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}

void State_Game::AddInputEvents() {
    m_ctx.inputManager->UpKey.onKey.AddOnPressed<&Player::OnUpArrow>(&player, 0);
    m_ctx.inputManager->LeftKey.onKey.AddOnPressed<&Player::OnLeftArrow>(&player, 0);
    m_ctx.inputManager->RightKey.onKey.AddOnPressed<&Player::OnRightArrow>(&player, 0);
    m_ctx.inputManager->SpaceKey.onKey.AddOnPressed<&Player::OnSpaceButton>(&player, 0);
    m_ctx.inputManager->GKey.onKey.AddOnPressed<&Player::OnGButton>(&player, 0);
}

void State_Game::RemoveInputEvents() {
    m_ctx.inputManager->UpKey.onKey.RemoveOnPressed<&Player::OnUpArrow>(&player, 0);
    m_ctx.inputManager->LeftKey.onKey.RemoveOnPressed<&Player::OnLeftArrow>(&player, 0);
    m_ctx.inputManager->RightKey.onKey.RemoveOnPressed<&Player::OnRightArrow>(&player, 0);
    m_ctx.inputManager->SpaceKey.onKey.RemoveOnPressed<&Player::OnSpaceButton>(&player, 0);
    m_ctx.inputManager->GKey.onKey.RemoveOnPressed<&Player::OnGButton>(&player, 0);
}

void State_Game::BulletAsteroidCollision(ParticleSystem &pd, Asteroids &asteroids) {
    //pd.debugPrint = true;
    float dirX = 0;
    float dirY = 0;
    auto& data = asteroids.asteroidDatas;

    auto dist = [&](float x1, float y1, float x2, float y2) {
        float dx = x2 - x1;
        float dy = y2 - y1;
        dirX = x2-x1;
        dirY = y2-y1;

        return std::sqrt(dx * dx + dy * dy);
    };

    int count = 0;
    for(int i = 0; i <= pd.GetParticleData().last_alive_idx; i++){
        if (!pd.GetParticleData().alive[i]) { continue; }
        for(auto& ast : data){

            if (!ast.alive) continue;

            auto d = dist(ast.translationX, ast.translationY,
                          pd.GetInstanceData()[i].px, pd.GetInstanceData()[i].py);


            //if (d < ast.radius && (i == ast.lastCollisionID)) std::cout << i << ast.lastCollisionID << "\n";
            if (d < ast.radius && ast.lastCollisionID != i){
                ast.SetCollisionID(i);
                ast.velocityX += dirX * -1.0f;
                ast.velocityY += dirY * -1.0f;
                //ast.health -= 1;
                asteroids.Damage(ast);
                if (ast.health <= 0) {
                    score++;
                    SetScoreText(score);
                }
                pd.GetParticleData().kill[i] = 1;
            } else if (d < ast.radius)
            {
                //std::cout << "Skipping collision: " << ast.lastCollisionID << "|" << i << "\n";
            }

        }
    }

    //std::cout << "checked: " << count << " particles\n";
}

void State_Game::PlayerAsteroidCollision(Player &p, std::vector<Asteroids::AsteroidData> &data) {
    const float theta = p.shipState.rotation;
    const float IMPACT_VEL = 1.0f;

    float dirX = 0;
    float dirY = 0;

    auto dist = [&](float x1, float y1, float x2, float y2) {
        float dx = x2 - x1;
        float dy = y2 - y1;

        dirX = x2-x1;
        dirY = y2-y1;

        return std::sqrt(dx * dx + dy * dy);
    };

    float scale = p.shipState.playerScale;

    // forward axis
    float fx = -std::sin(theta);
    float fy =  std::cos(theta);
    // right axis
    float rx = std::cos(theta);
    float ry = std::sin(theta);


    // front triangle point
    float fpx = fx * scale + p.shipState.translation[0];
    float fpy = fy * scale + p.shipState.translation[1];

    float rpx = rx * (scale) + (-fx * scale) + p.shipState.translation[0];
    float rpy = ry * (scale) + (-fy * scale) + p.shipState.translation[1];

    float lpx = -rx * (scale) + (-fx * scale) + p.shipState.translation[0];
    float lpy = -ry * (scale) + (-fy * scale) + p.shipState.translation[1];

#if DRAW_DEBUG
    glUseProgram(0);
    glPointSize(5);
    glBegin(GL_POINTS);
    glColor3f(0, 1, 0);
    glVertex3f(fpx, fpy, 0);
    glVertex3f(rpx, rpy, 0);
    glVertex3f(lpx, lpy, 0);
    glEnd();
#endif

    for(auto& ast : data){
        if (!ast.alive) continue;
        auto d = dist(ast.translationX, ast.translationY, fpx, fpy);
        if (d <= ast.radius){
            //std::cout << "player fp collided with ast" << "\n";
            p.Damage(dirX, dirY, IMPACT_VEL);
            continue;
        }

        auto d2 = dist(ast.translationX, ast.translationY, rpx, rpy);
        if (d2 <= ast.radius){
            //std::cout << "player rp collided with ast" << "\n";
            p.Damage(dirX, dirY, IMPACT_VEL);
            continue;
        }

        auto d3 = dist(ast.translationX, ast.translationY, lpx, lpy);
        if (d3 <= ast.radius){
            //std::cout << "player lp collided with ast" << "\n";
            p.Damage(dirX, dirY, IMPACT_VEL);
        }

    }
}

struct vec2{
    float x;
    float y;
    vec2(float x, float y) : x(x), y(y){};
};


void State_Game::OnLateUpdate(float dt) {

    vec2 offsets[] = {    vec2(-1,-1), vec2(0,-1), vec2(1,-1),
                          vec2(-1, 0), vec2(0, 0), vec2(1, 0),
                          vec2(-1, 1), vec2(0, 1), vec2(1, 1)};

    vec2 camUV{IShader::camPos[0] * -0.5f, IShader::camPos[1] * -0.5f};

    Application::BindParticleFBO();
    player.OnParticleRender(dt);
    asteroids.OnParticleRender(dt);
    Application::UnbindParticleFBO();


    fullscreenQuadShader.Use();
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Application::particleFBO.color);
    fullscreenQuadShader.flipUV = false;
    fullscreenQuadShader.SetFlipUV();

    fullscreenQuadShader.SetScale(2,2);

    for(int j = 0; j < 9; j++){
        fullscreenQuadShader.SetTranslation(-IShader::camPos[0] + offsets[j].x*2, -IShader::camPos[1] + offsets[j].y*2);

        for(int i = 0; i < 9; i++){

            fullscreenQuadShader.SetTileOffset(offsets[i].x, offsets[i].y);
            fullscreenQuad.draw();
        }
    }


    glDisable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, 0);


}


