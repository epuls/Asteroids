#pragma once

#include <vector>
#include <GameObject.h>
#include <Game/GameState.h>

struct ApplicationContext;

struct Scene{
    std::string name = "default_scene";
    std::vector<GameObject> sceneObjects{};
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

    void SpawnGameObject() { currentScene->sceneObjects.emplace_back(); }

    explicit SceneManager(ApplicationContext& ctx) : m_ctx(ctx) {}
    ~SceneManager() { OnDestroy(); }

private:
    ApplicationContext& m_ctx;

    void LoadScene();
};