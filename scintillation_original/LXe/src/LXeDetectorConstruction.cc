//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file optical/LXe/src/LXeDetectorConstruction.cc
/// \brief Implementation of the LXeDetectorConstruction class
//
//
#include "LXeDetectorConstruction.hh"
#include "LXePMTSD.hh"
#include "LXeScintSD.hh"
#include "LXeDetectorMessenger.hh"
#include "LXeMainVolume.hh"
// #include "LXeWLSSlab.hh"

#include "G4SDManager.hh"
#include "G4RunManager.hh"

#include "G4GeometryManager.hh"
#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"

#include "G4OpticalSurface.hh"
#include "G4MaterialTable.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"
#include "G4UImanager.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "SD.hh"
#include "G4ProductionCuts.hh"

G4bool LXeDetectorConstruction::fSphereOn = true;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LXeDetectorConstruction::LXeDetectorConstruction()
: fLXe_mt(nullptr), fMPTPStyrene(nullptr)
{
  fExperimentalHall_box = nullptr;
  fExperimentalHall_log = nullptr;
  fExperimentalHall_phys = nullptr;

  fLXe = fAl = fAir = fVacuum = fGlass = nullptr;
  fPstyrene = fPMMA = fPethylene1 = fPethylene2 = nullptr;

  fN = fO = fC = fH = nullptr;

  fSaveThreshold = 0;
  SetDefaults();

  DefineMaterials();
  fDetectorMessenger = new LXeDetectorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LXeDetectorConstruction::~LXeDetectorConstruction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeDetectorConstruction::DefineMaterials(){
  G4double a;  // atomic mass
  G4double z;  // atomic number
  G4double density;

  G4int polyPMMA = 1;
  G4int nC_PMMA = 3+2*polyPMMA;
  G4int nH_PMMA = 6+2*polyPMMA;

  G4int polyeth = 1;
  G4int nC_eth = 2*polyeth;
  G4int nH_eth = 4*polyeth;

  //***Elements
  fH = new G4Element("H", "H", z=1., a=1.01*g/mole);
  fC = new G4Element("C", "C", z=6., a=12.01*g/mole);
  fN = new G4Element("N", "N", z=7., a= 14.01*g/mole);
  fO = new G4Element("O"  , "O", z=8., a= 16.00*g/mole);

  //***Materials
  //Liquid Xenon
  fLXe = new G4Material("LXe",z=54.,a=131.29*g/mole,density=3.020*g/cm3);
  //Aluminum
  fAl = new G4Material("Al",z=13.,a=26.98*g/mole,density=2.7*g/cm3);
  //Vacuum
  fVacuum = new G4Material("Vacuum",z=1.,a=1.01*g/mole,
                          density=universe_mean_density,kStateGas,0.1*kelvin,
                          1.e-19*pascal);
  //Air
  fAir = new G4Material("Air", density= 1.29*mg/cm3, 2);
  fAir->AddElement(fN, 70*perCent);
  fAir->AddElement(fO, 30*perCent);
  //Glass
  fGlass = new G4Material("Glass", density=1.032*g/cm3,2);
  fGlass->AddElement(fC,91.533*perCent);
  fGlass->AddElement(fH,8.467*perCent);
  //Polystyrene
  fPstyrene = new G4Material("Polystyrene", density= 1.03*g/cm3, 2);
  fPstyrene->AddElement(fC, 8);
  fPstyrene->AddElement(fH, 8);
  //Fiber(PMMA)
  fPMMA = new G4Material("PMMA", density=1190*kg/m3,3);
  fPMMA->AddElement(fH,nH_PMMA);
  fPMMA->AddElement(fC,nC_PMMA);
  fPMMA->AddElement(fO,2);
  //Cladding(polyethylene)
  fPethylene1 = new G4Material("Pethylene1", density=1200*kg/m3,2);
  fPethylene1->AddElement(fH,nH_eth);
  fPethylene1->AddElement(fC,nC_eth);
  //Double cladding(flourinated polyethylene)
  fPethylene2 = new G4Material("Pethylene2", density=1400*kg/m3,2);
  fPethylene2->AddElement(fH,nH_eth);
  fPethylene2->AddElement(fC,nC_eth);

  Ej200 = new G4Material("Ej200", density = 1.023 * g / cm3, 2);
  Ej200->AddElement(fC, 9);
  Ej200->AddElement(fH, 10);
  //***Material properties tables
  const G4int NUMENTRIES = 12;
  G4double PhotonEnergy[NUMENTRIES] = {3.44 * eV, 3.26 * eV, 3.1 * eV, 3.02 * eV, 2.95 * eV,
                                       2.92 * eV, 2.82 * eV, 2.76 * eV, 2.7 * eV, 2.58 * eV,
                                       2.38 * eV, 2.08 * eV};
  G4double RINDEX_Ej200[NUMENTRIES] = {1.58, 1.58, 1.58, 1.58, 1.58,
                                       1.58, 1.58, 1.58, 1.58, 1.58,
                                       1.58, 1.58};
  G4double ABSORPTION_Ej200[NUMENTRIES] = {210 * cm, 210 * cm, 210 * cm, 210 * cm, 210 * cm,
                                           210 * cm, 210 * cm, 210 * cm, 210 * cm, 210 * cm,
                                           210 * cm, 210 * cm};
  G4double SCINTILLATION_Ej200[NUMENTRIES] = {0.04, 0.07, 0.20, 0.49, 0.84,
                                              1.00, 0.83, 0.55, 0.40, 0.17,
                                              0.03, 0};

  Ej200_mt = new G4MaterialPropertiesTable();
  Ej200_mt->AddProperty("RINDEX", PhotonEnergy, RINDEX_Ej200, NUMENTRIES);
  Ej200_mt->AddProperty("ABSLENGTH", PhotonEnergy, ABSORPTION_Ej200, NUMENTRIES);
  Ej200_mt->AddProperty("FASTCOMPONENT", PhotonEnergy, SCINTILLATION_Ej200, NUMENTRIES);
  Ej200_mt->AddConstProperty("SCINTILLATIONYIELD", 10000. / MeV);
  Ej200_mt->AddConstProperty("RESOLUTIONSCALE", 1.0);
  Ej200_mt->AddConstProperty("FASTTIMECONSTANT", 1. * ns);
  // Ej200_mt->AddConstProperty("SLOWTIMECONSTANT",1.*ns);
  Ej200_mt->AddConstProperty("YIELDRATIO", 1.);
  Ej200->SetMaterialPropertiesTable(Ej200_mt);

  //***Material properties tables

  G4double lxe_Energy[]    = { 7.0*eV , 7.07*eV, 7.14*eV };
  const G4int lxenum = sizeof(lxe_Energy)/sizeof(G4double);

  G4double lxe_SCINT[] = { 0.1, 1.0, 0.1 };
  assert(sizeof(lxe_SCINT) == sizeof(lxe_Energy));
  G4double lxe_RIND[]  = { 1.59 , 1.57, 1.54 };
  assert(sizeof(lxe_RIND) == sizeof(lxe_Energy));
  G4double lxe_ABSL[]  = { 35.*cm, 35.*cm, 35.*cm};
  assert(sizeof(lxe_ABSL) == sizeof(lxe_Energy));
  fLXe_mt = new G4MaterialPropertiesTable();
  fLXe_mt->AddProperty("FASTCOMPONENT", lxe_Energy, lxe_SCINT, lxenum);
  fLXe_mt->AddProperty("SLOWCOMPONENT", lxe_Energy, lxe_SCINT, lxenum);
  fLXe_mt->AddProperty("RINDEX",        lxe_Energy, lxe_RIND,  lxenum);
  fLXe_mt->AddProperty("ABSLENGTH",     lxe_Energy, lxe_ABSL,  lxenum);
  fLXe_mt->AddConstProperty("SCINTILLATIONYIELD",12000./MeV);
  fLXe_mt->AddConstProperty("RESOLUTIONSCALE",1.0);
  fLXe_mt->AddConstProperty("FASTTIMECONSTANT",20.*ns);
  fLXe_mt->AddConstProperty("SLOWTIMECONSTANT",45.*ns);
  fLXe_mt->AddConstProperty("YIELDRATIO",1.0);
  fLXe->SetMaterialPropertiesTable(fLXe_mt);

  // Set the Birks Constant for the LXe scintillator

  fLXe->GetIonisation()->SetBirksConstant(0.126*mm/MeV);
 
  G4double glass_RIND[]={1.49,1.49,1.49};
  assert(sizeof(glass_RIND) == sizeof(lxe_Energy));
  G4double glass_AbsLength[]={420.*cm,420.*cm,420.*cm};
  assert(sizeof(glass_AbsLength) == sizeof(lxe_Energy));
  G4MaterialPropertiesTable *glass_mt = new G4MaterialPropertiesTable();
  glass_mt->AddProperty("ABSLENGTH",lxe_Energy,glass_AbsLength,lxenum);
  glass_mt->AddProperty("RINDEX",lxe_Energy,glass_RIND,lxenum);
  fGlass->SetMaterialPropertiesTable(glass_mt);

  G4double vacuum_Energy[]={2.0*eV,7.0*eV,7.14*eV};
  const G4int vacnum = sizeof(vacuum_Energy)/sizeof(G4double);
  G4double vacuum_RIND[]={1.,1.,1.};
  assert(sizeof(vacuum_RIND) == sizeof(vacuum_Energy));
  G4MaterialPropertiesTable *vacuum_mt = new G4MaterialPropertiesTable();
  vacuum_mt->AddProperty("RINDEX", vacuum_Energy, vacuum_RIND,vacnum);
  fVacuum->SetMaterialPropertiesTable(vacuum_mt);
  fAir->SetMaterialPropertiesTable(vacuum_mt);//Give air the same rindex

  G4NistManager * man = G4NistManager::Instance();
  G4Material * Si = man->FindOrBuildMaterial("G4_Si");
  fSiMaterial = Si;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* LXeDetectorConstruction::Construct(){

  if (fExperimentalHall_phys) { return fExperimentalHall_phys; }

  //The experimental hall walls are all 1m away from housing walls
  G4double expHall_x = fScint_x+fD_mtl+1.*m;
  G4double expHall_y = fScint_y+fD_mtl+1.*m;
  G4double expHall_z = fScint_z+fD_mtl+1.5*m;

  //Create experimental hall
  fExperimentalHall_box
    = new G4Box("expHall_box",expHall_x,expHall_y,expHall_z);
  fExperimentalHall_log = new G4LogicalVolume(fExperimentalHall_box,
                                             fVacuum,"expHall_log",0,0,0);
  fExperimentalHall_phys = new G4PVPlacement(0,G4ThreeVector(),
                              fExperimentalHall_log,"expHall",0,false,0);

  fExperimentalHall_log->SetVisAttributes(G4VisAttributes::GetInvisible());

  //Place the main volumes - scintillators!
  if(fMainVolumeOn){
    fMainVolume
      = new LXeMainVolume(0,G4ThreeVector(),fExperimentalHall_log,false,0,this);
    fMainVolume2 
      = new LXeMainVolume(0, G4ThreeVector(0., 0., 0.64 * 2. * cm), fExperimentalHall_log, false, 0, this);
  }

  //Place the silicon!
  G4double TargetSizeZ = 0.3*mm;//0.2*um; 
  G4double TargetSizeX = 63.*mm;
  G4double TargetSizeY = 63.*mm;

  G4Box* targetSolid = new G4Box("Target",				     //its name
				 TargetSizeX/2,TargetSizeY/2,TargetSizeZ/2);   //its size


  G4LogicalVolume *logicTarget = new G4LogicalVolume(targetSolid, //its solid
                                                     fSiMaterial, //its material
                                                     "Target");   //its name

  new G4PVPlacement(0,                                      //no rotation
                    G4ThreeVector(0., 0., -0.64 * 2. * cm), //at (0,0,0)
                    logicTarget,                            //its logical volume
                    "Target",
                    fExperimentalHall_log, //its mother  volume
                    0,
                    false, //no boolean operation
                    0);    //copy number

  // Visualization attributes
  G4VisAttributes* worldVisAtt1 = new G4VisAttributes(G4Colour(1.0,0.0,0.0)); 
  worldVisAtt1->SetVisibility(true);
  logicTarget->SetVisAttributes(worldVisAtt1);

  // Create Target G4Region and add logical volume
  
  fRegion = new G4Region("Target");
  
  G4ProductionCuts* cuts = new G4ProductionCuts();
  
  G4double defCut = 1*nanometer;
  cuts->SetProductionCut(defCut,"gamma");
  cuts->SetProductionCut(defCut,"e-");
  cuts->SetProductionCut(defCut,"e+");
  cuts->SetProductionCut(defCut,"proton");
  
  fRegion->SetProductionCuts(cuts);
  fRegion->AddRootLogicalVolume(logicTarget); 


  return fExperimentalHall_phys;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeDetectorConstruction::ConstructSDandField() {

  if (!fMainVolume) return;

  // PMT SD

  if (!fPmt_SD.Get()) {
    //Created here so it exists as pmts are being placed
    G4cout << "Construction /LXeDet/pmtSD" << G4endl;
    LXePMTSD* pmt_SD = new LXePMTSD("/LXeDet/pmtSD");
    fPmt_SD.Put(pmt_SD);

    pmt_SD->InitPMTs((fNx*fNy+fNx*fNz+fNy*fNz)*2); //let pmtSD know # of pmts
    pmt_SD->SetPmtPositions(fMainVolume->GetPmtPositions());
  }
  G4SDManager::GetSDMpointer()->AddNewDetector(fPmt_SD.Get());
  //sensitive detector is not actually on the photocathode.
  //processHits gets done manually by the stepping action.
  //It is used to detect when photons hit and get absorbed&detected at the
  //boundary to the photocathode (which doesnt get done by attaching it to a
  //logical volume.
  //It does however need to be attached to something or else it doesnt get
  //reset at the begining of events

  SetSensitiveDetector(fMainVolume->GetLogPhotoCath(), fPmt_SD.Get());

  // Scint SD

  if (!fScint_SD.Get()) {
    G4cout << "Construction /LXeDet/scintSD" << G4endl;
    LXeScintSD* scint_SD = new LXeScintSD("/LXeDet/scintSD");
    fScint_SD.Put(scint_SD);
  }
  G4SDManager::GetSDMpointer()->AddNewDetector(fScint_SD.Get());
  SetSensitiveDetector(fMainVolume->GetLogScint(), fScint_SD.Get());

  G4String trackerChamberSDname = "B2/TrackerChamberSD";
  B2TrackerSD* aTrackerSD = new B2TrackerSD(trackerChamberSDname,
                                            "TrackerHitsCollection");
  G4SDManager::GetSDMpointer()->AddNewDetector(aTrackerSD);
  // Setting aTrackerSD to all logical volumes with the same name 
  // of "Chamber_LV".
  SetSensitiveDetector("Target", aTrackerSD, true);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeDetectorConstruction::SetDimensions(G4ThreeVector dims) {
  fScint_x=dims[0];
  fScint_y=dims[1];
  fScint_z=dims[2];
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}
 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeDetectorConstruction::SetHousingThickness(G4double d_mtl) {
  fD_mtl=d_mtl;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeDetectorConstruction::SetNX(G4int nx) {
  fNx=nx;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeDetectorConstruction::SetNY(G4int ny) {
  fNy=ny;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeDetectorConstruction::SetNZ(G4int nz) {
  fNz=nz;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeDetectorConstruction::SetPMTRadius(G4double outerRadius_pmt) {
  fOuterRadius_pmt=outerRadius_pmt;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeDetectorConstruction::SetDefaults() {

  //Resets to default values
  fD_mtl=0.023188*cm;

  fScint_x = 7.*cm;
  fScint_y = 7.*cm;
  fScint_z = 0.64*cm;

  fNx = 2;
  fNy = 1;
  fNz = 0;

  fOuterRadius_pmt = 2.3*cm;

  fSphereOn = false;
  fRefl = 1.0;

  fNfibers = 15;
  // fWLSslab = false;
  fMainVolumeOn = true;
  fMainVolume = nullptr;
  fSlab_z = 2.5*mm;

  G4UImanager::GetUIpointer()
    ->ApplyCommand("/LXe/detector/scintYieldFactor 1.");

  if(fLXe_mt)fLXe_mt->AddConstProperty("SCINTILLATIONYIELD",12000./MeV);
  if(fMPTPStyrene)fMPTPStyrene->AddConstProperty("SCINTILLATIONYIELD",10./keV);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeDetectorConstruction::SetSphereOn(G4bool b) {
  (void) b;
  fSphereOn=false;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeDetectorConstruction::SetHousingReflectivity(G4double r) {
  fRefl=r;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// void LXeDetectorConstruction::SetWLSSlabOn(G4bool b) {
//   fWLSslab=b;
//   G4RunManager::GetRunManager()->ReinitializeGeometry();
// }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeDetectorConstruction::SetMainVolumeOn(G4bool b) {
  fMainVolumeOn=b;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeDetectorConstruction::SetNFibers(G4int n) {
  fNfibers=n;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeDetectorConstruction::SetMainScintYield(G4double y) {
  fLXe_mt->AddConstProperty("SCINTILLATIONYIELD",y/MeV);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
// void LXeDetectorConstruction::SetWLSScintYield(G4double y) {
//   fMPTPStyrene->AddConstProperty("SCINTILLATIONYIELD",y/MeV);
// }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeDetectorConstruction::SetSaveThreshold(G4int save){
/*Sets the save threshold for the random number seed. If the number of photons
generated in an event is lower than this, then save the seed for this event
in a file called run###evt###.rndm
*/
  fSaveThreshold=save;
  G4RunManager::GetRunManager()->SetRandomNumberStore(true);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......