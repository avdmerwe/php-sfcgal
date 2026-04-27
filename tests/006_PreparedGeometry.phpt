--TEST--
SFCGALPreparedGeometry — readEWKT, srid, asEWKT, binary I/O round-trip
--SKIPIF--
<?php if (!extension_loaded('sfcgal')) { print "sfcgal extension not loaded\n"; exit(1); } ?>
--FILE--
<?php

$p = SFCGALGeometry::readEWKT('SRID=4326;POINT(1 2 3)');
if (!($p instanceof SFCGALPreparedGeometry)) { echo "FAIL not prepared\n"; exit(1); }
if ($p->srid() !== 4326) { echo "FAIL srid=".$p->srid()."\n"; exit(1); }

$g = $p->geometry();
if (!($g instanceof SFCGALPoint)) { echo "FAIL geometry not Point\n"; exit(1); }

$ewkt = $p->asEWKT(0);
if (strpos($ewkt, 'SRID=4326') !== 0) { echo "FAIL ewkt $ewkt\n"; exit(1); }

$bin = $p->asBinary();
if (!is_string($bin) || strlen($bin) === 0) { echo "FAIL asBinary\n"; exit(1); }

$rt = SFCGALPreparedGeometry::fromBinary($bin);
if ($rt->srid() !== 4326) { echo "FAIL rt srid\n"; exit(1); }

$p->setSrid(3857);
if ($p->srid() !== 3857) { echo "FAIL setSrid\n"; exit(1); }

echo "OK\n";
?>
--EXPECT--
OK
