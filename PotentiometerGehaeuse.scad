// Gehäuse für Schiebepotentiometer
// Maße in mm

// Gehäuse-Parameter
breite = 30;      // 3cm
laenge = 100;     // 10cm
hoehe = 17;       // 1.7cm
wandstaerke = 2;  // Wandstärke
spiel = 0.3;      // Toleranz für Passung

// Deckel-Parameter
deckel_hoehe = 3;
schlitz_breite = 2;
schlitz_laenge = 90;  // 9cm
schlitz_abstand = 22; // 2.2cm von der Seite
nut_tiefe = 2;    // Tiefe der Nut
nut_breite = 1;   // Breite der Nut

// Hauptgehäuse
module gehaeuse() {
    color([0.65, 0.08, 0.12]) {  // Farbe Rot für das Gehäuse
        difference() {
            // Äußere Hülle
            cube([breite, laenge, hoehe]);
            
            // Innere Aussparung
            translate([wandstaerke, wandstaerke, wandstaerke])
            cube([breite-2*wandstaerke, laenge-2*wandstaerke, hoehe-wandstaerke+1]);
            
            // Nut für Deckel
            translate([wandstaerke/2, wandstaerke/2, hoehe-nut_tiefe])
            cube([breite-wandstaerke, laenge-wandstaerke, nut_tiefe+1]);
        }
        
        // Rastnasen für Deckel
        translate([breite/2, wandstaerke, hoehe-deckel_hoehe-1])
        cube([2, 1, 1]);
        translate([breite/2, laenge-wandstaerke-1, hoehe-deckel_hoehe-1])
        cube([2, 1, 1]);
    }
}

// Deckel
module deckel() {
    translate([40, 0, 0]) {  // Verschoben zur besseren Sicht
        color([0.65, 0.08, 0.12]) {  // Farbe Rot für den Deckel
            difference() {
                // Hauptplatte
                cube([breite-wandstaerke+spiel, laenge-wandstaerke+spiel, deckel_hoehe]);
                
                // Schlitz
                translate([schlitz_abstand, 5, -1])
                cube([schlitz_breite, schlitz_laenge, deckel_hoehe+2]);
                
                // Aussparungen für Rastnasen
                translate([breite/2-wandstaerke/2, wandstaerke-spiel, -1])
                cube([2+spiel, 1+spiel, 2]);
                translate([breite/2-wandstaerke/2, laenge-wandstaerke-1-spiel, -1])
                cube([2+spiel, 1+spiel, 2]);
            }
        }
    }
}

// Zusammenbau
gehaeuse();
deckel();