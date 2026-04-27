--TEST--
Transforms — translate, rotate, scale, swapXY, force/drop Z/M
--SKIPIF--
<?php if (!extension_loaded('sfcgal')) { print "sfcgal extension not loaded\n"; exit(1); } ?>
--FILE--
<?php

$p = SFCGALPoint::create(1, 0, 0);

/* translate3D returns a NEW geometry */
$t = $p->translate3D(10, 20, 30);
if ($t === $p) { echo "FAIL translate3D returned same object\n"; exit(1); }
if (abs($t->x() - 11) > 1e-9) { echo "FAIL t.x=".$t->x()."\n"; exit(1); }
if (abs($t->y() - 20) > 1e-9) { echo "FAIL t.y=".$t->y()."\n"; exit(1); }
if (abs($t->z() - 30) > 1e-9) { echo "FAIL t.z=".$t->z()."\n"; exit(1); }
/* Original is untouched */
if ($p->x() !== 1.0 || $p->y() !== 0.0) { echo "FAIL p mutated\n"; exit(1); }

/* rotate Z by 90° → x=1,y=0 → x=0,y=1 (approx) */
$r = $p->rotateZ(M_PI / 2);
if (abs($r->x() - 0.0) > 1e-9 || abs($r->y() - 1.0) > 1e-9) {
    echo "FAIL rotateZ x=".$r->x()." y=".$r->y()."\n"; exit(1);
}

/* scale */
$s = $p->scale(2.5);
if ($s->x() !== 2.5) { echo "FAIL scale\n"; exit(1); }

/* swapXY is in-place; coords swap on a copy */
$pp = SFCGALGeometry::readWKT('POINT(7 9)');
$pp->swapXY();
if ($pp->x() !== 9.0 || $pp->y() !== 7.0) { echo "FAIL swapXY\n"; exit(1); }

/* dropZ removes the Z dimension in place */
$z = SFCGALPoint::create(1, 2, 3);
if (!$z->is3D()) { echo "FAIL z !is3D\n"; exit(1); }
$z->dropZ();
if ($z->is3D()) { echo "FAIL dropZ\n"; exit(1); }

echo "OK\n";
?>
--EXPECT--
OK
