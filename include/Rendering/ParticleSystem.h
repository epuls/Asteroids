#pragma once
#include <glad/glad.h>
#include <vector>
#include <array>
#include <span>
#include <memory>

#include <Rendering/Quad.hpp>
#include <Rendering/Shader.hpp>

/**
 * \struct InstanceGPU
 * \brief GPU representation of an individual particle in the particle system.
 * \details These are copied into Vertex Attribute Arrays and instanced.
 * Contains x/y coordinates, size, and rotation information
 */
struct InstanceGPU { float px, py, size, rot;};

/**
 * \struct ParticleData
 *  \brief CPU representation (struct of arrays) of all particles in a particle system
 *  \details In an effort to introduce minimal overhead, raw pointers are manually handled
 *  and should not be accessed by any other system.
 */
struct ParticleData{
    unsigned int MAX_PARTS;
    GLuint shaderProgram = 0;
    unsigned int total_alive = 0;
    unsigned int last_alive_idx = 0;

    float* velocitiesX;
    float* velocitiesY;
    float* velocitiesAngular;
    float* size;
    float* rotation;
    float* lifetime;
    unsigned int* alive;
    unsigned int* kill;

    explicit ParticleData(unsigned int max_parts = 1000) : MAX_PARTS(max_parts){
        velocitiesX = new float[max_parts];
        velocitiesY = new float[max_parts];
        velocitiesAngular = new float[max_parts];
        size = new float[max_parts];
        rotation = new float[max_parts];
        lifetime = new float[max_parts];
        alive = new unsigned int[max_parts];
        kill = new unsigned int[max_parts];
    }

    ~ParticleData(){
        delete velocitiesX;
        delete velocitiesY;
        delete velocitiesAngular;
        delete size;
        delete rotation;
        delete lifetime;
        delete alive;
        delete kill;
    }
};


/**
 * \struct EmissionParams
 * \brief Controls the emission parameters of the particle system
 * \details Contains a wide variety of parameters to alter spawn behavior, velocity, and attributes
 *  over time.
 */
struct EmissionParams{
    enum EmitType{
        BURST,
        CONTINUOUS
    };

    enum VelocityDirection{
        PARTICLE_RELATIVE,
        AWAY_FROM_SPAWN_POINT
    };

    EmitType emitType = EmitType::BURST;
    VelocityDirection velocityDirection = VelocityDirection::PARTICLE_RELATIVE;

    float cooldown = 0.0f;
    unsigned int numContinuousParticles = 1; //for OVER_TIME
    unsigned int numBurstParticles = 100;


    float spawnPoint[2]{
        0,0
    };
    float radius = 3;

    float sizeMin = 0.001;
    float sizeMax = 0.005;

    float minVel = -0.000;
    float maxVel = +0.000;

    float minVelAngular = -0.000;
    float maxVelAngular = +0.000;

    float minRotAngle = 0.000;
    float maxRotAngle = 360.000;

    float fixedX = 0.0f;
    float fixedY = 0.0f;

    float minLife = 1.0f;
    float maxLife = 1.0f;

    float drag = 1.0f;
    float sizeDecay = 1.0f;
};


/**
 * \class ParticleSystem
 * \brief Core particle system class, containing logic for initialization, emission, updating, and shutdown
 * \details Class contains logic for all stages of life of the class, handling buffer management,
 * simulation updates, emission, and the release of resources
 *
 */
class ParticleSystem{
public:
    explicit ParticleSystem(unsigned int maxParts = 1000);
    ParticleSystem(bool emitOnInit, bool isStatic, unsigned int maxParts = 1000);
    ~ParticleSystem();


    void Init();
    void Update(double dt);
    void Shutdown();

    void Emit();
    void EmitAt(float x, float y, float vx, float vy);

    [[nodiscard]] inline bool GetCanEmit() const{
        if (m_timeSinceLastEmit < m_emissionParams.cooldown) return false;
        return true;
    }

    inline ParticleData& GetParticleData(){
        return m_particleData;
    }

    inline std::vector<InstanceGPU>& GetInstanceData(){
        return m_instances;
    }

    inline EmissionParams& GetEmissionParams(){
        return m_emissionParams;
    }

    void SetEmissionParams(EmissionParams& params){
        m_emissionParams = params;
    }

    bool EmitOnInit = true;
    bool isStatic = false;
    bool debugPrint = false;
    bool drawTorus = true;

    std::unique_ptr<IShader> particleShader;


private:
    unsigned int m_maxParts;
    unsigned int m_emissionExtraCounter = 0;

    ParticleData m_particleData{m_maxParts};
    EmissionParams m_emissionParams{};
    float m_timeSinceLastEmit = 99999.000f; //Set arbitrarily high so we don't start with a cool-down

    unsigned int m_instanceCount;
    std::vector<InstanceGPU> m_instances{};

    GLuint m_vao = 0, m_ebo = 0, m_vbo = 0, m_vboInstances = 0;
    double m_timeAlive = 0;
    unsigned int m_queuedParticleCount;

    // Lifecycle functions
    void InitGPUData();
    void InitPositions(const unsigned int startIdx, const unsigned int endIdx);
    void DrawParticles();
    void InitBuffers();
    void UpdateInstanceData(double dt);
    void UpdateInstances(GLuint& vboInstances,
                         std::span<const InstanceGPU> instances,
                         int capacity);

    // Small helper function
    inline void UpdateInstances(GLuint vboInstances,
                                const std::vector<InstanceGPU>& instances,
                                int capacity)
    {
        UpdateInstances(vboInstances, std::span<const InstanceGPU>(instances), capacity);
    }

    void DisposeBuffers();

};