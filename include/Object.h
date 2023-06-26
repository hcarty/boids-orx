#pragma once

#include <optional>
#include <set>
#include "boids-orx.h"

namespace boid
{
    using neighbor = orxOBJECT *;

    class Flock
    {
    public:
        std::set<neighbor> neighbors{};

        void Add(orxVECTOR &reference, neighbor add);
        void Remove(neighbor remove);

        orxVECTOR Center();
        orxVECTOR Velocity();
        orxVECTOR ToAvoidanceTarget(orxVECTOR &position);
        orxVECTOR ToFlockCenter(orxVECTOR &position);
        orxVECTOR ToFlockVelocity(orxVECTOR &position);

        const size_t max_neighbors = 16;
        const orxFLOAT max_avoidance_distance = 100.0f;

    private:
        std::optional<neighbor> FindFarther(orxVECTOR &reference, neighbor check);
    };
}

/** Object Class
 */
class Object : public ScrollObject
{
public:
protected:
    void OnCreate();
    void OnDelete();
    void Update(const orxCLOCK_INFO &_rstInfo);

    void OnCollide(ScrollObject *_poCollider, orxBODY_PART *_pstPart, orxBODY_PART *_pstColliderPart, const orxVECTOR &_rvPosition, const orxVECTOR &_rvNormal);
    void OnSeparate(ScrollObject *_poCollider, orxBODY_PART *_pstPart, orxBODY_PART *_pstColliderPart);

private:
    boid::Flock flock{};

    orxVECTOR KeepInBounds();
    void BoidUpdate(const orxFLOAT dt);
};
