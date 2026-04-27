--TEST--
General — extension load + version reporting
--SKIPIF--
<?php if (!extension_loaded('sfcgal')) { print "sfcgal extension not loaded\n"; exit(1); } ?>
--FILE--
<?php

$v  = SFCGALVersion();
$fv = SFCGALFullVersion();

if (!is_string($v)  || $v  === '') { echo "FAIL version empty\n";      exit(1); }
if (!is_string($fv) || $fv === '') { echo "FAIL full version empty\n"; exit(1); }

if (version_compare($v, '2.2.0', '<')) { echo "FAIL: SFCGAL too old: $v\n"; exit(1); }

foreach ([
    'SFCGALGeometry', 'SFCGALPoint', 'SFCGALLineString', 'SFCGALPolygon',
    'SFCGALMultiPoint', 'SFCGALMultiLineString', 'SFCGALMultiPolygon',
    'SFCGALGeometryCollection', 'SFCGALTriangle', 'SFCGALTriangulatedSurface',
    'SFCGALPolyhedralSurface', 'SFCGALSolid', 'SFCGALMultiSolid',
    'SFCGALPreparedGeometry', 'SFCGALException',
] as $cls) {
    if (!class_exists($cls)) { echo "FAIL: $cls missing\n"; exit(1); }
}

/* Type-id constants */
if (SFCGALGeometry::TYPE_POINT  !== 1) { echo "FAIL TYPE_POINT\n";  exit(1); }
if (SFCGALGeometry::TYPE_SOLID  !== 101) { echo "FAIL TYPE_SOLID\n"; exit(1); }

echo "OK\n";
?>
--EXPECT--
OK
