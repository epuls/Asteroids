#pragma once
#include <Game/GameState.h>
#include <Game/State_MainMenu.h>
#include <Game/State_Game.h>
#include <iostream>

/**
 * \class AsteroidStateFactory Inherits \ref IStateFactory
 * \brief This game's specific state factory, creates our state objects during init.
 */
class AsteroidStateFactory : public IStateFactory{
public:
    explicit AsteroidStateFactory(GameStateManager& ctx) : IStateFactory(ctx){

        auto menuState = std::make_shared<State_MainMenu>(ctx);
        auto menuName = std::string("MainMenu");
        auto statePair = std::pair<std::string, std::shared_ptr<IGameState>>(menuName, menuState);
        ctx.states.emplace(statePair);

        auto gameState = std::make_shared<State_Game>(ctx);
        auto gameName = std::string("Game");
        auto gameStatePair = std::pair<std::string, std::shared_ptr<IGameState>>(gameName, gameState);
        ctx.states.emplace(gameStatePair);


        ctx.initialState = ctx.states.at(std::string("MainMenu"));
    }

};