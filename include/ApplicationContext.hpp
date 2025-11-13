#pragma once

#include <memory>

#include <ApplicationSettings.hpp>

#include <Assets/AssetManager.hpp>

#include <Game/Input.h>
#include <Game/GameState.h>
#include <Game/Scene.hpp>
#include <Game/Time.hpp>

#include <Rendering/Renderer.h>

struct ApplicationContext{
    const std::shared_ptr<AssetManager> assetManager = std::make_shared<AssetManager>(*this);
    const std::shared_ptr<InputManager> inputManager = std::make_shared<InputManager>();
    const std::shared_ptr<ApplicationSettings> applicationSettings = std::make_shared<ApplicationSettings>();
    const std::shared_ptr<SceneManager> sceneManager = std::make_shared<SceneManager>(*this);
    const std::shared_ptr<Time> time = std::make_shared<Time>();
    const std::shared_ptr<Renderer> renderer = std::make_shared<Renderer>(*this);

    void Init(){
        assetManager->Init();
        sceneManager->Init();
    }
};