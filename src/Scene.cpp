#include <Game/Scene.hpp>
#include <ApplicationContext.hpp>
#include <GameObject.h>


void SceneManager::Init(){
    LoadScene();
}

void SceneManager::Start() {
    for(auto& obj : currentScene->sceneObjects){
        obj->Start();
    }

    LOG_DEBUG("Scene", "Called Start on Current Scene's Game Objects");
}

void SceneManager::Update() {
    currentScene->gameStateManager->Update(m_ctx.time->deltaTime);
    currentScene->gameStateManager->OnRender();//TODO: Build renderer class
    for(auto& obj : currentScene->sceneObjects){
        obj->Update();
    }
}

void SceneManager::FixedUpdate() {
    currentScene->gameStateManager->FixedUpdate();
    currentScene->gameStateManager->OnPhysics();//TODO: Build physics/collision class
    for(auto& obj : currentScene->sceneObjects){
        obj->FixedUpdate();
    }
}

void SceneManager::LateUpdate() {
    currentScene->gameStateManager->LateUpdate(m_ctx.time->deltaTime);
    for(auto& obj : currentScene->sceneObjects){
        obj->LateUpdate();
    }
}

void SceneManager::OnDestroy() {
    for(auto& obj : currentScene->sceneObjects){
        obj->OnDestroy();
    }
}

void SceneManager::LoadScene() {
    LOG_DEBUG("Scene", "Setting up default scene");
    currentScene = std::make_shared<Scene>();
    currentScene->gameStateManager = std::make_unique<GameStateManager>(m_ctx);
    currentScene->gameStateManager->Init();
    currentScene->sceneObjects.reserve(2);

    auto playerObj = SpawnGameObject();
    auto& mr = playerObj->AddComponent<MeshRenderer>();

    mr.mesh = m_ctx.assetManager->GetAssetHandle("ShipMesh");
    mr.material = m_ctx.assetManager->GetAssetHandle("ShipMaterial");
}


