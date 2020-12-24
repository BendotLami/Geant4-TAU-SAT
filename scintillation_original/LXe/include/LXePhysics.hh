#ifndef LXePhysics_h
#define LXePhysics_h 1

#include "G4EmStandardPhysics_option4.hh"

class LXePhysics: public G4EmStandardPhysics_option4
{
    public:
    explicit LXePhysics(G4int ver = 1, const G4String &name = "");

    virtual ~LXePhysics();

    virtual void ConstructParticle();
    virtual void ConstructProcess();
};

#endif