#include "mscDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4MaterialTable.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"

#include "G4SDManager.hh"
#include "G4SDChargedFilter.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSTrackLength.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4GenericMessenger.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include <stdio.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mscDetectorConstruction::mscDetectorConstruction()
 : G4VUserDetectorConstruction(),
   fMessenger(0),
   fCheckOverlaps(true)
{
  // Define /msc/det commands using generic messenger class
  fMessenger 
    = new G4GenericMessenger(this, "/msc/det/", "Detector construction control");

  radiatorThickness = 2.*cm;  
  // Define /msc/det/setRadiatorWidth command
  G4GenericMessenger::Command& setRadiatorThicknessCmd
    = fMessenger->DeclareMethod("setRadiatorThickness", 
                                 &mscDetectorConstruction::SetRadiatorThickness, 
                                 "set the thickness (z component) of the radiator (in cm)");
  setRadiatorThicknessCmd.SetUnitCategory("Length");                                
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mscDetectorConstruction::~mscDetectorConstruction()
{ 
  delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* mscDetectorConstruction::Construct()
{
  // Define materials 
  DefineMaterials();
  
  // Define volumes
  return DefineVolumes();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void mscDetectorConstruction::DefineMaterials()
{ 

  // Lead material defined using specific parameters
  G4Material* matPb = new G4Material("Lead", 82, 207.19*g/mole, 11.35*g/cm3);

  // Antimony material defined using specific parameters
  G4Material* matSb = new G4Material("Antimony", 51, 121.76*g/mole, 6.68*g/cm3);

  // Radiator Material
  G4Material* matCollimator = new G4Material("PBA", 11.005*g/cm3, 2);
  matCollimator-> AddMaterial(matPb, 0.955);
  matCollimator-> AddMaterial(matSb, 0.045);
  
  //Detector Material == a material with very low density to not affect the tracks
  //                     but which we can look for later in the stepping action
  G4NistManager* nistManager = G4NistManager::Instance();
  G4Material *Ar = nistManager->FindOrBuildMaterial("G4_Ar");
  G4Material *detectorMat = new G4Material("detectorMat", 0.00000001*mg/cm3, 1);
  detectorMat->AddMaterial(Ar, 100.*perCent);

  // Vacuum
  new G4Material("Galactic", 1., 1.01*g/mole, universe_mean_density,
                  kStateGas, 2.73*kelvin, 3.e-18*pascal);

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* mscDetectorConstruction::DefineVolumes()
{
  // Geometry parameters
  G4double detectorThickness = 1.*cm;
  G4double SizeX  = 100.*cm;
  G4double SizeY  =  20.*cm;

  G4double worldSizeXY = 200 * cm;
  G4double worldSizeZ  =  20 * cm; 
  
  // Get materials
  G4Material* defaultMaterial = G4Material::GetMaterial("Galactic");
  G4Material* radiatorMaterial = G4Material::GetMaterial("PBA");
  G4Material* detectorMaterial = G4Material::GetMaterial("detectorMat");
  
  if ( ! defaultMaterial || ! radiatorMaterial || ! detectorMaterial ) {
    G4cerr << "Cannot retrieve materials already defined. " << G4endl;
    G4cerr << "Exiting application " << G4endl;
    exit(1);
  }  
   
  //     
  // World
  //
  G4VSolid* worldS 
    = new G4Box("World",           // its name
                 worldSizeXY, worldSizeXY, worldSizeZ); // its size
                         
  G4LogicalVolume* worldLV
    = new G4LogicalVolume(
                 worldS,           // its solid
                 defaultMaterial,  // its material
                 "World");         // its name
                                   
  G4VPhysicalVolume* worldPV
    = new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(),  // at (0,0,0)
                 worldLV,          // its logical volume                         
                 "World",          // its name
                 0,                // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 
  

  //
  //Radiator
  //
  G4VSolid* radiatorSol
    = new G4Box("radiator",		   // its name
		 SizeX/2, SizeY/2, radiatorThickness/2); // its size

  G4LogicalVolume* radiatorLV
    = new G4LogicalVolume(
                 radiatorSol,    // its solid
                 radiatorMaterial, // its material
                 "radiatorLV");  // its name
                                   
  new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(0., 0., 0.), 
                 radiatorLV,       // its logical volume                         
                 "Radiator",       // its name
                 worldLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 

  //
  //Detectors
  //
 
  G4VSolid* Detector1Solid 
    = new G4Box("Detector1",  // its name
		SizeX/2, SizeY/2, detectorThickness); // its size
  
  G4LogicalVolume* detector1Logical
    = new G4LogicalVolume(
			  Detector1Solid,    // its solid
			  detectorMaterial, // its material
			  "Detector1");  // its name
  
  new G4PVPlacement(
		    0,                // no rotation
		    G4ThreeVector(0., 0., radiatorThickness), 
		    detector1Logical,          // its logical volume                    
		    "Detector1",    // its name
		    worldLV,          // its mother  volume
		    false,            // no boolean operation
		    0,                // copy number
		    fCheckOverlaps);  // checking overlaps 


  /*add new detector 3 cm behind detector 1*/
  G4VSolid* Detector2Solid 
    = new G4Box("Detector2",  // its name
		SizeX/2, SizeY/2, detectorThickness); // its size
  
  G4LogicalVolume* detector2Logical
    = new G4LogicalVolume(
			  Detector2Solid,    // its solid
			  detectorMaterial, // its material
			  "Detector2");  // its name
  
  new G4PVPlacement(
		    0,                // no rotation
		    G4ThreeVector(0., 0., (radiatorThickness) + 3*cm), 
		    detector2Logical,          // its logical volume                    
		    "Detector2",    // its name
		    worldLV,          // its mother  volume
		    false,            // no boolean operation
		    0,                // copy number
		    fCheckOverlaps);  // checking overlaps 
	

  //                                        
  // Visualization attributes
  //
  worldLV->SetVisAttributes (G4VisAttributes::Invisible);


  //
  // Always return the physical World
  //
  return worldPV;
} 

