--TEST--
Geometry — readWKT, asText, typeId, dimension, classification
--SKIPIF--
<?php if (!extension_loaded('sfcgal')) { print "sfcgal extension not loaded\n"; exit(1); } ?>
--FILE--
<?php

$pt = SFCGALGeometry::readWKT('POINT(1 2 3)');
if (!($pt instanceof SFCGALPoint))      { echo "FAIL: not a Point\n"; exit(1); }
if ($pt->typeId() !== SFCGALGeometry::TYPE_POINT) { echo "FAIL: typeId\n"; exit(1); }
if ($pt->geometryType() !== 'Point')    { echo "FAIL: geometryType ".$pt->geometryType()."\n"; exit(1); }
if ($pt->dimension() !== 0)             { echo "FAIL: dim\n"; exit(1); }
if (!$pt->is3D())                       { echo "FAIL: is3D\n"; exit(1); }
if ($pt->isMeasured())                  { echo "FAIL: isMeasured\n"; exit(1); }
if ($pt->isEmpty())                     { echo "FAIL: isEmpty\n"; exit(1); }

if ($pt->x() !== 1.0 || $pt->y() !== 2.0 || $pt->z() !== 3.0) {
    echo "FAIL: point coords ".$pt->x().",".$pt->y().",".$pt->z()."\n"; exit(1);
}

$ls = SFCGALGeometry::readWKT('LINESTRING(0 0 0,1 1 1,2 2 2)');
if ($ls->dimension() !== 1)             { echo "FAIL: ls dim\n"; exit(1); }
if (!($ls instanceof SFCGALLineString)) { echo "FAIL: not a LineString\n"; exit(1); }
if ($ls->numPoints() !== 3)             { echo "FAIL: ls numPoints ".$ls->numPoints()."\n"; exit(1); }

$p1 = $ls->pointN(1);
if (!($p1 instanceof SFCGALPoint))      { echo "FAIL: pointN not a Point\n"; exit(1); }
if ($p1->x() !== 1.0)                   { echo "FAIL: pointN x\n"; exit(1); }

$poly = SFCGALGeometry::readWKT('POLYGON((0 0,1 0,1 1,0 1,0 0))');
if (!($poly instanceof SFCGALPolygon))  { echo "FAIL: not a Polygon\n"; exit(1); }
if ($poly->dimension() !== 2)           { echo "FAIL: poly dim\n"; exit(1); }
if ($poly->numInteriorRings() !== 0)    { echo "FAIL: numInteriorRings\n"; exit(1); }

$ext = $poly->exteriorRing();
if (!($ext instanceof SFCGALLineString)) { echo "FAIL: exteriorRing\n"; exit(1); }
if ($ext->numPoints() !== 5)            { echo "FAIL: exterior numPoints\n"; exit(1); }

$txt = $poly->asText(0);
if (strpos($txt, 'POLYGON') !== 0) { echo "FAIL: poly text $txt\n"; exit(1); }

$wkb = $poly->asWkb();
$rt  = SFCGALGeometry::readWKB($wkb);
if (!($rt instanceof SFCGALPolygon))    { echo "FAIL: wkb roundtrip\n"; exit(1); }

$hex = $poly->asHexWkb();
$rt2 = SFCGALGeometry::readHexWKB($hex);
if (!($rt2 instanceof SFCGALPolygon))   { echo "FAIL: hexwkb roundtrip\n"; exit(1); }

echo "OK\n";
?>
--EXPECT--
OK
