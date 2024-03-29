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
/// \file B2TrackerSD.cc
/// \brief Implementation of the B2TrackerSD class

#include "SD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2TrackerSD::B2TrackerSD(const G4String& name,
                         const G4String& hitsCollectionName) 
 : G4VSensitiveDetector(name),
   fHitsCollection(NULL)
{
  collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2TrackerSD::~B2TrackerSD() 
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2TrackerSD::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection

  fHitsCollection 
    = new B2TrackerHitsCollection(SensitiveDetectorName, collectionName[0]); 

  // Add this collection in hce

  G4int hcID 
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection ); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool B2TrackerSD::ProcessHits(G4Step* aStep, 
                                     G4TouchableHistory*)
{  
  // energy deposit
  G4double edep = aStep->GetTotalEnergyDeposit();
  // std::cout << "energy deposit: " << edep << ", " << aStep->GetNonIonizingEnergyDeposit() << std::endl;

  G4StepPoint* preStep = aStep->GetPreStepPoint();

  if (edep==0.) return false;

  {
    static G4int gunParticleId = -1;
    // if(gunParticleId == -1)
    // gunParticleId = aStep->GetTrack()->GetTrackID();

    if (aStep->IsFirstStepInVolume() && preStep->GetStepStatus() == fGeomBoundary)
    {
      G4cout << "Enter Location: " << (aStep->GetTrack()->GetPosition()) << /*aStep->GetTrack()->GetMaterial() <<*/ G4endl;
      gunParticleId = aStep->GetTrack()->GetTrackID();
      G4cout << aStep->GetTrack()->GetParticleDefinition()->GetParticleName() << G4endl;
      std::cout << "Energy at enter: " << aStep->GetTrack()->GetDynamicParticle()->GetKineticEnergy() / eV << std::endl;
      // std::cout << "type: " << aStep->GetTrack()->GetDynamicParticle()->GetParticleDefinition()->GetParticleTable()-> << std::endl;
      std::cout << "number: " << aStep->GetTrack()->GetDynamicParticle()->GetParticleDefinition()->GetAtomicNumber() << std::endl;
      std::cout << "mass: " << aStep->GetTrack()->GetDynamicParticle()->GetParticleDefinition()->GetPDGMass()/proton_mass_c2 << std::endl;
      std::cout << "charge: " << aStep->GetTrack()->GetDynamicParticle()->GetCharge() << std::endl;

    }
    if (aStep->IsLastStepInVolume() && gunParticleId == aStep->GetTrack()->GetTrackID())
    {
      std::cout << "Exit Location: " << (aStep->GetTrack()->GetPosition()) << std::endl;
      G4cout << "Exit Location: " << (aStep->GetTrack()->GetPosition()) << /*aStep->GetTrack()->GetMaterial() <<*/ G4endl;
    }
    if(gunParticleId == aStep->GetTrack()->GetTrackID())
    {
      std::cout << "Energyyyy: " << aStep->GetTrack()->GetKineticEnergy() / eV << std::endl;
    }
  }

  B2TrackerHit* newHit = new B2TrackerHit();

  newHit->SetTrackID  (aStep->GetTrack()->GetTrackID());
  newHit->SetChamberNb(aStep->GetPreStepPoint()->GetTouchableHandle()
                                               ->GetCopyNumber());
  newHit->SetEdep(edep);
  newHit->SetPos (aStep->GetPostStepPoint()->GetPosition());

  fHitsCollection->insert( newHit );

  //newHit->Print();

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2TrackerSD::EndOfEvent(G4HCofThisEvent*)
{
  if ( true ) { 
     G4int nofHits = fHitsCollection->entries();
     G4cout << G4endl
            << "-------->Hits Collection: in this event they are " << nofHits 
            << " hits in the tracker chambers: " << G4endl;
     for ( G4int i=0; i<nofHits; i++ ) (*fHitsCollection)[i]->Print();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
