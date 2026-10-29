#include "FlockingDirector.h"
#include <cmath>
#include <algorithm>

namespace Voidfall {

FlockingDirector::FlockingDirector() = default;

void FlockingDirector::Reset() {
    m_agents.clear();
    m_squads.clear();
    m_nextAgentId = 1;
}

int FlockingDirector::AddAgent(const Vec2& pos, int squadId, bool isLeader) {
    BoidAgent agent;
    agent.id = m_nextAgentId++;
    agent.position = pos;
    agent.velocity = Vec2(0, 0);
    agent.acceleration = Vec2(0, 0);
    agent.squadId = squadId;
    agent.isLeader = isLeader;

    // Distribute orbit angle
    float angleIndex = static_cast<float>(m_agents.size() % 8);
    agent.targetOffsetAngle = angleIndex * (Math::PI * 0.25f);

    m_agents.push_back(agent);

    // Update squad leader if necessary
    auto* sq = GetSquad(squadId);
    if (sq && (isLeader || sq->leaderAgentId == -1)) {
        sq->leaderAgentId = agent.id;
    }

    return agent.id;
}

void FlockingDirector::RemoveAgent(int id) {
    auto it = std::remove_if(m_agents.begin(), m_agents.end(), [id](const BoidAgent& a) {
        return a.id == id;
    });
    m_agents.erase(it, m_agents.end());

    // Update squad leader if destroyed
    for (auto& sq : m_squads) {
        if (sq.leaderAgentId == id) {
            sq.leaderAgentId = -1;
            sq.morale *= 0.6f; // Morale penalty when commander falls
            for (const auto& a : m_agents) {
                if (a.squadId == sq.id) {
                    sq.leaderAgentId = a.id;
                    break;
                }
            }
            if (sq.leaderAgentId == -1) {
                sq.formation = SquadFormation::SCATTER;
            }
        }
    }
}

BoidAgent* FlockingDirector::GetAgent(int id) {
    for (auto& a : m_agents) {
        if (a.id == id) return &a;
    }
    return nullptr;
}

void FlockingDirector::CreateSquad(int squadId, SquadFormation formation) {
    for (auto& sq : m_squads) {
        if (sq.id == squadId) {
            sq.formation = formation;
            return;
        }
    }
    Squad sq;
    sq.id = squadId;
    sq.formation = formation;
    m_squads.push_back(sq);
}

Squad* FlockingDirector::GetSquad(int squadId) {
    for (auto& sq : m_squads) {
        if (sq.id == squadId) return &sq;
    }
    return nullptr;
}

Vec2 FlockingDirector::ComputeSeparation(const BoidAgent& agent) const {
    Vec2 steer(0, 0);
    int count = 0;

    for (const auto& other : m_agents) {
        if (other.id == agent.id) continue;
        float d = (agent.position - other.position).Length();
        if (d > 0.001f && d < agent.personalRadius) {
            Vec2 diff = (agent.position - other.position).Normalized() / d;
            steer = steer + diff;
            count++;
        }
    }

    if (count > 0) {
        steer = steer / static_cast<float>(count);
        if (steer.LengthSq() > 0.0001f) {
            steer = steer.Normalized() * agent.maxSpeed - agent.velocity;
            if (steer.Length() > agent.maxForce) {
                steer = steer.Normalized() * agent.maxForce;
            }
        }
    }
    return steer;
}

Vec2 FlockingDirector::ComputeAlignment(const BoidAgent& agent) const {
    Vec2 sumVel(0, 0);
    int count = 0;

    for (const auto& other : m_agents) {
        if (other.id == agent.id || other.squadId != agent.squadId) continue;
        float d = (agent.position - other.position).Length();
        if (d > 0.001f && d < agent.neighborRadius) {
            sumVel = sumVel + other.velocity;
            count++;
        }
    }

    if (count > 0) {
        sumVel = sumVel / static_cast<float>(count);
        if (sumVel.LengthSq() > 0.0001f) {
            Vec2 steer = sumVel.Normalized() * agent.maxSpeed - agent.velocity;
            if (steer.Length() > agent.maxForce) {
                steer = steer.Normalized() * agent.maxForce;
            }
            return steer;
        }
    }
    return Vec2(0, 0);
}

Vec2 FlockingDirector::ComputeCohesion(const BoidAgent& agent) const {
    Vec2 sumPos(0, 0);
    int count = 0;

    for (const auto& other : m_agents) {
        if (other.id == agent.id || other.squadId != agent.squadId) continue;
        float d = (agent.position - other.position).Length();
        if (d > 0.001f && d < agent.neighborRadius) {
            sumPos = sumPos + other.position;
            count++;
        }
    }

    if (count > 0) {
        sumPos = sumPos / static_cast<float>(count);
        return ComputeSeek(agent, sumPos);
    }
    return Vec2(0, 0);
}

Vec2 FlockingDirector::ComputeSeek(const BoidAgent& agent, const Vec2& target) const {
    Vec2 desired = target - agent.position;
    float dist = desired.Length();
    if (dist < 0.001f) return Vec2(0, 0);

    // Arrival deceleration within 80 units
    float speed = agent.maxSpeed;
    if (dist < 80.0f) {
        speed = agent.maxSpeed * (dist / 80.0f);
    }

    desired = desired.Normalized() * speed;
    Vec2 steer = desired - agent.velocity;
    if (steer.Length() > agent.maxForce) {
        steer = steer.Normalized() * agent.maxForce;
    }
    return steer;
}

Vec2 FlockingDirector::ComputeObstacleAvoidance(const BoidAgent& agent, const std::vector<Vec2>& obstacles) const {
    Vec2 steer(0, 0);
    const float avoidRadius = 60.0f;

    for (const auto& obs : obstacles) {
        float d = (agent.position - obs).Length();
        if (d < avoidRadius && d > 0.001f) {
            Vec2 away = (agent.position - obs).Normalized();
            float strength = (avoidRadius - d) / avoidRadius;
            steer = steer + away * (strength * agent.maxForce);
        }
    }
    return steer;
}

Vec2 FlockingDirector::ComputeFormationSlot(const BoidAgent& agent, const Squad& squad, const Vec2& targetPos) const {
    // If agent is leader, head towards target
    if (agent.isLeader) {
        // Orbit or hover near player
        float angle = agent.targetOffsetAngle;
        Vec2 offset(std::cos(angle) * agent.targetOrbitRadius, std::sin(angle) * agent.targetOrbitRadius);
        return targetPos + offset;
    }

    // Follow leader or squad center
    Vec2 anchor = squad.squadCenter;
    Vec2 fwd = (targetPos - anchor);
    if (fwd.LengthSq() < 0.01f) fwd = Vec2(0, -1);
    else fwd = fwd.Normalized();

    Vec2 right(-fwd.y, fwd.x);

    // Slot position based on agent ID modulo
    int slotIndex = (agent.id % 6);
    float side = (slotIndex % 2 == 0) ? 1.0f : -1.0f;
    float rank = static_cast<float>((slotIndex / 2) + 1);

    switch (squad.formation) {
        case SquadFormation::WEDGE:
            // V shape behind leader
            return anchor - fwd * (rank * 45.0f) + right * (side * rank * 40.0f);

        case SquadFormation::PINCER:
            // Wide wing flanking
            return targetPos + right * (side * (180.0f + rank * 35.0f)) - fwd * (rank * 25.0f);

        case SquadFormation::DEFENSIVE_RING: {
            float ringAngle = static_cast<float>(agent.id) * 1.047f; // 60 deg increments
            return anchor + Vec2(std::cos(ringAngle) * 90.0f, std::sin(ringAngle) * 90.0f);
        }

        case SquadFormation::ECHELON_RIGHT:
            // Staggered to one flank
            return anchor - fwd * (static_cast<float>(slotIndex) * 35.0f) + right * (static_cast<float>(slotIndex) * 45.0f);

        case SquadFormation::SCATTER:
        default:
            return agent.position + Vec2(side * 80.0f, -fwd.y * 50.0f);
    }
}

void FlockingDirector::Update(float dt, const Vec2& playerPos, const std::vector<Vec2>& obstacles) {
    // 1. Calculate squad centers
    for (auto& sq : m_squads) {
        sq.targetPos = playerPos;
        Vec2 center(0, 0);
        int count = 0;
        for (const auto& a : m_agents) {
            if (a.squadId == sq.id) {
                center = center + a.position;
                count++;
            }
        }
        if (count > 0) {
            sq.squadCenter = center / static_cast<float>(count);
        } else {
            sq.squadCenter = playerPos;
        }

        // Morale regeneration if squad is intact
        if (count >= 4) {
            sq.morale = std::min(1.0f, sq.morale + dt * 0.05f);
        }

        sq.rearmTimer += dt;
        if (sq.rearmTimer > 4.5f && sq.morale > 0.5f) {
            sq.coordinatedStrikeReady = true;
            sq.rearmTimer = 0.0f;
        }
    }

    // 2. Compute forces for each agent
    for (auto& a : m_agents) {
        auto* sq = GetSquad(a.squadId);

        Vec2 sep = ComputeSeparation(a) * m_weightSeparation;
        Vec2 ali = ComputeAlignment(a) * m_weightAlignment;
        Vec2 coh = ComputeCohesion(a) * m_weightCohesion;
        Vec2 avoid = ComputeObstacleAvoidance(a, obstacles) * m_weightAvoidance;

        Vec2 formForce(0, 0);
        if (sq) {
            Vec2 slotTarget = ComputeFormationSlot(a, *sq, playerPos);
            formForce = ComputeSeek(a, slotTarget) * m_weightFormation;
        } else {
            formForce = ComputeSeek(a, playerPos) * m_weightFormation;
        }

        a.acceleration = sep + ali + coh + formForce + avoid;

        // Apply physics
        a.velocity = a.velocity + a.acceleration * dt;
        if (a.velocity.Length() > a.maxSpeed) {
            a.velocity = a.velocity.Normalized() * a.maxSpeed;
        }
        a.position = a.position + a.velocity * dt;

        // Rotate offset angle slowly for orbiting
        a.targetOffsetAngle += dt * 0.35f;
    }
}

} // namespace Voidfall
