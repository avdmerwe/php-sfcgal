--TEST--
SFCGALException — invalid input throws, abstract class can't be constructed
--SKIPIF--
<?php if (!extension_loaded('sfcgal')) { print "sfcgal extension not loaded\n"; exit(1); } ?>
--FILE--
<?php

/* Bad WKT must throw SFCGALException */
$caught = false;
try {
    SFCGALGeometry::readWKT('not actually wkt');
} catch (SFCGALException $e) {
    $caught = true;
}
if (!$caught) { echo "FAIL: expected SFCGALException on bad WKT\n"; exit(1); }

/* SFCGALGeometry is abstract */
$caught = false;
try {
    $r = new ReflectionClass('SFCGALGeometry');
    if ($r->isAbstract()) { $caught = true; }
} catch (Throwable $e) {}
if (!$caught) { echo "FAIL: SFCGALGeometry should be abstract\n"; exit(1); }

/* Index out of range */
$ls = SFCGALGeometry::readWKT('LINESTRING(0 0,1 1)');
$caught = false;
try { $ls->pointN(99); } catch (SFCGALException $e) { $caught = true; }
if (!$caught) { echo "FAIL: out-of-range pointN should throw\n"; exit(1); }

echo "OK\n";
?>
--EXPECT--
OK
