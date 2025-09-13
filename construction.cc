#include "construction.hh"

MyDetectorConstruction::MyDetectorConstruction()
{

  fMessenger = new G4GenericMessenger(this, "/geom/", "Geometry Construction");

  // materials
  /// These are for older parts of the sim, not in use for this build
  fMessenger->DeclareProperty("filt_mat", filt_mat, "filter material");
  fMessenger->DeclareProperty("smat_found", smat_found, "is samp material found");
  fMessenger->DeclareProperty("samp_mat", samp_mat, "sample material");
  fMessenger->DeclareProperty("samp_density", samp_density, "sample density");
  fMessenger->DeclareProperty("samp_ncompnts", samp_ncompnts, "sample # of components");
  fMessenger->DeclareProperty("samp_state", samp_state, "sample state");

  // placements
  /// These are for older parts of the sim, not in use for this build
  fMessenger->DeclareProperty("filt_start", filt_start, "filter start");
  fMessenger->DeclareProperty("samp_start", samp_start, "sample start");

  // **JUST INITIAL DEFS!**
  /// These are for older parts of the sim, not in use for this build
  filt_mat = "G4_GRAPHITE";
  smat_found = 0;
  samp_mat = "G4_GRAPHITE";
  samp_density = 0.95*g/cm3;
  samp_ncompnts = 2;
  samp_state = kStateSolid;
  samp_state = 0;
  filt_start = -0.270;
  samp_start = -0.289;

  // starts & ends of geoms (units m)
  /// descends from vacuum
  vac_start = 0.0;
  vac_air1  = -0.035;
  air1_ldb   = -0.08965; //lead block 10 mm (ld = lead shielding outside machine, ldb = lead block on top of detector)
  ldb_wd   = -0.09965;  // wd is beryllium window
  wd_det    = -0.09975; // 
  det_air2  = -0.10075;
  air2_bp   = -0.291;
  bp_ld     = -0.314;
  ld_end    = -0.34;
  


  // depth midpoints of geoms
  vac_mdpt  = (vac_start + vac_air1) / 2;
  air1_mdpt = (vac_air1 + air1_ldb) / 2;
  ldb_mdpt  = (air1_ldb + ldb_wd) / 2;
  wd_mdpt   = (ldb_wd + wd_det) / 2;
  det_mdpt  = (wd_det + det_air2) / 2;
  air2_mdpt = (det_air2 + air2_bp) / 2;
  bp_mdpt   = (air2_bp + bp_ld) / 2;
  ld_mdpt   = (bp_ld + ld_end) / 2;
  


  // half depths 
  vac_hlfdep  = (vac_start - vac_air1) / 2;
  air1_hlfdep = (vac_air1 - air1_ldb) / 2;
  ldb_hlfdep  = (air1_ldb - ldb_wd) / 2;
  wd_hlfdep   = (ldb_wd - wd_det) / 2;
  det_hlfdep  = (wd_det - det_air2) / 2;
  air2_hlfdep = (det_air2 - air2_bp) / 2;
  bp_hlfdep   = (air2_bp - bp_ld) / 2;
  ld_hlfdep   = (bp_ld - ld_end) / 2;
  

  xWorld = 0.10*m;
  yWorld = 0.10*m;
  zWorld = 0.35*m;

  dens96500  = 1.140;
  dens101500 = 1.200;
  dens106500 = 1.259;

  DefineMaterials();
}

MyDetectorConstruction::~MyDetectorConstruction()
{}

// seperate fxn used to define our materials
void MyDetectorConstruction::DefineMaterials()
{
  G4NistManager *nist = G4NistManager::Instance();

  // elements
  N = nist->FindOrBuildElement("N");
  O = nist->FindOrBuildElement("O");
  C = nist->FindOrBuildElement("C");
  Ar = nist->FindOrBuildElement("Ar");
  H = nist->FindOrBuildElement("H");
  Al = nist->FindOrBuildElement("Al");
  Cu = nist->FindOrBuildElement("Cu");

  // materials
  worldMat = nist->FindOrBuildMaterial("G4_AIR");
  graphite = nist->FindOrBuildMaterial("G4_GRAPHITE");
  vacuum = nist->FindOrBuildMaterial("G4_Galactic");
  aluminum = nist->FindOrBuildMaterial("G4_Al");
  copper = nist->FindOrBuildMaterial("G4_Cu");
  bery = nist->FindOrBuildMaterial("G4_Be");
  sil = nist->FindOrBuildMaterial("G4_Si");
  lead = nist->FindOrBuildMaterial("G4_Pb");
  cadmium_telluride = nist->FindOrBuildMaterial("G4_CADMIUM_TELLURIDE");

  

  worldMatVary = new G4Material("worldMatVary", dens101500*kg/m3, worldMat, kStateUndefined, NTP_Temperature, 101500*pascal);

  hdpe = new G4Material("hdpe", 0.95*g/cm3, 2, kStateSolid);
  hdpe->AddElement(C, 2);
  hdpe->AddElement(H, 4);

  ceramic = new G4Material("ceramic", 3.95*g/cm3, 2, kStateSolid);
  ceramic->AddElement(Al, 2);
  ceramic->AddElement(O, 3);

  filtMat = nist->FindOrBuildMaterial(filt_mat);

  if (smat_found == 0) // found material on G4 NIST database
  {
    sampMat = nist->FindOrBuildMaterial(samp_mat); // CB MIGHT NEED TO CONCATENATE
  }
  else if (smat_found == 1) // manually enter mat specifics
  {
    sampMat = new G4Material("sampMat", samp_density , samp_ncompnts, samp_state);
  }

  // converting red and blue wavelength
  G4double energy[2] = {1.239841939*eV/0.2,1.239841939*eV/0.9};
  G4double rindexWorld[2] = {1.0,1.0};

  G4MaterialPropertiesTable *mptWorld = new G4MaterialPropertiesTable();
  mptWorld->AddProperty("RINDEX", energy, rindexWorld, 2);

  worldMat->SetMaterialPropertiesTable(mptWorld);
}
 
G4VPhysicalVolume *MyDetectorConstruction::Construct()
{
  // writing out geom which will input to mt_combiner (make sure to update this when changing geometries otherwise energies at the end of the simulation will be wrong)

  static std::ofstream fileout("geometry.tsv");
  fileout << "vac_start " << vac_start << "\n" <<  "vac_air1 " << vac_air1 << "\n" <<  "air1_ldb " <<  air1_ldb << "\n" <<  "ldb_wd " <<  ldb_wd << "\n" <<  "wd_det " <<  wd_det << "\n" <<  "det_air2 " << det_air2 << "\n" << "air2_bp " <<  air2_bp << "\n" <<  "bp_ld " << bp_ld << "\n" <<  "ld_end " << ld_end;
  fileout.close();

  // defining solid volume, G4Box params half size in x,y,z [default in mm but we change to m]
  solidWorld = new G4Box("solidWorld", xWorld, yWorld, zWorld);

  // inserting above material into created volume, G4LogicalVolume(solid volume, material, logical volume)
  logicWorld = new G4LogicalVolume(solidWorld, worldMatVary, "logicWorld");

  // physical volume, G4PVPlacement(rotation, coordinates where we place it, logic volume, name, can tell geant4 to place volume inside another volume but we tell this it has no further mother volume, implement boolean operations?, copy number we don't want, should we check for overlaps? -- always set to true)
  physWorld = new G4PVPlacement(0, G4ThreeVector(0,0,0), logicWorld, "physWorld", 0, false, 0, true);

  // vacuum space from focal pt to be window (Spekpky input already accounting for Be filter)
  solidVac = new G4Box("solidVac", xWorld, yWorld, vac_hlfdep*m);

  logicVac = new G4LogicalVolume(solidVac, vacuum, "logicVac");

  physVac = new G4PVPlacement(0, G4ThreeVector(0.0*m,0.0*m, vac_mdpt*m), logicVac, "physVac", logicWorld, false, 0, true);


  //Lead Block (10x10x1cm)
  solidLBlock = new G4Box("solidLBlock", 0.05*m, 0.05*m, ldb_hlfdep*m);

  logicLBlock = new G4LogicalVolume(solidLBlock, lead, "logicLBlock");

  physLBlock = new G4PVPlacement(0, G4ThreeVector(0.0*m,0.0*m, ldb_mdpt*m), logicLBlock, "physLBlock", logicWorld, false, 0, true);


  //Window (5x5mm x100microm)
  solidWindow = new G4Box("solidWindow", 0.0025*m, 0.0025*m, wd_hlfdep*m);

  logicWindow = new G4LogicalVolume(solidWindow, bery, "logicWindow");

  physWindow = new G4PVPlacement(0, G4ThreeVector(0.0*m,0.0*m, wd_mdpt*m), logicWindow, "physWindow", logicWorld, false, 0, true);

  // CdTe detector (5x5x1mm)
  solidCdTe = new G4Box("solidCdTe", 0.0025*m, 0.0025*m, det_hlfdep*m);

  logicCdTe = new G4LogicalVolume(solidCdTe, cadmium_telluride, "logicCdTe");

  physCdTe = new G4PVPlacement(0, G4ThreeVector(0.0*m,0.0*m, det_mdpt*m), logicCdTe, "physCdTe", logicWorld, false, 0, true);

  
  // Plate 2 cm thick so .01 m half depth, 10 x 10 cm wide
  solidPlate = new G4Box("solidPlate", 0.05*m, 0.05*m, bp_hlfdep*m);

  logicPlate = new G4LogicalVolume(solidPlate, aluminum, "logicPlate");

  physPlate = new G4PVPlacement(0, G4ThreeVector(0.0*m,0.0*m, bp_mdpt*m), logicPlate, "physPlate", logicWorld, false, 0, true);


  // Lead 3 cm thick so 0.015 m half depth, 10 x 10 cm wide
  solidLead = new G4Box("solidLead", xWorld, yWorld, ld_hlfdep*m);

  logicLead = new G4LogicalVolume(solidLead, lead, "logicLead");

  physLead = new G4PVPlacement(0, G4ThreeVector(0.0*m,0.0*m, ld_mdpt*m), logicLead, "physLead", logicWorld, false, 0, true);

  // enforcing 10 um step size
  G4double maxStep = 10*micrometer;
  G4UserLimits* stepLimit = new G4UserLimits(maxStep);

  logicWorld->SetUserLimits(stepLimit);

  return physWorld;
}

void MyDetectorConstruction::ConstructSDandField()
{
  MySensitiveDetector *sensWorld = new MySensitiveDetector("SensitiveWorld");
  logicWorld->SetSensitiveDetector(sensWorld);

  MySensitiveDetector *sensVac = new MySensitiveDetector("SensitiveVac");
  logicVac->SetSensitiveDetector(sensVac);

  MySensitiveDetector *sensLBlock = new MySensitiveDetector("SensitiveLBlock");
  logicLBlock->SetSensitiveDetector(sensLBlock);

  MySensitiveDetector *sensWindow= new MySensitiveDetector("SensitiveWindow");
  logicWindow->SetSensitiveDetector(sensWindow);

  MySensitiveDetector *sensCdTe = new MySensitiveDetector("SensitiveCdTe");
  logicCdTe->SetSensitiveDetector(sensCdTe);

  // MySensitiveDetector *sensCeramic = new MySensitiveDetector("SensitiveCeramic");
  // logicCeramic->SetSensitiveDetector(sensCeramic);

  // MySensitiveDetector *sensFilter = new MySensitiveDetector("SensitiveFilter");
  // logicFilter->SetSensitiveDetector(sensFilter);

  // MySensitiveDetector *sensSample = new MySensitiveDetector("SensitiveSample");
  // logicSample->SetSensitiveDetector(sensSample);

  MySensitiveDetector *sensPlate = new MySensitiveDetector("SensitivePlate");
  logicPlate->SetSensitiveDetector(sensPlate);

  MySensitiveDetector *sensLead = new MySensitiveDetector("SensitiveLead");
  logicLead->SetSensitiveDetector(sensLead);
}
