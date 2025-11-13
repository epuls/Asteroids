#pragma once

#include <Component.h>
#include <Rendering/ParticleSystem.h>


// TODO: Default particles, integrate material into particle system, register particle system with renderer
class ParticleSystemRenderer : public Component{
public:
    ParticleSystem particleSystem = ParticleSystem();

};