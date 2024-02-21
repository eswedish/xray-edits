#ifndef CONSTRUCTION_HH
#define CONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4GenericMessenger.hh"
#include "G4UserLimits.hh"
#include "G4StepLimiterPhysics.hh"
#include "G4StepLimiter.hh"
#include "G4Step.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Gamma.hh"
#include "G4ProcessManager.hh"

#include "detector.hh"

class MyDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  MyDetectorConstruction();
  ~MyDetectorConstruction();

  G4LogicalVolume *GetScoringVolume() const { return fScoringVolume; }

  virtual G4VPhysicalVolume *Construct();

private:

  virtual void ConstructSDandField();

  G4double xWorld, yWorld, zWorld;
  G4double vac_start, vac_air1, air1_cu, cu_di, air2_fl, fl_air3, air3_samp, samp_bp, bp_ld, ld_end, cr_air2, di_cr; //air1_ic ic_air2 ic_cr  di_air2
  G4double vac_mdpt, air1_mdpt, cu_mdpt, di_mdpt, air2_mdpt, fl_mdpt, air3_mdpt, samp_mdpt, bp_mdpt, ld_mdpt, cr_mdpt; //ic_mdpt
  G4double vac_hlfdep, air1_hlfdep, cu_hlfdep, di_hlfdep, air2_hlfdep, fl_hlfdep, air3_hlfdep, samp_hlfdep, bp_hlfdep, ld_hlfdep, cr_hlfdep; //ic_hlfdep
  G4double filt_start, samp_start, samp_density;
  G4double dens96500, dens101500, dens106500;
  G4int smat_found, samp_ncompnts, samp_state;
  G4String samp_mat, filt_mat;
  G4State sampState;

  G4Box *solidWorld, *solidVac, *solidSample, *solidFilter, *solidCopper,  *solidCeramic, *solidDiode, *solidPlate, *solidLead; //*solidChamber
  G4LogicalVolume *logicWorld, *logicVac, *logicSample, *logicFilter, *logicCopper, *logicCeramic, *logicDiode, *logicPlate, *logicLead; //*logicChamber
  G4VPhysicalVolume *physWorld, *physVac, *physSample, *physFilter, *physCopper, *physCeramic, *physDiode, *physPlate, *physLead; //*physChamber

  G4GenericMessenger *fMessenger;

  G4LogicalVolume *fScoringVolume;

  G4Material *worldMat, *worldMatVary, *graphite, *vacuum, *aluminum, *bery, *sil, *lead, *hdpe, *filtMat, *ceramic, *sampMat, *copper;
  G4Element *N, *O, *C, *Ar, *H, *Al, *Cu;

  void DefineMaterials();

};

#endif
