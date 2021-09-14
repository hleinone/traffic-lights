$fa=5;
$fs=0.1;

module lightsCover() {
}

module visor() {
    intersection() {
        difference() {
            cylinder(6,6,6,false);
            translate([0,0,-0.5]) {
                cylinder(7,5.2,5.2,false);
            }
        }
        translate([0,4,-2]) {
            sphere(8);
        }
    }
}

module screwHolder(h,r) {
    difference() {
        cylinder(h,r,r);
        translate([0,0,-r/2]) {
            cylinder(5.5,r-0.5,r-0.5);
        }
    }
}

module lightsFacade() {
    difference() {
        translate([0,0,-3]) {
            cube([20,46,5]);
        }
        translate([10,36,-5]) {
            cylinder(8,5.2,5.2,false);
        }
        translate([10,24,-5]) {
            cylinder(8,5.2,5.2,false);
        }
        translate([10,12,-5]) {
            cylinder(8,5.2,5.2,false);
        }
    }
    translate([10,36,1]) {
        visor();
    }
    translate([10,24,1]) {
        visor();
    }
    translate([10,12,1]) {
        visor();
    }    
}

module lightsBox() {
    difference() {
        union() {
            translate([0,0,-19]) {
                difference() {
                    cube([20,46,17]);
                    translate([1,3,-0.5]) {
                        cube([18,42,15.5]);
                    }
                    translate([10,36,13]) {
                        cylinder(5,5.2,5.2,false);
                    }
                    translate([10,24,13]) {
                        cylinder(5,5.2,5.2,false);
                    }
                    translate([10,12,13]) {
                        cylinder(5,5.2,5.2,false);
                    }
                    translate([0.5,-0.5,36]) {
                        cube([18,46,2.5]);
                    }
                    rotate([90,0,0]) {
                        translate([10,10,-4]) {
                            cylinder(5,5,5,false);
                        }
                    }
                }
                
                translate([0.5,3,1]) {
                    difference() {
                        cube([19,3,15]);
                        rotate([90,0,0]) {
                            translate([9.5,9,-2]) {
                                cylinder(2.5,5,5);
                            }
                        }
                        rotate([90,0,0]) {
                            translate([9.5,9,-3.5]) {
                                cylinder(2,4,4);
                            }
                        }
                    }
                }
            }
        }
        
        rotate([180,-90,0]) {
            translate([-9,-2.5,14.5]) {
                cylinder(4.5,1,1);
            }
        }

        rotate([180,90,0]) {
            translate([9,-2.5,-5.5]) {
                cylinder(4.5,1,1);
            }
        }

        translate([2.5,4.5,-19.5]) {
            cylinder(4.5,1,1);
        }

        translate([17.5,4.5,-19.5]) {
            cylinder(4.5,1,1);
        }

    }

    translate([2.5,43.5,-18]) {
        screwHolder(15,1.5);
    }
    translate([17.5,43.5,-18]) {
        screwHolder(15,1.5);
    }
}

module pole() {
    difference() {
        cylinder(100,5,5);
        translate([0,0,-1]) {
            cylinder(102,4,4);
        }
    }
}

module bottomBox() {
    difference() {
        cube([50,95,30]);
        translate([-0.5,5,20.5]) {
            cube([10,15,10.5]);
        }
        translate([40.5,5,20.5]) {
            cube([10,15,10.5]);
        }
        translate([10,1,0.5]) {
            cube([30,93,30]);
        }
    }
    
    translate([12.5,4.5,25]) {
        rotate([180,0,0]) {
            screwHolder(25,3);
        }
    }
    translate([12.5,90.5,25]) {
        rotate([180,0,0]) {
            screwHolder(25,3);
        }
    }
    translate([37.5,4.5,25]) {
        rotate([180,0,0]) {
            screwHolder(25,3);
        }
    }
    translate([37.5,90.5,25]) {
        rotate([180,0,0]) {
            screwHolder(25,3);
        }
    }
}

translate([-10,0,15]) {
    !lightsFacade();
}

translate([-10,0,9]) {
    lightsBox();
}

*translate([0,-1,0]) {
    rotate([90,0,0]) {
        pole();
    }
}

*translate([-25,-102,-47.5]) {
    rotate([90,0,0]) {
        bottomBox();
    }
}
