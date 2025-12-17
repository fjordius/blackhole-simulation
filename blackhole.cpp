#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <cmath>
#include <cstdlib>

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const float BH_PI = 3.14159265359f;

struct Particle {
    Vector3 pos;
    float life;
    float maxLife;
    Color color;
    float orbitRadius;
    float orbitAngle;
    float orbitSpeed;
    float orbitHeight;
};

struct Star {
    Vector3 pos;
    float brightness;
    float twinkleSpeed;
    float twinkleOffset;
    Color color;
};

class BlackHole {
public:
    Vector3 position;
    float mass;
    float eventHorizonRadius;
    float accretionDiskInner;
    float accretionDiskOuter;
    float rotationSpeed;
    float currentRotation;
    
    BlackHole() {
        position = {0, 0, 0};
        mass = 50.0f;
        eventHorizonRadius = 2.0f;
        accretionDiskInner = 3.5f;
        accretionDiskOuter = 14.0f;
        rotationSpeed = 0.4f;
        currentRotation = 0;
    }
    
    void update(float dt) {
        currentRotation += rotationSpeed * dt;
    }
    
    Vector3 getGravity(Vector3 point) {
        Vector3 dir = Vector3Subtract(position, point);
        float dist = Vector3Length(dir);
        if (dist < 0.1f) dist = 0.1f;
        float strength = mass / (dist * dist);
        return Vector3Scale(Vector3Normalize(dir), strength);
    }
};

class SpacetimeGrid {
public:
    BlackHole* blackHole;
    int gridSize;
    float gridSpacing;
    float warpStrength;
    
    SpacetimeGrid(BlackHole* bh) {
        blackHole = bh;
        gridSize = 30;
        gridSpacing = 2.0f;
        warpStrength = 8.0f;
    }
    
    float getWarp(float x, float z) {
        float dist = sqrtf(x * x + z * z);
        if (dist < blackHole->eventHorizonRadius) return -100.0f;
        return -warpStrength / (dist * 0.5f);
    }
    
    void draw(float time) {
        float offset = gridSize * gridSpacing * 0.5f;
        float pulse = sinf(time * 0.5f) * 0.2f + 1.0f;
        
        for (int i = 0; i <= gridSize; i++) {
            for (int j = 0; j < gridSize; j++) {
                float x1 = i * gridSpacing - offset;
                float z1 = j * gridSpacing - offset;
                float z2 = (j + 1) * gridSpacing - offset;
                
                float y1 = getWarp(x1, z1);
                float y2 = getWarp(x1, z2);
                
                if (y1 < -50 || y2 < -50) continue;
                
                float dist1 = sqrtf(x1 * x1 + z1 * z1);
                float dist2 = sqrtf(x1 * x1 + z2 * z2);
                
                float intensity1 = 1.0f / (1.0f + dist1 * 0.1f);
                float intensity2 = 1.0f / (1.0f + dist2 * 0.1f);
                
                Color c1 = {
                    (unsigned char)(50 * intensity1 * pulse),
                    (unsigned char)(100 * intensity1 * pulse),
                    (unsigned char)(255 * intensity1 * pulse),
                    (unsigned char)(100 * intensity1)
                };
                
                DrawLine3D({x1, y1, z1}, {x1, y2, z2}, c1);
            }
        }
        
        for (int j = 0; j <= gridSize; j++) {
            for (int i = 0; i < gridSize; i++) {
                float x1 = i * gridSpacing - offset;
                float x2 = (i + 1) * gridSpacing - offset;
                float z1 = j * gridSpacing - offset;
                
                float y1 = getWarp(x1, z1);
                float y2 = getWarp(x2, z1);
                
                if (y1 < -50 || y2 < -50) continue;
                
                float dist1 = sqrtf(x1 * x1 + z1 * z1);
                float intensity1 = 1.0f / (1.0f + dist1 * 0.1f);
                
                Color c1 = {
                    (unsigned char)(50 * intensity1 * pulse),
                    (unsigned char)(100 * intensity1 * pulse),
                    (unsigned char)(255 * intensity1 * pulse),
                    (unsigned char)(100 * intensity1)
                };
                
                DrawLine3D({x1, y1, z1}, {x2, y2, z1}, c1);
            }
        }
    }
};

class GravityFieldLines {
public:
    BlackHole* blackHole;
    int lineCount;
    std::vector<std::vector<Vector3>> fieldLines;
    
    GravityFieldLines(BlackHole* bh) {
        blackHole = bh;
        lineCount = 24;
        generateLines();
    }
    
    void generateLines() {
        fieldLines.clear();
        
        for (int i = 0; i < lineCount; i++) {
            std::vector<Vector3> line;
            float angle = (float)i / lineCount * BH_PI * 2.0f;
            float startDist = 25.0f;
            
            Vector3 pos = {cosf(angle) * startDist, 0, sinf(angle) * startDist};
            
            for (int step = 0; step < 100; step++) {
                line.push_back(pos);
                
                Vector3 gravity = blackHole->getGravity(pos);
                pos = Vector3Add(pos, Vector3Scale(gravity, 0.15f));
                
                float dist = Vector3Length(pos);
                if (dist < blackHole->eventHorizonRadius * 1.2f) break;
            }
            
            fieldLines.push_back(line);
        }
    }
    
    void draw(float time) {
        for (size_t i = 0; i < fieldLines.size(); i++) {
            const auto& line = fieldLines[i];
            
            for (size_t j = 1; j < line.size(); j++) {
                float t = (float)j / line.size();
                float wave = sinf(time * 3.0f + t * 10.0f + i) * 0.5f + 0.5f;
                
                Color c = {
                    (unsigned char)(100 + 155 * t * wave),
                    (unsigned char)(50 * wave),
                    (unsigned char)(200 * (1.0f - t)),
                    (unsigned char)(200 * t)
                };
                
                DrawLine3D(line[j-1], line[j], c);
            }
        }
    }
};

class EinsteinRing {
public:
    BlackHole* blackHole;
    int segments;
    int layers;
    
    EinsteinRing(BlackHole* bh) {
        blackHole = bh;
        segments = 128;
        layers = 5;
    }
    
    void draw(float time, Camera3D camera) {
        Vector3 toCamera = Vector3Normalize(Vector3Subtract(camera.position, blackHole->position));
        Vector3 up = {0, 1, 0};
        Vector3 right = Vector3Normalize(Vector3CrossProduct(up, toCamera));
        Vector3 ringUp = Vector3Normalize(Vector3CrossProduct(toCamera, right));
        
        for (int layer = 0; layer < layers; layer++) {
            float radius = blackHole->eventHorizonRadius * (2.6f + layer * 0.15f);
            float layerT = (float)layer / layers;
            
            for (int i = 0; i < segments; i++) {
                float angle1 = (float)i / segments * BH_PI * 2.0f;
                float angle2 = (float)(i + 1) / segments * BH_PI * 2.0f;
                
                float flicker = sinf(time * 20.0f + angle1 * 5.0f + layer * 2.0f) * 0.3f + 0.7f;
                float wave = sinf(angle1 * 3.0f - time * 4.0f) * 0.1f;
                float r = radius + wave;
                
                Vector3 p1 = Vector3Add(blackHole->position, 
                    Vector3Add(Vector3Scale(right, cosf(angle1) * r), Vector3Scale(ringUp, sinf(angle1) * r)));
                Vector3 p2 = Vector3Add(blackHole->position, 
                    Vector3Add(Vector3Scale(right, cosf(angle2) * r), Vector3Scale(ringUp, sinf(angle2) * r)));
                
                float brightness = flicker * (1.0f - layerT * 0.5f);
                Color c;
                
                if (layer == 0) {
                    c = {
                        (unsigned char)(255 * brightness),
                        (unsigned char)(220 * brightness),
                        (unsigned char)(180 * brightness),
                        (unsigned char)(255 * brightness)
                    };
                } else {
                    c = {
                        (unsigned char)(255 * brightness * 0.8f),
                        (unsigned char)(200 * brightness * 0.6f),
                        (unsigned char)(100 * brightness * 0.4f),
                        (unsigned char)(200 * (1.0f - layerT) * brightness)
                    };
                }
                
                DrawLine3D(p1, p2, c);
            }
        }
        
        for (int i = 0; i < 50; i++) {
            float angle = (float)rand() / RAND_MAX * BH_PI * 2.0f;
            float r = blackHole->eventHorizonRadius * (2.5f + (float)rand() / RAND_MAX * 0.8f);
            float flicker = sinf(time * 30.0f + i * 0.5f);
            
            if (flicker > 0.7f) {
                Vector3 sparkPos = Vector3Add(blackHole->position,
                    Vector3Add(Vector3Scale(right, cosf(angle) * r), Vector3Scale(ringUp, sinf(angle) * r)));
                DrawPoint3D(sparkPos, WHITE);
            }
        }
    }
};

class AccretionDisk {
public:
    std::vector<Particle> particles;
    int particleCount;
    BlackHole* blackHole;
    
    AccretionDisk(BlackHole* bh, int count) {
        blackHole = bh;
        particleCount = count;
        initParticles();
    }
    
    void initParticles() {
        particles.clear();
        particles.reserve(particleCount);
        for (int i = 0; i < particleCount; i++) {
            spawnParticle();
        }
    }
    
    void spawnParticle() {
        Particle p;
        p.orbitRadius = blackHole->accretionDiskInner + 
            (float)rand() / RAND_MAX * (blackHole->accretionDiskOuter - blackHole->accretionDiskInner);
        p.orbitAngle = (float)rand() / RAND_MAX * BH_PI * 2.0f;
        p.orbitHeight = ((float)rand() / RAND_MAX - 0.5f) * 0.6f * (1.0f - (p.orbitRadius - blackHole->accretionDiskInner) / 
            (blackHole->accretionDiskOuter - blackHole->accretionDiskInner) * 0.5f);
        
        p.pos.x = cosf(p.orbitAngle) * p.orbitRadius;
        p.pos.y = p.orbitHeight;
        p.pos.z = sinf(p.orbitAngle) * p.orbitRadius;
        
        float orbitVel = sqrtf(blackHole->mass / p.orbitRadius) * 0.15f;
        p.orbitSpeed = orbitVel / p.orbitRadius;
        
        p.maxLife = 10.0f + (float)rand() / RAND_MAX * 20.0f;
        p.life = (float)rand() / RAND_MAX * p.maxLife;
        
        float temp = 1.0f - (p.orbitRadius - blackHole->accretionDiskInner) / 
            (blackHole->accretionDiskOuter - blackHole->accretionDiskInner);
        
        if (temp > 0.85f) {
            p.color = {255, 255, 255, 255};
        } else if (temp > 0.7f) {
            p.color = {255, 240, 200, 255};
        } else if (temp > 0.5f) {
            p.color = {255, 200, 120, 255};
        } else if (temp > 0.3f) {
            p.color = {255, 140, 60, 255};
        } else if (temp > 0.15f) {
            p.color = {255, 80, 30, 255};
        } else {
            p.color = {180, 40, 20, 255};
        }
        
        particles.push_back(p);
    }
    
    void update(float dt) {
        for (size_t i = 0; i < particles.size(); i++) {
            Particle& p = particles[i];
            
            p.orbitAngle += p.orbitSpeed * dt;
            
            float spiralFactor = 0.02f * dt;
            p.orbitRadius -= spiralFactor * (blackHole->mass / (p.orbitRadius * p.orbitRadius)) * 0.01f;
            
            p.pos.x = cosf(p.orbitAngle) * p.orbitRadius;
            p.pos.z = sinf(p.orbitAngle) * p.orbitRadius;
            p.pos.y = p.orbitHeight * (p.orbitRadius / blackHole->accretionDiskOuter);
            p.pos.y += sinf(p.orbitAngle * 3.0f + p.orbitRadius) * 0.08f;
            
            p.life -= dt;
            
            if (p.life <= 0 || p.orbitRadius < blackHole->eventHorizonRadius) {
                p.orbitRadius = blackHole->accretionDiskInner + 
                    (float)rand() / RAND_MAX * (blackHole->accretionDiskOuter - blackHole->accretionDiskInner);
                p.orbitAngle = (float)rand() / RAND_MAX * BH_PI * 2.0f;
                p.life = p.maxLife;
            }
        }
    }
    
    void draw(float time) {
        for (const Particle& p : particles) {
            float dopplerAngle = p.orbitAngle + BH_PI * 0.5f;
            float doppler = 0.6f + 0.4f * sinf(dopplerAngle);
            
            Color c = p.color;
            c.r = (unsigned char)(c.r * doppler);
            c.g = (unsigned char)(c.g * doppler);
            c.b = (unsigned char)(c.b * doppler * 0.8f);
            
            DrawPoint3D(p.pos, c);
        }
    }
};

class DiskGlow {
public:
    BlackHole* blackHole;
    int segments;
    int rings;
    
    DiskGlow(BlackHole* bh) {
        blackHole = bh;
        segments = 80;
        rings = 25;
    }
    
    void draw(float time) {
        for (int r = 0; r < rings; r++) {
            float radiusT = (float)r / rings;
            float radius = blackHole->accretionDiskInner + radiusT * (blackHole->accretionDiskOuter - blackHole->accretionDiskInner);
            
            float temp = 1.0f - radiusT;
            Color baseColor;
            if (temp > 0.8f) {
                baseColor = {255, 255, 255, 255};
            } else if (temp > 0.5f) {
                float t = (temp - 0.5f) / 0.3f;
                baseColor = {255, (unsigned char)(200 + t * 55), (unsigned char)(150 + t * 105), 255};
            } else if (temp > 0.2f) {
                float t = (temp - 0.2f) / 0.3f;
                baseColor = {255, (unsigned char)(100 + t * 100), (unsigned char)(30 + t * 120), 255};
            } else {
                float t = temp / 0.2f;
                baseColor = {(unsigned char)(150 + t * 105), (unsigned char)(30 + t * 70), (unsigned char)(10 + t * 20), 255};
            }
            
            for (int s = 0; s < segments; s++) {
                float angle1 = (float)s / segments * BH_PI * 2.0f + time * blackHole->rotationSpeed;
                float angle2 = (float)(s + 1) / segments * BH_PI * 2.0f + time * blackHole->rotationSpeed;
                
                float height1 = sinf(angle1 * 2.0f + radius) * 0.12f * (1.0f - radiusT);
                float height2 = sinf(angle2 * 2.0f + radius) * 0.12f * (1.0f - radiusT);
                
                Vector3 p1 = {cosf(angle1) * radius, height1, sinf(angle1) * radius};
                Vector3 p2 = {cosf(angle2) * radius, height2, sinf(angle2) * radius};
                
                float dopplerAngle = angle1 + BH_PI * 0.5f;
                float doppler = 0.5f + 0.5f * sinf(dopplerAngle);
                doppler = 0.4f + doppler * 0.6f;
                
                float brightness = (sinf(angle1 * 8.0f - time * 4.0f + radius * 2.0f) * 0.3f + 0.7f);
                brightness *= doppler;
                brightness *= (1.0f - radiusT * 0.5f);
                
                Color c = baseColor;
                c.r = (unsigned char)(c.r * brightness);
                c.g = (unsigned char)(c.g * brightness);
                c.b = (unsigned char)(c.b * brightness);
                c.a = (unsigned char)(220 * (1.0f - radiusT * 0.6f));
                
                DrawLine3D(p1, p2, c);
            }
        }
    }
};

class PhotonSphere {
public:
    BlackHole* blackHole;
    int particleCount;
    std::vector<float> angles;
    std::vector<float> speeds;
    std::vector<float> phases;
    
    PhotonSphere(BlackHole* bh) {
        blackHole = bh;
        particleCount = 150;
        
        for (int i = 0; i < particleCount; i++) {
            angles.push_back((float)rand() / RAND_MAX * BH_PI * 2.0f);
            speeds.push_back(3.0f + (float)rand() / RAND_MAX * 3.0f);
            phases.push_back((float)rand() / RAND_MAX * BH_PI * 2.0f);
        }
    }
    
    void draw(float time) {
        float radius = blackHole->eventHorizonRadius * 1.5f;
        
        for (int i = 0; i < particleCount; i++) {
            float angle = angles[i] + time * speeds[i];
            float heightAngle = phases[i] + time * speeds[i] * 0.7f;
            
            float x = cosf(angle) * radius * cosf(heightAngle * 0.5f);
            float y = sinf(heightAngle * 0.5f) * radius * 0.3f;
            float z = sinf(angle) * radius * cosf(heightAngle * 0.5f);
            
            float brightness = 0.5f + 0.5f * sinf(time * 15.0f + phases[i]);
            Color c = {
                (unsigned char)(255 * brightness),
                (unsigned char)(230 * brightness),
                (unsigned char)(150 * brightness),
                255
            };
            
            DrawPoint3D({x, y, z}, c);
        }
    }
};

class Starfield {
public:
    std::vector<Star> stars;
    int starCount;
    
    Starfield(int count) {
        starCount = count;
        generateStars();
    }
    
    void generateStars() {
        stars.reserve(starCount);
        for (int i = 0; i < starCount; i++) {
            Star s;
            float theta = (float)rand() / RAND_MAX * BH_PI * 2.0f;
            float phi = acosf(2.0f * (float)rand() / RAND_MAX - 1.0f);
            float radius = 80.0f + (float)rand() / RAND_MAX * 40.0f;
            
            s.pos.x = radius * sinf(phi) * cosf(theta);
            s.pos.y = radius * sinf(phi) * sinf(theta);
            s.pos.z = radius * cosf(phi);
            
            s.brightness = 0.3f + (float)rand() / RAND_MAX * 0.7f;
            s.twinkleSpeed = 1.0f + (float)rand() / RAND_MAX * 4.0f;
            s.twinkleOffset = (float)rand() / RAND_MAX * BH_PI * 2.0f;
            
            float colorRand = (float)rand() / RAND_MAX;
            if (colorRand > 0.9f) {
                s.color = {255, 200, 150, 255};
            } else if (colorRand > 0.8f) {
                s.color = {150, 180, 255, 255};
            } else {
                s.color = {255, 255, 255, 255};
            }
            
            stars.push_back(s);
        }
    }
    
    void draw(float time) {
        for (const Star& s : stars) {
            float twinkle = 0.7f + 0.3f * sinf(time * s.twinkleSpeed + s.twinkleOffset);
            float b = s.brightness * twinkle;
            
            Color c = {
                (unsigned char)(s.color.r * b),
                (unsigned char)(s.color.g * b),
                (unsigned char)(s.color.b * b),
                255
            };
            
            DrawPoint3D(s.pos, c);
        }
    }
};

class InfallingMatter {
public:
    struct Streamer {
        Vector3 pos;
        Vector3 vel;
        std::vector<Vector3> trail;
        int maxTrail;
        Color color;
        bool active;
        float life;
    };
    
    std::vector<Streamer> streamers;
    BlackHole* blackHole;
    int maxStreamers;
    
    InfallingMatter(BlackHole* bh, int count) {
        blackHole = bh;
        maxStreamers = count;
        
        for (int i = 0; i < maxStreamers; i++) {
            spawnStreamer();
        }
    }
    
    void spawnStreamer() {
        Streamer s;
        float angle = (float)rand() / RAND_MAX * BH_PI * 2.0f;
        float dist = 18.0f + (float)rand() / RAND_MAX * 12.0f;
        float height = ((float)rand() / RAND_MAX - 0.5f) * 8.0f;
        
        s.pos = {cosf(angle) * dist, height, sinf(angle) * dist};
        
        Vector3 toCenter = Vector3Normalize(Vector3Subtract(blackHole->position, s.pos));
        Vector3 perpendicular = Vector3CrossProduct(toCenter, {0, 1, 0});
        perpendicular = Vector3Normalize(perpendicular);
        
        float tangentStrength = 0.5f + (float)rand() / RAND_MAX * 0.5f;
        s.vel = Vector3Add(
            Vector3Scale(toCenter, 2.0f),
            Vector3Scale(perpendicular, tangentStrength * 3.0f)
        );
        
        s.maxTrail = 30;
        s.active = true;
        s.life = 15.0f + (float)rand() / RAND_MAX * 10.0f;
        
        float colorChoice = (float)rand() / RAND_MAX;
        if (colorChoice > 0.7f) {
            s.color = {255, 220, 150, 255};
        } else if (colorChoice > 0.4f) {
            s.color = {255, 160, 80, 255};
        } else {
            s.color = {255, 100, 50, 255};
        }
        
        s.trail.reserve(s.maxTrail);
        streamers.push_back(s);
    }
    
    void update(float dt) {
        for (size_t i = 0; i < streamers.size(); i++) {
            Streamer& s = streamers[i];
            
            if (!s.active) continue;
            
            Vector3 gravity = blackHole->getGravity(s.pos);
            s.vel = Vector3Add(s.vel, Vector3Scale(gravity, dt));
            s.pos = Vector3Add(s.pos, Vector3Scale(s.vel, dt));
            
            s.trail.push_back(s.pos);
            if (s.trail.size() > (size_t)s.maxTrail) {
                s.trail.erase(s.trail.begin());
            }
            
            s.life -= dt;
            
            float dist = Vector3Length(s.pos);
            if (dist < blackHole->eventHorizonRadius || s.life <= 0 || dist > 50.0f) {
                s.trail.clear();
                float angle = (float)rand() / RAND_MAX * BH_PI * 2.0f;
                float spawnDist = 18.0f + (float)rand() / RAND_MAX * 12.0f;
                float height = ((float)rand() / RAND_MAX - 0.5f) * 8.0f;
                s.pos = {cosf(angle) * spawnDist, height, sinf(angle) * spawnDist};
                
                Vector3 toCenter = Vector3Normalize(Vector3Subtract(blackHole->position, s.pos));
                Vector3 perpendicular = Vector3CrossProduct(toCenter, {0, 1, 0});
                perpendicular = Vector3Normalize(perpendicular);
                float tangentStrength = 0.5f + (float)rand() / RAND_MAX * 0.5f;
                s.vel = Vector3Add(Vector3Scale(toCenter, 2.0f), Vector3Scale(perpendicular, tangentStrength * 3.0f));
                s.life = 15.0f + (float)rand() / RAND_MAX * 10.0f;
            }
        }
    }
    
    void draw() {
        for (const Streamer& s : streamers) {
            if (!s.active || s.trail.size() < 2) continue;
            
            for (size_t i = 1; i < s.trail.size(); i++) {
                float t = (float)i / s.trail.size();
                Color c = s.color;
                c.r = (unsigned char)(c.r * (0.3f + t * 0.7f));
                c.g = (unsigned char)(c.g * (0.2f + t * 0.8f));
                c.b = (unsigned char)(c.b * (0.1f + t * 0.9f));
                c.a = (unsigned char)(t * 255);
                DrawLine3D(s.trail[i-1], s.trail[i], c);
            }
            
            DrawPoint3D(s.pos, s.color);
        }
    }
};

class JetStream {
public:
    struct JetParticle {
        Vector3 pos;
        Vector3 vel;
        float life;
        float maxLife;
    };
    
    std::vector<JetParticle> particles;
    BlackHole* blackHole;
    int maxParticles;
    bool topJet;
    
    JetStream(BlackHole* bh, bool top, int count) {
        blackHole = bh;
        topJet = top;
        maxParticles = count;
        particles.reserve(maxParticles);
    }
    
    void update(float dt, float time) {
        if ((int)particles.size() < maxParticles) {
            JetParticle p;
            float angle = (float)rand() / RAND_MAX * BH_PI * 2.0f;
            float radius = 0.2f + (float)rand() / RAND_MAX * 0.4f;
            
            p.pos = blackHole->position;
            p.pos.x += cosf(angle) * radius;
            p.pos.z += sinf(angle) * radius;
            p.pos.y += topJet ? blackHole->eventHorizonRadius : -blackHole->eventHorizonRadius;
            
            float speed = 10.0f + (float)rand() / RAND_MAX * 5.0f;
            float spread = 0.15f;
            p.vel = {
                ((float)rand() / RAND_MAX - 0.5f) * spread,
                topJet ? speed : -speed,
                ((float)rand() / RAND_MAX - 0.5f) * spread
            };
            
            p.maxLife = 2.5f + (float)rand() / RAND_MAX * 1.5f;
            p.life = p.maxLife;
            
            particles.push_back(p);
        }
        
        for (size_t i = 0; i < particles.size(); i++) {
            JetParticle& p = particles[i];
            
            p.pos = Vector3Add(p.pos, Vector3Scale(p.vel, dt));
            p.life -= dt;
            
            p.vel.x += sinf(time * 6.0f + p.pos.y) * 0.08f * dt;
            p.vel.z += cosf(time * 6.0f + p.pos.y) * 0.08f * dt;
            
            if (p.life <= 0) {
                p.life = p.maxLife;
                float angle = (float)rand() / RAND_MAX * BH_PI * 2.0f;
                float radius = 0.2f + (float)rand() / RAND_MAX * 0.4f;
                p.pos = blackHole->position;
                p.pos.x += cosf(angle) * radius;
                p.pos.z += sinf(angle) * radius;
                p.pos.y += topJet ? blackHole->eventHorizonRadius : -blackHole->eventHorizonRadius;
                float speed = 10.0f + (float)rand() / RAND_MAX * 5.0f;
                p.vel = {((float)rand() / RAND_MAX - 0.5f) * 0.15f, topJet ? speed : -speed, ((float)rand() / RAND_MAX - 0.5f) * 0.15f};
            }
        }
    }
    
    void draw() {
        for (const JetParticle& p : particles) {
            float t = p.life / p.maxLife;
            Color c = {
                (unsigned char)(120 + 135 * t),
                (unsigned char)(180 + 75 * t),
                255,
                (unsigned char)(t * 255)
            };
            DrawPoint3D(p.pos, c);
        }
    }
};

class EventHorizon {
public:
    BlackHole* blackHole;
    Model sphereModel;
    
    EventHorizon(BlackHole* bh) {
        blackHole = bh;
        sphereModel = LoadModelFromMesh(GenMeshSphere(blackHole->eventHorizonRadius, 32, 32));
    }
    
    ~EventHorizon() {
        UnloadModel(sphereModel);
    }
    
    void draw() {
        DrawModel(sphereModel, blackHole->position, 1.0f, BLACK);
    }
};

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Black Hole Simulation - Press ESC to exit");
    
    Camera3D camera = {0};
    camera.position = {0, 8, 25};
    camera.target = {0, 0, 0};
    camera.up = {0, 1, 0};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    DisableCursor();
    
    BlackHole blackHole;
    SpacetimeGrid spacetimeGrid(&blackHole);
    GravityFieldLines gravityField(&blackHole);
    EinsteinRing einsteinRing(&blackHole);
    AccretionDisk accretionDisk(&blackHole, 20000);
    DiskGlow diskGlow(&blackHole);
    PhotonSphere photonSphere(&blackHole);
    Starfield starfield(3000);
    InfallingMatter infallingMatter(&blackHole, 25);
    JetStream topJet(&blackHole, true, 400);
    JetStream bottomJet(&blackHole, false, 400);
    EventHorizon eventHorizon(&blackHole);
    
    float time = 0;
    bool autoRotate = true;
    float autoRotateSpeed = 0.08f;
    float cameraAngle = 0;
    float cameraHeight = 8.0f;
    float cameraDistance = 28.0f;
    
    bool showGrid = true;
    bool showFieldLines = true;
    
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        time += dt;
        
        if (IsKeyPressed(KEY_SPACE)) autoRotate = !autoRotate;
        if (IsKeyPressed(KEY_G)) showGrid = !showGrid;
        if (IsKeyPressed(KEY_F)) showFieldLines = !showFieldLines;
        if (IsKeyPressed(KEY_UP)) autoRotateSpeed += 0.02f;
        if (IsKeyPressed(KEY_DOWN)) autoRotateSpeed -= 0.02f;
        
        float wheel = GetMouseWheelMove();
        cameraDistance -= wheel * 2.0f;
        cameraDistance = Clamp(cameraDistance, 8.0f, 60.0f);
        
        if (autoRotate) {
            cameraAngle += autoRotateSpeed * dt;
        } else {
            UpdateCamera(&camera, CAMERA_FREE);
        }
        
        if (IsKeyDown(KEY_W)) cameraHeight += 5.0f * dt;
        if (IsKeyDown(KEY_S)) cameraHeight -= 5.0f * dt;
        if (IsKeyDown(KEY_A)) cameraAngle += 1.0f * dt;
        if (IsKeyDown(KEY_D)) cameraAngle -= 1.0f * dt;
        
        cameraHeight = Clamp(cameraHeight, -20.0f, 25.0f);
        
        if (autoRotate) {
            camera.position.x = cosf(cameraAngle) * cameraDistance;
            camera.position.z = sinf(cameraAngle) * cameraDistance;
            camera.position.y = cameraHeight;
            camera.target = blackHole.position;
        }
        
        blackHole.update(dt);
        accretionDisk.update(dt);
        infallingMatter.update(dt);
        topJet.update(dt, time);
        bottomJet.update(dt, time);
        
        BeginDrawing();
        ClearBackground({1, 1, 4, 255});
        
        BeginMode3D(camera);
        
        starfield.draw(time);
        if (showGrid) spacetimeGrid.draw(time);
        if (showFieldLines) gravityField.draw(time);
        diskGlow.draw(time);
        accretionDisk.draw(time);
        photonSphere.draw(time);
        infallingMatter.draw();
        topJet.draw();
        bottomJet.draw();
        einsteinRing.draw(time, camera);
        eventHorizon.draw();
        
        EndMode3D();
        
        DrawRectangle(10, 10, 300, 180, {0, 0, 0, 180});
        DrawText("BLACK HOLE", 20, 20, 28, WHITE);
        DrawText(TextFormat("FPS: %d", GetFPS()), 20, 55, 20, GREEN);
        DrawText(TextFormat("Particles: %d", (int)accretionDisk.particles.size()), 20, 80, 16, {200, 200, 200, 255});
        DrawText("---------------------------", 20, 100, 12, GRAY);
        DrawText("WASD - Camera | Scroll - Zoom", 20, 115, 14, GRAY);
        DrawText("SPACE - Auto Rotate", 20, 132, 14, GRAY);
        DrawText("G - Toggle Grid", 20, 149, 14, showGrid ? GREEN : GRAY);
        DrawText("F - Toggle Field Lines", 20, 166, 14, showFieldLines ? GREEN : GRAY);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}