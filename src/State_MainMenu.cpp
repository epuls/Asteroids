#include <Game/State_MainMenu.h>
#include <iostream>
#include <Application.h>
#include <glad/glad.h>


void State_MainMenu::EnterState() {
    m_ctx.inputManager->SpaceKey.onKey.AddOnPressed<&State_MainMenu::OnSpaceButton>(this, 0);
    uiShader.Create(); uiShader.SetUniformLocations();
    uiQuad.program = uiShader.GetProgram();
    uiQuad.InitBuffers();
    auto fontStr = ApplicationSettings::DIRECTORY + "/resources/fonts/MomoTrustDisplay-Regular.ttf";
    renderText = std::make_unique<RenderText>(fontStr.c_str(), "Press Space to Begin", 100);

    uiShader.Use();
    uiShader.SetScale(1.0, renderText->YScaleToPreserveAspect(1.0));
    uiShader.SetTranslation(0.0, 0.0);

}

void State_MainMenu::ExitState() {
    m_ctx.inputManager->SpaceKey.onKey.RemoveOnPressed<&State_MainMenu::OnSpaceButton>(this, 0);
    m_startGame = false;
    uiQuad.cleanup();
}

void State_MainMenu::CheckSwitchStates() {
    if (m_startGame) m_ctx.SwitchStates(m_ctx.states.at("Game"));

}

void State_MainMenu::OnUpdate(float dt) {
    // Oscillate text opacity
    m_stateTime += dt;
    float s = std::sinf(m_stateTime * 0.75f);
    float opacity = std::fabs(s);
    uiShader.SetOpacity(opacity + 0.1f);
}


void State_MainMenu::OnSpaceButton(KeyState state) {

    switch(state){
        case(PRESSED):
            m_startGame = true;
            break;
        case(HELD):
            m_startGame = true;
            break;
        default:
            //do nothing
            break;
    }
}

void State_MainMenu::OnRender() {
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    uiShader.Use();
    glBindTexture(GL_TEXTURE_2D, renderText->id);
    uiQuad.draw();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}
