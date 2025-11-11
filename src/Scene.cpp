#include <Game/Scene.hpp>
#include <ApplicationContext.hpp>


void SceneManager::Init(){
    LoadScene();
}

void SceneManager::Start() {
    for(auto& obj : currentScene->sceneObjects){
        obj.Start();
    }
}

void SceneManager::Update() {
    currentScene->gameStateManager->Update(m_ctx.time->deltaTime);
    currentScene->gameStateManager->OnRender();//TODO: Build renderer class
    for(auto& obj : currentScene->sceneObjects){
        obj.Update();
    }
}

void SceneManager::FixedUpdate() {
    currentScene->gameStateManager->FixedUpdate();
    currentScene->gameStateManager->OnPhysics();//TODO: Build physics/collision class
    for(auto& obj : currentScene->sceneObjects){
        obj.FixedUpdate();
    }
}

void SceneManager::LateUpdate() {
    currentScene->gameStateManager->LateUpdate(m_ctx.time->deltaTime);
    for(auto& obj : currentScene->sceneObjects){
        obj.LateUpdate();
    }
}

void SceneManager::OnDestroy() {
    for(auto& obj : currentScene->sceneObjects){
        obj.OnDestroy();
    }
}

void SceneManager::LoadScene() {
    currentScene = std::make_shared<Scene>();
    currentScene->gameStateManager = std::make_unique<GameStateManager>(m_ctx);
    currentScene->gameStateManager->Init();

    SpawnGameObject();
    SpawnGameObject();



}


