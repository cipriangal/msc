#include "mscDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4MaterialTable.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4SubtractionSolid.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4GenericMessenger.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4UserLimits.hh"
#include "G4RunManager.hh"

#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4Trap.hh"
#include "G4Polycone.hh"

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
   radiatorThickness(2*cm),
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
  //return BuildQweakGeometry();

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
  
  G4Element *elAl = nistManager->FindOrBuildElement("Al");
  G4Material *matAl = new G4Material("Aluminum",2.7*g/cm3,1);
  matAl -> AddElement(elAl,1);

  
  G4Element *elSi = nistManager->FindOrBuildElement("Si");
  G4Material *matQuartz = new G4Material("Quartz",2.2*g/cm3, 2);
  matQuartz->AddElement(elSi, 1);
  matQuartz->AddElement(elO , 2);

  G4Material *matSiElast = new G4Material("SiElast_Glue",2.2*g/cm3, 2);
  matSiElast->AddElement(elSi, 1);
  matSiElast->AddElement(elO , 2);

  G4Element *elC = nistManager->FindOrBuildElement("C");
  G4Material* matTyvek = new G4Material("Tyvek",0.96*g/cm3 , 2);
  matTyvek -> AddElement(elH, 2);
  matTyvek -> AddElement(elC, 1);

  
  G4Material *matLimeGlass = new G4Material("LimeGlass",2.200*g/cm3, 2);
  matLimeGlass->AddElement(elSi, 1);
  matLimeGlass->AddElement(elO , 2);

  // Photocathode material, approximated as elemental cesium
  G4Element* elK  = nistManager->FindOrBuildElement("K");
  G4Material *matPhotocathode =  new G4Material("Photocathode",5.0*g/cm3,1);
  matPhotocathode -> AddElement(elK, 1);
  
  // Print materials
  G4cout << G4endl << G4endl << "~~~~~~~~~~~~~~~~~~~~~Material Printout~~~~~~~~~~~~~~~~~~~~~~~~" << G4endl;
  G4cout << *(G4Material::GetMaterialTable()) << G4endl << G4endl;
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
  if(stepSizeG4>0)
    radiatorLogical->SetUserLimits(new G4UserLimits(stepSizeG4));
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
  //MDdetector
  //
  G4VSolid* mdDetectorSolid 
    = new G4Box("mdDetectorSol",  // its name
		SizeX/2, SizeY/2, detectorThickness/2); // its size
  
  G4LogicalVolume* mdDetectorLogical
    = new G4LogicalVolume(
			  mdDetectorSolid,     // its solid
			  detectorMaterial,  // its material
			  "mdDetectorLogical");// its name
  
  new G4PVPlacement(
		    0,                // no rotation
		    G4ThreeVector(0., 0., 5.*cm), 
		    mdDetectorLogical,  // its logical volume                    
		    "mdDetector",       // its name
		    worldLV,// its mother  volume
		    false,            // no boolean operation
		    0,                // copy number
		    fCheckOverlaps);  // checking overlaps 
  
  mdDetectorLogical->SetVisAttributes(detectorVisAtt);
  
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
		SizeX/2., SizeY/2., radiatorThickness/2.); // its size
  
  G4LogicalVolume* radiatorLogical
    = new G4LogicalVolume(
			  radiatorSol,    // its solid
			  radiatorMaterial, // its material
			  "radiatorLogical");  // its name

  // define step limitation for this container
  if(stepSizeG4>0)
    radiatorLogical->SetUserLimits(new G4UserLimits(stepSizeG4));
  // define step limitation for this container
  
  new G4PVPlacement(
		    0,                   // no rotation
		    G4ThreeVector(0., 0., radiatorThickness/2.), 
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
		    G4ThreeVector(0., 0., 5.*cm), 
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

  // Get materials
  G4Material* defaultMaterial  = G4Material::GetMaterial("Air");
  G4Material* radiatorMaterial = G4Material::GetMaterial("PBA");
  G4Material* frameMaterial    = G4Material::GetMaterial("Aluminum");
  G4Material* quartzMaterial   = G4Material::GetMaterial("Quartz");
  G4Material* leadMaterial     = G4Material::GetMaterial("Lead");
  G4Material* glueMaterial     = G4Material::GetMaterial("SiElast_Glue");
  G4Material* padMaterial      = G4Material::GetMaterial("Tyvek");
  G4Material* limeGlassMaterial= G4Material::GetMaterial("LimeGlass");
  G4Material* cathodeMaterial  = G4Material::GetMaterial("Photocathode");
  // placing the chamfers
  G4ThreeVector    Position_Chamfer1;
  G4RotationMatrix Rotation_Chamfer1;  
  G4ThreeVector    Position_Chamfer2;
  G4RotationMatrix Rotation_Chamfer2;  
  G4ThreeVector    Position_Chamfer3;
  G4RotationMatrix Rotation_Chamfer3;  
  G4ThreeVector    Position_Chamfer4;
  G4RotationMatrix Rotation_Chamfer4;  

  G4ThreeVector    Position_AngCut1;
  G4RotationMatrix Rotation_AngCut1;
  
  G4ThreeVector    Position_AngCut2;
  G4RotationMatrix Rotation_AngCut2;  

  G4ThreeVector    Position_LGRight;
  G4RotationMatrix Rotation_LGRight;  
  G4ThreeVector    Position_LGLeft;
  G4RotationMatrix Rotation_LGLeft;

  // needed for boolean union
  std::vector< G4SubtractionSolid* >  RightQuartz_Solid;
  std::vector< G4SubtractionSolid* >  LeftQuartz_Solid;
  std::vector< G4SubtractionSolid* >  LeftGuide_Solid;
  std::vector< G4SubtractionSolid* >  RightGuide_Solid;
  // clear vector containing temp solids for boolean soild union
  LeftQuartz_Solid.clear();
  LeftQuartz_Solid.resize(4);  //need 4 chamfers on quartz bar proper
  RightQuartz_Solid.clear();
  RightQuartz_Solid.resize(4); //need 4 chamfers on quartz bar proper
  LeftGuide_Solid.clear();
  LeftGuide_Solid.resize(5); //need 4 chamfers + 1 angle cut on light guide
  RightGuide_Solid.clear();
  RightGuide_Solid.resize(5);  //need 4 chamfers + 1 angle cut on light guide

  G4Colour  orange      (255/255., 127/255.,   0/255.);
  G4Colour  blue        (  0/255.,   0/255., 255/255.);
  G4Colour  magenta     (255/255.,   0/255., 255/255.);
  G4Colour  grey        (127/255., 127/255., 127/255.);
  G4Colour  lightblue   (139/255., 208/255., 255/255.);
  G4Colour  lightorange (255/255., 189/255., 165/255.);
  G4Colour  khaki3      (205/255., 198/255., 115/255.);
  G4Colour  brown       (178/255., 102/255.,  26/255.);
  G4Colour  darkbrown   (100/255.,  50/255.,  10/255.);  

  G4double QuartzBar_FullLength       =  100.00*cm;    // Full X length
  G4double QuartzBar_FullHeight       =   18.00*cm;    // Full Y length
  G4double QuartzBar_FullThickness    =    1.25*cm;    // Full Z length

  
  if ( !defaultMaterial || !radiatorMaterial || !frameMaterial || !quartzMaterial ||
       !leadMaterial || !glueMaterial || !padMaterial){
    G4cerr << __PRETTY_FUNCTION__ << " Cannot retrieve materials already defined. " << G4endl;
    G4cerr << "Exiting application " << G4endl;
    exit(1);
  }  
  
  //     
  // World
  //
  G4double worldSizeXY = 400 * cm;
  G4double worldSizeZ  =  50 * cm; 
  G4VSolid* worldS = new G4Box("World",           // its name
			       worldSizeXY, worldSizeXY, worldSizeZ); // its size
  
  G4LogicalVolume* worldLV = new G4LogicalVolume(
						 worldS,           // its solid
						 defaultMaterial,  // its material
						 "World");         // its name
  worldLV->SetVisAttributes (G4VisAttributes::Invisible);
  
  G4VPhysicalVolume* worldPV  = new G4PVPlacement(0,                // no rotation
						  G4ThreeVector(),  // at (0,0,0)
						  worldLV,          // its logical volume                         
						  "World",          // its name
						  0,                // its mother  volume
						  false,            // no boolean operation
						  0,                // copy number
						  fCheckOverlaps);  // checking overlaps

  static const G4double inch = 2.54*cm;
  G4double Container_Center_X         =  0.0*cm;
  G4double Container_Center_Y         =  0.0*cm;
  G4double Container_Center_Z         =  -4.25*cm;
  G4ThreeVector Position_CerenkovContainer=G4ThreeVector(0,0,0);
  G4ThreeVector Container_Center = G4ThreeVector(Container_Center_X, Container_Center_Y, Container_Center_Z);
  G4double Container_FullLength_X     =  99.8*inch;//95.6*inch;
  G4double Container_FullLength_Y     =  15.6*inch;//9.6*inch;
  G4double Container_FullLength_Z     =  14.0*inch+2.0*Container_Center_Z;

  G4Box* CerenkovContainer_Solid  = new G4Box("CerenkovContainer_Solid",
					      0.5 * Container_FullLength_X ,    // half X length required by Geant4
					      0.5 * Container_FullLength_Y ,    // half Y length required by Geant4
					      0.5 * Container_FullLength_Z );   // half Z length required by Geant4

  G4LogicalVolume* 
  CerenkovContainer_Logical  = new G4LogicalVolume(CerenkovContainer_Solid,
						   defaultMaterial,
						   "CerenkovContainer_Logical",
						   0,0,0);
  G4VisAttributes* CerenkovContainerVisAtt = new G4VisAttributes(blue);
  CerenkovContainerVisAtt->SetVisibility(false);
  //CerenkovContainerVisAtt->SetForceWireframe(true);
  //CerenkovContainerVisAtt->SetForceSolid(true);
  CerenkovContainer_Logical->SetVisAttributes(CerenkovContainerVisAtt);

  G4VPhysicalVolume*
    CerenkovContainer_Physical   = new G4PVPlacement(0,Position_CerenkovContainer + Container_Center,
						     CerenkovContainer_Logical,
						     "CerenkovContainer_Physical",
						     worldLV,
						     false,
						     0,
						     fCheckOverlaps);
  
  //********************************************************************************************
  //******************************Define Detector OuterFrame************************************

  G4double Frame_FullLength_X = 95.5*inch;
  G4double Frame_FullLength_Y = 9.5*inch;
  G4double Frame_FullLength_Z = 2.0*inch;
  G4double Frame_InnerFullLength_X = (95.5-0.75*2.0)*inch;
  G4double Frame_InnerFullLength_Y = (9.5-0.75*2.0)*inch;
  G4double Frame_InnerFullLength_Z = 2.0*inch+0.1*mm;  // a bit larger to avoid sharing surface with the outer which may cause vis-problem

  G4Box* OuterFrame  = new G4Box("OuterFrame",
				 0.5 * Frame_FullLength_X ,
				 0.5 * Frame_FullLength_Y ,
				 0.5 * Frame_FullLength_Z );
  
  G4Box* InnerFrame  = new G4Box("InnerFrame",
				 0.5 * Frame_InnerFullLength_X ,
				 0.5 * Frame_InnerFullLength_Y ,
				 0.5 * Frame_InnerFullLength_Z );
  
  G4SubtractionSolid* Frame_Solid = new G4SubtractionSolid("OuterFrame-InnerFrame", OuterFrame, InnerFrame);
  G4LogicalVolume* 
  Frame_Logical  = new G4LogicalVolume(Frame_Solid,
				       frameMaterial,
				       "Frame_Log",
				       0,0,0);
  G4VisAttributes* FrameVisAtt = new G4VisAttributes(grey);
  FrameVisAtt->SetVisibility(true);
  //FrameVisAtt->SetForceWireframe(true);
  //FrameVisAtt->SetForceSolid(true);
  Frame_Logical->SetVisAttributes(FrameVisAtt);
  
  G4ThreeVector Position_Frame  = G4ThreeVector(0,0,0.25*inch);  
  G4VPhysicalVolume*
  Frame_Physical   = new G4PVPlacement(0,Position_Frame + Container_Center,
				       Frame_Logical,
				       "Frame_Physical",
				       CerenkovContainer_Logical,
				       false,0,
				       fCheckOverlaps);

  //********************************************************************************************
  //******************************Define Side Bracket*******************************************

  G4double SideBracketLength_X = 3.0*inch;
  G4double SideBracketLength_Y = 0.75*inch;
  G4double SideBracketLength_Z = 1.75*inch;  
  G4double SideBracketCutLength_X = 3.0*inch+0.1*mm;
  G4double SideBracketCutLength_Y = 0.5*inch+0.1*mm;
  G4double SideBracketCutLength_Z = 0.75*inch;
  
  G4Box* OuterSideBracket  = new G4Box("OuterSideBracket",
				       0.5 * SideBracketLength_X,
				       0.5 * SideBracketLength_Y,
				       0.5 * SideBracketLength_Z);
  
  G4Box* InnerSideBracket  = new G4Box("InnerSideBracket",
				       0.5 * SideBracketCutLength_X,
				       0.5 * SideBracketCutLength_Y,
				       0.5 * SideBracketCutLength_Z);
  
  G4RotationMatrix* cutRot = new G4RotationMatrix(0,0,0);
  G4ThreeVector cutTrans(0, 0.125*inch, -0.25*inch);
  
  G4SubtractionSolid* SideBracket_Solid = new G4SubtractionSolid("OuterSideBracket-InnerSideBracket",
								 OuterSideBracket,
								 InnerSideBracket,
								 cutRot,
								 cutTrans);
  
  G4LogicalVolume* 
  SideBracket_Logical  = new G4LogicalVolume(SideBracket_Solid,
					     frameMaterial,
					     "SideBracket_Log",
					     0,0,0);
  G4VisAttributes* SideBracketVisAtt = new G4VisAttributes(blue);
  SideBracketVisAtt->SetVisibility(true);
  //SideBracketVisAtt->SetForceWireframe(true);
  //SideBracketVisAtt->SetForceSolid(true);
  SideBracket_Logical->SetVisAttributes(SideBracketVisAtt);
  
  std::vector< G4RotationMatrix* > Rotation_SideBracket;
  Rotation_SideBracket.clear();
  for (G4int i=0; i<12; i++) {
    Rotation_SideBracket.push_back(new G4RotationMatrix(0,0,0));
    if (i>=6)     Rotation_SideBracket[i]->rotateZ(180.0*degree);
  }
  
  std::vector< G4ThreeVector > Position_SideBracket;
  Position_SideBracket.clear();
  Position_SideBracket.push_back (G4ThreeVector(3.0*inch,-3.625*inch,0.25*inch) );
  Position_SideBracket.push_back (G4ThreeVector(-3.0*inch,-3.625*inch,0.25*inch) );
  Position_SideBracket.push_back (G4ThreeVector(35.37*inch,-3.625*inch,0.25*inch) );
  Position_SideBracket.push_back (G4ThreeVector(-35.37*inch,-3.625*inch,0.25*inch) );
  Position_SideBracket.push_back (G4ThreeVector(43.62*inch,-3.625*inch,0.25*inch) );
  Position_SideBracket.push_back (G4ThreeVector(-43.62*inch,-3.625*inch,0.25*inch) );
  Position_SideBracket.push_back (G4ThreeVector(3.0*inch,3.625*inch,0.25*inch) );
  Position_SideBracket.push_back (G4ThreeVector(-3.0*inch,3.625*inch,0.25*inch) );
  Position_SideBracket.push_back (G4ThreeVector(35.37*inch,3.625*inch,0.25*inch) );
  Position_SideBracket.push_back (G4ThreeVector(-35.37*inch,3.625*inch,0.25*inch) );
  Position_SideBracket.push_back (G4ThreeVector(43.62*inch,3.625*inch,0.25*inch) );
  Position_SideBracket.push_back (G4ThreeVector(-43.62*inch,3.625*inch,0.25*inch) );
  
  std::vector< G4VPhysicalVolume* >  SideBracket_Physical;
  for (G4int i = 0; i<12; i++) {
    SideBracket_Physical.push_back( new G4PVPlacement(Rotation_SideBracket.at(i),Position_SideBracket.at(i) + Container_Center,
						      SideBracket_Logical,
						      "SideBracket_Physical",
						      CerenkovContainer_Logical,
						      false,i,
						      fCheckOverlaps));    
  }
    
  //************************************************************************************************
  //******************************Define Side Bracket Pad*******************************************

  G4double SideBracketPadLength_X = SideBracketLength_X;
  G4double SideBracketPadLength_Y = SideBracketCutLength_Y-0.2*mm;
  G4double SideBracketPadLength_Z = SideBracketCutLength_Z-0.2*mm;
  G4double SideBracketPadCutLength_X = SideBracketCutLength_X;
  G4double SideBracketPadCutLength_Y = SideBracketCutLength_Y-5.0*mm;
  G4double SideBracketPadCutLength_Z = QuartzBar_FullThickness;

  G4Box* OuterSideBracketPad  = new G4Box("OuterSideBracketPad",
					  0.5 * SideBracketPadLength_X,
					  0.5 * SideBracketPadLength_Y,
					  0.5 * SideBracketPadLength_Z);
  
  G4Box* InnerSideBracketPad  = new G4Box("InnerSideBracketPad",
					  0.5 * SideBracketPadCutLength_X,
					  0.5 * SideBracketPadCutLength_Y,
					  0.5 * SideBracketPadCutLength_Z);
  
  G4RotationMatrix* PadCutRot = new G4RotationMatrix(0,0,0);
  G4ThreeVector PadCutTrans(0., 0.11*inch, 0.);

  G4SubtractionSolid* SideBracketPad_Solid = new G4SubtractionSolid("OuterSideBracketPad-InnerSideBracketPad",
								    OuterSideBracketPad,
								    InnerSideBracketPad,
								    PadCutRot,
								    PadCutTrans);
  
  G4LogicalVolume* 
  SideBracketPad_Logical  = new G4LogicalVolume(SideBracketPad_Solid,
						padMaterial,
						"SideBracketPad_Log",
						0,0,0);
  G4VisAttributes* SideBracketPadVisAtt = new G4VisAttributes(brown);
  SideBracketPadVisAtt->SetVisibility(true);
  //SideBracketPadVisAtt->SetForceWireframe(true);
  SideBracketPadVisAtt->SetForceSolid(true);
  SideBracketPad_Logical->SetVisAttributes(SideBracketPadVisAtt);
  
  std::vector< G4RotationMatrix* > Rotation_SideBracketPad;
  for (G4int i=0; i<12; i++) {
    Rotation_SideBracketPad.push_back(new G4RotationMatrix(0,0,0));
    if (i>=6)     Rotation_SideBracketPad[i]->rotateZ(180.0*degree);
  }
  
  std::vector< G4ThreeVector > Position_SideBracketPad;
  Position_SideBracketPad.push_back (G4ThreeVector(3.0*inch,-3.5*inch,0.) );
  Position_SideBracketPad.push_back (G4ThreeVector(-3.0*inch,-3.5*inch,0.) );
  Position_SideBracketPad.push_back (G4ThreeVector(35.37*inch,-3.5*inch,0.) );
  Position_SideBracketPad.push_back (G4ThreeVector(-35.37*inch,-3.5*inch,0.) );
  Position_SideBracketPad.push_back (G4ThreeVector(43.62*inch,-3.5*inch,0.) );
  Position_SideBracketPad.push_back (G4ThreeVector(-43.62*inch,-3.5*inch,0.) );
  Position_SideBracketPad.push_back (G4ThreeVector(3.0*inch,3.5*inch,0.) );
  Position_SideBracketPad.push_back (G4ThreeVector(-3.0*inch,3.5*inch,0.) );
  Position_SideBracketPad.push_back (G4ThreeVector(35.37*inch,3.5*inch,0.) );
  Position_SideBracketPad.push_back (G4ThreeVector(-35.37*inch,3.5*inch,0.) );
  Position_SideBracketPad.push_back (G4ThreeVector(43.62*inch,3.5*inch,0.) );
  Position_SideBracketPad.push_back (G4ThreeVector(-43.62*inch,3.5*inch,0.) );
  
  
 std::vector< G4VPhysicalVolume* >  SideBracketPad_Physical;
 for (G4int i = 0; i<12; i++) {
    SideBracketPad_Physical.push_back( new G4PVPlacement(Rotation_SideBracketPad.at(i),Position_SideBracketPad.at(i) + Container_Center,
							 SideBracketPad_Logical,
							 "SideBracketPad_Physical",
							 CerenkovContainer_Logical,
							 false,i,
							 fCheckOverlaps));    
  }
  
  //********************************************************************************************
  //******************************Define End Bracket *******************************************

  G4double EndBracketLength_X = 1.5*inch;
  G4double EndBracketLength_Y = 0.75*inch;
  G4double EndBracketLength_Z = 1.75*inch;
  G4double EndBracketCutLength_X = 1.5*inch+0.1*mm;
  G4double EndBracketCutLength_Y = 0.5*inch+0.1*mm;
  G4double EndBracketCutLength_Z = 0.75*inch;

  G4Box* OuterEndBracket  = new G4Box("OuterEndBracket",
				      0.5 * EndBracketLength_X,
				      0.5 * EndBracketLength_Y,
				      0.5 * EndBracketLength_Z);
  
  G4Box* InnerEndBracket  = new G4Box("InnerEndBracket",
				      0.5 * EndBracketCutLength_X,
				      0.5 * EndBracketCutLength_Y,
				      0.5 * EndBracketCutLength_Z);
    
  G4SubtractionSolid* EndBracket_Solid = new G4SubtractionSolid("OuterEndBracket-InnerEndBracket",
								OuterEndBracket,
								InnerEndBracket,
								cutRot,
								cutTrans);
  
  G4LogicalVolume* 
  EndBracket_Logical  = new G4LogicalVolume(EndBracket_Solid,
					    frameMaterial,
					    "EndBracket_Log",
					    0,0,0);
  G4VisAttributes* EndBracketVisAtt = new G4VisAttributes(blue);
  EndBracketVisAtt->SetVisibility(true);
  //EndBracketVisAtt->SetForceWireframe(true);
  //EndBracketVisAtt->SetForceSolid(true);
  EndBracket_Logical->SetVisAttributes(EndBracketVisAtt);
  
  std::vector< G4RotationMatrix* > Rotation_EndBracket;
  for (G4int i=0; i<4; i++) {
    Rotation_EndBracket.push_back(new G4RotationMatrix(0,0,0));
    if (i<2)     Rotation_EndBracket[i]->rotateZ(-90.0*degree);
    else         Rotation_EndBracket[i]->rotateZ(90.0*degree);
  }
  
  G4double Distance_EndBracketToBarCenter = Frame_InnerFullLength_X*0.5 - 0.375*inch;
  std::vector< G4ThreeVector > Position_EndBracket;
  Position_EndBracket.push_back (G4ThreeVector(Distance_EndBracketToBarCenter,3.0*inch,0.25*inch) );
  Position_EndBracket.push_back (G4ThreeVector(Distance_EndBracketToBarCenter,-3.0*inch,0.25*inch) );
  Position_EndBracket.push_back (G4ThreeVector(-Distance_EndBracketToBarCenter,3.0*inch,0.25*inch) );
  Position_EndBracket.push_back (G4ThreeVector(-Distance_EndBracketToBarCenter,-3.0*inch,0.25*inch) );

  std::vector< G4VPhysicalVolume* >  EndBracket_Physical;
  for (G4int i = 0; i<4; i++) {
    EndBracket_Physical.push_back( new G4PVPlacement(Rotation_EndBracket.at(i),Position_EndBracket.at(i) + Container_Center,
						     EndBracket_Logical,
						     "EndBracket_Physical",
						     CerenkovContainer_Logical,
						     false,i,
						     fCheckOverlaps));    
    }

    
  //***********************************************************************************************
  //******************************Define End Bracket Pad*******************************************

  G4double EndBracketPadLength_X = EndBracketLength_X;
  G4double EndBracketPadLength_Y = EndBracketCutLength_Y-0.1*mm;
  G4double EndBracketPadLength_Z = EndBracketCutLength_Z-0.1*mm;
  G4double EndBracketPadCutLength_X = EndBracketPadLength_X+0.1*mm;
  G4double EndBracketPadCutLength_Y = EndBracketCutLength_Y-5.0*mm;
  G4double EndBracketPadCutLength_Z = QuartzBar_FullThickness;;

  G4Box* OuterEndBracketPad  = new G4Box("OuterEndBracketPad",
					 0.5 * EndBracketPadLength_X,
					 0.5 * EndBracketPadLength_Y,
					 0.5 * EndBracketPadLength_Z);
  
  G4Box* InnerEndBracketPad  = new G4Box("InnerEndBracketPad",
					 0.5 * EndBracketPadCutLength_X,
					 0.5 * EndBracketPadCutLength_Y,
					 0.5 * EndBracketPadCutLength_Z);
  

  G4ThreeVector EndPadCutTrans(0., 0.19*inch, 0.);  
  G4SubtractionSolid* EndBracketPad_Solid = new G4SubtractionSolid("OuterEndBracketPad-InnerEndBracketPad",
								   OuterEndBracketPad,
								   InnerEndBracketPad,
								   PadCutRot,
								   EndPadCutTrans);
  
  G4LogicalVolume* 
  EndBracketPad_Logical  = new G4LogicalVolume(EndBracketPad_Solid,
					       padMaterial,
					       "EndBracketPad_Log",
					       0,0,0);
  G4VisAttributes* EndBracketPadVisAtt = new G4VisAttributes(brown);
  EndBracketPadVisAtt->SetVisibility(true);
  //EndBracketPadVisAtt->SetForceWireframe(true);
  EndBracketPadVisAtt->SetForceSolid(true);
  EndBracketPad_Logical->SetVisAttributes(EndBracketPadVisAtt);

  std::vector< G4RotationMatrix* > Rotation_EndBracketPad;
  for (G4int i=0; i<4; i++) {
    Rotation_EndBracketPad.push_back(new G4RotationMatrix(0,0,0));
    if (i<2)     Rotation_EndBracketPad[i]->rotateZ(-90.0*degree);
    else         Rotation_EndBracketPad[i]->rotateZ(90.0*degree);
  }
  
  G4double Distance_EndBracketPadToBarCenter = Frame_InnerFullLength_X*0.5 - 0.5*inch;
  std::vector< G4ThreeVector > Position_EndBracketPad;
  Position_EndBracketPad.push_back (G4ThreeVector(Distance_EndBracketPadToBarCenter,3.0*inch,0.) );
  Position_EndBracketPad.push_back (G4ThreeVector(Distance_EndBracketPadToBarCenter,-3.0*inch,0.) );
  Position_EndBracketPad.push_back (G4ThreeVector(-Distance_EndBracketPadToBarCenter,3.0*inch,0.) );
  Position_EndBracketPad.push_back (G4ThreeVector(-Distance_EndBracketPadToBarCenter,-3.0*inch,0.) );

  std::vector< G4VPhysicalVolume* >  EndBracketPad_Physical;
  for (G4int i = 0; i<4; i++) {
    EndBracketPad_Physical.push_back( new G4PVPlacement(Rotation_EndBracketPad.at(i),Position_EndBracketPad.at(i) + Container_Center,
							EndBracketPad_Logical,
							"EndBracketPad_Physical",
							CerenkovContainer_Logical,
							false,i,
							fCheckOverlaps));
  }

  //**********************************************************************************************
  //******************************Define the Cross Bar *******************************************

  G4double CrossBar_FullLength_X = 1.5*inch;
  G4double CrossBar_FullLength_Y = Frame_FullLength_Y - 0.75*2.0*inch;
  G4double CrossBar_FullLength_Z = 0.75*inch;
  
  G4Box* CrossBar_Solid  = new G4Box("CrossBar_Solid",
				     0.5 * CrossBar_FullLength_X ,
				     0.5 * CrossBar_FullLength_Y ,
				     0.5 * CrossBar_FullLength_Z );
  
  G4LogicalVolume* 
  CrossBar_Logical  = new G4LogicalVolume(CrossBar_Solid,
					  frameMaterial,
					  "CrossBar_Log",
					  0,0,0);
  G4VisAttributes* CrossBarVisAtt = new G4VisAttributes(blue);
  CrossBarVisAtt->SetVisibility(true);
  //CrossBarVisAtt->SetForceWireframe(true);
  CrossBarVisAtt->SetForceSolid(true);
  CrossBar_Logical->SetVisAttributes(CrossBarVisAtt);
  
  G4ThreeVector Position_CrossBar_R  = G4ThreeVector((47.75-8.25)*inch,0,0.25*inch+0.625*inch);
  G4ThreeVector Position_CrossBar_L  = G4ThreeVector(-(47.75-8.25)*inch,0,0.25*inch+0.625*inch);
  
  G4VPhysicalVolume*
  CrossBarR_Physical   = new G4PVPlacement(0,Position_CrossBar_R + Container_Center,
					   CrossBar_Logical,
					   "CrossBarR_Physical",
					   CerenkovContainer_Logical,
					   false,0,
					   fCheckOverlaps);
  
  G4VPhysicalVolume*
  CrossBarL_Physical   = new G4PVPlacement(0,Position_CrossBar_L + Container_Center,
					   CrossBar_Logical,
					   "CrossBarL_Physical",
					   CerenkovContainer_Logical,
					   false,0,
					   fCheckOverlaps);

  //*************************************************************************************************
  //******************************Define Detector Windows *******************************************

  G4double WindowThickness = 5.0*mm;

  G4Box* FrontWindow_Solid  = new G4Box("FrontWindow_Solid",
					0.5 * Frame_FullLength_X ,
					0.5 * Frame_FullLength_Y ,
					0.5 * WindowThickness );
  
  G4LogicalVolume* 
  FrontWindow_Logical  = new G4LogicalVolume(FrontWindow_Solid,
					     padMaterial,
					     "FrontWindow_Log",
					     0,0,0);
  G4VisAttributes* WindowVisAtt = new G4VisAttributes(blue);
  WindowVisAtt->SetVisibility(true);
  //FrontWindowVisAtt->SetForceWireframe(true);
  WindowVisAtt->SetForceSolid(true);
  FrontWindow_Logical->SetVisAttributes(WindowVisAtt);
  
  G4ThreeVector Position_FrontWindow  = G4ThreeVector(0,0,0.25*inch-(1.0*inch+2.5*mm));
  
  G4VPhysicalVolume*
  FrontWindow_Physical   = new G4PVPlacement(0,Position_FrontWindow + Container_Center,
					     FrontWindow_Logical,
					     "FrontWindow_Physical",
					     CerenkovContainer_Logical,
					     false,0,
					     fCheckOverlaps);
  
  
  G4Box* BackWindow_Solid  = new G4Box("BackWindow_Solid",
				       0.5 * Frame_FullLength_X - 8.25*inch,
				       0.5 * Frame_FullLength_Y ,
				       0.5 * WindowThickness );
  
  G4LogicalVolume* 
  BackWindow_Logical  = new G4LogicalVolume(BackWindow_Solid,
					    padMaterial,
					    "BackWindow_Log",
					    0,0,0);
  BackWindow_Logical->SetVisAttributes(WindowVisAtt);
  
  G4ThreeVector Position_BackWindow  = G4ThreeVector(0,0,0.25*inch+1.0*inch+2.5*mm);
  G4VPhysicalVolume*
  BackWindow_Physical   = new G4PVPlacement(0,Position_BackWindow + Container_Center,
					    BackWindow_Logical,
					    "BackWindow_Physical",
					    CerenkovContainer_Logical,
					    false,0,
					    fCheckOverlaps);
  
  //*************************************************************************************************
  //******************************Define Front Window Clip*******************************************

  G4double FrontClip_FullLength_X = 95.5*inch;
  G4double FrontClip_FullLength_Y = 9.5*inch;
  G4double FrontClip_FullLength_Z = 0.38*inch;
  G4double FrontClip_InnerFullLength_X = (95.5-0.75*2.0)*inch;
  G4double FrontClip_InnerFullLength_Y = (9.5-0.75*2.0)*inch;
  G4double FrontClip_InnerFullLength_Z = 0.38*inch+1.0*mm;  // a bit larger to avoid sharing surface with the outer which may cause vis-problem
  
  G4Box* FrontClip_Outer  = new G4Box("FrontClip_Outer",
				      0.5 * FrontClip_FullLength_X ,
				      0.5 * FrontClip_FullLength_Y ,
				      0.5 * FrontClip_FullLength_Z );
  
  G4Box* FrontClip_Inner  = new G4Box("FrontClip_Inner",
				      0.5 * FrontClip_InnerFullLength_X ,
				      0.5 * FrontClip_InnerFullLength_Y ,
				      0.5 * FrontClip_InnerFullLength_Z );
  
  G4SubtractionSolid* FrontClip_Solid = new G4SubtractionSolid("FrontClip_Outer-FrontClip_Inner",
							       FrontClip_Outer,
							       FrontClip_Inner);
  
  G4LogicalVolume* 
  FrontClip_Logical  = new G4LogicalVolume(FrontClip_Solid,
					   frameMaterial,
					   "FrontClip_Log",
					   0,0,0);
  G4VisAttributes* ClipVisAtt = new G4VisAttributes(lightorange);
  ClipVisAtt->SetVisibility(true);
  //ClipVisAtt->SetForceWireframe(true);
  //ClipVisAtt->SetForceSolid(true);
  FrontClip_Logical->SetVisAttributes(ClipVisAtt);
  
  G4ThreeVector Position_FrontClip  = G4ThreeVector(0,0,0.25*inch -(1.0*inch+5.0*mm+0.38/2.0*inch));  
  G4VPhysicalVolume*
  FrontClip_Physical   = new G4PVPlacement(0,Position_FrontClip + Container_Center,
					   FrontClip_Logical,
					   "FrontClip_Physical",
					   CerenkovContainer_Logical,
					   false,0,
					   fCheckOverlaps);

  
  //************************************************************************************************
  //******************************Define Back Window Clip*******************************************

  G4double BackClip_FullLength_X = 95.5*inch - 8.25*2.0*inch;
  G4double BackClip_FullLength_Y = 9.5*inch;
  G4double BackClip_FullLength_Z = 0.38*inch;  
  G4double BackClip_InnerFullLength_X = 95.5*inch - 9.0*2.0*inch;
  G4double BackClip_InnerFullLength_Y = (9.5-0.75*2.0)*inch;
  G4double BackClip_InnerFullLength_Z = 0.38*inch+0.1*mm;  // a bit larger to avoid sharing surface with the outer which may cause vis-problem

  G4Box* BackClip_Outer  = new G4Box("BackClip_Outer",
				     0.5 * BackClip_FullLength_X ,
				     0.5 * BackClip_FullLength_Y ,
				     0.5 * BackClip_FullLength_Z );  

  G4Box* BackClip_Inner  = new G4Box("BackClip_Inner",
				     0.5 * BackClip_InnerFullLength_X ,
				     0.5 * BackClip_InnerFullLength_Y ,
				     0.5 * BackClip_InnerFullLength_Z );  

  G4SubtractionSolid* BackClip_Solid = new G4SubtractionSolid("BackClip_Outer-BackClip_Inner",
							      BackClip_Outer,
							      BackClip_Inner);

  G4LogicalVolume* 
  BackClip_Logical  = new G4LogicalVolume(BackClip_Solid,
					  frameMaterial,
					  "BackClip_Log",
					  0,0,0);
  BackClip_Logical->SetVisAttributes(ClipVisAtt);
  
  G4ThreeVector Position_BackClip  = G4ThreeVector(0,0,0.25*inch + 1.0*inch+5.0*mm+0.38/2.0*inch);
  
  G4VPhysicalVolume*
  BackClip_Physical   = new G4PVPlacement(0,Position_BackClip + Container_Center,
					  BackClip_Logical,
					  "BackClip_Physical",
					  CerenkovContainer_Logical,
					  false,0,
					  fCheckOverlaps);

  //***************************************************************************************************
  //******************************Define Square Falange Seal*******************************************

  G4double SquareFalangeSeal_FullLength_X = 8.21*inch;
  G4double SquareFalangeSeal_FullLength_Y = 9.50*inch;
  G4double SquareFalangeSeal_FullLength_Z = 5.0*mm;
  
  G4Box* SquareSealOuter_Solid  = new G4Box("SquareSealOuter_Solid",
					    0.5 * SquareFalangeSeal_FullLength_X,
					    0.5 * SquareFalangeSeal_FullLength_Y,
					    0.5 * SquareFalangeSeal_FullLength_Z);
  
  G4Box* SquareSealInner_Solid  = new G4Box("SquareSealInner_Solid",
					    0.5 * SquareFalangeSeal_FullLength_X - 0.75*inch,
					    0.5 * SquareFalangeSeal_FullLength_Y - 0.75*inch,
					    0.5 * SquareFalangeSeal_FullLength_Z + 0.1*mm);
  
  G4SubtractionSolid* SquareFalangeSeal_Solid
    = new G4SubtractionSolid("SquareSealOuter_Solid-SquareSealInner_Solid",
                             SquareSealOuter_Solid,
                             SquareSealInner_Solid);
  
  G4LogicalVolume* 
  SquareFalangeSeal_Logical  = new G4LogicalVolume(SquareFalangeSeal_Solid,
						   padMaterial,
						   "SquareFalangeSeal_Log",
						   0,0,0);
  G4VisAttributes* SquareFalangeSealVisAtt = new G4VisAttributes(lightorange);
  SquareFalangeSealVisAtt->SetVisibility(true);
  //SquareFalangeSealVisAtt->SetForceWireframe(true);
  //SquareFalangeSealVisAtt->SetForceSolid(true);
  SquareFalangeSeal_Logical->SetVisAttributes(SquareFalangeSealVisAtt);
  
  G4ThreeVector Position_SquareFalangeSeal_R = G4ThreeVector((47.75-8.21/2.0)*inch,
							     0,
							     0.25*inch + 1.0*inch+2.5*mm);
  
  G4ThreeVector Position_SquareFalangeSeal_L = G4ThreeVector(-(47.75-8.21/2.0)*inch,
							     0,
							     0.25*inch + 1.0*inch+2.5*mm);
  
  G4VPhysicalVolume*
  SquareFalangeSealR_Physical   = new G4PVPlacement(0,Position_SquareFalangeSeal_R + Container_Center,
						    SquareFalangeSeal_Logical,
						    "SquareFalangeSealR_Physical",
						    CerenkovContainer_Logical,
						    false,0,
						    fCheckOverlaps);
  
  G4VPhysicalVolume*
  SquareFalangeSealL_Physical   = new G4PVPlacement(0,Position_SquareFalangeSeal_L + Container_Center,
						    SquareFalangeSeal_Logical,
						    "SquareFalangeSealL_Physical",
						    CerenkovContainer_Logical,
						    false,0,
						    fCheckOverlaps);
  
  //***********************************************************************************************
  //******************************Define Square Falange *******************************************

  G4double SquareFalange_FullLength_X = 8.21*inch;
  G4double SquareFalange_FullLength_Y = 9.50*inch;
  G4double SquareFalange_FullLength_Z = 0.38*inch;  
  G4double CutOuterRadius = 5.75*0.5*inch;
  G4double CutInnerRadius = 0.0*inch;
  G4double CutThickness = 0.38*inch+0.1*mm;
  
  G4Box* SquareBase_Solid  = new G4Box("SquareBase_Solid",
				       0.5 * SquareFalange_FullLength_X,
				       0.5 * SquareFalange_FullLength_Y,
				       0.5 * SquareFalange_FullLength_Z);
  
  G4Tubs* CylinderCut_Solid = new G4Tubs("CylinderCut_Solid",
					 CutInnerRadius,
					 CutOuterRadius,
					 0.5 * CutThickness,
					 0.0, 360.0*degree);
  
  G4SubtractionSolid* SquareFalange_Solid = new G4SubtractionSolid("SquareBase_Solid-CylinderCut_Solid",
								   SquareBase_Solid,
								   CylinderCut_Solid);
  
  G4LogicalVolume* 
  SquareFalange_Logical  = new G4LogicalVolume(SquareFalange_Solid,
					       frameMaterial,
					       "SquareFalange_Log",
					       0,0,0);
  SquareFalange_Logical->SetVisAttributes(SquareFalangeSealVisAtt);  
  
  G4ThreeVector Position_SquareFalange_R = G4ThreeVector((47.75-8.21/2.0)*inch,
							 0,
							 0.25*inch + 1.0*inch+5.0*mm+0.38/2.0*inch);
  
  G4ThreeVector Position_SquareFalange_L = G4ThreeVector(-(47.75-8.21/2.0)*inch,
							 0,
							 0.25*inch + 1.0*inch+5.0*mm+0.38/2.0*inch);
  
  G4VPhysicalVolume*
  SquareFalangeR_Physical   = new G4PVPlacement(0,Position_SquareFalange_R + Container_Center,
						SquareFalange_Logical,
						"SquareFalangeR_Physical",
						CerenkovContainer_Logical,
						false,0,
						fCheckOverlaps);
  
  G4VPhysicalVolume*
  SquareFalangeL_Physical   = new G4PVPlacement(0,Position_SquareFalange_L + Container_Center,
						SquareFalange_Logical,
						"SquareFalangeL_Physical",
						CerenkovContainer_Logical,
						false,0,
						fCheckOverlaps);

  
  //*************************************************************************************************  
  //******************************Define PMT Housing Wall *******************************************

  G4double PMTHousingWallOuterRadius = 6.63*0.5*inch;
  G4double PMTHousingWallInnerRadius = 6.07*0.5*inch;
  G4double PMTHousingWallLength = 3.53*inch;
  
  G4Tubs* PMTHousingWall_Solid = new G4Tubs("PMTHousingWall_Solid",
					    PMTHousingWallInnerRadius,
					    PMTHousingWallOuterRadius,
					    0.5 * PMTHousingWallLength,
					    0.0, 360.0*degree);
  
  G4LogicalVolume* 
  PMTHousingWall_Logical  = new G4LogicalVolume(PMTHousingWall_Solid,
						frameMaterial,
						"PMTHousingWall_Log",
						0,0,0);
  G4VisAttributes* PMTHousingWallVisAtt = new G4VisAttributes(lightorange);
  PMTHousingWallVisAtt->SetVisibility(true);
  //PMTHousingWallVisAtt->SetForceWireframe(true);
  //PMTHousingWallVisAtt->SetForceSolid(true);
  PMTHousingWall_Logical->SetVisAttributes(PMTHousingWallVisAtt);
  
  G4ThreeVector Position_PMTHousingWall_R = G4ThreeVector((47.75-8.21/2.0)*inch,
							  0,
							  0.25*inch + 1.0*inch+5.0*mm+0.38*inch+PMTHousingWallLength*0.5);
  
  G4ThreeVector Position_PMTHousingWall_L = G4ThreeVector(-(47.75-8.21/2.0)*inch,
							  0,
							  0.25*inch + 1.0*inch+5.0*mm+0.38*inch+PMTHousingWallLength*0.5);
  
  G4VPhysicalVolume*
  PMTHousingWallR_Physical   = new G4PVPlacement(0,Position_PMTHousingWall_R + Container_Center,
						 PMTHousingWall_Logical,
						 "PMTHousingWallR_Physical",
						 CerenkovContainer_Logical,
						 false,0,
						 fCheckOverlaps);
  
  G4VPhysicalVolume*
  PMTHousingWallL_Physical   = new G4PVPlacement(0,Position_PMTHousingWall_L + Container_Center,
						 PMTHousingWall_Logical,
						 "PMTHousingWallL_Physical",
						 CerenkovContainer_Logical,
						 false,0,
						 fCheckOverlaps);
  
  //************************************************************************************************
  //******************************Define PMT Housing Falange ***************************************
  
  G4double PMTHousingFalangeOuterRadius = 7.88*0.5*inch;
  G4double PMTHousingFalangeInnerRadius = 5.75*0.5*inch;
  G4double PMTHousingFalangeLength = 0.75*inch;
  
  G4Tubs* PMTHousingFalange_Solid = new G4Tubs("PMTHousingFalange_Solid",
					       PMTHousingFalangeInnerRadius,
					       PMTHousingFalangeOuterRadius,
					       0.5 * PMTHousingFalangeLength,
					       0.0, 360.0*degree);
  
  G4LogicalVolume* 
  PMTHousingFalange_Logical  = new G4LogicalVolume(PMTHousingFalange_Solid,
						   frameMaterial,
						   "PMTHousingFalange_Log",
						   0,0,0);
  PMTHousingFalange_Logical->SetVisAttributes(PMTHousingWallVisAtt);

  G4ThreeVector Position_PMTHousingFalange_R = G4ThreeVector((47.75-8.21/2.0)*inch,
							     0,
							     0.25*inch + 1.0*inch+5.0*mm+0.38*inch+PMTHousingWallLength+PMTHousingFalangeLength*0.5);
  
  G4ThreeVector Position_PMTHousingFalange_L = G4ThreeVector(-(47.75-8.21/2.0)*inch,
							     0,
							     0.25*inch + 1.0*inch+5.0*mm+0.38*inch+PMTHousingWallLength+PMTHousingFalangeLength*0.5);
  
  G4VPhysicalVolume*
  PMTHousingFalangeR_Physical   = new G4PVPlacement(0,Position_PMTHousingFalange_R + Container_Center,
						    PMTHousingFalange_Logical,
						    "PMTHousingFalangeR_Physical",
						    CerenkovContainer_Logical,
						    false,0,
						    fCheckOverlaps);
  
  G4VPhysicalVolume*
  PMTHousingFalangeL_Physical   = new G4PVPlacement(0,Position_PMTHousingFalange_L + Container_Center,
						    PMTHousingFalange_Logical,
						    "PMTHousingFalangeL_Physical",
						    CerenkovContainer_Logical,
						    false,0,
						    fCheckOverlaps);

  
  //************************************************************************************************
  //******************************Define PMT Housing Lid *******************************************
  
  G4double phiStart = 0.0;
  G4double phiTotal = 360*degree;
  G4int    numZPlanes = 9;
  G4double rInner[9] = { 6.5/2.0*inch, 6.5/2.0*inch, 2.52/2.0*inch, 2.52/2.0*inch, 2.52/2.0*inch, 2.52/2.0*inch, 2.52/2.0*inch, 2.52/2.0*inch, 2.52/2.0*inch};
  G4double rOuter[9] = { 7.88/2.0*inch, 7.88/2.0*inch, 7.88/2.0*inch, 7.88/2.0*inch, 2.75/2.0*inch, 2.75/2.0*inch, 3.0/2.0*inch, 3.0/2.0*inch, 2.99/2.0*inch,};
  G4double zPlane[9] = { 0.0, 0.37*inch, 0.38*inch, 0.39*inch, 0.5*inch, 0.69*inch, 0.70*inch, 0.85*inch, 0.86*inch };
  
  G4Polycone* PMTHousingLid_Solid = new G4Polycone("PMTHousingLid_Soild",
						   phiStart,
						   phiTotal,
						   numZPlanes,
						   zPlane,
						   rInner,
						   rOuter);
  
  G4LogicalVolume* 
  PMTHousingLid_Logical  = new G4LogicalVolume(PMTHousingLid_Solid,
					       frameMaterial,
					       "PMTHousingLid_Log",
					       0,0,0);
  PMTHousingLid_Logical->SetVisAttributes(PMTHousingWallVisAtt);

  G4ThreeVector Position_PMTHousingLid_R = G4ThreeVector( (47.75-8.21/2.0)*inch,
							  0,
							  0.25*inch+1.0*inch+5.0*mm+0.38*inch+PMTHousingWallLength+PMTHousingFalangeLength);
  
  G4ThreeVector Position_PMTHousingLid_L = G4ThreeVector( -(47.75-8.21/2.0)*inch,
							  0,
							  0.25*inch+1.0*inch+5.0*mm+0.38*inch+PMTHousingWallLength+PMTHousingFalangeLength);
  
  G4VPhysicalVolume*
  PMTHousingLidR_Physical   = new G4PVPlacement(0,Position_PMTHousingLid_R + Container_Center,
						PMTHousingLid_Logical,
						"PMTHousingLidR_Physical",
						CerenkovContainer_Logical,
						false,0,
						fCheckOverlaps);
  
  G4VPhysicalVolume*
  PMTHousingLidL_Physical   = new G4PVPlacement(0,Position_PMTHousingLid_L + Container_Center,
						PMTHousingLid_Logical,
						"PMTHousingLidL_Physical",
						CerenkovContainer_Logical,
						false,0,
						fCheckOverlaps);

  //************************************************************************************************
  //******************************Define Exo-Skelton Frame******************************************
  
  G4double ExoSkeltonFrame_X = Frame_FullLength_X+0.2*cm;
  G4double ExoSkeltonFrame_Y = Frame_FullLength_Y+6.0*inch;
  G4double ExoSkeltonFrame_Z = 3.0*inch;  
  G4double ExoSkeltonInnerFrame_X = ExoSkeltonFrame_X+0.2*mm;
  G4double ExoSkeltonInnerFrame_Y = Frame_FullLength_Y+0.2*cm;
  G4double ExoSkeltonInnerFrame_Z = ExoSkeltonFrame_Z+0.1*mm;  // a bit larger to avoid sharing surface with the outer which may cause vis-problem

  G4Box* ExoSkeltonOuterFrame  = new G4Box("ExoSkeltonOuterFrame",
					   0.5 * ExoSkeltonFrame_X ,
					   0.5 * ExoSkeltonFrame_Y ,
					   0.5 * ExoSkeltonFrame_Z );
  
  G4Box* ExoSkeltonInnerFrame  = new G4Box("ExoSkeltonInnerFrame",
					   0.5 * ExoSkeltonInnerFrame_X ,
					   0.5 * ExoSkeltonInnerFrame_Y ,
					   0.5 * ExoSkeltonInnerFrame_Z );
  
  G4SubtractionSolid* ExoSkeltonFrame_Solid = new G4SubtractionSolid("ExoSkeltonOuterFrame-ExoSkeltonInnerFrame", ExoSkeltonOuterFrame, ExoSkeltonInnerFrame);
  
  G4LogicalVolume* 
  ExoSkeltonFrame_Logical  = new G4LogicalVolume(ExoSkeltonFrame_Solid,
						 frameMaterial,
						 "ExoSkeltonFrame_Log",
						 0,0,0);
  G4VisAttributes* ExoSkeltonFrameVisAtt = new G4VisAttributes(darkbrown);
  ExoSkeltonFrameVisAtt->SetVisibility(true);
  //ExoSkeltonFrameVisAtt->SetForceWireframe(true);
  //ExoSkeltonFrameVisAtt->SetForceSolid(true);
  ExoSkeltonFrame_Logical->SetVisAttributes(ExoSkeltonFrameVisAtt);
  
  G4ThreeVector Position_ExoSkeltonFrame  = G4ThreeVector(0,0,-1.0*inch);
  
  G4VPhysicalVolume*
  ExoSkeltonFrame_Physical   = new G4PVPlacement(0,Position_ExoSkeltonFrame + Container_Center,
						 ExoSkeltonFrame_Logical,
						 "ExoSkeltonFrame_Physical",
						 CerenkovContainer_Logical,
						 false,0,
						 fCheckOverlaps);

  //****************************************************************************************************        
  //******************************Define Detector Active Area*******************************************
  G4double LightGuide_FullLength      =   18.00*cm;
  G4double LightGuide_FullWidth1      =   18.00*cm;
  G4double LightGuide_FullWidth2      =   18.00*cm;
  G4double LightGuide_FullThickness   =    1.25*cm;

  G4double GlueFilm_FullLength_X      =   0.1*mm;
  G4double GlueFilm_FullLength_Y      =   18.00*cm;
  G4double GlueFilm_FullLength_Z      =    1.25*cm;

  G4double ActiveArea_FullLength_X    =    2.0*(LightGuide_FullLength + QuartzBar_FullLength +GlueFilm_FullLength_X) + GlueFilm_FullLength_X +2.0*mm;
  G4double ActiveArea_FullLength_Y    =    QuartzBar_FullHeight + 1.0*mm;
  G4double ActiveArea_FullLength_Z    =    QuartzBar_FullThickness + 2.0*mm;

  G4Box* ActiveArea_Solid  = new G4Box("CerenkoDetector_Solid",
				       0.5 * ActiveArea_FullLength_X ,
				       0.5 * ActiveArea_FullLength_Y ,
				       0.5 * ActiveArea_FullLength_Z );

  G4LogicalVolume* 
  ActiveArea_Logical  = new G4LogicalVolume(ActiveArea_Solid,
					    defaultMaterial,
					    "ActiveArea_Log",
					    0,0,0);
  G4VisAttributes* ActiveAreaVisAtt = new G4VisAttributes(darkbrown);
  ActiveAreaVisAtt->SetVisibility(true);
  //ActiveAreaVisAtt->SetForceWireframe(true);
  //ActiveAreaVisAtt->SetForceSolid(true);
  ActiveArea_Logical->SetVisAttributes(ActiveAreaVisAtt);
  
  G4ThreeVector Position_ActiveArea  = G4ThreeVector(0,0,0);
  
  G4VPhysicalVolume*
    ActiveArea_Physical   = new G4PVPlacement(0,Position_ActiveArea + Container_Center,
					      ActiveArea_Logical,
					      "ActiveArea_Physical",
					      CerenkovContainer_Logical,
					      false,0,
					      fCheckOverlaps);

  //****************************************************************************************************
  //****************************************************************************************************

  G4double ChamferRotation = 45.0*degree;
  G4double ChamferScew = 0.0;
  G4double delta = 0.0;

  //****************************************************************************************************
  //******************************Define Right Detector Quartz Bar With Chamfers************************

  
  G4double Chamfer_FullLength         =  120.00*cm;
  G4double Chamfer_FullHeight         =    7.00*mm;
  G4double Chamfer_FullThickness      =    7.00*mm;

  G4Box* Chamfer_Solid    = new G4Box("Chamfer_Solid",
				      0.5 * Chamfer_FullLength,       // half X length required by Geant4
				      0.5 * Chamfer_FullHeight ,      // half Y length required by Geant4
				      0.5 * Chamfer_FullThickness );
    
  G4Trd* QuartzBar_Solid  = new G4Trd("QuartzBar_Solid",
				      0.5*QuartzBar_FullLength,
				      0.5*QuartzBar_FullLength,
				      0.5*QuartzBar_FullHeight+0.1*cm,
				      0.5*QuartzBar_FullHeight-0.1*cm,
				      0.5*QuartzBar_FullThickness);
  
  //Boolean Union:
  //Upper-upstream edge chamfer
  ChamferScew = 0.021486*degree;
  delta = 0.5*(Chamfer_FullHeight - 1.0*mm)/sqrt(2.0);
  G4double ChamferAdjRotZ = atan(sin(ChamferScew)*cos(90*degree - ChamferRotation));
  G4double ChamferAdjRotY = atan(sin(ChamferScew)*sin(90*degree - ChamferRotation));
  Position_Chamfer1.setX(0.0*cm);//33.333333*cm);
  Position_Chamfer1.setY(0.5*QuartzBar_FullHeight + delta);
  Position_Chamfer1.setZ(-(0.5*QuartzBar_FullThickness + delta));
  Rotation_Chamfer1.rotateX(45.0*degree);
  Rotation_Chamfer1.rotateY(ChamferAdjRotY*radian);
  Rotation_Chamfer1.rotateZ(ChamferAdjRotZ*radian);
  G4Transform3D Transform_Chamfer1(Rotation_Chamfer1,Position_Chamfer1);
  Rotation_Chamfer1.rotateZ(-ChamferAdjRotZ*radian);
  Rotation_Chamfer1.rotateY(-ChamferAdjRotY*radian);
  
  RightQuartz_Solid[0]=  new G4SubtractionSolid ("UpperUpstreamChamfer-RightQuartzBar",
						 QuartzBar_Solid,
						 Chamfer_Solid,
						 Transform_Chamfer1);
  
  //Boolean Union:
  //Upper-downstream edge chamfer  
  delta = 0.5*(Chamfer_FullHeight - 0.5*mm)/sqrt(2.0);
  ChamferScew = 0.0;//0.014*PI/180.0;
  ChamferAdjRotZ = atan(sin(ChamferScew)*cos(ChamferRotation));
  ChamferAdjRotY = atan(sin(ChamferScew)*sin(ChamferRotation));
  Position_Chamfer2.setX(0.0*mm);
  Position_Chamfer2.setY(0.5*QuartzBar_FullHeight + delta);
  Position_Chamfer2.setZ(0.5*QuartzBar_FullThickness + delta);
  Rotation_Chamfer2.rotateX(45.0*degree);
  Rotation_Chamfer2.rotateY(-ChamferAdjRotY*radian);
  Rotation_Chamfer2.rotateZ(ChamferAdjRotZ*radian);
  G4Transform3D Transform_Chamfer2(Rotation_Chamfer2,Position_Chamfer2);
  Rotation_Chamfer2.rotateZ(-ChamferAdjRotZ*radian);
  Rotation_Chamfer2.rotateY(ChamferAdjRotY*radian);
  
  RightQuartz_Solid[1] =  new G4SubtractionSolid ("UpperDownstreamChamfer-RightQuartzBar",
						  RightQuartz_Solid[0],
						  Chamfer_Solid,
						  Transform_Chamfer2);
  
  //Boolean Union:
  //Lower-Upstream edge chamfer
  ChamferAdjRotZ = atan(sin(ChamferScew)*cos(ChamferRotation));
  ChamferAdjRotY = atan(sin(ChamferScew)*sin(ChamferRotation));
  Position_Chamfer3.setX(0.0*mm);
  Position_Chamfer3.setY(-(0.5*QuartzBar_FullHeight + delta));
  Position_Chamfer3.setZ(-(0.5*QuartzBar_FullThickness + delta));
  Rotation_Chamfer3.rotateX(45.0*degree);
  Rotation_Chamfer3.rotateY(ChamferAdjRotY*radian);
  Rotation_Chamfer3.rotateZ(-ChamferAdjRotZ*radian);
  G4Transform3D Transform_Chamfer3(Rotation_Chamfer3,Position_Chamfer3);
  Rotation_Chamfer3.rotateZ(ChamferAdjRotZ*radian);
  Rotation_Chamfer3.rotateY(-ChamferAdjRotY*radian);
  
  RightQuartz_Solid[2] =  new G4SubtractionSolid ("LowerUpstreamChamfer-RightQuartzBar",
						  RightQuartz_Solid[1],Chamfer_Solid,
						  Transform_Chamfer3);
  
  //Boolean Union:
  //Lower-Downstream edge chamfer
  ChamferAdjRotZ = atan(sin(ChamferScew)*cos(90*degree - ChamferRotation));
  ChamferAdjRotY = atan(sin(ChamferScew)*sin(90*degree - ChamferRotation));
  Position_Chamfer4.setX(0.0*mm);
  Position_Chamfer4.setY(-(0.5*QuartzBar_FullHeight + delta));
  Position_Chamfer4.setZ(0.5*QuartzBar_FullThickness + delta);
  Rotation_Chamfer4.rotateX(45.0*degree);
  Rotation_Chamfer4.rotateY(-ChamferAdjRotY*radian);
  Rotation_Chamfer4.rotateZ(-ChamferAdjRotZ*radian);
  G4Transform3D Transform_Chamfer4(Rotation_Chamfer4,Position_Chamfer4);
  Rotation_Chamfer4.rotateY(ChamferAdjRotY*radian);
  Rotation_Chamfer4.rotateZ(ChamferAdjRotZ*radian);
  
  RightQuartz_Solid[3] =  new G4SubtractionSolid ("LowerUpstreamChamfer-RightQuartzBar",
						  RightQuartz_Solid[2], Chamfer_Solid,
						  Transform_Chamfer4);
  
  
  G4LogicalVolume* 
  QuartzBar_LogicalRight  = new G4LogicalVolume(RightQuartz_Solid[3],
						quartzMaterial,
						"QuartzBar_LogicalRight",
						0,0,0);
  G4VisAttributes* CerenkovDetectorVisAtt = new G4VisAttributes(orange);
  CerenkovDetectorVisAtt->SetVisibility(true);
  //CerenkovDetectorVisAtt->SetForceSolid(true);
  CerenkovDetectorVisAtt->SetForceWireframe(true);
  QuartzBar_LogicalRight->SetVisAttributes(CerenkovDetectorVisAtt);
  
  G4ThreeVector Position_RightQuartzBar = G4ThreeVector(-0.5*(QuartzBar_FullLength+GlueFilm_FullLength_X),0,0);
  
  G4VPhysicalVolume*
  QuartzBar_PhysicalRight   = new G4PVPlacement(0,Position_RightQuartzBar,
						QuartzBar_LogicalRight,
						"QuartzBar_PhysicalRight",
						ActiveArea_Logical,
						false,0,
						fCheckOverlaps);

  //****************************************************************************************************
  //****************************************************************************************************
  
  //****************************************************************************************************
  //******************************Define Center Quartz Glue Film ***************************************
  
  G4Box* CenterGlueFilm_Solid    = new G4Box("CenterGlueFilm_Solid",
					     0.5 * GlueFilm_FullLength_X,
					     0.5 * GlueFilm_FullLength_Y,
					     0.5 * GlueFilm_FullLength_Z);
  
  G4LogicalVolume* 
  QuartzGlue_Logical  = new G4LogicalVolume(CenterGlueFilm_Solid,
					    glueMaterial,
					    "CenterGlueFilm_Log",
					    0,0,0);
  QuartzGlue_Logical->SetVisAttributes(CerenkovDetectorVisAtt);
  
  G4ThreeVector Position_CenterGlueFilm = G4ThreeVector(0,0,0);  
  G4VPhysicalVolume*
  QuartzGlue_PhysicalCenter  = new G4PVPlacement(0,Position_CenterGlueFilm,
						 QuartzGlue_Logical,
						 "QuartzGlue_PhysicalCenter",
						 ActiveArea_Logical,
						 false,0,
						 fCheckOverlaps);


  //****************************************************************************************************
  //****************************************************************************************************
  
  //****************************************************************************************************
  //******************************Define Right Quartz Glue Film ****************************************
  
  G4ThreeVector Position_RightGlueFilm = G4ThreeVector(-1.0*(QuartzBar_FullLength+GlueFilm_FullLength_X),0,0);  
  G4VPhysicalVolume*
  QuartzGlue_PhysicalRight  = new G4PVPlacement(0,Position_RightGlueFilm,
						QuartzGlue_Logical,
						"QuartzGlue_PhysicalRight",
						ActiveArea_Logical,
						false,1,
						fCheckOverlaps);

  //****************************************************************************************************
  //****************************************************************************************************
  
  //****************************************************************************************************
  //******************************Define Left Detector Quartz Bar With Chamfers  ***********************
  
  
  //Boolean Union:
  //Upper-upstream edge chamfer  
  ChamferScew = -0.021486*degree;
  delta = 0.5*(Chamfer_FullHeight - 1.0*mm)/sqrt(2.0);
  ChamferAdjRotZ = atan(sin(ChamferScew)*cos(90*degree - ChamferRotation));
  ChamferAdjRotY = atan(sin(ChamferScew)*sin(90*degree - ChamferRotation));
  Position_Chamfer1.setX(0.0*cm);//33.333333*cm);
  Position_Chamfer1.setY(0.5*QuartzBar_FullHeight + delta);
  Position_Chamfer1.setZ(-(0.5*QuartzBar_FullThickness + delta));
  //   Rotation_Chamfer1.rotateX(45.0*degree);
  Rotation_Chamfer1.rotateY(ChamferAdjRotY*radian);
  Rotation_Chamfer1.rotateZ(ChamferAdjRotZ*radian);
  G4Transform3D Transform_Chamfer5(Rotation_Chamfer1,Position_Chamfer1);
  Rotation_Chamfer1.rotateZ(-ChamferAdjRotZ*radian);
  Rotation_Chamfer1.rotateY(-ChamferAdjRotY*radian);
  
  LeftQuartz_Solid[0]=  new G4SubtractionSolid ("UpperUpstreamChamfer-LeftQuartzBar",
						QuartzBar_Solid,
						Chamfer_Solid,
						Transform_Chamfer5);
  
  //Boolean Union:
  //Upper-downstream edge chamfer
  delta = 0.5*(Chamfer_FullHeight - 0.5*mm)/sqrt(2.0);
  ChamferScew = 0.0;//0.014*PI/180.0;
  ChamferAdjRotZ = atan(sin(ChamferScew)*cos(ChamferRotation));
  ChamferAdjRotY = atan(sin(ChamferScew)*sin(ChamferRotation));
  Position_Chamfer2.setX(0.0*mm);
  Position_Chamfer2.setY(0.5*QuartzBar_FullHeight + delta);
  Position_Chamfer2.setZ(0.5*QuartzBar_FullThickness + delta);
  //   Rotation_Chamfer2.rotateX(45.0*degree);
  Rotation_Chamfer2.rotateY(-ChamferAdjRotY*radian);
  Rotation_Chamfer2.rotateZ(ChamferAdjRotZ*radian);
  G4Transform3D Transform_Chamfer6(Rotation_Chamfer2,Position_Chamfer2);
  Rotation_Chamfer2.rotateZ(-ChamferAdjRotZ*radian);
  Rotation_Chamfer2.rotateY(ChamferAdjRotY*radian);
  
  LeftQuartz_Solid[1] =  new G4SubtractionSolid ("UpperDownstreamChamfer-LeftQuartzBar",
						 LeftQuartz_Solid[0],
						 Chamfer_Solid,
						 Transform_Chamfer6);
  
  //Boolean Union:
  //Lower-Upstream edge chamfer
  ChamferAdjRotZ = atan(sin(ChamferScew)*cos(ChamferRotation));
  ChamferAdjRotY = atan(sin(ChamferScew)*sin(ChamferRotation));
  Position_Chamfer3.setX(0.0*mm);
  Position_Chamfer3.setY(-(0.5*QuartzBar_FullHeight + delta));
  Position_Chamfer3.setZ(-(0.5*QuartzBar_FullThickness + delta));
  //   Rotation_Chamfer3.rotateX(45.0*degree);
  Rotation_Chamfer3.rotateY(ChamferAdjRotY*radian);
  Rotation_Chamfer3.rotateZ(-ChamferAdjRotZ*radian);
  G4Transform3D Transform_Chamfer7(Rotation_Chamfer3,Position_Chamfer3);
  Rotation_Chamfer3.rotateZ(ChamferAdjRotZ*radian);
  Rotation_Chamfer3.rotateY(-ChamferAdjRotY*radian);
  
  LeftQuartz_Solid[2] =  new G4SubtractionSolid ("LowerUpstreamChamfer-LeftQuartzBar",
						 LeftQuartz_Solid[1],Chamfer_Solid,
						 Transform_Chamfer7);
  
  //Boolean Union:
  //Lower-Downstream edge chamfer
  ChamferAdjRotZ = atan(sin(ChamferScew)*cos(90*degree - ChamferRotation));
  ChamferAdjRotY = atan(sin(ChamferScew)*sin(90*degree - ChamferRotation));
  Position_Chamfer4.setX(0.0*mm);
  Position_Chamfer4.setY(-(0.5*QuartzBar_FullHeight + delta));
  Position_Chamfer4.setZ(0.5*QuartzBar_FullThickness + delta);
  //   Rotation_Chamfer4.rotateX(45.0*degree);
  Rotation_Chamfer4.rotateY(-ChamferAdjRotY*radian);
  Rotation_Chamfer4.rotateZ(-ChamferAdjRotZ*radian);
  G4Transform3D Transform_Chamfer8(Rotation_Chamfer4,Position_Chamfer4);
  Rotation_Chamfer4.rotateY(ChamferAdjRotY*radian);
  Rotation_Chamfer4.rotateZ(ChamferAdjRotZ*radian);
  
  LeftQuartz_Solid[3] =  new G4SubtractionSolid ("LowerUpstreamChamfer-LeftQuartzBar",
						 LeftQuartz_Solid[2], Chamfer_Solid,
						 Transform_Chamfer8);
  
  
  G4LogicalVolume* 
  QuartzBar_LogicalLeft  = new G4LogicalVolume(LeftQuartz_Solid[3],
					       quartzMaterial,
					       "QuartzBar_LogicalLeft",
					       0,0,0);
  QuartzBar_LogicalLeft->SetVisAttributes(CerenkovDetectorVisAtt);
  
  G4ThreeVector Position_LeftQuartzBar = G4ThreeVector(0.5*(QuartzBar_FullLength+GlueFilm_FullLength_X),0,0);
  
  G4VPhysicalVolume*
  QuartzBar_PhysicalLeft   = new G4PVPlacement(0,Position_LeftQuartzBar,
					       QuartzBar_LogicalLeft,
					       "QuartzBar_PhysicalLeft",
					       ActiveArea_Logical,
					       false,0,
					       fCheckOverlaps);

  //****************************************************************************************************
  //****************************************************************************************************
  
  //****************************************************************************************************
  //******************************Define Left Quartz Glue Film *****************************************
  
  G4ThreeVector Position_LeftGlueFilm = G4ThreeVector((QuartzBar_FullLength+GlueFilm_FullLength_X),0,0);
  
  G4VPhysicalVolume*
  QuartzGlue_PhysicalLeft  = new G4PVPlacement(0,Position_LeftGlueFilm,
					       QuartzGlue_Logical,
					       "QuartzGlue_PhysicalLeft",
					       ActiveArea_Logical,
					       false,1,
					       fCheckOverlaps);
  
  //****************************************************************************************************
  //****************************************************************************************************
  
  //****************************************************************************************************
  //******************************Define Light Guides With Chamfers And Any Sculpting*******************
  
  G4double redfr = 1.0; //0.5
  G4double pTheta = atan(LightGuide_FullThickness*(1 - redfr)/(2.0*LightGuide_FullLength));
  
  G4Trap* LightGuide_Solid = new G4Trap("LightGuide_Solid",
					0.5*LightGuide_FullLength,pTheta,0.0,
					0.5*LightGuide_FullWidth1,
					redfr*0.5*LightGuide_FullThickness,
					redfr*0.5*LightGuide_FullThickness,0.0,
					0.5*LightGuide_FullWidth2,
					0.5*LightGuide_FullThickness,
					0.5*LightGuide_FullThickness,
					0.0);
  
  G4double LGAngCutXDim = 8.0*cm;
  G4double LGAngCutYDim = LightGuide_FullWidth1+1.0*cm;
  G4double LGAngCutZDim = 2.0*cm;
  
  G4Box* LGEdgeAngleCut_Solid = new G4Box("LGEdgeAngleCut_Solid",
					  0.5*LGAngCutXDim,
					  0.5*LGAngCutYDim,
					  0.5*LGAngCutZDim);
  G4double ad = 0.0; //45.0;  //0.0;
  G4double ar = ad*4.0*atan(1.0)/180.0;
  G4double dx = 0.5*LGAngCutZDim*cos(ar) -
    0.5*(LightGuide_FullThickness - LGAngCutZDim*sin(ar))*tan(ar) +    
    LightGuide_FullThickness*(1 - redfr)*tan(ar);    

  
  //******************************Left Light Guide *****************************************************

  //Boolean Union:
  //Left Light Guide Angular cut-off at edge
  Position_AngCut1.setX(0.0*cm);
  Position_AngCut1.setY(0.0*cm);
  Position_AngCut1.setZ(-(0.5*LightGuide_FullLength+dx));
  Rotation_AngCut1.rotateY(ad*degree);
  G4Transform3D Transform_AngCut1(Rotation_AngCut1,Position_AngCut1);
  
  LeftGuide_Solid[0] =  new G4SubtractionSolid ("LGLeft-AngCut",
						LightGuide_Solid,
						LGEdgeAngleCut_Solid,
						Transform_AngCut1);
  
  delta = 0.5*(Chamfer_FullHeight - 0.5*mm)/sqrt(2.0);
  
  Position_Chamfer1.setX(-(0.5*QuartzBar_FullThickness + delta));
  Position_Chamfer1.setY(0.5*QuartzBar_FullHeight + delta);
  Position_Chamfer1.setZ(0.0);
  Rotation_Chamfer1.rotateY(90.0*degree);
  G4Transform3D Transform_Chamfer9(Rotation_Chamfer1,Position_Chamfer1);
  
  LeftGuide_Solid[1]=  new G4SubtractionSolid ("LeftLGChamfer1",
					       LeftGuide_Solid[0],
					       Chamfer_Solid,
					       Transform_Chamfer9);
  
  
  Position_Chamfer2.setX(0.5*QuartzBar_FullThickness + delta);
  Position_Chamfer2.setY(0.5*QuartzBar_FullHeight + delta);
  Position_Chamfer2.setZ(0.0*cm);
  Rotation_Chamfer2.rotateY(90.0*degree);
  G4Transform3D Transform_Chamfer10(Rotation_Chamfer2,Position_Chamfer2);
  
  LeftGuide_Solid[2]=  new G4SubtractionSolid ("LeftLGChamfer2",
					       LeftGuide_Solid[1],
					       Chamfer_Solid,
					       Transform_Chamfer10);
  
  
  Position_Chamfer3.setX(0.5*QuartzBar_FullThickness + delta);
  Position_Chamfer3.setY(-(0.5*QuartzBar_FullHeight + delta));
  Position_Chamfer3.setZ(0.0*cm);
  Rotation_Chamfer3.rotateY(90.0*degree);
  G4Transform3D Transform_Chamfer11(Rotation_Chamfer3,Position_Chamfer3);
  
  LeftGuide_Solid[3]=  new G4SubtractionSolid ("LeftLGChamfer3",
					       LeftGuide_Solid[2],
					       Chamfer_Solid,
					       Transform_Chamfer11);
  
  Position_Chamfer4.setX(-(0.5*QuartzBar_FullThickness + delta));
  Position_Chamfer4.setY(-(0.5*QuartzBar_FullHeight + delta));
  Position_Chamfer4.setZ(0.0*cm);
  Rotation_Chamfer4.rotateY(90.0*degree);
  G4Transform3D Transform_Chamfer12(Rotation_Chamfer4,Position_Chamfer4);
  
  LeftGuide_Solid[4]=  new G4SubtractionSolid ("LeftLGChamfer4",
					       LeftGuide_Solid[3],
					       Chamfer_Solid,
					       Transform_Chamfer12);
  
  
  //****************************************************************************************************
  //******************************Right Light Guide ****************************************************
  
  
  //Boolean Union:
  //Right Light Guide Angular cut-off at edge
  Position_AngCut2.setX(0.0*cm);
  Position_AngCut2.setY(0.0*cm);
  Position_AngCut2.setZ(-(0.5*LightGuide_FullLength+dx));
  Rotation_AngCut2.rotateY(-ad*degree);
  G4Transform3D Transform_AngCut2(Rotation_AngCut2,Position_AngCut2);
  
  RightGuide_Solid[0] =  new G4SubtractionSolid ("LGRight-AngCut",
						 LightGuide_Solid,
						 LGEdgeAngleCut_Solid,
						 Transform_AngCut2);
  
  G4Transform3D Transform_Chamfer13(Rotation_Chamfer1,Position_Chamfer1);
  
  RightGuide_Solid[1]=  new G4SubtractionSolid ("RightLGChamfer1",
						RightGuide_Solid[0],
						Chamfer_Solid,
						Transform_Chamfer13);
  
  
  G4Transform3D Transform_Chamfer14(Rotation_Chamfer2,Position_Chamfer2);
  
  RightGuide_Solid[2]=  new G4SubtractionSolid ("RightLGChamfer2",
						RightGuide_Solid[1],
						Chamfer_Solid,
						Transform_Chamfer14);
  
  
  G4Transform3D Transform_Chamfer15(Rotation_Chamfer3,Position_Chamfer3);
  
  RightGuide_Solid[3]=  new G4SubtractionSolid ("RightLGChamfer3",
						RightGuide_Solid[2],
						Chamfer_Solid,
						Transform_Chamfer15);
  
  G4Transform3D Transform_Chamfer16(Rotation_Chamfer4,Position_Chamfer4);
  
  RightGuide_Solid[4]=  new G4SubtractionSolid ("RightLGChamfer4",
						RightGuide_Solid[3],
						Chamfer_Solid,
						Transform_Chamfer16);
    
  //Boolean Union:
  //Left Light Guide
  Position_LGLeft.setX((QuartzBar_FullLength+0.5*LightGuide_FullLength+1.5*GlueFilm_FullLength_X));
  Position_LGLeft.setY(0.0*cm);
  Position_LGLeft.setZ(0.0*cm - LightGuide_FullThickness*(1 - redfr)/(4.0));
  Rotation_LGLeft.rotateY(-90.0*degree);
  G4Transform3D Transform_LGLeft(Rotation_LGLeft,Position_LGLeft);
  
  //Boolean Union:
  //Right Light Guide
  Position_LGRight.setX(-(QuartzBar_FullLength+0.5*LightGuide_FullLength+1.5*GlueFilm_FullLength_X));
  Position_LGRight.setY(0.0*cm);
  Position_LGRight.setZ(0.0*cm - LightGuide_FullThickness*(1 - redfr)/(4.0));
  //   Rotation_LGRight.rotateY(-90.0*degree);
  Rotation_LGRight.rotateY(90.0*degree);
  //   Rotation_LGRight.rotateZ(180.0*degree);
  G4Transform3D Transform_LGRight(Rotation_LGRight,Position_LGRight);
  
  
  G4LogicalVolume* 
  LightGuide_LogicalLeft  = new G4LogicalVolume(LeftGuide_Solid[4],
						quartzMaterial,
						"LightGuide_LogicalLeft",
						0,0,0);
  LightGuide_LogicalLeft->SetVisAttributes(CerenkovDetectorVisAtt);

  
  G4VPhysicalVolume*
  LightGuide_PhysicalLeft = new G4PVPlacement(Transform_LGLeft,
					      LightGuide_LogicalLeft,
					      "LightGuide_PhysicalLeft",
					      ActiveArea_Logical,
					      false,0,
					      fCheckOverlaps);
  
  
  G4LogicalVolume* 
  LightGuide_LogicalRight  = new G4LogicalVolume(RightGuide_Solid[4],
						 quartzMaterial,
						 "LightGuide_LogicalRight",
						 0,0,0);
  LightGuide_LogicalRight->SetVisAttributes(CerenkovDetectorVisAtt);
  
  
  G4VPhysicalVolume*
  LightGuide_PhysicalRight = new G4PVPlacement(Transform_LGRight,
					       LightGuide_LogicalRight,
					       "LightGuide_PhysicalRight",
					       ActiveArea_Logical,
					       false,0,
					       fCheckOverlaps); 
  
  // //******************************Edge Mirrors******************
  
  // G4Box* LGEdgeMirror_Solid = new G4Box("LGEdgeMirror_Solid",
  // 					0.1*mm,0.5*LightGuide_FullWidth1,
  // 					redfr*0.5*LightGuide_FullThickness/cos(ar));
  
  // Position_LGEdgeMirrorLeft.setX(1.5*GlueFilm_FullLength_X + QuartzBar_FullLength+LightGuide_FullLength+0.1*mm/cos(ar)-
  // 				 0.5*LightGuide_FullThickness*tan(ar)+
  // 				 0.5*LightGuide_FullThickness*(1 - redfr)*tan(ar));
  // Position_LGEdgeMirrorLeft.setY(0.0*cm);
  // Position_LGEdgeMirrorLeft.setZ(-0.5*LightGuide_FullThickness*(1-redfr));
  // Rotation_LGEdgeMirrorLeft.rotateY(ad*degree);
  // G4Transform3D Transform_LGEMLeft(Rotation_LGEdgeMirrorLeft,Position_LGEdgeMirrorLeft);
  
  
  // mirror_logical[1]  = new G4LogicalVolume(LGEdgeMirror_Solid,
  // 					   mirror_material,
  // 					   "mirrorface_log2",
  // 					   0,0,0);
  
  // Position_LGEdgeMirrorRight.setX(-1.5*GlueFilm_FullLength_X-QuartzBar_FullLength-LightGuide_FullLength-0.1*mm/cos(ar)+
  // 				  0.5*LightGuide_FullThickness*tan(ar)-
  // 				  0.5*LightGuide_FullThickness*(1 - redfr)*tan(ar));
  // Position_LGEdgeMirrorRight.setY(0.0*cm);
  // Position_LGEdgeMirrorRight.setZ(-0.5*LightGuide_FullThickness*(1-redfr));
  // Rotation_LGEdgeMirrorRight.rotateY(-ad*degree);
  // G4Transform3D Transform_LGEMRight(Rotation_LGEdgeMirrorRight,Position_LGEdgeMirrorRight);
    
  // mirror_logical[3]  = new G4LogicalVolume(LGEdgeMirror_Solid,
  // 					   mirror_material,
  // 					   "mirrorface_log4",
  // 					   0,0,0);
 


  //*********************************************************
  //******************************Radiator*******************
  
  G4double Radiator_FullWidth = 8.62*inch;

  G4Box* RadiatorSolid = new G4Box("Radiator_Sol",
				   QuartzBar_FullLength,   // half X length required by Geant4
				   0.5*Radiator_FullWidth, // 0.5*Frame_FullLength_Y // half Y length required by Geant4
				   1.0*cm );  // half Z length required by Geant4
  
  G4LogicalVolume* 
  Radiator_Logical  = new G4LogicalVolume(RadiatorSolid,
					  radiatorMaterial,
					  "Radiator_Log",
					  0,0,0);
  G4VisAttributes* RadiatorVisAtt = new G4VisAttributes(lightblue);
  RadiatorVisAtt->SetVisibility(true);
  Radiator_Logical->SetVisAttributes(RadiatorVisAtt);
  
  //FIXME -- define step limitation for this container
  //0.001 produces 10x more output in o-tuple
  //G4double MaxStepInPbRadiator = 0.01*mm;
  //Radiator_Logical->SetUserLimits(new G4UserLimits(MaxStepInPbRadiator));
  //FIXME -- define step limitation for this container
  
  G4ThreeVector Position_Radiator  = G4ThreeVector(0, 0,-5.0*cm);//-2.0*cm);  
  G4VPhysicalVolume*
  Radiator_Physical   = new G4PVPlacement(0,Position_Radiator + Container_Center,
					  Radiator_Logical,
					  "Radiator_Physical",
					  CerenkovContainer_Logical,
					  false,
					  0,
					  fCheckOverlaps);

  //******************************PMT_PbShield*******************

  // Each Pb brick is 8x4x2 [inch], each PMT has 2 Pb bricks, i.e. 8x8x2 [inch]
  G4Box* PMT_PbShieldSolid = new G4Box("PMT_PbShield_Sol",
				       0.5*8.0*inch, // half X length required by Geant4
				       0.5*8.0*inch, // half Y length required by Geant4
				       0.5*2.0*inch );     // half Z length required by Geant4
  
  G4LogicalVolume* 
  PMT_PbShield_Logical  = new G4LogicalVolume(PMT_PbShieldSolid,
					      leadMaterial,
					      "PMT_PbShield_Log",
					      0,0,0);
  G4VisAttributes* PMT_PbShieldVisAtt = new G4VisAttributes(blue);
  PMT_PbShieldVisAtt->SetVisibility(true);
  PMT_PbShield_Logical->SetVisAttributes(PMT_PbShieldVisAtt);    

  G4ThreeVector Position_PMT_PbShield_Left   = G4ThreeVector(-100.0*cm-4.0*inch, 0, -6.5*cm);
  G4ThreeVector Position_PMT_PbShield_Right  = G4ThreeVector( 100.0*cm+4.0*inch, 0, -6.5*cm);
  G4VPhysicalVolume*
  PMT_PbShield_Physical_Left   = new G4PVPlacement(0,Position_PMT_PbShield_Left + Container_Center,
					      PMT_PbShield_Logical,
					      "PMT_PbShield_Physical",
					      CerenkovContainer_Logical,
					      false,
					      0,
					      fCheckOverlaps);
  
  G4VPhysicalVolume*
  PMT_PbShield_Physical_Right   = new G4PVPlacement(0,Position_PMT_PbShield_Right + Container_Center,
					      PMT_PbShield_Logical,
					      "PMT_PbShield_Physical",
					      CerenkovContainer_Logical,
					      false,
					      1,
					      fCheckOverlaps);

  //~~~~~~~~~~~~~~~~~~ FIXME
  
  //-----------------------------------
  // define the PMTContainer
  //-----------------------------------
  
  G4double thetaY = atan(LightGuide_FullThickness*(1 - redfr)/(LightGuide_FullLength));
  G4double Xoffs = 0.0*cm;//7.0*cm;

  G4double PMTEntranceWindow_Thickness   =  1.0*mm; // assumed PMT glass thickness
  G4double PMTEntranceWindow_Diameter    =  12.7*cm;//QuartzBar_FullHeight;
  G4double PMTContainer_Diameter    =  PMTEntranceWindow_Diameter+1.0*mm;
  G4double Cathode_Thickness   = 1.0*mm;
  G4double PMTContainer_FullLength_Z    =  2.0*mm+PMTEntranceWindow_Thickness+Cathode_Thickness;
  //Flat on guide face configuration
  G4double PMTContXShift = QuartzBar_FullLength + LightGuide_FullLength - 0.5*PMTEntranceWindow_Diameter - Xoffs;
  G4double PMTContYShift = 0.0;
  G4double PMTContZShift = 0.5*QuartzBar_FullThickness + 0.5*PMTContainer_FullLength_Z
    - (LightGuide_FullLength - 0.5*PMTEntranceWindow_Diameter-Xoffs)*tan(thetaY);

    // placing the left PMTContainer

  G4ThreeVector     Translation_PMTContainerLeft;
  // relocation of the left Photon Detector Container
  Translation_PMTContainerLeft.setX(1.0*PMTContXShift);
  Translation_PMTContainerLeft.setY(1.0*PMTContYShift);
  Translation_PMTContainerLeft.setZ(1.0*PMTContZShift);
  
  //Flat on guide face configuration
  G4RotationMatrix Rotation_PMTContainerLeft; 
  Rotation_PMTContainerLeft.rotateY(thetaY);
  G4Transform3D Transform3D_PMTContainerLeft(Rotation_PMTContainerLeft,
					     Translation_PMTContainerLeft);

  G4ThreeVector     Translation_PMTContainerRight;
  // relocation of the right Photon Detector Container
  Translation_PMTContainerRight.setX(-1.0*PMTContXShift);
  Translation_PMTContainerRight.setY(1.0*PMTContYShift);
  Translation_PMTContainerRight.setZ(1.0*PMTContZShift);

  //Flat on guide face configuration
  G4RotationMatrix Rotation_PMTContainerRight; 
  Rotation_PMTContainerRight.rotateY(-thetaY);
  G4Transform3D Transform3D_PMTContainerRight(Rotation_PMTContainerRight,
					      Translation_PMTContainerRight);


  G4double PMTQuartzOpticalFilm_Thickness=  0.1*mm;
  G4double PMTQuartzOpticalFilmZShift = 0.5*(PMTQuartzOpticalFilm_Thickness - PMTContainer_FullLength_Z);

  // relocation of the PMTEntranceWindow
  G4ThreeVector Translation_PMTQuartzOpticalFilm;
  Translation_PMTQuartzOpticalFilm.setX(0.0*cm);
  Translation_PMTQuartzOpticalFilm.setY(0.0*cm);
  Translation_PMTQuartzOpticalFilm.setZ(PMTQuartzOpticalFilmZShift);

  //-------------------------------------------------------------------------------------
  // location and orientation of the PMT Entrance Window within the PMT Container
  //-------------------------------------------------------------------------------------

  G4double PMTEntWindZShift = 0.5*(PMTEntranceWindow_Thickness - PMTContainer_FullLength_Z)+PMTQuartzOpticalFilm_Thickness;
  
  // relocation of the PMTEntranceWindow
  G4ThreeVector    Translation_PMTEntranceWindow;
  Translation_PMTEntranceWindow.setX(0.0*cm);
  Translation_PMTEntranceWindow.setY(0.0*cm);
  Translation_PMTEntranceWindow.setZ(PMTEntWindZShift);
  
  //-------------------------------------------------------------------------------------
  // location and orientation of the cathode WITHIN the PMT
  //-------------------------------------------------------------------------------------

  G4double CathodeZShift = PMTEntranceWindow_Thickness + 0.5*(Cathode_Thickness - PMTContainer_FullLength_Z) + PMTQuartzOpticalFilm_Thickness;

  // location of the Photon Detector relative to  Photon Detector Container
  G4ThreeVector    Translation_Cathode;
  Translation_Cathode.setX(0.0*cm);
  Translation_Cathode.setY(0.0*cm);
  Translation_Cathode.setZ(CathodeZShift);


  G4Tubs* PMTContainer_Solid    =
    new G4Tubs("PMTContainer_Solid",0.0*cm,
	       0.5 * PMTContainer_Diameter,
	       0.5 * PMTContainer_FullLength_Z,
	       0.0*degree,360.0*degree);


  G4LogicalVolume* PMTContainer_Logical  =
    new G4LogicalVolume(PMTContainer_Solid,
			defaultMaterial,
			"PMTContainer_Log",
			0,0,0);
  
  // left side
  G4VPhysicalVolume* PMTContainer_PhysicalLeft  =
    new G4PVPlacement(Transform3D_PMTContainerLeft,
		      PMTContainer_Logical,
		      "PMTContainer_Physical",
		      ActiveArea_Logical,
		      false,
		      0, // copy number for left PMTContainer
		      fCheckOverlaps);

    // right side
   G4VPhysicalVolume* PMTContainer_PhysicalRight =
     new G4PVPlacement(Transform3D_PMTContainerRight,
		       PMTContainer_Logical,
		       "PMTContainer_Physical",
		       ActiveArea_Logical,
		       false,
		       1, // copy number for right PMTContainer
		       fCheckOverlaps);


   //----------------------------------------
   // define the glue or grease or cookie layer
   //----------------------------------------
   G4double PMTQuartzOpticalFilm_Diameter =  12.7*cm;
   G4Tubs* PMTQuartzOpticalFilm_Solid = new G4Tubs("PMTQuartzOpticalFilm_Solid",0.0*cm,
						   0.5*PMTQuartzOpticalFilm_Diameter,
						   0.5*PMTQuartzOpticalFilm_Thickness,
						   0.0*degree,360.0*degree);

   G4LogicalVolume* PMTQuartzOpticalFilm_Logical  =
     new G4LogicalVolume(PMTQuartzOpticalFilm_Solid,
			 glueMaterial,
			 "PMTQuartzOpticalFilm_Log",
			 0,0,0);
   
   G4VPhysicalVolume* PMTQuartzOpticalFilm_Physical =
     new G4PVPlacement(0,Translation_PMTQuartzOpticalFilm,
		       PMTQuartzOpticalFilm_Logical,
		       "PMTQuartzOpticalFilm_Physical",
		       PMTContainer_Logical,
		       false, 0, fCheckOverlaps); // copy number for left photon detector

   //----------------------------------------
   // define the PMTEntranceWindow
   //----------------------------------------
   
   G4Tubs* PMTEntranceWindow_Solid = new G4Tubs("PMTEntranceWindow_Solid",0.0*cm,
						0.5*PMTEntranceWindow_Diameter,
						0.5*PMTEntranceWindow_Thickness,
						0.0*degree,360.0*degree);
   
   G4LogicalVolume* PMTEntranceWindow_Logical  =
     new G4LogicalVolume(PMTEntranceWindow_Solid,
			 limeGlassMaterial,
			 "PMTEntranceWindow_Log",
			 0,0,0);
   
   G4VPhysicalVolume* PMTEntranceWindow_Physical =
     new G4PVPlacement(0,Translation_PMTEntranceWindow,
		       PMTEntranceWindow_Logical,
		       "PMTEntranceWindow_Physical",
		       PMTContainer_Logical,
		       false, 0, fCheckOverlaps); // copy number for left photon detector
   
   //---------------------------
   // define the Photon Detector
   //---------------------------
   G4double ReductionInPhotocathodeDiameter = 5*mm;
   G4double Cathode_Diameter = PMTEntranceWindow_Diameter - ReductionInPhotocathodeDiameter;
   G4Tubs* Cathode_Solid = new G4Tubs("Cathode_Solid",0.0*cm,0.5*Cathode_Diameter,
				      0.5*Cathode_Thickness,0.0*degree,360.0*degree);
   
   G4LogicalVolume* Cathode_Logical  =
     new G4LogicalVolume(Cathode_Solid,
			 cathodeMaterial,
			 "Cathode_Log",0,0,0);
   
   G4VPhysicalVolume* Cathode_Physical =
     new G4PVPlacement(0,Translation_Cathode,
		       Cathode_Logical,
		       "Cathode_Physical",
		       PMTContainer_Logical,
		       false, 0, fCheckOverlaps); // copy number for left photon detector
   



// // Setting Optical Properties

//     const G4int nEntries = 11;
//     G4double PhotonEnergy[nEntries] = {
//         1.54986*eV,  //800.59 nm
//         1.77127*eV,  //700.51 nm
//         2.06648*eV,  //600.44 nm
//         2.47978*eV,  //500.37 nm
//         2.8178*eV,   //440.34 nm
//         3.09973*eV,  //400.29 nm
//         3.5424*eV,   //350.27 nm
//         4.13297*eV,  //300.22 nm
//         4.95956*eV,  //250.18 nm
//         5.51063*eV,  //225.16 nm
//         5.90424*eV   //210.15 nm
//     };

//     G4double Reflectivity[nEntries];
//     G4double mylambda;
//     for (G4int kk= 0; kk < nEntries; kk++) {
//         // Nevens empiric formular for the reflectivity
//         // lamda = h*c/E

//         mylambda = (h_Planck*c_light/PhotonEnergy[kk])/nanometer;
//         Reflectivity[kk] =  1.0 - 0.027*exp(-0.004608*mylambda);
//         //Reflectivity[kk] =  1.0;
//     };

//     G4OpticalSurface* QuartzBarLeft_OpticalSurface = new G4OpticalSurface("QuartzBarLeftOpticalSurface");
//     G4OpticalSurface* QuartzBarRight_OpticalSurface = new G4OpticalSurface("QuartzBarRightOpticalSurface");
//     G4OpticalSurface* LightGuideLeft_OpticalSurface = new G4OpticalSurface("LightGuideLeftOpticalSurface");
//     G4OpticalSurface* LightGuideRight_OpticalSurface = new G4OpticalSurface("LightGuideRightOpticalSurface");

//     G4OpticalSurface* GlueFilmCenter_OpticalSurface = new G4OpticalSurface("GlueFilmCenterOpticalSurface");
//     G4OpticalSurface* GlueFilmLeft_OpticalSurface = new G4OpticalSurface("GlueFilmLeftOpticalSurface");
//     G4OpticalSurface* GlueFilmRight_OpticalSurface = new G4OpticalSurface("GlueFilmRightOpticalSurface");


// // boundary optical properties between pad and quartz

//     G4OpticalSurface* LeftEndPad_Quartz_OpticalSurface = new G4OpticalSurface("LeftEndPadQuartzOpticalSurface");
//     G4OpticalSurface* RightEndPad_Quartz_OpticalSurface = new G4OpticalSurface("RightEndPadQuartzOpticalSurface");
//     G4OpticalSurface* SidePad_LeftLightGuide_OpticalSurface
//       = new G4OpticalSurface("SidePadLeftLightGuideOpticalSurface");
//     G4OpticalSurface* SidePad_RightLightGuide_OpticalSurface
//       = new G4OpticalSurface("SidePadRightLightGuideOpticalSurface");
//     G4OpticalSurface* SidePad_QuartzBarLeft_OpticalSurface
//       = new G4OpticalSurface("SidePadQuartzBarLeftOpticalSurface");
//     G4OpticalSurface* SidePad_QuartzBarRight_OpticalSurface
//       = new G4OpticalSurface("SidePadQuartzBarRightOpticalSurface");
    

//     QuartzBarLeft_OpticalSurface->SetType(dielectric_dielectric);
//     QuartzBarLeft_OpticalSurface->SetFinish(polished);
//     QuartzBarLeft_OpticalSurface->SetPolish(0.997);
//     QuartzBarLeft_OpticalSurface->SetModel(glisur);
    
//     QuartzBarRight_OpticalSurface->SetType(dielectric_dielectric);
//     QuartzBarRight_OpticalSurface->SetFinish(polished);
//     QuartzBarRight_OpticalSurface->SetPolish(0.997);
//     QuartzBarRight_OpticalSurface->SetModel(glisur);
    
//     LightGuideLeft_OpticalSurface->SetType(dielectric_dielectric);
//     LightGuideLeft_OpticalSurface->SetFinish(polished);
//     LightGuideLeft_OpticalSurface->SetPolish(0.997);
//     LightGuideLeft_OpticalSurface->SetModel(glisur);
    
//     LightGuideRight_OpticalSurface->SetType(dielectric_dielectric);
//     LightGuideRight_OpticalSurface->SetFinish(polished);
//     LightGuideRight_OpticalSurface->SetPolish(0.997);
//     LightGuideRight_OpticalSurface->SetModel(glisur);
    
//     GlueFilmLeft_OpticalSurface->SetType(dielectric_dielectric);
//     GlueFilmLeft_OpticalSurface->SetFinish(polished);
//     GlueFilmLeft_OpticalSurface->SetPolish(0.9);
//     GlueFilmLeft_OpticalSurface->SetModel(glisur);
    
//     GlueFilmCenter_OpticalSurface->SetType(dielectric_dielectric);
//     GlueFilmCenter_OpticalSurface->SetFinish(polished);
//     GlueFilmCenter_OpticalSurface->SetPolish(0.9);
//     GlueFilmCenter_OpticalSurface->SetModel(glisur);
    
//     GlueFilmRight_OpticalSurface->SetType(dielectric_dielectric);
//     GlueFilmRight_OpticalSurface->SetFinish(polished);
//     GlueFilmRight_OpticalSurface->SetPolish(0.9);
//     GlueFilmRight_OpticalSurface->SetModel(glisur);


// //##############################################

//     G4OpticalSurface* BracketPad_OpticalSurface = 
//                          new G4OpticalSurface("BracketPad_Optical_Surface");
//     BracketPad_OpticalSurface->SetType(dielectric_metal);
//     BracketPad_OpticalSurface->SetFinish(ground);
//     BracketPad_OpticalSurface->SetModel(glisur);
//     new G4LogicalSkinSurface("SideBracketPad_SkinSurface",SideBracketPad_Logical,BracketPad_OpticalSurface);
//     new G4LogicalSkinSurface("EndBracketPad_SkinSurface",EndBracketPad_Logical,BracketPad_OpticalSurface);

//     G4double Pad_Reflectivity[nEntries]={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
//     G4MaterialPropertiesTable *BracketPad_MPT = new G4MaterialPropertiesTable();
//     BracketPad_MPT->AddProperty("REFLECTIVITY",PhotonEnergy,Pad_Reflectivity,nEntries);
//     BracketPad_OpticalSurface->SetMaterialPropertiesTable(BracketPad_MPT);

//     LeftEndPad_Quartz_OpticalSurface->SetType(dielectric_metal);
//     LeftEndPad_Quartz_OpticalSurface->SetFinish(polished);
//     LeftEndPad_Quartz_OpticalSurface->SetPolish(0.9);
//     LeftEndPad_Quartz_OpticalSurface->SetModel(glisur);
//     LeftEndPad_Quartz_OpticalSurface->SetMaterialPropertiesTable(BracketPad_MPT);

//     RightEndPad_Quartz_OpticalSurface->SetType(dielectric_metal);
//     RightEndPad_Quartz_OpticalSurface->SetFinish(polished);
//     RightEndPad_Quartz_OpticalSurface->SetPolish(0.9);
//     RightEndPad_Quartz_OpticalSurface->SetModel(glisur);
//     RightEndPad_Quartz_OpticalSurface->SetMaterialPropertiesTable(BracketPad_MPT);

//     SidePad_LeftLightGuide_OpticalSurface->SetType(dielectric_metal);
//     SidePad_LeftLightGuide_OpticalSurface->SetFinish(polished);
//     SidePad_LeftLightGuide_OpticalSurface->SetPolish(0.9);
//     SidePad_LeftLightGuide_OpticalSurface->SetModel(glisur);
//     SidePad_LeftLightGuide_OpticalSurface->SetMaterialPropertiesTable(BracketPad_MPT);

//     SidePad_RightLightGuide_OpticalSurface->SetType(dielectric_metal);
//     SidePad_RightLightGuide_OpticalSurface->SetFinish(polished);
//     SidePad_RightLightGuide_OpticalSurface->SetPolish(0.9);
//     SidePad_RightLightGuide_OpticalSurface->SetModel(glisur);
//     SidePad_RightLightGuide_OpticalSurface->SetMaterialPropertiesTable(BracketPad_MPT);

//     SidePad_QuartzBarLeft_OpticalSurface->SetType(dielectric_metal);
//     SidePad_QuartzBarLeft_OpticalSurface->SetFinish(polished);
//     SidePad_QuartzBarLeft_OpticalSurface->SetPolish(0.9);
//     SidePad_QuartzBarLeft_OpticalSurface->SetModel(glisur);
//     SidePad_QuartzBarLeft_OpticalSurface->SetMaterialPropertiesTable(BracketPad_MPT);

//     SidePad_QuartzBarRight_OpticalSurface->SetType(dielectric_metal);
//     SidePad_QuartzBarRight_OpticalSurface->SetFinish(polished);
//     SidePad_QuartzBarRight_OpticalSurface->SetPolish(0.9);
//     SidePad_QuartzBarRight_OpticalSurface->SetModel(glisur);
//     SidePad_QuartzBarRight_OpticalSurface->SetMaterialPropertiesTable(BracketPad_MPT);
    
// //#####################################################3

//     G4MaterialPropertiesTable *quartzST = new G4MaterialPropertiesTable();
//     quartzST->AddProperty("REFLECTIVITY",  PhotonEnergy , Reflectivity, nEntries);
//     QuartzBarLeft_OpticalSurface->SetMaterialPropertiesTable(quartzST);
//     QuartzBarRight_OpticalSurface->SetMaterialPropertiesTable(quartzST);
//     LightGuideLeft_OpticalSurface->SetMaterialPropertiesTable(quartzST);
//     LightGuideRight_OpticalSurface->SetMaterialPropertiesTable(quartzST);
//     GlueFilmLeft_OpticalSurface->SetMaterialPropertiesTable(quartzST);
//     GlueFilmCenter_OpticalSurface->SetMaterialPropertiesTable(quartzST);
//     GlueFilmRight_OpticalSurface->SetMaterialPropertiesTable(quartzST);
    
//     G4OpticalSurface* ActiveArea_OpticalSurface = new G4OpticalSurface("ActiveAreaOpticalSurface");
//     //     G4LogicalBorderSurface* ActiveArea_BorderSurface
//     //                             = new G4LogicalBorderSurface("ActiveArea_BorderSurface",
//     //                                                          ActiveArea_Physical,
//     //                                                          CerenkovMasterContainer_Physical,
//     //                                                          ActiveArea_OpticalSurface);
    
//     ActiveArea_OpticalSurface->SetFinish(groundbackpainted); //new for wrapping test
//     //   ActiveArea_OpticalSurface->SetPolish(0.0);          //new for wrapping test
//     //   ActiveArea_OpticalSurface->SetModel(glisur);    	 //new for wrapping test
//     ActiveArea_OpticalSurface->SetModel(unified);         //new for wrapping test
//     ActiveArea_OpticalSurface->SetSigmaAlpha(0.25);        //new for wrapping test
    
//     // for wrapping test
//     G4double RefractiveIndex_Air[nEntries] = {1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};
//     G4double MilliPoreRefl[nEntries]       = {0.94,0.94,0.945,0.945,0.95,0.95,0.945,0.91,0.85,0.80,0.80};
//     G4double specularlobe[nEntries]        = {0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1};
//     G4double specularspike[nEntries]       = {0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1};
//     G4double backscatter[nEntries]         = {0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1};
    
//     G4MaterialPropertiesTable *myST = new G4MaterialPropertiesTable();
    
//     myST->AddProperty("RINDEX",  PhotonEnergy , RefractiveIndex_Air, nEntries);     //new for wrapping test
//     myST->AddProperty("REFLECTIVITY",  PhotonEnergy , MilliPoreRefl, nEntries);     //new for wrapping test
//     myST->AddProperty("SPECULARLOBECONSTANT", PhotonEnergy ,specularlobe,nEntries); //new for wrapping test
//     myST->AddProperty("SPECULARSPIKECONSTANT",PhotonEnergy,specularspike,nEntries); //new for wrapping test
//     myST->AddProperty("BACKSCATTERCONSTANT",PhotonEnergy,backscatter,nEntries);     //new for wrapping test
//     //   myST->AddProperty("ABSLENGTH", PhotonEnergy, AbsorptionCoeff_Air, nEntries);    //new for wrapping test
    
//     ActiveArea_OpticalSurface->SetMaterialPropertiesTable(myST);
    
//     // Set Optical Properties for the Detector Windows
//     G4OpticalSurface* Window_OpticalSurface = new G4OpticalSurface("Window_Optical_Surface");
//     Window_OpticalSurface->SetType(dielectric_metal);
//     Window_OpticalSurface->SetFinish(ground);
//     Window_OpticalSurface->SetModel(glisur);
//     new G4LogicalSkinSurface("FrontWindow_SkinSurface",FrontWindow_Logical,Window_OpticalSurface);
//     new G4LogicalSkinSurface("BackWindow_SkinSurface",BackWindow_Logical,Window_OpticalSurface);
    
//     G4double Tyvek_Reflectivity[nEntries]={0.95,0.95,0.95,0.95,0.95,0.95,0.95,0.90,0.90,0.85,0.85};
//     G4MaterialPropertiesTable *Window_MPT = new G4MaterialPropertiesTable();
//     Window_MPT->AddProperty("REFLECTIVITY",PhotonEnergy,Tyvek_Reflectivity,nEntries);
//     Window_OpticalSurface->SetMaterialPropertiesTable(Window_MPT);
    
//     // set bracket optical properties
//     G4OpticalSurface* Bracket_OpticalSurface = 
//       new G4OpticalSurface("Bracket_Optical_Surface");
//     Bracket_OpticalSurface->SetType(dielectric_metal);
//     Bracket_OpticalSurface->SetFinish(ground);
//     Bracket_OpticalSurface->SetModel(glisur);
//     new G4LogicalSkinSurface("SideBracket_SkinSurface",SideBracket_Logical,Bracket_OpticalSurface);
//     new G4LogicalSkinSurface("EndBracket_SkinSurface",EndBracket_Logical,Bracket_OpticalSurface);
    
//     G4double Bracket_Reflectivity[nEntries]={0.8,0.8,0.8,0.8,0.8,0.8,0.8,0.8,0.8,0.8,0.8};
//     G4MaterialPropertiesTable *Bracket_MPT = new G4MaterialPropertiesTable();
//     Bracket_MPT->AddProperty("REFLECTIVITY",PhotonEnergy,Bracket_Reflectivity,nEntries);
//     Bracket_OpticalSurface->SetMaterialPropertiesTable(Bracket_MPT);
    
    
//     //Setting Skin Optical Properties for GlueFilm
//     G4OpticalSurface* GlueFilm_OpticalSurface = new G4OpticalSurface("GlueFilmOpticalSurface");
//     GlueFilm_OpticalSurface->SetType(dielectric_dielectric);
//     GlueFilm_OpticalSurface->SetFinish(polished);
//     GlueFilm_OpticalSurface->SetPolish(0.9);
//     GlueFilm_OpticalSurface->SetModel(glisur);
//     new G4LogicalSkinSurface("GlueFilm_SkinSurface",QuartzGlue_Logical,GlueFilm_OpticalSurface);
    
//     //Setting the Optical Properties of PMTs
    
//     // peiqing, Dec. 20, 2011
//     // if QE is processed in EventAction, set it to 100% here
//     G4double Photocathode_Efficiency[nEntries]={
//       0.0080,      //800.59 nm
//       0.0298,      //700.51 nm
//       0.0638,      //600.44 nm
//       0.1240,      //500.37 nm
//       0.1670,      //440.34 nm
//       0.1770,      //400.29 nm
//       0.1800,      //350.27 nm
//       0.2110,      //300.22 nm
//       0.1780,      //250.18 nm
//       0.0890,      //225.16 nm
//       0.0355       //210.15 nm
//     };
    
    
//     //S20 reflectance taken from "Optics Communications, issue 180, 2000. p89–102"
//     //average of 40 degree incident angle assumed
//     ////data below 400 nm, taken from //http://www.photek.com/support/Papers/
//     //Experimental%20data%20on%20the%20reflection%20and%20transmission%20spectral%20response%20of%20photocathodes.pdf
//     G4double Photocathode_Reflectivity[nEntries]={
//       0.25, //800 nm
//       0.25, //700 nm
//       0.25, //600 nm
//       0.25, //500 nm
//       0.25, //440 nm
//       0.25, //400 nm
//       0.25, //350 nm
//       0.25, //300 nm
//       0.25, //250 nm
//       0.25, //225 nm
//       0.25  //210 nm
//     };
    
//     G4OpticalSurface* Photocathode_OpticalSurface =  new G4OpticalSurface("Photocathode_OS");
//     Photocathode_OpticalSurface ->SetType(dielectric_metal); 
//     Photocathode_OpticalSurface ->SetFinish(polished); 
//     Photocathode_OpticalSurface ->SetModel(glisur); 
    
//     G4MaterialPropertiesTable* Photocathode_MPT = new G4MaterialPropertiesTable();
//     Photocathode_MPT->AddProperty("REFLECTIVITY", PhotonEnergy, Photocathode_Reflectivity,nEntries);
//     Photocathode_MPT->AddProperty("EFFICIENCY", PhotonEnergy, Photocathode_Efficiency,nEntries);
//     Photocathode_OpticalSurface ->SetMaterialPropertiesTable(Photocathode_MPT);
    
//     new G4LogicalBorderSurface("PMT_BorderSurface",
//                                PMTEntranceWindow_Physical,
//                                Cathode_Physical,
//                                Photocathode_OpticalSurface);
    
//     // Setting Sensitive detectors
//     G4SDManager* SDman = G4SDManager::GetSDMpointer();
    
//     CerenkovDetectorSD = new QweakSimCerenkov_DetectorSD("/CerenkovDetectorSD");
//     SDman->AddNewDetector(CerenkovDetectorSD);
//     ActiveArea_Logical->SetSensitiveDetector(CerenkovDetectorSD);
    
//     CerenkovDetector_PMTSD = new QweakSimCerenkovDetector_PMTSD("/CerenkovPMTSD",myUserInfo);
//     SDman->AddNewDetector(CerenkovDetector_PMTSD);
//     Cathode_Logical->SetSensitiveDetector(CerenkovDetector_PMTSD);
    
    

  //
  // Always return the physical World
  //
  return worldPV;
} 

