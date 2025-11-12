#pragma once

#include <vector>
#include <GameObject.h>
#include <Game/GameState.h>

struct ApplicationContext;

struct Scene{
    std::string name = "default_scene";
    std::vector<std::shared_ptr<GameObject>> sceneObjects{};
    std::unique_ptr<GameStateManager> gameStateManager;
};


class SceneManager{
public:
    std::shared_ptr<Scene> currentScene;

    void Init();
    void Start();
    void Update();
    void FixedUpdate();
    void LateUpdate();
    void OnDestroy();

    // Need to switch to a handle slot/index system, references to objects in vector can dangle if vec resizes
    std::shared_ptr<GameObject> SpawnGameObject(){
        currentScene->sceneObjects.emplace_back(std::make_shared<GameObject>());
        return currentScene->sceneObjects.back();
    }

    std::shared_ptr<GameObject>& GetObjectFromIndex(uint32_t idx){
        return  currentScene->sceneObjects.at(idx);
    }

    explicit SceneManager(ApplicationContext& ctx) : m_ctx(ctx) {}
    ~SceneManager() { OnDestroy(); }

private:
    ApplicationContext& m_ctx;

    void LoadScene();
};