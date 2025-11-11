#pragma once



class Time{
public:
    double applicationTime = 0;
    double deltaTime = 0;
    double fixedUpdateRate = 1.0f/60.0f;

    bool fixedUpdate = false;

    void HandleTime(double dt){
        applicationTime += dt;
        m_fixedTickTime += dt;
        deltaTime = dt;

        if (m_fixedTickTime >= fixedUpdateRate) fixedUpdate = true;
    }

    void HandleFixedTick(){
        fixedUpdate = false;
        m_fixedTickTime = 0;
    }

private:
    double m_fixedTickTime = 0;
};