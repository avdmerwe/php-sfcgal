<?php
// SPDX-License-Identifier: LGPL-2.0-or-later

require_once 'PHPUnit/Autoload.php';

abstract class SFCGALTest extends PHPUnit_Framework_TestCase
{
    protected function readWKT(string $wkt): \SFCGAL\Geometry
    {
        return \SFCGAL\Geometry::readWKT($wkt);
    }

    protected function assertGeometryEquals(string $expectedWkt, \SFCGAL\Geometry $actual, int $decimals = 6): void
    {
        $expected = \SFCGAL\Geometry::readWKT($expectedWkt);
        $this->assertSame(
            $expected->asText($decimals),
            $actual->asText($decimals),
            "Geometries differ"
        );
    }
}
