--TEST--
GEOS bridge — auto-decompose Solid/Surfaces; decomposeToFaces; BUFFER3D constants; Multi* addGeometry
--SKIPIF--
<?php
if (!extension_loaded('sfcgal')) { print "sfcgal extension not loaded\n"; exit(1); }
if (!extension_loaded('geos'))   { print "geos extension not loaded\n"; exit(1); }
?>
--FILE--
<?php

/* BUFFER3D class constants (Gap #4) */
if (SFCGALGeometry::BUFFER3D_ROUND     !== 0) { echo "FAIL ROUND\n"; exit(1); }
if (SFCGALGeometry::BUFFER3D_CYLSPHERE !== 1) { echo "FAIL CYLSPHERE\n"; exit(1); }
if (SFCGALGeometry::BUFFER3D_FLAT      !== 2) { echo "FAIL FLAT\n"; exit(1); }

/* Multi* now extend SFCGALGeometryCollection (binding bug fix) */
$mp = SFCGALMultiPolygon::create();
if (!($mp instanceof SFCGALGeometryCollection)) { echo "FAIL MP not GC\n"; exit(1); }
$mp->addGeometry(SFCGALGeometry::readWKT('POLYGON((0 0,1 0,1 1,0 1,0 0))'));
$mp->addGeometry(SFCGALGeometry::readWKT('POLYGON((2 2,3 2,3 3,2 3,2 2))'));
if ($mp->numGeometries() !== 2) { echo "FAIL MP add\n"; exit(1); }

$ms = SFCGALMultiSolid::create();
if (!($ms instanceof SFCGALGeometryCollection)) { echo "FAIL MultiSolid not GC\n"; exit(1); }

/* decomposeToFaces (Gap #1 explicit path) */
$cube = SFCGALGeometry::readWKT('POLYGON((0 0 0,1 0 0,1 1 0,0 1 0,0 0 0))')->extrude(0, 0, 1);
$faces = $cube->decomposeToFaces();
if (!($faces instanceof SFCGALMultiPolygon))     { echo "FAIL faces type\n"; exit(1); }
if ($faces->numGeometries() !== 6)               { echo "FAIL face count ".$faces->numGeometries()."\n"; exit(1); }
if (!$faces->is3D())                             { echo "FAIL faces !is3D\n"; exit(1); }

/* TIN → MultiPolygon */
$tin = SFCGALGeometry::readWKT('POLYGON((0 0 0,4 0 0,4 4 0,0 4 0,0 0 0))')->tesselate();
if (!($tin instanceof SFCGALTriangulatedSurface)) { echo "FAIL tin\n"; exit(1); }
$tinMp = $tin->decomposeToFaces();
if (!($tinMp instanceof SFCGALMultiPolygon))     { echo "FAIL tin faces type\n"; exit(1); }
if ($tinMp->numGeometries() < 1)                 { echo "FAIL tin face count\n"; exit(1); }

/* Triangle → MultiPolygon (single polygon) */
$tri = SFCGALGeometry::readWKT('TRIANGLE((0 0 0,1 0 0,0 1 0,0 0 0))');
$triMp = $tri->decomposeToFaces();
if ($triMp->numGeometries() !== 1)               { echo "FAIL triangle face count\n"; exit(1); }

/* GEOS bridge auto-decompose: toGEOS on a Solid (Gap #1 implicit path) */
$g = $cube->toGEOS();
if ($g->typeName() !== 'MultiPolygon')           { echo "FAIL toGEOS type ".$g->typeName()."\n"; exit(1); }
if (!$g->hasZ())                                 { echo "FAIL toGEOS !hasZ\n"; exit(1); }

/* Round-trip Solid → GEOS → SFCGAL — lossy on solid identity, MP-typed back */
$rt = SFCGALGeometry::fromGEOS($cube->toGEOS());
if (!($rt instanceof SFCGALMultiPolygon))        { echo "FAIL roundtrip ".get_class($rt)."\n"; exit(1); }
if ($rt->numGeometries() !== 6)                  { echo "FAIL roundtrip face count\n"; exit(1); }

/* Plain Polygon round-trip preserves type */
$p = SFCGALGeometry::readWKT('POLYGON((0 0,10 0,10 10,0 10,0 0))');
$prt = SFCGALGeometry::fromGEOS($p->toGEOS());
if (!($prt instanceof SFCGALPolygon))            { echo "FAIL polygon roundtrip\n"; exit(1); }

/* fromGEOS still rejects curves */
$caught = false;
try {
    $cs = (new GEOSWKTReader)->read('CIRCULARSTRING(0 0, 1 1, 2 0)');
    SFCGALGeometry::fromGEOS($cs);
} catch (SFCGALException $e) { $caught = true; }
if (!$caught) { echo "FAIL: curve should throw\n"; exit(1); }

/* force3DM / force4D composition (Gap #5) */
$pt = SFCGALGeometry::readWKT('POINT ZM(1 2 3 9)');
$xym  = SFCGALGeometry::readWKT($pt->asText(0))->dropZ()->forceM(0.0);
$xyzm = SFCGALGeometry::readWKT($pt->asText(0))->forceZ(0.0)->forceM(0.0);
if ($xym->is3D() || !$xym->isMeasured())         { echo "FAIL force3DM dim\n"; exit(1); }
if (!$xyzm->is3D() || !$xyzm->isMeasured())      { echo "FAIL force4D dim\n"; exit(1); }

echo "OK\n";
?>
--EXPECT--
OK
