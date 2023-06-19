#include "generator.hh"

MyPrimaryGenerator::MyPrimaryGenerator()
{
  // one primary vertex per event is created w the (1)
  fParticleGun = new G4GeneralParticleSource();
}

MyPrimaryGenerator::~MyPrimaryGenerator()
{
  delete fParticleGun;
}

void MyPrimaryGenerator::GeneratePrimaries(G4Event *anEvent)
{
  fParticleGun->GeneratePrimaryVertex(anEvent);
}
