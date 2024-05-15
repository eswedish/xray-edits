#include "construction.hh"

MyDetectorConstruction::MyDetectorConstruction()
{

  fMessenger = new G4GenericMessenger(this, "/geom/", "Geometry Construction");

  // materials
  fMessenger->DeclareProperty("filt_mat", filt_mat, "filter material");
  fMessenger->DeclareProperty("smat_found", smat_found, "is samp material found");
  fMessenger->DeclareProperty("samp_mat", samp_mat, "sample material");
  fMessenger->DeclareProperty("samp_density", samp_density, "sample density");
  fMessenger->DeclareProperty("samp_ncompnts", samp_ncompnts, "sample # of components");
  fMessenger->DeclareProperty("samp_state", samp_state, "sample state");

  // placements
  fMessenger->DeclareProperty("filt_start", filt_start, "filter start");
  fMessenger->DeclareProperty("samp_start", samp_start, "sample start");

  // **JUST INITIAL DEFS!**
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
  vac_start = 0.0;
  vac_air1  = -0.035;
  air1_cu  = -0.09875; 
  cu_di    = -0.09975;// act sensitive region (.5 mm)
  di_cr     = -0.10025;
  cr_air2   = -0.10225;
  air2_fl   = filt_start;
  fl_air3   = -0.272;
  air3_samp = samp_start;
  samp_bp   = -0.291;
  bp_ld     = -0.314;
  ld_end    = -0.34;
  


  // depth midpoints of geoms
  vac_mdpt  = (vac_start + vac_air1) / 2;
  air1_mdpt = (vac_air1 + air1_cu) / 2;
  cu_mdpt  = (air1_cu + cu_di) / 2;
  di_mdpt   = (cu_di + di_cr) / 2;
  cr_mdpt   = (di_cr + cr_air2)/2;
  air2_mdpt = (cr_air2 + air2_fl) / 2;
  fl_mdpt   = (air2_fl + fl_air3) / 2;
  air3_mdpt = (fl_air3 + air3_samp) / 2;
  samp_mdpt = (air3_samp + samp_bp) / 2;
  bp_mdpt   = (samp_bp + bp_ld) / 2;
  ld_mdpt   = (bp_ld + ld_end) / 2;
  


  // half depths
  vac_hlfdep  = (vac_start - vac_air1) / 2;
  air1_hlfdep = (vac_air1 - air1_cu) / 2;
  cu_hlfdep  = (air1_cu - cu_di) / 2;
  di_hlfdep   = (cu_di - di_cr) / 2;
  cr_hlfdep   = (di_cr - cr_air2)/2;
  air2_hlfdep = (cr_air2 - air2_fl) / 2;
  fl_hlfdep   = (air2_fl - fl_air3) / 2;
  air3_hlfdep = (fl_air3 - air3_samp) / 2;
  samp_hlfdep = (air3_samp - samp_bp) / 2;
  bp_hlfdep   = (samp_bp - bp_ld) / 2;
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
  // writing out geom which will input to mt_combiner
  static std::ofstream fileout("geometry.tsv");
  fileout << "vac_start " << vac_start << "\n" <<  "vac_air1 " << vac_air1 << "\n" <<  "air1_cu " <<  air1_cu << "\n" <<  "cu_di " <<  cu_di << "\n" <<  "di_cr " << di_cr << "\n" <<  "cr_air2 " <<  cr_air2 << "\n" << "air2_fl " <<  air2_fl << "\n" << "fl_air3 " <<  fl_air3 << "\n" << "air3_samp " <<  air3_samp << "\n" << "samp_bp " <<  samp_bp << "\n" <<  "bp_ld " << bp_ld << "\n" <<  "ld_end " << ld_end;
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

  // // composed of the 13 mm IC, there's 20 mm air above 
  // solidChamber = new G4Box("solidChamber", 0.025*m, 0.025*m, ic_hlfdep*m);

  // logicChamber = new G4LogicalVolume(solidChamber, worldMatVary, "logicChamber");

  // physChamber = new G4PVPlacement(0, G4ThreeVector(0.0*m,0.0*m, ic_mdpt*m), logicChamber, "physChamber", logicWorld, false, 0, true);

  // copper filter 1 mm above diode
  solidCopper = new G4Box("solidCopper", 0.02*m, 0.02*m, cu_hlfdep*m);

  logicCopper = new G4LogicalVolume(solidCopper, copper, "logicCopper");

  physCopper = new G4PVPlacement(0, G4ThreeVector(0.0*m, 0.0*m, cu_mdpt*m), logicCopper, "physCopper", logicWorld, false, 0, true);

  // composed of the .5 mm diode (using IC variables for all sil diode bc too lazy to change all names)
  solidDiode = new G4Box("solidDiode", 0.02*m, 0.02*m, di_hlfdep*m);

  logicDiode = new G4LogicalVolume(solidDiode, sil, "logicDiode");

  physDiode = new G4PVPlacement(0, G4ThreeVector(0.0*m,0.0*m, di_mdpt*m), logicDiode, "physDiode", logicWorld, false, 0, true);

  //ceramic, assuming same size as diode on x,y
  solidCeramic = new G4Box("solidCeramic", 0.02*m, 0.02*m, cr_hlfdep*m);

  logicCeramic = new G4LogicalVolume(solidCeramic, ceramic, "logicCeramic");

  physCeramic = new G4PVPlacement(0, G4ThreeVector(0.0*m,0.0*m, cr_mdpt*m), logicCeramic, "physCeramic", logicWorld, false, 0, true);

  // Filter 2 mm thick so 1 mm half depth, 5x5 cm wide offset a bit higher than the IC
  solidFilter = new G4Box("solidFilter", 0.025*m, 0.025*m, fl_hlfdep*m);

  logicFilter = new G4LogicalVolume(solidFilter, filtMat, "logicFilter");

  physFilter = new G4PVPlacement(0, G4ThreeVector(0.0*m,0.0*m, fl_mdpt*m), logicFilter, "physFilter", logicWorld, false, 0, true);

  // first define what each dectector looks like G4Box(name,half-width,half-length,half-height)
  solidSample = new G4Box("solidSample", 0.025*m, 0.025*m, samp_hlfdep*m);

  logicSample = new G4LogicalVolume(solidSample, sampMat, "logicSample");

  physSample = new G4PVPlacement(0, G4ThreeVector(0.0*m,0.0*m, samp_mdpt*m), logicSample, "physSample", logicWorld, false, 0, true);

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

  MySensitiveDetector *sensCopper = new MySensitiveDetector("SensitiveCopper");
  logicCopper->SetSensitiveDetector(sensCopper);

  MySensitiveDetector *sensDiode = new MySensitiveDetector("SensitiveDiode");
  logicDiode->SetSensitiveDetector(sensDiode);

  MySensitiveDetector *sensCeramic = new MySensitiveDetector("SensitiveCeramic");
  logicCeramic->SetSensitiveDetector(sensCeramic);

  MySensitiveDetector *sensFilter = new MySensitiveDetector("SensitiveFilter");
  logicFilter->SetSensitiveDetector(sensFilter);

  MySensitiveDetector *sensSample = new MySensitiveDetector("SensitiveSample");
  logicSample->SetSensitiveDetector(sensSample);

  MySensitiveDetector *sensPlate = new MySensitiveDetector("SensitivePlate");
  logicPlate->SetSensitiveDetector(sensPlate);

  MySensitiveDetector *sensLead = new MySensitiveDetector("SensitiveLead");
  logicLead->SetSensitiveDetector(sensLead);
}
