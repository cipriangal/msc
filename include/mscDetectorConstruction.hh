#ifndef mscDetectorConstruction_h
#define mscDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4Box;
class G4VPhysicalVolume;
class G4UniformMagField;
class G4GenericMessenger;

class mscDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  mscDetectorConstruction();
  virtual ~mscDetectorConstruction();
  
  virtual G4VPhysicalVolume* Construct();
  
  void SetRadiatorThickness(G4double val){radiatorThickness = val;}

  virtual void UpdateGeometry();
  void SetNrUnits( G4int val ){nrUnits=val;}
  
private:
  // methods
  //
  void DefineMaterials();  
  G4VPhysicalVolume* BuildQweakGeometry();
  G4VPhysicalVolume* BuildSimpleDetector();
  G4VPhysicalVolume* BuildStackedDetector();
  // data members
  //
  G4int nrUnits;
  G4double radiatorThickness;
  G4bool  fCheckOverlaps; // option to activate checking of volumes overlaps
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

