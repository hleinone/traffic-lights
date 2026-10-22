$fa = 2;
$fs = 0.1;
part = "layout"; // [layout,base_box,base_cover,pole,head_box,head_facade,head_cover,power_switch_hat,source_layout]
pole_joint_clearance = 0.2; // mm on each side of each cover opening

module visor() {
    intersection() {
        difference() {
            cylinder(6, 6, 6, false);
            translate([0, 0, -0.5]) {
                cylinder(7, 5.2, 5.2, false);
            }
        }
        translate([0, 4, -2]) {
            sphere(8);
        }
    }
}

// Square post with the corner facing the cavity rounded. The two outer sides
// extend 0.2 mm into the base walls, making a solid joint at each corner.
module screwHolder(h, xDirection, yDirection) {
    outerReach = 3.2;
    innerReach = 3;
    cornerRadius = 3.5;
    side = outerReach + innerReach;

    difference() {
        linear_extrude(h) {
            scale([xDirection, yDirection]) {
                translate([-outerReach, -outerReach]) {
                    union() {
                        square([side - cornerRadius, side]);
                        square([side, side - cornerRadius]);
                        translate([side - cornerRadius, side - cornerRadius])
                            circle(cornerRadius);
                    }
                }
            }
        }
        translate([0, 0, h + 3]) rotate([180, 0, 0]) screwHole(13);
    }
}

module screwHole(h) {
    union() {
        cylinder(h - 7, 1, 1);
        translate([0, 0, h - 7]) {
            cylinder(7, 1, 0.2);
        }
    }
}

module lightsFacade() {
    difference() {
        translate([0, 0, -3]) {
            cube([20, 44, 5]);
        }
        translate([10, 34, -5]) {
            cylinder(8, 5.2, 5.2, false);
        }
        translate([10, 22, -5]) {
            cylinder(8, 5.2, 5.2, false);
        }
        translate([10, 10, -5]) {
            cylinder(8, 5.2, 5.2, false);
        }
    }
    translate([10, 34, 1]) {
        visor();
    }
    translate([10, 22, 1]) {
        visor();
    }
    translate([10, 10, 1]) {
        visor();
    }    
}

module lightsBox() {
    translate([0, 0, -19]) {
        difference() {
            union() {
                difference() {
                    cube([20, 44, 17]);
                    translate([1, 1, -0.5]) {
                        cube([18, 42, 15.5]);
                    }
                }
                translate([1, 1, 3]) {
                    linear_extrude(14) {
                        polygon([[0, 0], [0, 9], [9, 0], [0, 0]]);
                    }
                }

                translate([19, 1, 3]) {
                    linear_extrude(14) {
                        polygon([[0, 0], [-9, 0], [0, 9], [0, 0]]);
                    }
                }
                translate([1, 43, 3]) {
                    linear_extrude(14) {
                        polygon([[0, 0], [0, -9], [9, 0], [0, 0]]);
                    }
                }

                translate([19, 43, 3]) {
                    linear_extrude(14) {
                        polygon([[0, 0], [-9, 0], [0, -9], [0, 0]]);
                    }
                }
                translate([1, 0, 3]) {
                    cube([18, 4, 14]);
                }
            }
            translate([10, 34, 13]) {
                cylinder(5, 5.2, 5.2, false);
            }
            translate([10, 22, 13]) {
                cylinder(5, 5.2, 5.2, false);
            }
            translate([10, 10, 13]) {
                cylinder(5, 5.2, 5.2, false);
            }
            translate([0.5, -0.5, 36]) {
                cube([18, 46, 2.5]);
            }
            rotate([90, 0, 0]) {
                translate([10, 10, -2]) {
                    cylinder(3, 5, 5, false);
                }
            }

            translate([0, 1, 1]) {
                rotate([90, 0, 0]) {
                    translate([10, 9, -2]) {
                        cylinder(2.5, 5, 5);
                    }
                }

                rotate([90, 0, 0]) {
                    translate([10, 9, -3.5]) {
                        cylinder(2, 3, 3);
                    }
                }
            }
        }
    }
}

module lightsBoxCover() {
    cube([18, 42, 3]);
}

// Four identical guides at 90-degree intervals around the pole axis.
// Clearance expands the matching openings without changing the tabs.
module poleJointTabs(clearance = 0, height = 3) {
    for (angle = [0 : 90 : 270]) {
        rotate([0, 0, angle]) {
            translate([6 - clearance, -1 - clearance, 0]) {
                cube([1 + 2 * clearance, 2 + 2 * clearance, height]);
            }
        }
    }
}

module pole() {
    difference() {
        union() {
            cylinder(100, 5, 5);
            translate([0, 0, 100]) {
                rotate([180, 0, 0]) {
                    cylinder(10, 15, 6);
                }
            }
            translate([0, 0, 100]) poleJointTabs();
        }
        translate([0, 0, -1]) {
            cylinder(102, 3, 3);
        }
    }
}

module base() {
    h = 31;
    w = 34;
    screwInset = 23.5;
    
    difference() {
        translate([9, 0, 0]) {
            cube([w, 96, h]);
        }
        translate([1, 8, 25]) {
            cube([10, 10, 10.5]);
        }
        translate([39.5, 10, 25]) {
            cube([10, 7, 10.5]);
        }
        translate([10.5, 1.5, 1.5]) {
            cube([w - 3, 93, h +1]);
        }
    }
    
    translate([13.5, 4.5, 0]) screwHolder(screwInset,  1,  1);
    translate([13.5, 91.5, 0]) screwHolder(screwInset,  1, -1);
    translate([38.5, 4.5, 0]) screwHolder(screwInset, -1,  1);
    translate([38.5, 91.5, 0]) screwHolder(screwInset, -1, -1);
}

module baseCover() {
    screwInset = 5.5;
    poleCenter = [31 / 2, 93 / 2];
    
    difference() {
        union() {
            cube([31, 93, 1.5]);
            translate([3, 3, screwInset]) {
                rotate([180, 0, 0]) {
                    cylinder(screwInset, 3, 3);
                }
            }
            translate([3, 90, screwInset]) {
                rotate([180, 0, 0]) {
                    cylinder(screwInset, 3, 3);
                }
            }
            translate([28, 3, screwInset]) {
                rotate([180, 0, 0]) {
                    cylinder(screwInset, 3, 3);
                }
            }
            translate([28, 90, screwInset]) {
                rotate([180, 0, 0]) {
                    cylinder(screwInset, 3, 3);
                }
            }

            translate([31, 6.5, 0]) {
                cube([1.5, 10, 1.5]);
            }
            translate([-1.5, 8.5, 0]) {
                cube([1.5, 7, 1.5]);
            }
        }
        translate([poleCenter[0], poleCenter[1], 2]) {
            rotate([180, 0, 0]) {
                cylinder(3, 3, 3);
            }
        }
        // Matching through-openings for the four tabs.
        translate([poleCenter[0], poleCenter[1], -0.1]) {
            poleJointTabs(clearance = pole_joint_clearance, height = 1.7);
        }
        
        translate([3, 3, screwInset]) {
            rotate([180, 0, 0]) {
                translate([0, 0, -1.5]) {
                    cylinder(screwInset + 1, 1.5, 1.5);
                }
                translate([0, 0, 4]) {
                    cylinder(2, 1.5, 3);
                }
            }
        }
        translate([3, 90, screwInset]) {
            rotate([180, 0, 0]) {
                translate([0, 0, -1.5]) {
                    cylinder(screwInset + 1, 1.5, 1.5);
                }
                translate([0, 0, 4]) {
                    cylinder(2, 1.5, 3);
                }
            }
        }
        translate([28, 3, screwInset]) {
            rotate([180, 0, 0]) {
                translate([0, 0, -1.5]) {
                    cylinder(screwInset + 1, 1.5, 1.5);
                }
                translate([0, 0, 4]) {
                    cylinder(2, 1.5, 3);
                }
            }
        }
        translate([28, 90, screwInset]) {
            rotate([180, 0, 0]) {
                translate([0, 0, -1.5]) {
                    cylinder(screwInset + 1, 1.5, 1.5);
                }
                translate([0, 0, 4]) {
                    cylinder(2, 1.5, 3);
                }
            }
        }
    }
}

module powerSwitchHat() {
    difference() {
        cube([3, 3, 5]);
        translate([0.9, 0.9, -0.2]) {
            cube([1.5, 1.5, 2.2]);
        }
    }
}


// Print orientations. The geometry above is shared by the preview and STL exports.
module print_base_box() { translate([-9, 0, 0]) base(); }
module print_base_cover() { baseCover(); }
module print_pole() { pole(); }
module print_head_box() {
    translate([0, 44, -2]) rotate([180, 0, 0]) lightsBox();
}
module print_head_facade() { translate([0, 0, 3]) lightsFacade(); }
module print_head_cover() { lightsBoxCover(); }
module print_power_switch_hat() {
    // Put the closed end on the bed and leave the switch socket facing up.
    translate([0, 3, 5]) rotate([180, 0, 0]) powerSwitchHat();
}

module source_layout() {
    // The original exploded arrangement, retained as an optional view.
    translate([-10, 0, 15]) lightsFacade();
    translate([-10, 0, 9]) lightsBox();
    translate([-9, 1, -30]) lightsBoxCover();
    translate([0, -10, 0]) rotate([90, 0, 0]) pole();
    translate([25, -182, -48]) rotate([90, 0, 180]) base();
    translate([-16.5, -151, -46.5]) rotate([90, 0, 0]) baseCover();
    translate([35, -10, 0]) powerSwitchHat();
}

if (part == "base_box") print_base_box();
else if (part == "base_cover") print_base_cover();
else if (part == "pole") print_pole();
else if (part == "head_box") print_head_box();
else if (part == "head_facade") print_head_facade();
else if (part == "head_cover") print_head_cover();
else if (part == "power_switch_hat") print_power_switch_hat();
else if (part == "layout") {
    print_base_box();
    translate([45, 0, 0]) print_base_cover();
    translate([92, 0, 0]) print_head_box();
    translate([125, 0, 0]) print_head_facade();
    translate([158, 0, 0]) print_head_cover();
    translate([185, 0, 0]) print_power_switch_hat();
    translate([215, 22, 0]) print_pole();
} else if (part == "source_layout") source_layout();
else assert(false, "Unknown part");
