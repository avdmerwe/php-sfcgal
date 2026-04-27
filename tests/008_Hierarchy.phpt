--TEST--
Hierarchy — numGeometries, geometryN polymorphic over collection types
--SKIPIF--
<?php if (!extension_loaded('sfcgal')) { print "sfcgal extension not loaded\n"; exit(1); } ?>
--FILE--
<?php

$mp = SFCGALGeometry::readWKT('MULTIPOINT((0 0),(1 1),(2 2))');
if ($mp->numGeometries() !== 3) { echo "FAIL multipoint count\n"; exit(1); }
$p = $mp->geometryN(1);
if (!($p instanceof SFCGALPoint))  { echo "FAIL multipoint child class\n"; exit(1); }
if ($p->x() !== 1.0)              { echo "FAIL multipoint child coord\n"; exit(1); }

$gc = SFCGALGeometryCollection::create();
$gc->addGeometry(SFCGALPoint::create(0, 0))
   ->addGeometry(SFCGALPoint::create(5, 5));
if ($gc->numGeometries() !== 2) { echo "FAIL collection count\n"; exit(1); }

/* Polygon: numGeometries = 1 (exterior) + N interior; geometryN(0) is the exterior ring */
$poly = SFCGALGeometry::readWKT('POLYGON((0 0,10 0,10 10,0 10,0 0),(2 2,3 2,3 3,2 3,2 2))');
if ($poly->numGeometries() !== 2) { echo "FAIL poly numGeometries\n"; exit(1); }
$ext = $poly->geometryN(0);
if (!($ext instanceof SFCGALLineString)) { echo "FAIL poly[0] !LineString\n"; exit(1); }

echo "OK\n";
?>
--EXPECT--
OK
