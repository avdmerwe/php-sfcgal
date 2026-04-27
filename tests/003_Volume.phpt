--TEST--
Volume — unit cube SOLID has volume 1; polyhedral shell has area 6
--SKIPIF--
<?php if (!extension_loaded('sfcgal')) { print "sfcgal extension not loaded\n"; exit(1); } ?>
--FILE--
<?php

$cube = SFCGALGeometry::readWKT(
    'SOLID((' .
        '((0 0 0,0 1 0,1 1 0,1 0 0,0 0 0)),' .
        '((0 0 1,1 0 1,1 1 1,0 1 1,0 0 1)),' .
        '((0 0 0,1 0 0,1 0 1,0 0 1,0 0 0)),' .
        '((1 0 0,1 1 0,1 1 1,1 0 1,1 0 0)),' .
        '((1 1 0,0 1 0,0 1 1,1 1 1,1 1 0)),' .
        '((0 1 0,0 0 0,0 0 1,0 1 1,0 1 0))' .
    '))'
);

if (!($cube instanceof SFCGALSolid)) { echo "FAIL: not a Solid\n"; exit(1); }
if (!$cube->is3D()) { echo "FAIL: cube !is3D\n"; exit(1); }

$v = $cube->volume();
if (abs($v - 1.0) > 1e-9) { echo "FAIL: volume=$v expected 1.0\n"; exit(1); }

$ps = SFCGALGeometry::readWKT(
    'POLYHEDRALSURFACE Z(' .
        '((0 0 0,0 1 0,1 1 0,1 0 0,0 0 0)),' .
        '((0 0 1,1 0 1,1 1 1,0 1 1,0 0 1)),' .
        '((0 0 0,1 0 0,1 0 1,0 0 1,0 0 0)),' .
        '((1 0 0,1 1 0,1 1 1,1 0 1,1 0 0)),' .
        '((1 1 0,0 1 0,0 1 1,1 1 1,1 1 0)),' .
        '((0 1 0,0 0 0,0 0 1,0 1 1,0 1 0))' .
    ')'
);
$a = $ps->area3D();
if (abs($a - 6.0) > 1e-9) { echo "FAIL: shell area3D=$a expected 6.0\n"; exit(1); }

$ext = SFCGALGeometry::readWKT('POLYGON((0 0 0,1 0 0,1 1 0,0 1 0,0 0 0))')->extrude(0, 0, 2.0);
if (!($ext instanceof SFCGALSolid)) { echo "FAIL: extrude did not produce a Solid\n"; exit(1); }
if (abs($ext->volume() - 2.0) > 1e-9) { echo "FAIL: extruded volume=" . $ext->volume() . "\n"; exit(1); }

echo "OK\n";
?>
--EXPECT--
OK
