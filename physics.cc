#include "physics.hh"

MyPhysicsList::MyPhysicsList()
{
  //RegisterPhysics (new G4EmStandardPhysics());
  RegisterPhysics (new G4EmPenelopePhysics());
  RegisterPhysics (new G4OpticalPhysics());
  RegisterPhysics (new G4StepLimiterPhysics());

}

MyPhysicsList::~MyPhysicsList()
{}
