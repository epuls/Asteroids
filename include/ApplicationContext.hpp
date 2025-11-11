#pragma once

#include <memory>
#include <Assets/AssetManager.hpp>
#include <Game/Input.h>
#include <ApplicationSettings.hpp>

#include <Game/GameState.h>

struct ApplicationContext{
    const std::shared_ptr<AssetManager> assetManager = std::make_shared<AssetManager>(*this);
    const std::shared_ptr<InputManager> inputManager = std::make_shared<InputManager>();
    const std::shared_ptr<ApplicationSettings> applicationSettings = std::make_shared<ApplicationSettings>();
    const std::shared_ptr<GameStateManager> gameStateManager = std::make_shared<GameStateManager>(*this);
};