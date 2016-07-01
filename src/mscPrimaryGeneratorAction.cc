#include "mscPrimaryGeneratorAction.hh"

#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mscPrimaryGeneratorAction::mscPrimaryGeneratorAction()
 : G4VUserPrimaryGeneratorAction(),
   fParticleGun(0),
   polarization("V"),
   beamEnergy( (1160.-0.511)*MeV)
{
  G4int nofParticles = 1;
  fParticleGun = new G4ParticleGun(nofParticles);

  // default particle kinematic
  //
  G4ParticleDefinition* particleDefinition 
    = G4ParticleTable::GetParticleTable()->FindParticle("e-");
  fParticleGun->SetParticleDefinition(particleDefinition);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mscPrimaryGeneratorAction::~mscPrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void mscPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  
  fParticleGun->SetParticleEnergy(beamEnergy);
  
  // Set gun position
  fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., - 30.*cm));

  if(polarization=="V"){    
    fParticleGun->SetParticlePolarization(G4ThreeVector(0.,1.,0.));
  }else if(polarization=="L"){
    fParticleGun->SetParticlePolarization(G4ThreeVector(0.,0.,1.));
  }else if(polarization=="mL"){
    fParticleGun->SetParticlePolarization(G4ThreeVector(0.,0.,-1.));
  }else if(polarization=="mV"){
    fParticleGun->SetParticlePolarization(G4ThreeVector(0.,-1.,0.));
  }else
    fParticleGun->SetParticlePolarization(G4ThreeVector(0.,0.,1.));//default longitudinal

  fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

