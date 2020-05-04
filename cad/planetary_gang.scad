//  Open BVM Ventilator - planetary gear drive train
//
//  Created by WHPThomas <me(at)henri(dot)net> on 20/02/20.
//  Copyright (c) 2020 WHPThomas
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

// IF YOU ARE HAVING TROUBLE PRINTING THIS PART START BY CHANGING THE
// FILIMENT DIAMETER OR FLOW RATE IN YOUR 3D PRINTER SLICER SOFTWARE.

// planetary gear module [enter 1 or 2] axle or hex keyed drive
gang=1;

// gear mesh clearance
tolerance=0.125;

// center drive shaft diameter clearance
center_clearance=0.15;

// bearing clearance
bearing_clearance=0.1;

// stepper motor drive shaft diameter
drive_shaft_diameter=5.0;

// stepper motor drive shaft edge to flat key surface
drive_shaft_key=3.9; // 3.9 or 4.5

D=86; // diameter of ring
T=10; // thickness
number_of_planets=4;
number_of_teeth_on_planets=15;
approximate_number_of_teeth_on_sun=9;
P=45; //[30:60] pressure angle
nTwist=0.75; // number of teeth to twist across
planet_bearing_diameter=22;
planet_hole_diameter=planet_bearing_diameter-4;
ring_radius=round(D/2+8); // outer hexagon
mount_radius=round(D/2+1); // bolt center radius
mount_hole_diameter=4;
mount_cap_diameter=8; // cap screw head radius
mount_boss_radius=7;
DR=0.5*1;// maximum depth ratio of teeth
m=round(number_of_planets);
np=round(number_of_teeth_on_planets);
ns1=approximate_number_of_teeth_on_sun;
k1=round(2/m*(ns1+np));
k= k1*m%2!=0 ? k1+1 : k1;
ns=k*m/2-np;
nr=ns+2*np;
pitchD=0.9*D/(1+min(PI/(2*nr*tan(P)),PI*DR/nr));
pitch=pitchD*PI/nr;
helix_angle=atan(2*nTwist*pitch/T);
planet_radius=pitchD/2*(ns+np)/nr; // 21.92
phi=$t*360/m;
ko=drive_shaft_key-(drive_shaft_diameter/2)+1;

echo(str("Teeth on planets: " , np));
echo(str("Teeth on sun: " , ns));
echo(str("Pitch: " , pitch));
echo(str("Helix angle: " , helix_angle));
echo(str("Planet diameter: " , planet_radius * 2));
echo(str("Ring diameter: " , ring_radius * 2));
echo(str("Bolt center diameter: " , mount_radius * 2));

translate([0,0,T/2]){
    // ring
	difference(){
        union() {
            // outer ring (gusset)
            cylinder(r=ring_radius,h=T,center=true,$fn=6);
           // boss
           for(i=[0:5])
                rotate([0,0,i*60])
                    translate([mount_radius,0,0])
                        cylinder(r=mount_boss_radius,h=T,center=true,$fn=100);
        }
        union() {
            herringbone(nr,pitch,P,DR,-tolerance,helix_angle,T+0.2);
            // hole
            for(i=[0,2,4])
                rotate([0,0,i*60])
                    translate([mount_radius,0,0]) {
                        mirror([0,0,1])
                            cylinder(r=(mount_hole_diameter/2)+center_clearance,h=T+1,center=false,$fn=100);
                            cylinder(r=mount_cap_diameter/2,h=T+1,center=false,$fn=100);
                    }
            for(i=[1,3,5])
                rotate([0,0,i*60])
                    translate([mount_radius,0,0]) {
                            cylinder(r=(mount_hole_diameter/2)+center_clearance,h=T+1,center=false,$fn=100);
                        mirror([0,0,1])
                            cylinder(r=mount_cap_diameter/2,h=T+1,center=false,$fn=100);
                    }
        }
	}
    // sun
    difference() {
        union() {
            rotate([0,0,(np+1)*180/ns+phi*(ns+np)*2/ns])
                difference(){
                    // gear
                    mirror([0,1,0])
                        herringbone(ns,pitch,P,DR,tolerance,helix_angle,T);
                    // hole
                    if(gang==1) 
                        cylinder(r=(drive_shaft_diameter/2)+center_clearance,h=T+1,center=true,$fn=100);
                    else
                        cylinder(r=5+center_clearance,h=T+1,center=true,$fn=6);                    
                }
            // key way
            if(gang==1)
                translate([0,ko+center_clearance,0])
                    cube([6,2,T], true);
        }
    }
    // planets
	for(i=[1:m])
        rotate([0,0,i*360/m+phi])
            translate([planet_radius,0,0])
                rotate([0,0,i*ns/m*360/np-phi*(ns+np)/np-phi])
                    difference(){
                        // gear
                        herringbone(np,pitch,P,DR,tolerance,helix_angle,T);
                        union() {
                            // hole
                            cylinder(r=planet_hole_diameter/2,h=T+1,center=true,$fn=100);
                            // bearing housing
                            translate([0,0,-2]) {
                                cylinder(r=(planet_bearing_diameter/2)+bearing_clearance,h=T+1,center=false,$fn=100);
                            }
                        }
                    }
}

module rack(
	number_of_teeth=15,
	circular_pitch=10,
	pressure_angle=28,
	helix_angle=0,
	clearance=0,
	gear_thickness=5,
	flat=false){
addendum=circular_pitch/(4*tan(pressure_angle));

flat_extrude(h=gear_thickness,flat=flat)translate([0,-clearance*cos(pressure_angle)/2])
	union(){
		translate([0,-0.5-addendum])square([number_of_teeth*circular_pitch,1],center=true);
		for(i=[1:number_of_teeth])
			translate([circular_pitch*(i-number_of_teeth/2-0.5),0])
			polygon(points=[[-circular_pitch/2,-addendum],[circular_pitch/2,-addendum],[0,addendum]]);
	}
}

module herringbone(
	number_of_teeth=15,
	circular_pitch=10,
	pressure_angle=28,
	depth_ratio=1,
	clearance=0,
	helix_angle=0,
	gear_thickness=5){
union(){
	gear(number_of_teeth,
		circular_pitch,
		pressure_angle,
		depth_ratio,
		clearance,
		helix_angle,
		gear_thickness/2);
	mirror([0,0,1])
		gear(number_of_teeth,
			circular_pitch,
			pressure_angle,
			depth_ratio,
			clearance,
			helix_angle,
			gear_thickness/2);
}}

module gear (
	number_of_teeth=15,
	circular_pitch=10,
	pressure_angle=28,
	depth_ratio=1,
	clearance=0,
	helix_angle=0,
	gear_thickness=5,
	flat=false){
pitch_radius = number_of_teeth*circular_pitch/(2*PI);
twist=tan(helix_angle)*gear_thickness/pitch_radius*180/PI;

flat_extrude(h=gear_thickness,twist=twist,flat=flat)
	gear2D (
		number_of_teeth,
		circular_pitch,
		pressure_angle,
		depth_ratio,
		clearance);
}

module flat_extrude(h,twist,flat){
	if(flat==false)
		linear_extrude(height=h,twist=twist,slices=twist/6)children(0);
	else
		children(0);
}

module gear2D (
	number_of_teeth,
	circular_pitch,
	pressure_angle,
	depth_ratio,
	clearance){
pitch_radius = number_of_teeth*circular_pitch/(2*PI);
base_radius = pitch_radius*cos(pressure_angle);
depth=circular_pitch/(2*tan(pressure_angle));
outer_radius = clearance<0 ? pitch_radius+depth/2-clearance : pitch_radius+depth/2;
root_radius1 = pitch_radius-depth/2-clearance/2;
root_radius = (clearance<0 && root_radius1<base_radius) ? base_radius : root_radius1;
backlash_angle = clearance/(pitch_radius*cos(pressure_angle)) * 180 / PI;
half_thick_angle = 90/number_of_teeth - backlash_angle/2;
pitch_point = involute (base_radius, involute_intersect_angle (base_radius, pitch_radius));
pitch_angle = atan2 (pitch_point[1], pitch_point[0]);
min_radius = max (base_radius,root_radius);

intersection(){
	rotate(90/number_of_teeth)
		circle($fn=number_of_teeth*3,r=pitch_radius+depth_ratio*circular_pitch/2-clearance/2);
	union(){
		rotate(90/number_of_teeth)
			circle($fn=number_of_teeth*2,r=max(root_radius,pitch_radius-depth_ratio*circular_pitch/2-clearance/2));
		for (i = [1:number_of_teeth])rotate(i*360/number_of_teeth){
			halftooth (
				pitch_angle,
				base_radius,
				min_radius,
				outer_radius,
				half_thick_angle);		
			mirror([0,1])halftooth (
				pitch_angle,
				base_radius,
				min_radius,
				outer_radius,
				half_thick_angle);
		}
	}
}}

module halftooth (
	pitch_angle,
	base_radius,
	min_radius,
	outer_radius,
	half_thick_angle){
index=[0,1,2,3,4,5];
start_angle = max(involute_intersect_angle (base_radius, min_radius)-5,0);
stop_angle = involute_intersect_angle (base_radius, outer_radius);
angle=index*(stop_angle-start_angle)/index[len(index)-1];
p=[[0,0],
	involute(base_radius,angle[0]+start_angle),
	involute(base_radius,angle[1]+start_angle),
	involute(base_radius,angle[2]+start_angle),
	involute(base_radius,angle[3]+start_angle),
	involute(base_radius,angle[4]+start_angle),
	involute(base_radius,angle[5]+start_angle)];

difference(){
	rotate(-pitch_angle-half_thick_angle)polygon(points=p);
	square(2*outer_radius);
}}

// Mathematical Functions
//===============

// Finds the angle of the involute about the base radius at the given distance (radius) from it's center.
//source: http://www.mathhelpforum.com/math-help/geometry/136011-circle-involute-solving-y-any-given-x.html

function involute_intersect_angle (base_radius, radius) = sqrt (pow (radius/base_radius, 2) - 1) * 180 / PI;

// Calculate the involute position for a given base radius and involute angle.

function involute (base_radius, involute_angle) =
[
	base_radius*(cos (involute_angle) + involute_angle*PI/180*sin (involute_angle)),
	base_radius*(sin (involute_angle) - involute_angle*PI/180*cos (involute_angle))
];
