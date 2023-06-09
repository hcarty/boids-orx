#include <set>

#include "Object.h"

orxVECTOR *orxVector_FromRotation(orxFLOAT rotation, orxVECTOR *dst)
{
    if (dst)
    {
        dst->fX = cosf(rotation);
        dst->fY = sinf(rotation);
        dst->fZ = 0.0f;
    }
    return dst;
}

orxFLOAT orxVector_ToRotation(orxVECTOR *v)
{
    return atan2(v->fY, v->fX);
}

std::optional<boid::neighbor> boid::Flock::FindFarther(orxVECTOR &reference, neighbor check)
{
    orxVECTOR checkPosition = orxVECTOR_0;
    orxObject_GetWorldPosition(check, &checkPosition);
    auto checkDistance = orxVector_GetDistance(&checkPosition, &reference);

    orxVECTOR neighborPosition = orxVECTOR_0;
    for (auto neighbor : neighbors)
    {
        orxObject_GetWorldPosition(neighbor, &neighborPosition);
        auto neighborDistance = orxVector_GetDistance(&neighborPosition, &reference);
        if (neighborDistance > checkDistance)
            return neighbor;
    }

    return std::nullopt;
}

void boid::Flock::Add(orxVECTOR &reference, neighbor add)
{
    if (neighbors.size() < max_neighbors)
    {
        neighbors.insert(add);
    }
    else
    {
        auto kicked = FindFarther(reference, add);
        if (kicked.has_value())
        {
            Remove(kicked.value());
            neighbors.insert(add);
        }
    }
}

void boid::Flock::Remove(neighbor remove)
{
    neighbors.erase(remove);
}

orxVECTOR boid::Flock::Center()
{
    orxVECTOR center = orxVECTOR_0;
    for (auto *o : neighbors)
    {
        orxVECTOR pos = orxVECTOR_0;
        orxObject_GetWorldPosition(o, &pos);
        orxVector_Add(&center, &center, &pos);
    }
    orxVector_Divf(&center, &center, static_cast<orxFLOAT>(neighbors.size()));
    return center;
}

orxVECTOR boid::Flock::Velocity()
{
    orxVECTOR velocity = orxVECTOR_0;
    for (auto o : neighbors)
    {
        orxVECTOR vel = orxVECTOR_0;
        orxObject_GetSpeed(o, &vel);
        orxVector_Add(&velocity, &velocity, &vel);
    }
    orxVector_Divf(&velocity, &velocity, static_cast<orxFLOAT>(neighbors.size()));
    return velocity;
}

orxVECTOR boid::Flock::ToAvoidanceTarget(orxVECTOR &position)
{
    orxVECTOR avoidanceTarget = orxVECTOR_0;
    for (auto neighbor : neighbors)
    {
        orxVECTOR neighborPosition = orxVECTOR_0;
        orxVECTOR positionDiff = orxVECTOR_0;
        orxObject_GetWorldPosition(neighbor, &neighborPosition);
        orxVector_Sub(&positionDiff, &neighborPosition, &position);
        auto distance = orxVector_GetSize(&positionDiff);
        if (distance < max_avoidance_distance)
        {
            orxVector_Sub(&avoidanceTarget, &avoidanceTarget, &positionDiff);
        }
    }
    return avoidanceTarget;
}

orxVECTOR boid::Flock::ToFlockCenter(orxVECTOR &position)
{
    auto center = Center();
    orxVECTOR diff = orxVECTOR_0;
    orxVector_Sub(&diff, &center, &position);
    return diff;
}

orxVECTOR boid::Flock::ToFlockVelocity(orxVECTOR &velocity)
{
    auto flockVelocity = Velocity();
    orxVECTOR diff = orxVECTOR_0;
    orxVector_Sub(&diff, &flockVelocity, &velocity);
    return diff;
}

orxVECTOR Object::KeepInBounds()
{
    // orxVECTOR arenaMin = {-500, -300};
    // orxVECTOR arenaMax = {500, 300};
    orxVECTOR arenaMin = {-350, -200};
    orxVECTOR arenaMax = {350, 200};
    auto delta = orxVECTOR_0;
    auto position = orxVECTOR_0;
    GetPosition(position, orxTRUE);

    if (position.fX < arenaMin.fX)
        delta.fX = 300.0;
    if (position.fX > arenaMax.fX)
        delta.fX = -300.0;
    if (position.fY < arenaMin.fY)
        delta.fY = 300.0;
    if (position.fY > arenaMax.fY)
        delta.fY = -300.0;

    return delta;
}

void Object::BoidUpdate(orxFLOAT dt)
{
    orxVECTOR speed = orxVECTOR_0;
    GetSpeed(speed);
    orxVECTOR position = orxVECTOR_0;
    GetPosition(position, orxTRUE);
    PushConfigSection();
    auto targetSpeed = orxConfig_GetFloat("TargetSpeed");
    PopConfigSection();

    auto inBoundsDelta = KeepInBounds();
    if (false)
    {
        if (orxVector_GetSize(&inBoundsDelta) > 0.0)
        {
            orxVECTOR rgb = {255, 0, 0};
            orxCOLOR color = {rgb, 1.0};
            SetColor(color);
        }
        else
        {
            orxVECTOR rgb = {255, 255, 255};
            orxCOLOR color = {rgb, 1.0};
            SetColor(color);
        }
    }

    orxVECTOR speedDelta = orxVECTOR_0;
    orxVector_Add(&speedDelta, &speedDelta, &inBoundsDelta);

    if (flock.neighbors.size() > 0)
    {
        auto avoidanceDelta = flock.ToAvoidanceTarget(position);
        orxVector_Mulf(&avoidanceDelta, &avoidanceDelta, 10);
        auto centerDelta = flock.ToFlockCenter(position);
        orxVector_Mulf(&centerDelta, &centerDelta, 1);
        auto velocityDelta = flock.ToFlockVelocity(speed);
        orxVector_Mulf(&velocityDelta, &velocityDelta, 1);

        orxVector_Add(&speedDelta, &speedDelta, &avoidanceDelta);
        orxVector_Add(&speedDelta, &speedDelta, &centerDelta);
        orxVector_Add(&speedDelta, &speedDelta, &velocityDelta);
    }

    if (flock.neighbors.size() == flock.max_neighbors)
    {
        orxVECTOR rgb = {255, 0, 0};
        orxCOLOR color = {rgb, 1.0};
        SetColor(color);
    }
    else
    {
        orxVECTOR rgb = {255, 255, 255};
        orxCOLOR color = {rgb, 1.0};
        SetColor(color);
    }

    orxVector_Mulf(&speedDelta, &speedDelta, dt);
    orxVector_Add(&speed, &speed, &speedDelta);

    orxVECTOR fullSpeed = orxVECTOR_0;
    orxVector_Copy(&fullSpeed, &speed);
    orxVector_Normalize(&fullSpeed, &fullSpeed);
    orxVector_Mulf(&fullSpeed, &fullSpeed, targetSpeed);
    orxVector_Lerp(&speed, &speed, &fullSpeed, 0.1);

    SetSpeed(speed);
    SetRotation(orxVector_ToRotation(&speed) + M_PI);
}

void Object::OnCreate()
{
    orxConfig_SetBool("IsObject", orxTRUE);
}

void Object::OnDelete()
{
}

void Object::Update(const orxCLOCK_INFO &_rstInfo)
{
    BoidUpdate(_rstInfo.fDT);
}

void Object::OnCollide(ScrollObject *_poCollider, orxBODY_PART *_pstPart, orxBODY_PART *_pstColliderPart, const orxVECTOR &_rvPosition, const orxVECTOR &_rvNormal)
{
    orxASSERT(_poCollider);
    auto partName = orxBody_GetPartName(_pstPart);
    auto colliderName = orxBody_GetPartName(_pstColliderPart);
    auto isSensor = orxString_Compare(partName, "BoidSensorPart") == 0;
    auto isBody = orxString_Compare(colliderName, "BoidBodyPart") == 0;
    orxVECTOR pos = orxVECTOR_0;
    GetPosition(pos, orxTRUE);
    if (isSensor && isBody)
        flock.Add(pos, _poCollider->GetOrxObject());
}

void Object::OnSeparate(ScrollObject *_poCollider, orxBODY_PART *_pstPart, orxBODY_PART *_pstColliderPart)
{
    orxASSERT(_poCollider);
    flock.Remove(_poCollider->GetOrxObject());
}
