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
// -------------------------------------------------------------------
// -------------------------------------------------------------------

#include "DetectorConstruction.hh"
#include "SD.hh"
#include "G4SystemOfUnits.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"
#include "G4SDManager.hh"
#include "G4PhysicalConstants.hh"
// #include "G4MaterialTable.hh"
// #include "G4Material.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

DetectorConstruction::DetectorConstruction()
:fPhysiWorld(NULL), fLogicWorld(NULL), fSolidWorld(NULL)
{}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

DetectorConstruction::~DetectorConstruction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4VPhysicalVolume* DetectorConstruction::Construct()

{
  DefineMaterials();
  return ConstructDetector();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void DetectorConstruction::DefineMaterials()
{ 
  // Silicon is defined from NIST material database
  G4NistManager * man = G4NistManager::Instance();
  G4Material * Si = man->FindOrBuildMaterial("G4_Si");

  // Default materials in setup.
  fSiMaterial = Si;
  fVoidMaterial = man->FindOrBuildMaterial("G4_Galactic"); // vacuum approxiamtion
  // fVoidMaterial = Si;

  // vacuum, taken from LXe example:
  // G4double a;  // atomic mass
  // G4double z;  // atomic number
  // G4double density;
  // fVoidMaterial = new G4Material("Vacuum", z = 1., a = 1.01 * g / mole,
  //                                density = universe_mean_density, kStateGas, 0.1 * kelvin,
  //                                1.e-19 * pascal);

  // vacuum from internet:
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
G4VPhysicalVolume* DetectorConstruction::ConstructDetector()
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

  // WORLD VOLUME
  
  fWorldSizeX  = 10*cm; 
  fWorldSizeY  = 10*cm; 
  fWorldSizeZ  = 10*cm;

  fSolidWorld = new G4Box("World",				     //its name
			   fWorldSizeX/2,fWorldSizeY/2,fWorldSizeZ/2);  //its size
  

  fLogicWorld = new G4LogicalVolume(fSolidWorld,	//its solid
				   fVoidMaterial,		//its material
				   "World");		//its name
  
  fPhysiWorld = new G4PVPlacement(0,			//no rotation
  				 G4ThreeVector(),	//at (0,0,0)
                                 "World",		//its name
                                 fLogicWorld,		//its logical volume
                                 0,			//its mother  volume
                                 false,			//no boolean operation
                                 0);			//copy number

  G4double TargetSizeZ = 0.3*mm;//0.2*um; 
  G4double TargetSizeX = 63.*mm;
  G4double TargetSizeY = 63.*mm;

  G4Box* targetSolid = new G4Box("Target",				     //its name
				 TargetSizeX/2,TargetSizeY/2,TargetSizeZ/2);   //its size
  

  G4LogicalVolume* logicTarget = new G4LogicalVolume(targetSolid,       //its solid
						     fSiMaterial,	//its material
						     "Target");		//its name
  
  new G4PVPlacement(0,			                               //no rotation
		    G4ThreeVector(),	                               //at (0,0,0)
		    "Target",		//its name
		    logicTarget,	//its logical volume
		    fPhysiWorld,		//its mother  volume
		    false,		//no boolean operation
		    0);			//copy number

  // Visualization attributes
  G4VisAttributes* worldVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0)); //White
  worldVisAtt->SetVisibility(true);
  fLogicWorld->SetVisAttributes(worldVisAtt);

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

  return fPhysiWorld;
}

void DetectorConstruction::ConstructSDandField()
{
  // Sensitive detectors

  G4String trackerChamberSDname = "B2/TrackerChamberSD";
  B2TrackerSD* aTrackerSD = new B2TrackerSD(trackerChamberSDname,
                                            "TrackerHitsCollection");
  G4SDManager::GetSDMpointer()->AddNewDetector(aTrackerSD);
  // Setting aTrackerSD to all logical volumes with the same name 
  // of "Chamber_LV".
  SetSensitiveDetector("Target", aTrackerSD, true);

  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  // G4ThreeVector fieldValue = G4ThreeVector();
  // fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  // fMagFieldMessenger->SetVerboseLevel(2);
  
  // // Register the field messenger for deleting
  // G4AutoDelete::Register(fMagFieldMessenger);
}
