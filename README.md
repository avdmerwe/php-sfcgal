# php-sfcgal — PHP bindings for SFCGAL

PHP bindings for [SFCGAL](https://sfcgal.gitlab.io/SFCGAL/) — ISO 19107:2013 /
OGC SFA 1.2 3D operations on top of CGAL. Companion to
[php-geos](https://github.com/avdmerwe/php-geos) for the 3D operations GEOS
does not provide.

## Building

```sh
phpize
./configure --enable-sfcgal --with-sfcgal-config=/path/to/sfcgal-config
make -j$(nproc)
make test
```

## Class summary (flat global namespace, matching php-geos style)

- `SFCGALGeometry` — abstract base. Static factories `::readWKT($wkt)`,
  `::readWKB($wkb)`, `::readHexWKB($hex)`, `::readEWKT($ewkt)`,
  `::fromGEOS($g)`. Concrete subclasses one per OGC type:
  `SFCGALPoint`, `SFCGALLineString`, `SFCGALPolygon`, `SFCGALMultiPoint`,
  `SFCGALMultiLineString`, `SFCGALMultiPolygon`, `SFCGALGeometryCollection`,
  `SFCGALTriangle`, `SFCGALTriangulatedSurface`, `SFCGALPolyhedralSurface`,
  `SFCGALSolid`, `SFCGALMultiSolid`.
- `SFCGALPreparedGeometry` — SRID-bearing wrapper.
- `SFCGALException` — thrown on SFCGAL error.

## Quick example

```php
$cube = SFCGALGeometry::readWKT('POLYGON((0 0 0,1 0 0,1 1 0,0 1 0,0 0 0))')
    ->extrude(0, 0, 1);
echo $cube->volume();           // 1.0
echo get_class($cube);          // SFCGALSolid

$p = SFCGALPoint::create(1, 2, 3);
$q = $p->translate3D(10, 0, 0); // new SFCGALPoint at (11,2,3)
```

## Bridge to/from php-geos

```php
$g = GEOSWKTReader::read('POLYGON((...))');
$s = SFCGALGeometry::fromGEOS($g);
$v = $s->extrude(0, 0, 10)->volume();
$g2 = $s->toGEOS();
```

Curved geometry types (`CircularString`, `CompoundCurve`, `CurvePolygon`,
`MultiCurve`, `MultiSurface`) throw on `fromGEOS()` — call
`GEOSGeometry::curveToLine()` first.  This mirrors PostGIS's `LWGEOM2SFCGAL`
behaviour.

## License

LGPL-2.0-or-later. See `LICENSE` and `COPYING`.
