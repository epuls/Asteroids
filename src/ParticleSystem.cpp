#include <Rendering/ParticleSystem.h>
#include <cmath>
#include <random>

struct Point {
    double x;
    double y;
};

inline static int BufferCleanupCount = 0;

Point generateRandomPointInCircle(double center_x, double center_y, double radius) {
    static std::random_device rd;
    static std::mt19937 generator(rd());
    static std::uniform_real_distribution<double> radius_dist(0.0, 1.0); // For scaled radius
    static std::uniform_real_distribution<double> angle_dist(0.0, 2.0 * M_PI); // For angle

    double r_scaled = radius * std::sqrt(radius_dist(generator));
    double theta = angle_dist(generator);

    Point p;
    p.x = center_x + r_scaled * std::cos(theta);
    p.y = center_y + r_scaled * std::sin(theta);

    return p;
}


ParticleSystem::ParticleSystem(unsigned int maxParts){
    //m_particleData = ParticleData{maxParts};
    particleShader = std::make_unique<IShader>();
}

ParticleSystem::ParticleSystem(bool emitOnInit, bool isStatic, unsigned int maxParts) : EmitOnInit(emitOnInit), isStatic(isStatic), m_maxParts(maxParts){
    //m_particleData = ParticleData{maxParts};
    particleShader = std::make_unique<IShader>();
}

void ParticleSystem::Init() {
    m_instances.clear();
    m_instances.reserve(m_particleData.MAX_PARTS);
    try{
        InitBuffers();
        InitGPUData();
        if (EmitOnInit){
            Emit();
            if(isStatic){
                UpdateInstanceData(0);
                UpdateInstances(m_vboInstances, m_instances, m_particleData.MAX_PARTS);
            }
        }

    } catch (...){
        std::cout << "Particle initialization failed!\n";
    }

}

void ParticleSystem::Update(double dt) {
    try{
        m_timeSinceLastEmit += (float) dt;
        if(!isStatic){
            // Update values
            UpdateInstanceData(dt);
            // Update m_vboInstances
            UpdateInstances(m_vboInstances, m_instances, m_particleData.MAX_PARTS);
        }
        // Draw the particles
        DrawParticles();
    } catch (...){
        std::cout << "Particle Update failed!\n";
    }

}

// Need to work on updates,
void ParticleSystem::UpdateInstanceData(double dt) {
    m_timeAlive += dt;
    unsigned int tmp_alive = 0;
    unsigned int tmp_last_idx = 0;
    unsigned int iterateToIdx = m_particleData.last_alive_idx + m_emissionExtraCounter + 1;
    if (iterateToIdx >= m_particleData.MAX_PARTS) iterateToIdx = m_particleData.MAX_PARTS - 1;


    for(int i = 0; i < iterateToIdx; i++){

        if (m_particleData.kill[i]) {
            m_particleData.lifetime[i] = dt;
            m_particleData.kill[i] = 0;
        }


        m_instances[i].px += m_particleData.velocitiesX[i];
        m_instances[i].py += m_particleData.velocitiesY[i];

        // used it for alpha in some shaders, need to change and update
        m_instances[i].rot += m_particleData.velocitiesAngular[i] * (float)dt;

        m_particleData.velocitiesX[i] *= m_emissionParams.drag;
        m_particleData.velocitiesY[i] *= m_emissionParams.drag;
        m_particleData.lifetime[i] -= (float)dt;
        m_particleData.alive[i] = m_particleData.lifetime[i] <= 0 ? 0 : 1;

        tmp_alive += m_particleData.alive[i];

        m_particleData.size[i] *= m_emissionParams.sizeDecay;
        m_particleData.size[i] *= m_particleData.alive[i];

        m_instances[i].size = m_particleData.size[i];

        if (m_particleData.alive[i])
            tmp_last_idx = i;



        auto wrap_axis = [](float p, float radius) {
            const float min = -1.0f - radius;
            const float max =  1.0f + radius;
            const float W   =  max - min; // 2 + 2*radius
            // wrap for negatives:
            return min + std::fmod(std::fmod(p - min, W) + W, W);
        };

        m_instances[i].px = wrap_axis(m_instances[i].px, 0.0000001);
        m_instances[i].py = wrap_axis(m_instances[i].py, 0.0000001);
    }
    m_particleData.total_alive = tmp_alive;
    m_particleData.last_alive_idx = tmp_last_idx;
    // Test
    m_instanceCount = tmp_last_idx + 1;
    m_emissionExtraCounter = 0;

}

void ParticleSystem::DrawParticles() {
    try{
        // Eventually this will be alive count
        //m_instanceCount = m_instances.size();
        unsigned int count = m_instanceCount < m_particleData.MAX_PARTS ? m_instanceCount : m_particleData.MAX_PARTS;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        particleShader->Use();
        glBindVertexArray(m_vao);

        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, count);
        glBindVertexArray(0);

        glDisable(GL_BLEND);
        glActiveTexture(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        particleShader->Cleanup();
        glUseProgram(0);
    } catch (...){
        std::cout << "Particle Drawing failed!\n";
    }

}

inline float direction(float x1, float x2){
    return x2 - x1;
}

inline float distance(float x1, float y1, float x2, float y2){
    float dx = x2 - x1;
    float dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}

void ParticleSystem::InitPositions(const unsigned int startIdx, const unsigned int endIdx) {
    // Reset our cooldown timer
    m_timeSinceLastEmit = 0.0f;

    //TODO: Cache random values when setting emission params and use floating_index = float_index % random_values.size();
    std::random_device partRandDev;
    std::default_random_engine partRandEngine(partRandDev());

    std::uniform_real_distribution<float> spawnRand(-m_emissionParams.radius, m_emissionParams.radius);
    std::uniform_real_distribution<float> velRand(m_emissionParams.minVel, m_emissionParams.maxVel);
    std::uniform_real_distribution<float> spawnSize(m_emissionParams.sizeMin, m_emissionParams.sizeMax);
    std::uniform_real_distribution<float> lifeRand(m_emissionParams.minLife, m_emissionParams.maxLife);
    std::uniform_real_distribution<float> initRotRand(m_emissionParams.minRotAngle, m_emissionParams.maxRotAngle);
    std::uniform_real_distribution<float> velAngularRand(m_emissionParams.minVelAngular, m_emissionParams.maxVelAngular);

    unsigned int partsToSpawn = m_emissionParams.numContinuousParticles;

    for(unsigned int i = startIdx; i < endIdx; i++){
        // Pool management
        if (m_particleData.alive[i]) continue;
        m_particleData.kill[i] = 0;
        m_particleData.alive[i] = 1;

        m_queuedParticleCount -= 1;

        // SPAWN DATA
        Point p = generateRandomPointInCircle(m_emissionParams.spawnPoint[0], m_emissionParams.spawnPoint[1], m_emissionParams.radius);
        float partSize = spawnSize(partRandEngine);

        // Set generated spawn data
        m_instances[i].px = p.x;
        m_instances[i].py = p.y;
        m_instances[i].size = partSize;
        m_instances[i].rot = initRotRand(partRandEngine);
        m_particleData.velocitiesAngular[i] = velAngularRand(partRandEngine);

        // SIM DATA
        switch(m_emissionParams.velocityDirection){
            case(EmissionParams::VelocityDirection::PARTICLE_RELATIVE):
                m_particleData.velocitiesX[i] = m_emissionParams.fixedX + velRand(partRandEngine);
                m_particleData.velocitiesY[i] = m_emissionParams.fixedY + velRand(partRandEngine);
                break;
            case(EmissionParams::VelocityDirection::AWAY_FROM_SPAWN_POINT):
                auto velRandMag = velRand(partRandEngine);
                m_particleData.velocitiesX[i] = m_emissionParams.fixedX + direction(m_emissionParams.spawnPoint[0], p.x) * velRandMag;
                m_particleData.velocitiesY[i] = m_emissionParams.fixedY + direction(m_emissionParams.spawnPoint[1], p.y) * velRandMag;
                break;
        }

        m_particleData.lifetime[i] = lifeRand(partRandEngine);
        m_particleData.size[i] = partSize;




        if (m_emissionParams.emitType == EmissionParams::EmitType::CONTINUOUS){
            partsToSpawn -= 1;
            if (partsToSpawn <= 0) break;
        }

        if (m_queuedParticleCount <= 0) break;

    }

    m_instanceCount = endIdx;
}



void ParticleSystem::InitBuffers(){
    BufferCleanupCount += 4;
    // Init Buffers
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_ebo);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_vboInstances);

    // Create Vertex Array
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Quad::vertices), Quad::vertices, GL_STATIC_DRAW);

    // Create Element Array
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Quad::indices), Quad::indices, GL_STATIC_DRAW);



    // Create vertex attrrib pointers
    // stride = vertex (3 floats) + uv (2 floats) = 5 floats
    constexpr GLsizei stride = 5 * sizeof(float);

    // location 0 = pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

    // location 1 = uv
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));




    // Note: not using stride because buffer is separate
    // layout(location=2) iPosSizeRot -> (px,py,size,rot)
    glEnableVertexAttribArray(2);
    // Create instance array
    glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
    glBufferData(GL_ARRAY_BUFFER, m_particleData.MAX_PARTS*sizeof(InstanceGPU), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceGPU), (void*)0);
    glVertexAttribDivisor(2, 1);


    // clean state
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ParticleSystem::UpdateInstances(GLuint& vboInstances,
                                     std::span<const InstanceGPU> instances,
                                     int capacity) {
    if (instances.empty()) return;
    const GLsizeiptr bytes = GLsizeiptr(instances.size() * sizeof(InstanceGPU));
    const GLsizeiptr cap   = GLsizeiptr(capacity * sizeof(InstanceGPU));
    if (bytes > cap) {
        std::cout<<"WARNING BYTES > CAP\nABORTING FOR DEBUG\n";
        m_instanceCount = 0;
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, vboInstances);


        glBufferData(GL_ARRAY_BUFFER, cap, nullptr, GL_DYNAMIC_DRAW);
        void* dst = glMapBufferRange(GL_ARRAY_BUFFER, 0, bytes,
                                     GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
        std::memcpy(dst, instances.data(), size_t(bytes));
        glUnmapBuffer(GL_ARRAY_BUFFER);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //m_instanceCount = m_instances.size();
    }


}

void ParticleSystem::DisposeBuffers() {
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    if (m_vboInstances)   { glDeleteBuffers(1, &m_vboInstances);   m_vboInstances = 0; BufferCleanupCount--;}
    if (m_vbo)            { glDeleteBuffers(1, &m_vbo);            m_vbo = 0; BufferCleanupCount--;}
    if (m_ebo)            { glDeleteBuffers(1, &m_ebo);            m_ebo = 0; BufferCleanupCount--;}
    if (m_vao)            { glDeleteVertexArrays(1, &m_vao);        m_vao = 0; BufferCleanupCount--;}



}

void ParticleSystem::Shutdown(){
    for(int i = 0; i < m_particleData.MAX_PARTS; i++){
        m_particleData.kill[i] = 1;
    }
    DisposeBuffers();
    particleShader->Cleanup();
    m_instances.clear();
    m_queuedParticleCount = 0;
    m_instanceCount = 0;
    m_timeSinceLastEmit = 9999;
    m_timeAlive = 0;
    m_emissionExtraCounter = 0;
}


void ParticleSystem::Emit() {
    //if (m_timeSinceLastEmit < m_emissionParams.cooldown) return;
    m_emissionExtraCounter = m_emissionParams.numBurstParticles;
    m_instanceCount += m_emissionParams.numBurstParticles;
    m_queuedParticleCount += m_emissionParams.numBurstParticles;
    m_instanceCount = m_instanceCount < m_particleData.MAX_PARTS ? m_instanceCount : m_particleData.MAX_PARTS;

    m_queuedParticleCount = m_queuedParticleCount < m_particleData.MAX_PARTS ? m_queuedParticleCount : m_particleData.MAX_PARTS;
    InitPositions(0, m_instanceCount);
}

void ParticleSystem::EmitAt(float x, float y, float vx, float vy) {
    if (m_timeSinceLastEmit < m_emissionParams.cooldown) return;
    m_emissionParams.spawnPoint[0] = x;
    m_emissionParams.spawnPoint[1] = y;

    m_emissionParams.fixedX = vx;
    m_emissionParams.fixedY = vy;

    Emit();
}

void ParticleSystem::InitGPUData() {

    for(unsigned int i = 0; i < m_particleData.MAX_PARTS; i++){
        m_instances.emplace_back(InstanceGPU(0, 0, 1, 1));
    }

}

// Ensure our buffers are properly disposed of
ParticleSystem::~ParticleSystem() {
    DisposeBuffers();
}




