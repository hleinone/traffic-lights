$fa=5;
$fs=0.1;

module lightsCover() {
}

module visor() {
    intersection() {
        difference() {
            cylinder(3,3,3,false);
            translate([0,0,-0.25]) {
                cylinder(3.5,2.5,2.5,false);
            }
        }
        translate([0,2,-1]) {
            sphere(4);
        }
    }
}

module screwHolder(height) {
    difference() {
        cylinder(height,1,1);
        translate([0,0,-0.5]) {
            cylinder(5.5,0.5,0.5);
        }
    }
}

module lightsBox() {
    difference() {
        cube([9,22,1]);
        translate([4.5,17,-0.25]) {
            cylinder(1.5,2.5,2.5,false);
        }
        translate([4.5,11,-0.25]) {
            cylinder(1.5,2.5,2.5,false);
        }
        translate([4.5,5,-0.25]) {
            cylinder(1.5,2.5,2.5,false);
        }
    }
    translate([4.5,17,1]) {
        visor();
    }
    translate([4.5,11,1]) {
        visor();
    }
    translate([4.5,5,1]) {
        visor();
    }

    translate([-0.5,0,-9]) {
        difference() {
            cube([10,22,10]);
            translate([0.5,0.5,-0.5]) {
                cube([9,21,10.5]);
            }
            translate([0.5,-0.5,9]) {
                cube([9,23,1.5]);
            }
            rotate([90,0,0]) {
                translate([5,5,-1]) {
                    cylinder(1.5,2,2,false);
                }
            }
            translate([0.5,-0.5,-0.5]) {
                cube([9,1.5,1.5]);
            }
            translate([3,-0.5,0.5]) {
                cube([4,1.5,4.5]);
            }
        }
        
    }
    
    translate([1.5,2,-8]) {
        screwHolder(9);
    }
    translate([7.5,2,-8]) {
        screwHolder(9);
    }
    translate([1.5,20,-8]) {
        screwHolder(9);
    }
    translate([7.5,20,-8]) {
        screwHolder(9);
    }
}

lightsBox();
