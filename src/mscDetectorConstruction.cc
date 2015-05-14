#include "mscDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4UniformMagField.hh"

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
   fMagField(0),
   fCheckOverlaps(true)
{
  // Define /B4/det commands using generic messenger class
  fMessenger 
    = new G4GenericMessenger(this, "/B4/det/", "Detector construction control");

  // Define /B4/det/setMagField command
  G4GenericMessenger::Command& setMagFieldCmd
    = fMessenger->DeclareMethod("setMagField", 
                                &mscDetectorConstruction::SetMagField, 
                                "Define magnetic field value (in X direction");
  setMagFieldCmd.SetUnitCategory("Magnetic flux density");                                
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mscDetectorConstruction::~mscDetectorConstruction()
{ 
  delete fMagField;
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
  // Lead material defined using NIST Manager
  G4NistManager* nistManager = G4NistManager::Instance();
  G4bool fromIsotopes = false;
  nistManager->FindOrBuildMaterial("G4_Pb", fromIsotopes);
  
  // Liquid argon material
  G4double a;  // mass of a mole;
  G4double z;  // z=mean number of protons;  
  G4double density; 
  new G4Material("liquidArgon", z=18., a= 39.95*g/mole, density= 1.390*g/cm3);
         // The argon by NIST Manager is a gas with a different density

  // Vacuum
  new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                  kStateGas, 2.73*kelvin, 3.e-18*pascal);

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* mscDetectorConstruction::DefineVolumes()
{
  // Geometry parameters
  G4int nofLayers = 10;
  G4double absoThickness = 10.*mm;
  G4double gapThickness =  5.*mm;
  G4double calorSizeXY  = 10.*cm;

  G4double layerThickness = absoThickness + gapThickness;
  G4double calorThickness = nofLayers * layerThickness;
  G4double worldSizeXY = 1.2 * calorSizeXY;
  G4double worldSizeZ  = 1.2 * calorThickness; 
  
  // Get materials
  G4Material* defaultMaterial = G4Material::GetMaterial("Galactic");
  G4Material* absorberMaterial = G4Material::GetMaterial("G4_Pb");
  G4Material* gapMaterial = G4Material::GetMaterial("liquidArgon");
  
  if ( ! defaultMaterial || ! absorberMaterial || ! gapMaterial ) {
    G4cerr << "Cannot retrieve materials already defined. " << G4endl;
    G4cerr << "Exiting application " << G4endl;
    exit(1);
  }  
   
  //     
  // World
  //
  G4VSolid* worldS 
    = new G4Box("World",           // its name
                 worldSizeXY/2, worldSizeXY/2, worldSizeZ/2); // its size
                         
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
  // Calorimeter
  //  
  G4VSolid* calorimeterS
    = new G4Box("Calorimeter",     // its name
                 calorSizeXY/2, calorSizeXY/2, calorThickness/2); // its size
                         
  G4LogicalVolume* calorLV
    = new G4LogicalVolume(
                 calorimeterS,    // its solid
                 defaultMaterial, // its material
                 "Calorimeter");  // its name
                                   
  new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(),  // at (0,0,0)
                 calorLV,          // its logical volume                         
                 "Calorimeter",    // its name
                 worldLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 
  
  //                                 
  // Layer
  //
  G4VSolid* layerS 
    = new G4Box("Layer",           // its name
                 calorSizeXY/2, calorSizeXY/2, layerThickness/2); // its size
                         
  G4LogicalVolume* layerLV
    = new G4LogicalVolume(
                 layerS,           // its solid
                 defaultMaterial,  // its material
                 "Layer");         // its name

  new G4PVReplica(
                 "Layer",          // its name
                 layerLV,          // its logical volume
                 calorLV,          // its mother
                 kZAxis,           // axis of replication
                 nofLayers,        // number of replica
                 layerThickness);  // witdth of replica
  
  //                               
  // Absorber
  //
  G4VSolid* absorberS 
    = new G4Box("Abso",            // its name
                 calorSizeXY/2, calorSizeXY/2, absoThickness/2); // its size
                         
  G4LogicalVolume* absorberLV
    = new G4LogicalVolume(
                 absorberS,        // its solid
                 absorberMaterial, // its material
                 "Abso");          // its name
                                   
   new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(0., 0., -gapThickness/2), //  its position
                 absorberLV,       // its logical volume                         
                 "Abso",           // its name
                 layerLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 

  //                               
  // Gap
  //
  G4VSolid* gapS 
    = new G4Box("Gap",             // its name
                 calorSizeXY/2, calorSizeXY/2, gapThickness/2); // its size
                         
  G4LogicalVolume* gapLV
    = new G4LogicalVolume(
                 gapS,             // its solid
                 gapMaterial,      // its material
                 "Gap");      // its name
                                   
  new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(0., 0., absoThickness/2), //  its position
                 gapLV,            // its logical volume                         
                 "Gap",            // its name
                 layerLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 
  
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
  G4MultiFunctionalDetector* absDetector 
    = new G4MultiFunctionalDetector("Absorber");

  G4VPrimitiveScorer* primitive;
  G4SDChargedFilter* charged = new G4SDChargedFilter("chargedFilter");
  primitive = new G4PSEnergyDeposit("Edep");
  absDetector->RegisterPrimitive(primitive);

  primitive = new G4PSTrackLength("TrackLength");
  primitive ->SetFilter(charged);
  absDetector->RegisterPrimitive(primitive);  

  G4SDManager::GetSDMpointer()->AddNewDetector(absDetector);
  absorberLV->SetSensitiveDetector(absDetector);
  
  // declare Gap as a MultiFunctionalDetector scorer
  //  
  G4MultiFunctionalDetector* gapDetector 
    = new G4MultiFunctionalDetector("Gap");

  primitive = new G4PSEnergyDeposit("Edep");
  gapDetector->RegisterPrimitive(primitive);
  
  primitive = new G4PSTrackLength("TrackLength");
  primitive ->SetFilter(charged);
  gapDetector->RegisterPrimitive(primitive);  
  
  G4SDManager::GetSDMpointer()->AddNewDetector(gapDetector);
  gapLV->SetSensitiveDetector(gapDetector);  

  //                                        
  // Visualization attributes
  //
  worldLV->SetVisAttributes (G4VisAttributes::Invisible);

  G4VisAttributes* simpleBoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  simpleBoxVisAtt->SetVisibility(true);
  calorLV->SetVisAttributes(simpleBoxVisAtt);

  //
  // Always return the physical World
  //
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void mscDetectorConstruction::SetMagField(G4double fieldValue)
{
  // Apply a global uniform magnetic field along X axis
  G4FieldManager* fieldManager
    = G4TransportationManager::GetTransportationManager()->GetFieldManager();

  // Delete the existing magnetic field
  if ( fMagField )  delete fMagField; 

  if ( fieldValue != 0. ) {
    // create a new one if not null
    fMagField 
      = new G4UniformMagField(G4ThreeVector(fieldValue, 0., 0.));
      
    fieldManager->SetDetectorField(fMagField);
    fieldManager->CreateChordFinder(fMagField);
  } 
  else {
    fMagField = 0;
    fieldManager->SetDetectorField(fMagField);
  }
}
