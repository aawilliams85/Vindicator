SteamVR Saber Staff based on Tundra Labs sample project here: https://github.com/tundra-labs/rp2040_examples
![Vanquisher Assembled](https://github.com/user-attachments/assets/a8963114-0400-49d0-bbd0-622c49ecac14)
![Vanquisher Parts](https://github.com/user-attachments/assets/524e640e-56ec-4427-9a9f-e16506df9018)

## Structure
The subfolders in the main directory are as follows:
* VindicatorDriver
    * This contains the driver that tells SteamVR how to use the controller.
    * The subfolder "vindicator" should be copied to "C:\Program Files (x86)\Steam\steamapps\common\SteamVR\drivers"
* VindicatorFirmware
    * This is the RP2040 Arduino code that is programmed into the dev board
* VindicatorInputPCB
    * This is the KiCAD design for the button input board that connects to the RP2040 dev board.
* VindicatorJson
    * This contains example before and after device JSON files to show how to convert to/from the stock Tundra Tracker configuration. 
* VindicatorMechanical
    * This is the FreeCAD model for the 3D printed parts.
