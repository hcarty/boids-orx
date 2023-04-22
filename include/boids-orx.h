/**
 * @file boids-orx.h
 * @date 22-Apr-2023
 */

#ifndef __boids_orx_H__
#define __boids_orx_H__

#define __NO_SCROLLED__
#include "Scroll.h"

/** Game Class
 */
class boids_orx : public Scroll<boids_orx>
{
public:


private:

                orxSTATUS       Bootstrap() const;

                void            Update(const orxCLOCK_INFO &_rstInfo);

                orxSTATUS       Init();
                orxSTATUS       Run();
                void            Exit();
                void            BindObjects();


private:
};

#endif // __boids_orx_H__
