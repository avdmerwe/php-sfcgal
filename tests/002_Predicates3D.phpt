--TEST--
Predicates 3D — intersects3D, distance3D, covers3D
--SKIPIF--
<?php if (!extension_loaded('sfcgal')) { print "sfcgal extension not loaded\n"; exit(1); } ?>
--FILE--
<?php

$a = SFCGALGeometry::readWKT('TRIANGLE((0 0 0,1 0 0,0 1 0,0 0 0))');
$b = SFCGALGeometry::readWKT('TRIANGLE((0 0 1,1 0 1,0 1 1,0 0 1))');

if ($a->intersects3D($b)) { echo "FAIL: parallel triangles 1u apart should not intersect\n"; exit(1); }

$d = $a->distance3D($b);
if (abs($d - 1.0) > 1e-9) { echo "FAIL: distance3D=$d expected 1.0\n"; exit(1); }

$c = SFCGALGeometry::readWKT('TRIANGLE((0 0 0,2 0 0,0 2 0,0 0 0))');
if (!$a->intersects3D($c)) { echo "FAIL: coplanar overlapping triangles must intersect\n"; exit(1); }

if (!$c->covers3D($a)) { echo "FAIL: bigger triangle must cover smaller (3D)\n"; exit(1); }

echo "OK\n";
?>
--EXPECT--
OK
