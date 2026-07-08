## readme.md: Virtube-DIY

<img width="1500" height="1081" alt="AmpLightFront" src="https://github.com/user-attachments/assets/f70892f7-6486-40b8-8978-b6342a517ef4" />

"Virtube Amps VA50" – Virtueller Vollröhrenverstärker: So habe ich das "Kastl" genannt. Nun gebe ich die Fertigunsdaten der Hardware sowie das Software-Grundgerüst frei.

* ./brd: Schaltplan, Layout und Eagle-BRD-Datei.

* ./fertigungsdaten: Fertigungsdaten für Hauptprint und den Blenden, diverse Beschreibungen und Anleitungen.  
  *Anmerkung: Das so gefertigte Gerät ist von einem akkreditierten Testlabor auf alle CE-notwendigen Eigenschaften erfolgreich getestet worden.*

* ./Virtube-DIY: Code Composer Studio Projekt.
    * Framework und Treiber, jeder kann seinen Verstärker selbst programmieren.
    * Das gesamte Software-Framework ist Doxygen-dokumentiert.
    * Die Simulationen und Berechnungen der virtuellen Rörhren in den Verstärkerstufen (Virtual Tube ... Virtube) sowie die Verstärkermodelle sind nicht Teil der Veröffentlichung.
    * Die vier Effekte (Noise Gate, Tube Screamer, Equalizer, Reverb) sind dabei.
    * Library-Dateien von TI sind aus lizenzrechtlicher Vorsicht nicht dabei, sehrwohl aber ein Verzeichnisbaum mit den benötigten Dateien (C2000Ware). Siehe: https://e2e.ti.com/support/microcontrollers/c2000-microcontrollers-group/c2000/f/c2000-microcontrollers-forum/1654050/tms320f28377s-c2000ware-license-and-public-distribution-for-a-do-it-yourself-project

* Lizenzinformationen zur Hardware:
    * CERN Open Hardware Licence Version 2 - Weakly Reciprocal (CERN-OHL-W v2)
    * Copyright © 2025 Edwin Krasser
    * This hardware design is released under the CERN-OHL-W v2 license with the following additional restriction:
    * Commercial use, production, sale, or incorporation into commercial products is explicitly prohibited without prior written permission from the author.
    * Contact Edwin Krasser for commercial licensing.
    * Gerber files, Board files (.brd) and BOM are provided for non-commercial use.
    * Schematic is provided as PDF only (not as editable source).

* Edwin Krasser, Ingenieurbüro für Elektrotechnik, Schwerpunkt F&E Elektronik.
* ib.ekrasser@gmail.com
* www.ib-ekrasser.at
* www.ib-ekrasser.at/virtube
