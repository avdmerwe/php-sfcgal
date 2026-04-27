--TEST--
Typed factories — SFCGALPoint::create, LineString::addPoint, Polygon::createFromExteriorRing, Solid::createFromExteriorShell
--SKIPIF--
<?php if (!extension_loaded('sfcgal')) { print "sfcgal extension not loaded\n"; exit(1); } ?>
--FILE--
<?php

/* Point factories */
$p2 = SFCGALPoint::create(1, 2);
$p3 = SFCGALPoint::create(1, 2, 3);
$p4 = SFCGALPoint::create(1, 2, 3, 4);
if ($p2->is3D()       || $p2->isMeasured()) { echo "FAIL p2\n"; exit(1); }
if (!$p3->is3D()      || $p3->isMeasured()) { echo "FAIL p3\n"; exit(1); }
if (!$p4->is3D()      || !$p4->isMeasured()){ echo "FAIL p4\n"; exit(1); }
if ($p4->m() !== 4.0) { echo "FAIL p4.m=".$p4->m()."\n"; exit(1); }

/* LineString building */
$ls = SFCGALLineString::create();
$ls->addPoint(SFCGALPoint::create(0, 0, 0))
   ->addPoint(SFCGALPoint::create(1, 0, 0))
   ->addPoint(SFCGALPoint::create(1, 1, 0));
if ($ls->numPoints() !== 3) { echo "FAIL ls numPoints\n"; exit(1); }

/* Closed ring → Polygon */
$ring = SFCGALLineString::create();
$ring->addPoint(SFCGALPoint::create(0, 0, 0))
     ->addPoint(SFCGALPoint::create(1, 0, 0))
     ->addPoint(SFCGALPoint::create(1, 1, 0))
     ->addPoint(SFCGALPoint::create(0, 1, 0))
     ->addPoint(SFCGALPoint::create(0, 0, 0));
$poly = SFCGALPolygon::createFromExteriorRing($ring);
if (!($poly instanceof SFCGALPolygon)) { echo "FAIL polygon\n"; exit(1); }
if ($poly->numInteriorRings() !== 0)   { echo "FAIL no holes\n"; exit(1); }

/* Solid extrude → numShells == 1 */
$solid = $poly->extrude(0, 0, 5);
if ($solid->numShells() !== 1) { echo "FAIL solid shells\n"; exit(1); }

echo "OK\n";
?>
--EXPECT--
OK
