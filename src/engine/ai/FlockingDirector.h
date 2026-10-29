#pragma once

#include <vector>
#include <raylib.h>
#include "core/Types.h"
#include "core/math/Vector.h"

namespace Voidfall {

struct BoidAgent {
    int id = 0;
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;
    float maxSpeed = 320.0f;
    float maxForce = 450.0f;
    float personalRadius = 35.0f;
    float neighborRadius = 160.0f;
    int squadId = 0;
    bool isLeader = false;
    float health = 100.0f;
    float targetOffsetAngle = 0.0f;
    float targetOrbitRadius = 180.0f;
};

enum class SquadFormation {
    WEDGE,          // V-shape attack formation
    PINCER,         // Split wings flanking target
    DEFENSIVE_RING, // Orbiting shield formation
    ECHELON_RIGHT,  // Staggered firing line
    SCATTER         // Evasive chaos maneuvers
};

struct Squad {
    int id = 0;
    SquadFormation formation = SquadFormation::WEDGE;
    int leaderAgentId = -1;
    Vec2 squadCenter;
    Vec2 targetPos;
    float morale = 1.0f; // 1.0 = fearless, 0.0 = broken/scattered
    float rearmTimer = 0.0f;
    bool coordinatedStrikeReady = false;
};

class FlockingDirector {
public:
    FlockingDirector();
    ~FlockingDirector() = default;

    void Reset();
    int AddAgent(const Vec2& pos, int squadId, bool isLeader = false);
    void RemoveAgent(int id);
    BoidAgent* GetAgent(int id);

    void CreateSquad(int squadId, SquadFormation formation);
    Squad* GetSquad(int squadId);

    void Update(float dt, const Vec2& playerPos, const std::vector<Vec2>& obstacles);

    // Boids steering behaviors
    Vec2 ComputeSeparation(const BoidAgent& agent) const;
    Vec2 ComputeAlignment(const BoidAgent& agent) const;
    Vec2 ComputeCohesion(const BoidAgent& agent) const;
    Vec2 ComputeSeek(const BoidAgent& agent, const Vec2& target) const;
    Vec2 ComputeObstacleAvoidance(const BoidAgent& agent, const std::vector<Vec2>& obstacles) const;
    Vec2 ComputeFormationSlot(const BoidAgent& agent, const Squad& squad, const Vec2& targetPos) const;

    const std::vector<BoidAgent>& GetAgents() const { return m_agents; }
    const std::vector<Squad>& GetSquads() const { return m_squads; }

    void SetWeights(float sep, float ali, float coh, float form, float avoid) {
        m_weightSeparation = sep;
        m_weightAlignment = ali;
        m_weightCohesion = coh;
        m_weightFormation = form;
        m_weightAvoidance = avoid;
    }

private:
    std::vector<BoidAgent> m_agents;
    std::vector<Squad> m_squads;
    int m_nextAgentId = 1;

    float m_weightSeparation = 2.2f;
    float m_weightAlignment = 1.0f;
    float m_weightCohesion = 1.1f;
    float m_weightFormation = 2.5f;
    float m_weightAvoidance = 3.5f;
};

} // namespace Voidfall
