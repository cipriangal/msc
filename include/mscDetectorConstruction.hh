
#ifndef mscDetectorConstruction_h
#define mscDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4Box;
class G4VPhysicalVolume;
class G4UniformMagField;
class G4GenericMessenger;

/// Detector construction class to define materials and geometry.
/// The calorimeter is a box made of a given number of layers. A layer consists
/// of an absorber plate and of a detection gap. The layer is replicated.
///
/// Four parameters define the geometry of the calorimeter :
///
/// - the thickness of an absorber plate,
/// - the thickness of a gap,
/// - the number of layers,
/// - the transverse size of the calorimeter (the input face is a square).
///
/// In DefineVolumes(), sensitive detectors of G4MultiFunctionalDetector type
/// with primitive scorers are created and associated with the Absorber 
/// and Gap volumes.
///
/// In addition a transverse uniform magnetic field is defined in
/// SetMagField() method which can be activated
/// via a command defined using G4GenericMessenger class: 
/// - /B4/det/setMagField value unit


class mscDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    mscDetectorConstruction();
    virtual ~mscDetectorConstruction();

  public:
    virtual G4VPhysicalVolume* Construct();

    // set methods
    //
    void SetMagField(G4double fieldValue);
     
  private:
    // methods
    //
    void DefineMaterials();
    G4VPhysicalVolume* DefineVolumes();
  
    // data members
    //
    G4GenericMessenger*  fMessenger; // messenger 
    G4UniformMagField*   fMagField;  // magnetic field

    G4bool  fCheckOverlaps; // option to activate checking of volumes overlaps
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

