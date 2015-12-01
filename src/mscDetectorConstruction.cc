#include "mscDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4MaterialTable.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4GenericMessenger.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4UserLimits.hh"
#include "G4RunManager.hh"

#include "G4GeometryManager.hh"
#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"

#include <stdio.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mscDetectorConstruction::mscDetectorConstruction()
 : G4VUserDetectorConstruction(),
   nrUnits(0),
   fCheckOverlaps(true)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mscDetectorConstruction::~mscDetectorConstruction()
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void mscDetectorConstruction::UpdateGeometry()
{
  // clean-up previous geometry
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
  G4LogicalSkinSurface::CleanSurfaceTable();
  G4LogicalBorderSurface::CleanSurfaceTable();
  
  //define new one
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* mscDetectorConstruction::Construct()
{
  // Define materials 
  DefineMaterials();

  G4cout<<G4endl<<G4endl<< " ~~~~~~~~~~~~~ "<< G4endl<< nrUnits<<G4endl<< __PRETTY_FUNCTION__<<G4endl;
  
  // Define volumes
  if( nrUnits == -1 )                                   
    return BuildQweakGeometry();
  else if( nrUnits == 0 )
    return BuildSimpleDetector();
  else
    return BuildStackedDetector();

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

  // // Vacuum
  new G4Material("Galactic", 1., 1.01*g/mole, universe_mean_density,
		 kStateGas, 2.73*kelvin, 3.e-18*pascal);

  // Air material: Air 18 degr.C and 58% humidity
  G4double fractionmass(0);
  G4Element* elH  = nistManager->FindOrBuildElement("H");
  G4Element* elN  = nistManager->FindOrBuildElement("N");
  G4Element* elO  = nistManager->FindOrBuildElement("O");
  G4Element* elAr = nistManager->FindOrBuildElement("Ar");
  G4Material  *matAir = new G4Material("Air",1.214*mg/cm3,4);
  matAir -> AddElement(elN,  fractionmass=0.7494);
  matAir -> AddElement(elO,  fractionmass=0.2369);
  matAir -> AddElement(elAr, fractionmass=0.0129);
  matAir -> AddElement(elH,  fractionmass=0.0008);
  
  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4VPhysicalVolume* mscDetectorConstruction::BuildStackedDetector()
{

  G4double unitRadThickness = 2. * mm;
  
  // Geometry parameters
  G4double detectorThickness = 0.01 * mm;
  G4double SizeX  = 200. * cm;
  G4double SizeY  =  20. * cm;

  G4double worldSizeXY = 400 * cm;
  G4double worldSizeZ  =  50 * cm; 
  
  // Get materials
  G4Material* vacuumMaterial=G4Material::GetMaterial("Galactic");
  G4Material* defaultMaterial = G4Material::GetMaterial("Air");
  G4Material* radiatorMaterial = G4Material::GetMaterial("PBA");
  G4Material* detectorMaterial = G4Material::GetMaterial("detectorMat");
  
  if ( ! defaultMaterial || ! radiatorMaterial ||
       ! detectorMaterial || ! vacuumMaterial) {
    G4cerr << __PRETTY_FUNCTION__ << " Cannot retrieve materials already defined. " << G4endl;
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
  

  G4VSolid *unitContainerSol =
    new G4Box("unitSol",SizeX/2.,SizeY/2.,(unitRadThickness+detectorThickness)/2.);
  G4LogicalVolume *unitContainerLogical =
    new G4LogicalVolume(unitContainerSol,vacuumMaterial,"unitContainerLV");
  unitContainerLogical->SetVisAttributes (G4VisAttributes::Invisible);
  
  //
  //Radiator
  //
  G4VSolid* radiatorSol
    = new G4Box("radiator",		   // its name
		SizeX/2., SizeY/2., unitRadThickness/2.); // its size
  
  G4LogicalVolume* radiatorLogical
    = new G4LogicalVolume(
			  radiatorSol,    // its solid
			  radiatorMaterial, // its material
			  "radiatorLogical");  // its name

  // define step limitation for this container
  G4double MaxStepInPbRadiator = 0.01*mm;
  radiatorLogical->SetUserLimits(new G4UserLimits(MaxStepInPbRadiator));
  // define step limitation for this container
  
  new G4PVPlacement(
		    0,                   // no rotation
		    G4ThreeVector(0., 0., (-detectorThickness)/2.), 
		    radiatorLogical,     // its logical volume                         
		    "Radiator",          // its name
		    unitContainerLogical,// its mother  volume
		    false,               // no boolean operation
		    0,                   // copy number
		    fCheckOverlaps);     // checking overlaps 
  
  G4Colour  blue(0/255.,0/255.,255/255.);
  G4VisAttributes* radiatorVisAtt = new G4VisAttributes(blue);
  radiatorVisAtt->SetVisibility(true);
  radiatorLogical->SetVisAttributes(radiatorVisAtt);

  //
  //Detector
  //
  G4VSolid* detectorSolid 
    = new G4Box("detectorSol",  // its name
		SizeX/2, SizeY/2, detectorThickness/2); // its size
  
  G4LogicalVolume* detectorLogical
    = new G4LogicalVolume(
			  detectorSolid,     // its solid
			  detectorMaterial,  // its material
			  "detectorLogical");// its name
  
  new G4PVPlacement(
		    0,                // no rotation
		    G4ThreeVector(0., 0., (unitRadThickness/2.)), 
		    detectorLogical,  // its logical volume                    
		    "detector",       // its name
		    unitContainerLogical,// its mother  volume
		    false,            // no boolean operation
		    0,                // copy number
		    fCheckOverlaps);  // checking overlaps 
  
  G4Colour  red(255/255.,0/255.,0/255.);
  G4VisAttributes* detectorVisAtt = new G4VisAttributes(red);
  detectorVisAtt->SetVisibility(true);
  detectorLogical->SetVisAttributes(detectorVisAtt);
  
  for(G4int i=0;i<nrUnits;i++){
    G4double zpos=(i+0.5)*(unitRadThickness+detectorThickness);
    std::stringstream gppIsDumb;
    gppIsDumb<<"container"<<i;
    std::string cnm=gppIsDumb.str();
    new G4PVPlacement(
		      0,                // no rotation
		      G4ThreeVector(0., 0., zpos), 
		      unitContainerLogical,  // its logical volume                    
		      cnm,  // its name
		      worldLV,// its mother  volume
		      false,            // no boolean operation
		      i,                // copy number
		      fCheckOverlaps);  // checking overlaps 
    
  }
  
  //                                        
  // Visualization attributes
  //
  worldLV->SetVisAttributes (G4VisAttributes::Invisible);
  

  //
  // Always return the physical World
  //
  return worldPV;
} 



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4VPhysicalVolume* mscDetectorConstruction::BuildSimpleDetector()
{
  // Geometry parameters
  G4double radThickness = 2. * cm;  
  G4double detectorThickness = 0.01 * mm;
  G4double SizeX  = 200. * cm;
  G4double SizeY  =  20. * cm;
  G4double worldSizeXY = 400 * cm;
  G4double worldSizeZ  =  50 * cm; 
  // G4double PbRadiationLength = 0.5612 * cm;
  
  // Get materials
  G4Material* defaultMaterial  = G4Material::GetMaterial("Air");
  G4Material* radiatorMaterial = G4Material::GetMaterial("PBA");
  G4Material* detectorMaterial = G4Material::GetMaterial("detectorMat");
  
  if ( ! defaultMaterial || ! radiatorMaterial || ! detectorMaterial) {
    G4cerr << __PRETTY_FUNCTION__ << " Cannot retrieve materials already defined. " << G4endl;
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
		SizeX/2., SizeY/2., radThickness/2.); // its size
  
  G4LogicalVolume* radiatorLogical
    = new G4LogicalVolume(
			  radiatorSol,    // its solid
			  radiatorMaterial, // its material
			  "radiatorLogical");  // its name

  // define step limitation for this container
  G4double MaxStepInPbRadiator = 0.01*mm;
  radiatorLogical->SetUserLimits(new G4UserLimits(MaxStepInPbRadiator));
  // define step limitation for this container
  
  new G4PVPlacement(
		    0,                   // no rotation
		    G4ThreeVector(0., 0., 0.), 
		    radiatorLogical,     // its logical volume                         
		    "Radiator",          // its name
		    worldLV,// its mother  volume
		    false,               // no boolean operation
		    0,                   // copy number
		    fCheckOverlaps);     // checking overlaps 
  
  G4Colour  blue(0/255.,0/255.,255/255.);
  G4VisAttributes* radiatorVisAtt = new G4VisAttributes(blue);
  radiatorVisAtt->SetVisibility(true);
  radiatorLogical->SetVisAttributes(radiatorVisAtt);

  //
  //Detector
  //
  G4VSolid* detectorSolid 
    = new G4Box("detectorSol",  // its name
		SizeX/2, SizeY/2, detectorThickness/2); // its size
  
  G4LogicalVolume* detectorLogical
    = new G4LogicalVolume(
			  detectorSolid,     // its solid
			  detectorMaterial,  // its material
			  "detectorLogical");// its name
  
  new G4PVPlacement(
		    0,                // no rotation
		    G4ThreeVector(0., 0., 5.), 
		    detectorLogical,  // its logical volume                    
		    "detector",       // its name
		    worldLV,// its mother  volume
		    false,            // no boolean operation
		    0,                // copy number
		    fCheckOverlaps);  // checking overlaps 
  
  G4Colour  red(255/255.,0/255.,0/255.);
  G4VisAttributes* detectorVisAtt = new G4VisAttributes(red);
  detectorVisAtt->SetVisibility(true);
  detectorLogical->SetVisAttributes(detectorVisAtt);
  
  
  //                                        
  // Visualization attributes
  //
  worldLV->SetVisAttributes (G4VisAttributes::Invisible);
  
  
  //
  // Always return the physical World
  //
  return worldPV;
} 


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4VPhysicalVolume* mscDetectorConstruction::BuildQweakGeometry()
{
  // Geometry parameters
  G4double worldSizeXY = 400 * cm;
  G4double worldSizeZ  =  50 * cm; 
  
  // Get materials
  G4Material* vacuumMaterial=G4Material::GetMaterial("Galactic");
  G4Material* defaultMaterial = G4Material::GetMaterial("Air");
  G4Material* radiatorMaterial = G4Material::GetMaterial("PBA");
  G4Material* detectorMaterial = G4Material::GetMaterial("detectorMat");
  
  if ( ! defaultMaterial || ! radiatorMaterial ||
       ! detectorMaterial || ! vacuumMaterial) {
    G4cerr << __PRETTY_FUNCTION__ << " Cannot retrieve materials already defined. " << G4endl;
    G4cerr << "Exiting application " << G4endl;
    exit(1);
  }  
   
  //     
  // World
  //
  G4VSolid* worldS = new G4Box("World",           // its name
			       worldSizeXY, worldSizeXY, worldSizeZ); // its size
                         
  G4LogicalVolume* worldLV = new G4LogicalVolume(
						 worldS,           // its solid
						 defaultMaterial,  // its material
						 "World");         // its name
  
  G4VPhysicalVolume* worldPV  = new G4PVPlacement(
						  0,                // no rotation
						  G4ThreeVector(),  // at (0,0,0)
						  worldLV,          // its logical volume                         
						  "World",          // its name
						  0,                // its mother  volume
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

