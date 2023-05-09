#pragma once

#include <set>
#include "boids-orx.h"

namespace boid
{
    using neighbor = orxOBJECT *;

    class Flock
    {
    public:
        std::set<neighbor> neighbors{};

        void Add(neighbor add);
        void Remove(neighbor remove);

        orxVECTOR Center();
        orxVECTOR Velocity();
        orxVECTOR ToAvoidanceTarget(orxVECTOR &position);
        orxVECTOR ToFlockCenter(orxVECTOR &position);
        orxVECTOR ToFlockVelocity(orxVECTOR &position);

    private:
        const size_t max_neighbors = 16;
        const orxFLOAT max_avoidance_distance = 45.0f;
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

    orxBOOL OnCollide(ScrollObject *_poCollider, orxBODY_PART *_pstPart, orxBODY_PART *_pstColliderPart, const orxVECTOR &_rvPosition, const orxVECTOR &_rvNormal);
    orxBOOL OnSeparate(ScrollObject *_poCollider);

private:
    boid::Flock flock{};

    orxVECTOR KeepInBounds();
    void BoidUpdate(const orxFLOAT dt);
};
