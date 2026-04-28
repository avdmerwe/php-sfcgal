--TEST--
1.0.2 bug-fix regressions: distance3D Solid path, forceRHR/forceLHR, extrudePolygonStraightSkeleton
--SKIPIF--
<?php if (!extension_loaded('sfcgal')) { print "sfcgal extension not loaded\n"; exit(1); } ?>
--FILE--
<?php

/* ----- Bug 1: distance3D returned 0 across the buggy upstream paths. ----- */
$cube = SFCGALGeometry::readWKT('POLYGON((0 0,1 0,1 1,0 1,0 0))')->extrude(0, 0, 1);
$cube_high = $cube->translate3D(0, 0, 5);

if (abs($cube->distance3D($cube_high) - 4.0) > 1e-9) {
    echo "FAIL: Solid<->Solid distance3D got ", $cube->distance3D($cube_high), "\n"; exit(1);
}

$pt_above = SFCGALGeometry::readWKT('POINT Z(0 0 5)');
if (abs($cube->distance3D($pt_above) - 4.0) > 1e-9) {
    echo "FAIL: Solid<->Point(close) distance3D got ", $cube->distance3D($pt_above), "\n"; exit(1);
}

$pt_far = SFCGALGeometry::readWKT('POINT Z(10 10 10)');
$expected = sqrt(9*9 + 9*9 + 9*9);  /* point to (1,1,1) corner of cube */
if (abs($cube->distance3D($pt_far) - $expected) > 1e-6) {
    echo "FAIL: Solid<->Point(far) distance3D got ", $cube->distance3D($pt_far), "\n"; exit(1);
}

$slice = SFCGALGeometry::readWKT('POLYGON Z((0 0 5,1 0 5,1 1 5,0 1 5,0 0 5))');
if (abs($cube->distance3D($slice) - 4.0) > 1e-9) {
    echo "FAIL: Solid<->Polygon Z distance3D got ", $cube->distance3D($slice), "\n"; exit(1);
}

/* Polygon-vs-Polygon at different Z (used to return 0) */
$bottom = SFCGALGeometry::readWKT('POLYGON Z((0 0 0,1 0 0,1 1 0,0 1 0,0 0 0))');
if (abs($bottom->distance3D($slice) - 5.0) > 1e-9) {
    echo "FAIL: Polygon Z<->Polygon Z (vertical) got ", $bottom->distance3D($slice), "\n"; exit(1);
}

/* Sanity baselines that already worked in 1.0.1 — must keep working. */
$p1 = SFCGALGeometry::readWKT('POINT Z(0 0 0)');
$p2 = SFCGALGeometry::readWKT('POINT Z(0 0 5)');
if (abs($p1->distance3D($p2) - 5.0) > 1e-9) {
    echo "FAIL: Point<->Point regression\n"; exit(1);
}
$line = SFCGALGeometry::readWKT('LINESTRING Z(2 0 0,2 1 0)');
if (abs($p1->distance3D($line) - 2.0) > 1e-9) {
    echo "FAIL: Point<->LineString regression\n"; exit(1);
}

/* ----- Bug 2: forceRHR / forceLHR were no-ops (silently leaked the clone). -- */
$ccw = SFCGALGeometry::readWKT('POLYGON((0 0,1 0,1 1,0 1,0 0))');
if ($ccw->orientation() !== -1)        { echo "FAIL: ccw orient setup\n"; exit(1); }

$rhr = $ccw->forceRHR();
if (!($rhr instanceof SFCGALPolygon))   { echo "FAIL: forceRHR class\n"; exit(1); }
if ($rhr->orientation() !== 1)          { echo "FAIL: forceRHR not applied (still ".$rhr->orientation().")\n"; exit(1); }
if ($ccw->orientation() !== -1)        { echo "FAIL: forceRHR mutated source (should return new)\n"; exit(1); }

$cw = SFCGALGeometry::readWKT('POLYGON((0 0,0 1,1 1,1 0,0 0))');
if ($cw->orientation() !== 1)          { echo "FAIL: cw orient setup\n"; exit(1); }
$lhr = $cw->forceLHR();
if ($lhr->orientation() !== -1)        { echo "FAIL: forceLHR not applied\n"; exit(1); }

/* ----- Bug 3: extrudePolygonStraightSkeleton documented as Solid; was returning Surface. */
$lot   = SFCGALGeometry::readWKT('POLYGON((0 0,10 0,10 10,0 10,0 0))');
$house = $lot->extrudePolygonStraightSkeleton(3.0, 2.0);
if (!($house instanceof SFCGALSolid))   { echo "FAIL: house class ".get_class($house)."\n"; exit(1); }
if ($house->typeId() !== SFCGALGeometry::TYPE_SOLID) { echo "FAIL: house typeId\n"; exit(1); }
if ($house->volume() <= 0)              { echo "FAIL: house volume=".$house->volume()."\n"; exit(1); }

/* extrude unchanged */
$ext = SFCGALGeometry::readWKT('POLYGON((0 0,1 0,1 1,0 1,0 0))')->extrude(0, 0, 5);
if (!($ext instanceof SFCGALSolid))     { echo "FAIL: extrude not Solid\n"; exit(1); }

echo "OK\n";
?>
--EXPECT--
OK
