/*
 * php-sfcgal — argument-info declarations.
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 * Mirrors the php-geos arginfo.h style: ZEND_BEGIN_ARG_INFO_EX with explicit
 * required-arg counts; one block per PHP-visible function/method.
 */

#ifndef PHP_SFCGAL_ARGINFO_H
#define PHP_SFCGAL_ARGINFO_H

/* -- module-level functions -------------------------------------------- */

ZEND_BEGIN_ARG_INFO_EX(arginfo_SFCGALVersion, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_SFCGALFullVersion, 0, 0, 0)
ZEND_END_ARG_INFO()

/* -- SFCGAL\Geometry --------------------------------------------------- */

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry___toString, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_typeId, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_geometryType, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_dimension, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_is3D, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_isMeasured, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_isEmpty, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_isValid, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_isSimple, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_isPlanar, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_asText, 0, 0, 0)
    ZEND_ARG_INFO(0, decimals)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_asWkb, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_asHexWkb, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_asObj, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_asStl, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_asVtk, 0, 0, 0)
ZEND_END_ARG_INFO()

/* Static factories: SFCGAL\Geometry::readWKT(...), readWKB(...) etc. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_readWKT, 0, 0, 1)
    ZEND_ARG_INFO(0, wkt)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_readWKB, 0, 0, 1)
    ZEND_ARG_INFO(0, wkb)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_readHexWKB, 0, 0, 1)
    ZEND_ARG_INFO(0, hexwkb)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_readEWKT, 0, 0, 1)
    ZEND_ARG_INFO(0, ewkt)
ZEND_END_ARG_INFO()

/* Predicates (binary, 3D) */

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_intersects3D, 0, 0, 1)
    ZEND_ARG_INFO(0, other)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_covers3D, 0, 0, 1)
    ZEND_ARG_INFO(0, other)
ZEND_END_ARG_INFO()

/* Measures (3D) */

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_distance3D, 0, 0, 1)
    ZEND_ARG_INFO(0, other)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_area3D, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_length3D, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_volume, 0, 0, 0)
ZEND_END_ARG_INFO()

/* Set ops (3D) */

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_intersection3D, 0, 0, 1)
    ZEND_ARG_INFO(0, other)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_difference3D, 0, 0, 1)
    ZEND_ARG_INFO(0, other)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_union3D, 0, 0, 1)
    ZEND_ARG_INFO(0, other)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_convexHull3D, 0, 0, 0)
ZEND_END_ARG_INFO()

/* Mesh ops */

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_tesselate, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_extrude, 0, 0, 3)
    ZEND_ARG_INFO(0, dx)
    ZEND_ARG_INFO(0, dy)
    ZEND_ARG_INFO(0, dz)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_makeSolid, 0, 0, 0)
ZEND_END_ARG_INFO()

/* Transforms */

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_forceZ, 0, 0, 0)
    ZEND_ARG_INFO(0, defaultZ)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_dropZ, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_forceM, 0, 0, 0)
    ZEND_ARG_INFO(0, defaultM)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_dropM, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_swapXY, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_forceLHR, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_forceRHR, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_round, 0, 0, 1)
    ZEND_ARG_INFO(0, scale)
ZEND_END_ARG_INFO()

/* Skeleton family */

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_straightSkeleton, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_approximateMedialAxis, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_extrudeStraightSkeleton, 0, 0, 1)
    ZEND_ARG_INFO(0, height)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_extrudePolygonStraightSkeleton, 0, 0, 2)
    ZEND_ARG_INFO(0, building_height)
    ZEND_ARG_INFO(0, roof_height)
ZEND_END_ARG_INFO()

/* Alpha */

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_alphaShapes, 0, 0, 0)
    ZEND_ARG_INFO(0, alpha)
    ZEND_ARG_INFO(0, allow_holes)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_optimalAlphaShapes, 0, 0, 0)
    ZEND_ARG_INFO(0, allow_holes)
    ZEND_ARG_INFO(0, nb_components)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_alphaWrapping3D, 0, 0, 1)
    ZEND_ARG_INFO(0, relative_alpha)
    ZEND_ARG_INFO(0, relative_offset)
ZEND_END_ARG_INFO()

/* Buffer */

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_buffer3D, 0, 0, 2)
    ZEND_ARG_INFO(0, radius)
    ZEND_ARG_INFO(0, segments)
    ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_minkowskiSum, 0, 0, 1)
    ZEND_ARG_INFO(0, other)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_simplify, 0, 0, 1)
    ZEND_ARG_INFO(0, threshold)
    ZEND_ARG_INFO(0, preserveTopology)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_lineSubstring, 0, 0, 2)
    ZEND_ARG_INFO(0, start)
    ZEND_ARG_INFO(0, end)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_translate3D, 0, 0, 3)
    ZEND_ARG_INFO(0, dx)
    ZEND_ARG_INFO(0, dy)
    ZEND_ARG_INFO(0, dz)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_scale3D, 0, 0, 3)
    ZEND_ARG_INFO(0, sx)
    ZEND_ARG_INFO(0, sy)
    ZEND_ARG_INFO(0, sz)
ZEND_END_ARG_INFO()

/* GEOS bridge */

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_fromGEOS, 0, 0, 1)
    ZEND_ARG_INFO(0, geos_geometry)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_toGEOS, 0, 0, 0)
ZEND_END_ARG_INFO()

/* PreparedGeometry */

ZEND_BEGIN_ARG_INFO_EX(arginfo_PreparedGeometry___construct, 0, 0, 0)
    ZEND_ARG_INFO(0, geometry)
    ZEND_ARG_INFO(0, srid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_PreparedGeometry_geometry, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_PreparedGeometry_srid, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_PreparedGeometry_setSrid, 0, 0, 1)
    ZEND_ARG_INFO(0, srid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_PreparedGeometry_asEWKT, 0, 0, 0)
    ZEND_ARG_INFO(0, decimals)
ZEND_END_ARG_INFO()

/* ---------- v1 expansion ---------- */

/* 2D twins of 3D ops */
ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_intersects, 0, 0, 1)
    ZEND_ARG_INFO(0, other)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_intersection, 0, 0, 1)
    ZEND_ARG_INFO(0, other)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_difference, 0, 0, 1)
    ZEND_ARG_INFO(0, other)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_union, 0, 0, 1)
    ZEND_ARG_INFO(0, other)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_convexHull, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_area, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_length, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_distance, 0, 0, 1)
    ZEND_ARG_INFO(0, other)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_covers, 0, 0, 1)
    ZEND_ARG_INFO(0, other)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_envelope, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_envelope3D, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_centroid, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_centroid3D, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_boundary, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_orientation, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_triangulate2DZ, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_offsetPolygon, 0, 0, 1)
    ZEND_ARG_INFO(0, radius)
ZEND_END_ARG_INFO()

/* Equality */
ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_equals, 0, 0, 1)
    ZEND_ARG_INFO(0, other)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_almostEquals, 0, 0, 2)
    ZEND_ARG_INFO(0, other)
    ZEND_ARG_INFO(0, tolerance)
ZEND_END_ARG_INFO()

/* Validity detail */
ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_validityDetail, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_simpleDetail, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_hasValidityFlag, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_forceValid, 0, 0, 0)
    ZEND_ARG_INFO(0, valid)
ZEND_END_ARG_INFO()

/* Skeleton extras */
ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_straightSkeletonDistanceInM, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_straightSkeletonPartition, 0, 0, 0)
    ZEND_ARG_INFO(0, autoOrientation)
ZEND_END_ARG_INFO()

/* Partition_2 family */
ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_yMonotonePartition2, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_approxConvexPartition2, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_greeneApproxConvexPartition2, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_optimalConvexPartition2, 0, 0, 0)
ZEND_END_ARG_INFO()

/* Visibility */
ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_visibilityPoint, 0, 0, 1)
    ZEND_ARG_INFO(0, point)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_visibilitySegment, 0, 0, 2)
    ZEND_ARG_INFO(0, pointA)
    ZEND_ARG_INFO(0, pointB)
ZEND_END_ARG_INFO()

/* Rotate / scale / translate */
ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_rotate, 0, 0, 1)
    ZEND_ARG_INFO(0, angle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_rotate2D, 0, 0, 3)
    ZEND_ARG_INFO(0, angle)
    ZEND_ARG_INFO(0, cx)
    ZEND_ARG_INFO(0, cy)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_rotate3D, 0, 0, 4)
    ZEND_ARG_INFO(0, angle)
    ZEND_ARG_INFO(0, ax)
    ZEND_ARG_INFO(0, ay)
    ZEND_ARG_INFO(0, az)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_rotate3DAroundCenter, 0, 0, 7)
    ZEND_ARG_INFO(0, angle)
    ZEND_ARG_INFO(0, ax)
    ZEND_ARG_INFO(0, ay)
    ZEND_ARG_INFO(0, az)
    ZEND_ARG_INFO(0, cx)
    ZEND_ARG_INFO(0, cy)
    ZEND_ARG_INFO(0, cz)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_rotateX, 0, 0, 1)
    ZEND_ARG_INFO(0, angle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_rotateY, 0, 0, 1)
    ZEND_ARG_INFO(0, angle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_rotateZ, 0, 0, 1)
    ZEND_ARG_INFO(0, angle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_scale, 0, 0, 1)
    ZEND_ARG_INFO(0, factor)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_scale3DAroundCenter, 0, 0, 6)
    ZEND_ARG_INFO(0, sx)
    ZEND_ARG_INFO(0, sy)
    ZEND_ARG_INFO(0, sz)
    ZEND_ARG_INFO(0, cx)
    ZEND_ARG_INFO(0, cy)
    ZEND_ARG_INFO(0, cz)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_translate2D, 0, 0, 2)
    ZEND_ARG_INFO(0, dx)
    ZEND_ARG_INFO(0, dy)
ZEND_END_ARG_INFO()

/* I/O variants */
ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_writeVtkFile, 0, 0, 1)
    ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_writeObjFile, 0, 0, 1)
    ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_writeStlFile, 0, 0, 1)
    ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

/* Hierarchy accessors */
ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_numGeometries, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Geometry_geometryN, 0, 0, 1)
    ZEND_ARG_INFO(0, n)
ZEND_END_ARG_INFO()

/* Point accessors / factories */
ZEND_BEGIN_ARG_INFO_EX(arginfo_Point_create, 0, 0, 0)
    ZEND_ARG_INFO(0, x)
    ZEND_ARG_INFO(0, y)
    ZEND_ARG_INFO(0, z)
    ZEND_ARG_INFO(0, m)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Point_x, 0, 0, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_Point_y, 0, 0, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_Point_z, 0, 0, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_Point_m, 0, 0, 0)
ZEND_END_ARG_INFO()

/* LineString factories / mutators */
ZEND_BEGIN_ARG_INFO_EX(arginfo_LineString_create, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_LineString_numPoints, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_LineString_pointN, 0, 0, 1)
    ZEND_ARG_INFO(0, n)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_LineString_addPoint, 0, 0, 1)
    ZEND_ARG_INFO(0, point)
ZEND_END_ARG_INFO()

/* Triangle */
ZEND_BEGIN_ARG_INFO_EX(arginfo_Triangle_create, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Triangle_createFromPoints, 0, 0, 3)
    ZEND_ARG_INFO(0, a)
    ZEND_ARG_INFO(0, b)
    ZEND_ARG_INFO(0, c)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Triangle_vertex, 0, 0, 1)
    ZEND_ARG_INFO(0, n)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Triangle_setVertex, 0, 0, 2)
    ZEND_ARG_INFO(0, n)
    ZEND_ARG_INFO(0, point)
ZEND_END_ARG_INFO()

/* Polygon */
ZEND_BEGIN_ARG_INFO_EX(arginfo_Polygon_create, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Polygon_createFromExteriorRing, 0, 0, 1)
    ZEND_ARG_INFO(0, ring)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Polygon_exteriorRing, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Polygon_numInteriorRings, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Polygon_interiorRingN, 0, 0, 1)
    ZEND_ARG_INFO(0, n)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Polygon_addInteriorRing, 0, 0, 1)
    ZEND_ARG_INFO(0, ring)
ZEND_END_ARG_INFO()

/* Collection-style mutators */
ZEND_BEGIN_ARG_INFO_EX(arginfo_GeometryCollection_create, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_GeometryCollection_addGeometry, 0, 0, 1)
    ZEND_ARG_INFO(0, geometry)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_GeometryCollection_setGeometryN, 0, 0, 2)
    ZEND_ARG_INFO(0, n)
    ZEND_ARG_INFO(0, geometry)
ZEND_END_ARG_INFO()

/* PolyhedralSurface */
ZEND_BEGIN_ARG_INFO_EX(arginfo_PolyhedralSurface_create, 0, 0, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_PolyhedralSurface_numPatches, 0, 0, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_PolyhedralSurface_patchN, 0, 0, 1)
    ZEND_ARG_INFO(0, n)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_PolyhedralSurface_addPatch, 0, 0, 1)
    ZEND_ARG_INFO(0, polygon)
ZEND_END_ARG_INFO()

/* TriangulatedSurface */
ZEND_BEGIN_ARG_INFO_EX(arginfo_TriangulatedSurface_create, 0, 0, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_TriangulatedSurface_numPatches, 0, 0, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_TriangulatedSurface_patchN, 0, 0, 1)
    ZEND_ARG_INFO(0, n)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_TriangulatedSurface_addPatch, 0, 0, 1)
    ZEND_ARG_INFO(0, triangle)
ZEND_END_ARG_INFO()

/* Solid */
ZEND_BEGIN_ARG_INFO_EX(arginfo_Solid_create, 0, 0, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_Solid_createFromExteriorShell, 0, 0, 1)
    ZEND_ARG_INFO(0, shell)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_Solid_numShells, 0, 0, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_Solid_shellN, 0, 0, 1)
    ZEND_ARG_INFO(0, n)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_Solid_addInteriorShell, 0, 0, 1)
    ZEND_ARG_INFO(0, shell)
ZEND_END_ARG_INFO()

/* PreparedGeometry binary I/O */
ZEND_BEGIN_ARG_INFO_EX(arginfo_PreparedGeometry_setGeometry, 0, 0, 1)
    ZEND_ARG_INFO(0, geometry)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_PreparedGeometry_asBinary, 0, 0, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_PreparedGeometry_fromBinary, 0, 0, 1)
    ZEND_ARG_INFO(0, bytes)
ZEND_END_ARG_INFO()

#endif /* PHP_SFCGAL_ARGINFO_H */
