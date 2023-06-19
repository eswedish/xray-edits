#include "detector.hh"
#include <iostream>
#include <iomanip>

MySensitiveDetector::MySensitiveDetector(G4String name) : G4VSensitiveDetector(name)
{
}

MySensitiveDetector::~MySensitiveDetector()
{}

G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist)
{
  G4Track *track = aStep->GetTrack();

  //info ab when particle begins step
  G4StepPoint *preStepPoint = aStep->GetPreStepPoint();
  //info ab particle once it is done w that step
  G4StepPoint *postStepPoint = aStep->GetPostStepPoint();

  G4ThreeVector posPhoton_pre = preStepPoint->GetPosition();
  G4ThreeVector momPhoton_pre = preStepPoint->GetMomentum();

  G4ThreeVector posPhoton_post = postStepPoint->GetPosition();
  G4ThreeVector momPhoton_post = postStepPoint->GetMomentum();


  // randomized placement of in between for the energy deposit not to spike severely
  // using xr = x1+ m(x2-x1) lin interpoltn form to do it :)
  G4ThreeVector posPhoton_btwn = posPhoton_pre + G4UniformRand()*(posPhoton_post - posPhoton_pre);

  G4double time = preStepPoint->GetGlobalTime();

  const G4VTouchable *touchable = aStep->GetPreStepPoint()->GetTouchable();

  // copy no corresponds to the i j arrays
  G4int copyNo = touchable->GetCopyNumber();

  G4VPhysicalVolume *physVol = touchable->GetVolume();

  const G4LogicalVolume* preVolume = preStepPoint->GetPhysicalVolume()->GetLogicalVolume();
  const G4LogicalVolume* postVolume = postStepPoint->GetPhysicalVolume()->GetLogicalVolume();

  // current particle for hit
  const G4ParticleDefinition* particle = track->GetDynamicParticle()->GetDefinition();

  G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

  // G4int press = G4GenericMessenger::GetCurrentValue(
  // G4int press = physVol->GetPressure();

  G4double edep = aStep->GetTotalEnergyDeposit();

  G4double parentID = track->GetParentID(); // whether gamma (0) or e- (1)

  G4double trackID = track->GetTrackID();

  G4AnalysisManager *man = G4AnalysisManager::Instance();

  if (edep!=0.0)
    {
      // just a current check for sanity while running long runs
      if (evt % 10000 == 1)
      {
        G4cout << evt << "\t" << posPhoton_btwn[0]  << "\t" << posPhoton_btwn[1] <<  "\t" << -posPhoton_btwn[2] << "\t" << edep << "\t" << parentID << "\t" << trackID << G4endl;
      }

      man->FillNtupleIColumn(0, 0, evt);
      man->FillNtupleDColumn(0, 1, posPhoton_btwn[0]);
      man->FillNtupleDColumn(0, 2, posPhoton_btwn[1]);
      man->FillNtupleDColumn(0, 3, -posPhoton_btwn[2]);
      man->FillNtupleDColumn(0, 4, time);
      man->FillNtupleDColumn(0, 5, edep);
      man->FillNtupleIColumn(0, 6, parentID);
      man->FillNtupleIColumn(0, 7, trackID);
      // man->FillNtupleIColumn(0, 8, press);
      //man->FillNtupleIColumn(0, 8, processID);
      man->AddNtupleRow(0);

    }

  return true;
}
