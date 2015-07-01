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

  // // Define /msc/det/setMagField command
  // G4GenericMessenger::Command& setMagFieldCmd
  //   = fMessenger->DeclareMethod("setMagField", 
  //                               &mscDetectorConstruction::SetMagField, 
  //                               "Define magnetic field value (in X direction");
  // setMagFieldCmd.SetUnitCategory("Magnetic flux density");                                
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
<<<<<<< HEAD
 
  //Scintillator Material
  G4Material* Ar = nistManager->FindOrBuildMaterial("G4_Ar");
  //Ar = matman->FindOrBuildMaterial("G4_Ar");
  G4Material* matman = new G4Material("Kryptonite", 0.00000001*mg/cm3, 1);
  matman->AddMaterial(Ar, 1);
=======
>>>>>>> 55555208103a7c2588f7c01ac5fb724336932c56

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* mscDetectorConstruction::DefineVolumes()
{
  // Geometry parameters
  G4double radiatorThickness = 2.*cm;
  G4double detectorThickness = 1.*cm;
  G4double SizeX  = 100.*cm;
  G4double SizeY  =  20.*cm;

  G4double worldSizeXY = 200 * cm;
  G4double worldSizeZ  =  20 * cm; 
  
  // Get materials
  G4Material* defaultMaterial = G4Material::GetMaterial("Galactic");
<<<<<<< HEAD
  G4Material* absorberMaterial = G4Material::GetMaterial("G4_Pb");
  G4Material* gapMaterial = G4Material::GetMaterial("liquidArgon");
  G4Material* wallMaterial = G4Material::GetMaterial("PBA");
  G4Material* scintillator = G4Material::GetMaterial("Kryptonite");
=======
  G4Material* radiatorMaterial = G4Material::GetMaterial("PBA");
  G4Material* detectorMaterial = G4Material::GetMaterial("detectorMat");
>>>>>>> 55555208103a7c2588f7c01ac5fb724336932c56
 
  
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
                 "radiator",       // its name
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

	 
<<<<<<< HEAD
  */

  
  //
  // print parameters
  //
  G4cout << "\n------------------------------------------------------------"
         << "\n---> The calorimeter is " << nofLayers << " layers of: [ "
         << absoThickness/mm << "mm of " << absorberMaterial->GetName() 
         << " + "
         << gapThickness/mm << "mm of " << gapMaterial->GetName() << " ] " 
         << "\n------------------------------------------------------------\n";
  
   
  // 
  // Scorers
  //

  // declare Absorber as a MultiFunctionalDetector scorer
  //  
  //G4MultiFunctionalDetector* absDetector 
  //  = new G4MultiFunctionalDetector("Absorber");

  //G4VPrimitiveScorer* primitive;
  //G4SDChargedFilter* charged = new G4SDChargedFilter("chargedFilter");
  //primitive = new G4PSEnergyDeposit("Edep");
  //absDetector->RegisterPrimitive(primitive);

  //primitive = new G4PSTrackLength("TrackLength");
  //primitive ->SetFilter(charged);
  //absDetector->RegisterPrimitive(primitive);  

  //G4SDManager::GetSDMpointer()->AddNewDetector(absDetector);
  //absorberLV->SetSensitiveDetector(absDetector);
  
  // declare Gap as a MultiFunctionalDetector scorer
  //  
  //G4MultiFunctionalDetector* gapDetector 
  //  = new G4MultiFunctionalDetector("Gap");

  //primitive = new G4PSEnergyDeposit("Edep");
  //gapDetector->RegisterPrimitive(primitive);
  
  //primitive = new G4PSTrackLength("TrackLength");
  //primitive ->SetFilter(charged);
  //gapDetector->RegisterPrimitive(primitive);  
  
  //G4SDManager::GetSDMpointer()->AddNewDetector(gapDetector);
  //gapLV->SetSensitiveDetector(gapDetector);  
=======
  /*add new detector 3 cm behind detector 1*/

  
>>>>>>> 55555208103a7c2588f7c01ac5fb724336932c56

  //                                        
  // Visualization attributes
  //
  worldLV->SetVisAttributes (G4VisAttributes::Invisible);


  //
  // Always return the physical World
  //
  return worldPV;
} 

