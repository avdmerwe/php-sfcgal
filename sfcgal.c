/*
 * php-sfcgal — PHP bindings for SFCGAL.
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 * Copyright (C) 2026 Abraham van der Merwe <abz@frogfoot.com>
 *
 * Patterns mirror php-geos (Sandro Santilli, J Smith) — Proxy struct,
 * ZTS-aware globals, error handlers — adapted to SFCGAL's C API.
 */

/* PHP */
#include "php.h"
#include "ext/standard/info.h"
#include "Zend/zend_exceptions.h"

/* SFCGAL */
#include <SFCGAL/capi/sfcgal_c.h>

/* Own */
#include "php_sfcgal.h"
#include "arginfo.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================
 * Globals + module entry
 * ============================================================ */

static ZEND_DECLARE_MODULE_GLOBALS(sfcgal)

/* Forward decls of class entries */
static zend_class_entry *Geometry_ce;
static zend_class_entry *Point_ce;
static zend_class_entry *LineString_ce;
static zend_class_entry *Polygon_ce;
static zend_class_entry *MultiPoint_ce;
static zend_class_entry *MultiLineString_ce;
static zend_class_entry *MultiPolygon_ce;
static zend_class_entry *GeometryCollection_ce;
static zend_class_entry *Triangle_ce;
static zend_class_entry *TriangulatedSurface_ce;
static zend_class_entry *PolyhedralSurface_ce;
static zend_class_entry *Solid_ce;
static zend_class_entry *MultiSolid_ce;
static zend_class_entry *PreparedGeometry_ce;
static zend_class_entry *Exception_ce;

static zend_object_handlers Geometry_object_handlers;
static zend_object_handlers PreparedGeometry_object_handlers;

/* Forward decls */
PHP_MINIT_FUNCTION(sfcgal);
PHP_MSHUTDOWN_FUNCTION(sfcgal);
PHP_RINIT_FUNCTION(sfcgal);
PHP_RSHUTDOWN_FUNCTION(sfcgal);
PHP_MINFO_FUNCTION(sfcgal);
PHP_GINIT_FUNCTION(sfcgal);

PHP_FUNCTION(SFCGALVersion);
PHP_FUNCTION(SFCGALFullVersion);

/* Geometry methods */
PHP_METHOD(Geometry, __construct);
PHP_METHOD(Geometry, __toString);
PHP_METHOD(Geometry, typeId);
PHP_METHOD(Geometry, geometryType);
PHP_METHOD(Geometry, dimension);
PHP_METHOD(Geometry, is3D);
PHP_METHOD(Geometry, isMeasured);
PHP_METHOD(Geometry, isEmpty);
PHP_METHOD(Geometry, isValid);
PHP_METHOD(Geometry, isSimple);
PHP_METHOD(Geometry, isPlanar);
PHP_METHOD(Geometry, asText);
PHP_METHOD(Geometry, asWkb);
PHP_METHOD(Geometry, asHexWkb);
PHP_METHOD(Geometry, asObj);
PHP_METHOD(Geometry, asStl);
PHP_METHOD(Geometry, asVtk);
PHP_METHOD(Geometry, readWKT);
PHP_METHOD(Geometry, readWKB);
PHP_METHOD(Geometry, readHexWKB);
PHP_METHOD(Geometry, readEWKT);
PHP_METHOD(Geometry, intersects3D);
PHP_METHOD(Geometry, covers3D);
PHP_METHOD(Geometry, distance3D);
PHP_METHOD(Geometry, area3D);
PHP_METHOD(Geometry, length3D);
PHP_METHOD(Geometry, volume);
PHP_METHOD(Geometry, intersection3D);
PHP_METHOD(Geometry, difference3D);
PHP_METHOD(Geometry, union3D);
PHP_METHOD(Geometry, convexHull3D);
PHP_METHOD(Geometry, tesselate);
PHP_METHOD(Geometry, extrude);
PHP_METHOD(Geometry, makeSolid);
PHP_METHOD(Geometry, forceZ);
PHP_METHOD(Geometry, dropZ);
PHP_METHOD(Geometry, forceM);
PHP_METHOD(Geometry, dropM);
PHP_METHOD(Geometry, swapXY);
PHP_METHOD(Geometry, forceLHR);
PHP_METHOD(Geometry, forceRHR);
PHP_METHOD(Geometry, round);
PHP_METHOD(Geometry, straightSkeleton);
PHP_METHOD(Geometry, approximateMedialAxis);
PHP_METHOD(Geometry, extrudeStraightSkeleton);
PHP_METHOD(Geometry, extrudePolygonStraightSkeleton);
PHP_METHOD(Geometry, alphaShapes);
PHP_METHOD(Geometry, optimalAlphaShapes);
PHP_METHOD(Geometry, alphaWrapping3D);
PHP_METHOD(Geometry, buffer3D);
PHP_METHOD(Geometry, minkowskiSum);
PHP_METHOD(Geometry, simplify);
PHP_METHOD(Geometry, lineSubstring);
PHP_METHOD(Geometry, translate3D);
PHP_METHOD(Geometry, scale3D);
PHP_METHOD(Geometry, fromGEOS);
PHP_METHOD(Geometry, toGEOS);
PHP_METHOD(Geometry, decomposeToFaces);

/* Forward decl — defined after the wrap helpers because it walks the
 * SFCGAL hierarchy and is used by toGEOS to bridge 3D-only types. */
static sfcgal_geometry_t *build_face_multipolygon(const sfcgal_geometry_t *g);

/* php-geos constructors initialise the C relay; object_init_ex alone is
 * not enough.  Invoke __construct explicitly on a freshly-allocated zval. */
static int
php_sfcgal_call_default_ctor(zval *obj)
{
    zval method, rv;
    ZVAL_STRINGL(&method, "__construct", sizeof("__construct") - 1);
    int r = call_user_function(NULL, obj, &method, &rv, 0, NULL);
    zval_ptr_dtor(&method);
    zval_ptr_dtor(&rv);
    return r;
}

/* PreparedGeometry methods */
PHP_METHOD(PreparedGeometry, __construct);
PHP_METHOD(PreparedGeometry, geometry);
PHP_METHOD(PreparedGeometry, srid);
PHP_METHOD(PreparedGeometry, setSrid);
PHP_METHOD(PreparedGeometry, asEWKT);
PHP_METHOD(PreparedGeometry, setGeometry);
PHP_METHOD(PreparedGeometry, asBinary);
PHP_METHOD(PreparedGeometry, fromBinary);

/* Geometry — v1 expansion */
PHP_METHOD(Geometry, intersects);
PHP_METHOD(Geometry, intersection);
PHP_METHOD(Geometry, difference);
PHP_METHOD(Geometry, union);
PHP_METHOD(Geometry, convexHull);
PHP_METHOD(Geometry, area);
PHP_METHOD(Geometry, length);
PHP_METHOD(Geometry, distance);
PHP_METHOD(Geometry, covers);
PHP_METHOD(Geometry, envelope);
PHP_METHOD(Geometry, envelope3D);
PHP_METHOD(Geometry, centroid);
PHP_METHOD(Geometry, centroid3D);
PHP_METHOD(Geometry, boundary);
PHP_METHOD(Geometry, orientation);
PHP_METHOD(Geometry, triangulate2DZ);
PHP_METHOD(Geometry, offsetPolygon);
PHP_METHOD(Geometry, equals);
PHP_METHOD(Geometry, almostEquals);
PHP_METHOD(Geometry, validityDetail);
PHP_METHOD(Geometry, simpleDetail);
PHP_METHOD(Geometry, hasValidityFlag);
PHP_METHOD(Geometry, forceValid);
PHP_METHOD(Geometry, straightSkeletonDistanceInM);
PHP_METHOD(Geometry, straightSkeletonPartition);
PHP_METHOD(Geometry, yMonotonePartition2);
PHP_METHOD(Geometry, approxConvexPartition2);
PHP_METHOD(Geometry, greeneApproxConvexPartition2);
PHP_METHOD(Geometry, optimalConvexPartition2);
PHP_METHOD(Geometry, visibilityPoint);
PHP_METHOD(Geometry, visibilitySegment);
PHP_METHOD(Geometry, rotate);
PHP_METHOD(Geometry, rotate2D);
PHP_METHOD(Geometry, rotate3D);
PHP_METHOD(Geometry, rotate3DAroundCenter);
PHP_METHOD(Geometry, rotateX);
PHP_METHOD(Geometry, rotateY);
PHP_METHOD(Geometry, rotateZ);
PHP_METHOD(Geometry, scale);
PHP_METHOD(Geometry, scale3DAroundCenter);
PHP_METHOD(Geometry, translate2D);
PHP_METHOD(Geometry, writeVtkFile);
PHP_METHOD(Geometry, writeObjFile);
PHP_METHOD(Geometry, writeStlFile);
PHP_METHOD(Geometry, numGeometries);
PHP_METHOD(Geometry, geometryN);

/* Typed factories / accessors */
PHP_METHOD(Point, create);
PHP_METHOD(Point, x);
PHP_METHOD(Point, y);
PHP_METHOD(Point, z);
PHP_METHOD(Point, m);

PHP_METHOD(LineString, create);
PHP_METHOD(LineString, numPoints);
PHP_METHOD(LineString, pointN);
PHP_METHOD(LineString, addPoint);

PHP_METHOD(Triangle, create);
PHP_METHOD(Triangle, createFromPoints);
PHP_METHOD(Triangle, vertex);
PHP_METHOD(Triangle, setVertex);

PHP_METHOD(Polygon, create);
PHP_METHOD(Polygon, createFromExteriorRing);
PHP_METHOD(Polygon, exteriorRing);
PHP_METHOD(Polygon, numInteriorRings);
PHP_METHOD(Polygon, interiorRingN);
PHP_METHOD(Polygon, addInteriorRing);

PHP_METHOD(GeometryCollection, create);
PHP_METHOD(GeometryCollection, addGeometry);
PHP_METHOD(GeometryCollection, setGeometryN);

PHP_METHOD(PolyhedralSurface, create);
PHP_METHOD(PolyhedralSurface, numPatches);
PHP_METHOD(PolyhedralSurface, patchN);
PHP_METHOD(PolyhedralSurface, addPatch);

PHP_METHOD(TriangulatedSurface, create);
PHP_METHOD(TriangulatedSurface, numPatches);
PHP_METHOD(TriangulatedSurface, patchN);
PHP_METHOD(TriangulatedSurface, addPatch);

PHP_METHOD(Solid, create);
PHP_METHOD(Solid, createFromExteriorShell);
PHP_METHOD(Solid, numShells);
PHP_METHOD(Solid, shellN);
PHP_METHOD(Solid, addInteriorShell);

/* MultiPoint/MultiLineString/MultiPolygon/MultiSolid empty factories */
PHP_METHOD(MultiPoint, create);
PHP_METHOD(MultiLineString, create);
PHP_METHOD(MultiPolygon, create);
PHP_METHOD(MultiSolid, create);

/* ============================================================
 * Module skeleton
 * ============================================================ */

static const zend_function_entry sfcgal_functions[] = {
    PHP_FE(SFCGALVersion,     arginfo_SFCGALVersion)
    PHP_FE(SFCGALFullVersion, arginfo_SFCGALFullVersion)
    PHP_FE_END
};

zend_module_entry sfcgal_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_SFCGAL_EXTNAME,
    sfcgal_functions,
    PHP_MINIT(sfcgal),
    PHP_MSHUTDOWN(sfcgal),
    PHP_RINIT(sfcgal),
    PHP_RSHUTDOWN(sfcgal),
    PHP_MINFO(sfcgal),
    PHP_SFCGAL_VERSION,
    PHP_MODULE_GLOBALS(sfcgal),
    PHP_GINIT(sfcgal),
    NULL,                       /* GSHUTDOWN */
    NULL,                       /* post-deactivate */
    STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_SFCGAL
ZEND_GET_MODULE(sfcgal)
#endif

/* ============================================================
 * Error / warning handlers
 *
 * SFCGAL's C API takes two function pointers (warning, error). They are
 * vararg-style printf-like callbacks (sfcgal_error_handler_t).
 * On error we throw SFCGAL\Exception so PHP code can catch it.
 * ============================================================ */

static int php_sfcgal_warning_handler(const char *fmt, ...)
{
    char message[512];
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(message, sizeof(message) - 1, fmt, args);
    va_end(args);
    php_error_docref(NULL, E_WARNING, "%s", message);
    return n;
}

static int php_sfcgal_error_handler(const char *fmt, ...)
{
    char message[512];
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(message, sizeof(message) - 1, fmt, args);
    va_end(args);
    zend_throw_exception(Exception_ce, message, 0);
    return n;
}

/* ============================================================
 * Proxy object — holds an opaque SFCGAL pointer for a Zend object.
 * ============================================================ */

typedef struct {
    void *relay;            /* sfcgal_geometry_t* or sfcgal_prepared_geometry_t* */
    int   owns_relay;       /* 1 = call sfcgal_*_delete on dtor; 0 = borrowed */
    int   kind;             /* 0=geometry, 1=prepared */
    zend_object std;
} php_sfcgal_obj;

static inline php_sfcgal_obj *php_sfcgal_fetch_object(zend_object *obj)
{
    return (php_sfcgal_obj *)((char *) obj - XtOffsetOf(php_sfcgal_obj, std));
}
#define Z_SFCGAL_OBJ_P(zv) php_sfcgal_fetch_object(Z_OBJ_P(zv))

/* Called when the PHP GC frees a Geometry/PreparedGeometry. */
static void php_sfcgal_free_obj(zend_object *object)
{
    php_sfcgal_obj *intern = php_sfcgal_fetch_object(object);

    if (intern->relay && intern->owns_relay) {
        if (intern->kind == 0) {
            sfcgal_geometry_delete((sfcgal_geometry_t *) intern->relay);
        } else if (intern->kind == 1) {
            sfcgal_prepared_geometry_delete((sfcgal_prepared_geometry_t *) intern->relay);
        }
        intern->relay = NULL;
    }

    zend_object_std_dtor(object);
}

/* Generic object-create. The `kind` argument tags Geometry vs PreparedGeometry. */
static zend_object *php_sfcgal_create_object_kind(zend_class_entry *ce, int kind)
{
    php_sfcgal_obj *intern = (php_sfcgal_obj *)
        ecalloc(1, sizeof(php_sfcgal_obj) + zend_object_properties_size(ce));

    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);

    intern->relay = NULL;
    intern->owns_relay = 0;
    intern->kind = kind;

    intern->std.handlers = (kind == 1)
        ? &PreparedGeometry_object_handlers
        : &Geometry_object_handlers;

    return &intern->std;
}

static zend_object *Geometry_create_object(zend_class_entry *ce)
{
    return php_sfcgal_create_object_kind(ce, 0);
}

static zend_object *PreparedGeometry_create_object(zend_class_entry *ce)
{
    return php_sfcgal_create_object_kind(ce, 1);
}

/* Pick the right concrete PHP class for a given SFCGAL type-id. */
static zend_class_entry *
geometry_ce_for_type(sfcgal_geometry_type_t t)
{
    switch (t) {
        case SFCGAL_TYPE_POINT:              return Point_ce;
        case SFCGAL_TYPE_LINESTRING:         return LineString_ce;
        case SFCGAL_TYPE_POLYGON:            return Polygon_ce;
        case SFCGAL_TYPE_MULTIPOINT:         return MultiPoint_ce;
        case SFCGAL_TYPE_MULTILINESTRING:    return MultiLineString_ce;
        case SFCGAL_TYPE_MULTIPOLYGON:       return MultiPolygon_ce;
        case SFCGAL_TYPE_GEOMETRYCOLLECTION: return GeometryCollection_ce;
        case SFCGAL_TYPE_TRIANGLE:           return Triangle_ce;
        case SFCGAL_TYPE_TRIANGULATEDSURFACE:return TriangulatedSurface_ce;
        case SFCGAL_TYPE_POLYHEDRALSURFACE:  return PolyhedralSurface_ce;
        case SFCGAL_TYPE_SOLID:              return Solid_ce;
        case SFCGAL_TYPE_MULTISOLID:         return MultiSolid_ce;
        default:                             return Geometry_ce;
    }
}

/* Wrap a freshly-allocated sfcgal_geometry_t* in a new PHP object.
 * The new PHP object takes ownership (owns_relay=1).
 * Allocation failure throws and returns FAILURE. */
static int
php_sfcgal_wrap_geometry(zval *out, sfcgal_geometry_t *g)
{
    if (!g) {
        zend_throw_exception(Exception_ce, "SFCGAL returned NULL geometry", 0);
        return FAILURE;
    }
    sfcgal_geometry_type_t t = sfcgal_geometry_type_id(g);
    zend_class_entry *ce = geometry_ce_for_type(t);

    object_init_ex(out, ce);
    php_sfcgal_obj *intern = Z_SFCGAL_OBJ_P(out);
    intern->relay = g;
    intern->owns_relay = 1;
    intern->kind = 0;
    return SUCCESS;
}

static int
php_sfcgal_wrap_prepared(zval *out, sfcgal_prepared_geometry_t *p)
{
    if (!p) {
        zend_throw_exception(Exception_ce, "SFCGAL returned NULL prepared geometry", 0);
        return FAILURE;
    }
    object_init_ex(out, PreparedGeometry_ce);
    php_sfcgal_obj *intern = Z_SFCGAL_OBJ_P(out);
    intern->relay = p;
    intern->owns_relay = 1;
    intern->kind = 1;
    return SUCCESS;
}

/* Fetch sfcgal_geometry_t* from a zval that must be an SFCGAL\Geometry. */
static sfcgal_geometry_t *
php_sfcgal_get_geom(zval *zv)
{
    if (Z_TYPE_P(zv) != IS_OBJECT
        || !instanceof_function(Z_OBJCE_P(zv), Geometry_ce)) {
        zend_throw_exception(Exception_ce,
            "Expected SFCGALGeometry instance", 0);
        return NULL;
    }
    php_sfcgal_obj *intern = Z_SFCGAL_OBJ_P(zv);
    if (!intern->relay) {
        zend_throw_exception(Exception_ce,
            "SFCGALGeometry has no underlying geometry", 0);
        return NULL;
    }
    return (sfcgal_geometry_t *) intern->relay;
}

static sfcgal_geometry_t *
php_sfcgal_get_self(zval *zv)
{
    php_sfcgal_obj *intern = Z_SFCGAL_OBJ_P(zv);
    if (!intern->relay) {
        zend_throw_exception(Exception_ce,
            "SFCGALGeometry has no underlying geometry", 0);
        return NULL;
    }
    return (sfcgal_geometry_t *) intern->relay;
}

/* ============================================================
 * Method tables — declared up here so MINIT can register them.
 * ============================================================ */

static const zend_function_entry Geometry_methods[] = {
    PHP_ME(Geometry, __construct,             arginfo_Geometry___construct,             ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Geometry, __toString,              arginfo_Geometry___toString,              ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, typeId,                  arginfo_Geometry_typeId,                  ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, geometryType,            arginfo_Geometry_geometryType,            ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, dimension,               arginfo_Geometry_dimension,               ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, is3D,                    arginfo_Geometry_is3D,                    ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, isMeasured,              arginfo_Geometry_isMeasured,              ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, isEmpty,                 arginfo_Geometry_isEmpty,                 ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, isValid,                 arginfo_Geometry_isValid,                 ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, isSimple,                arginfo_Geometry_isSimple,                ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, isPlanar,                arginfo_Geometry_isPlanar,                ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, asText,                  arginfo_Geometry_asText,                  ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, asWkb,                   arginfo_Geometry_asWkb,                   ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, asHexWkb,                arginfo_Geometry_asHexWkb,                ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, asObj,                   arginfo_Geometry_asObj,                   ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, asStl,                   arginfo_Geometry_asStl,                   ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, asVtk,                   arginfo_Geometry_asVtk,                   ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, readWKT,                 arginfo_Geometry_readWKT,                 ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Geometry, readWKB,                 arginfo_Geometry_readWKB,                 ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Geometry, readHexWKB,              arginfo_Geometry_readHexWKB,              ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Geometry, readEWKT,                arginfo_Geometry_readEWKT,                ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Geometry, intersects3D,            arginfo_Geometry_intersects3D,            ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, covers3D,                arginfo_Geometry_covers3D,                ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, distance3D,              arginfo_Geometry_distance3D,              ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, area3D,                  arginfo_Geometry_area3D,                  ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, length3D,                arginfo_Geometry_length3D,                ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, volume,                  arginfo_Geometry_volume,                  ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, intersection3D,          arginfo_Geometry_intersection3D,          ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, difference3D,            arginfo_Geometry_difference3D,            ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, union3D,                 arginfo_Geometry_union3D,                 ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, convexHull3D,            arginfo_Geometry_convexHull3D,            ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, tesselate,               arginfo_Geometry_tesselate,               ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, extrude,                 arginfo_Geometry_extrude,                 ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, makeSolid,               arginfo_Geometry_makeSolid,               ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, forceZ,                  arginfo_Geometry_forceZ,                  ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, dropZ,                   arginfo_Geometry_dropZ,                   ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, forceM,                  arginfo_Geometry_forceM,                  ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, dropM,                   arginfo_Geometry_dropM,                   ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, swapXY,                  arginfo_Geometry_swapXY,                  ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, forceLHR,                arginfo_Geometry_forceLHR,                ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, forceRHR,                arginfo_Geometry_forceRHR,                ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, round,                   arginfo_Geometry_round,                   ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, straightSkeleton,        arginfo_Geometry_straightSkeleton,        ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, approximateMedialAxis,   arginfo_Geometry_approximateMedialAxis,   ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, extrudeStraightSkeleton, arginfo_Geometry_extrudeStraightSkeleton, ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, extrudePolygonStraightSkeleton, arginfo_Geometry_extrudePolygonStraightSkeleton, ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, alphaShapes,             arginfo_Geometry_alphaShapes,             ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, optimalAlphaShapes,      arginfo_Geometry_optimalAlphaShapes,      ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, alphaWrapping3D,         arginfo_Geometry_alphaWrapping3D,         ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, buffer3D,                arginfo_Geometry_buffer3D,                ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, minkowskiSum,            arginfo_Geometry_minkowskiSum,            ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, simplify,                arginfo_Geometry_simplify,                ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, lineSubstring,           arginfo_Geometry_lineSubstring,           ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, translate3D,             arginfo_Geometry_translate3D,             ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, scale3D,                 arginfo_Geometry_scale3D,                 ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, fromGEOS,                arginfo_Geometry_fromGEOS,                ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Geometry, toGEOS,                  arginfo_Geometry_toGEOS,                  ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, decomposeToFaces,        arginfo_Geometry_decomposeToFaces,        ZEND_ACC_PUBLIC)

    /* v1 expansion */
    PHP_ME(Geometry, intersects,              arginfo_Geometry_intersects,              ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, intersection,            arginfo_Geometry_intersection,            ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, difference,              arginfo_Geometry_difference,              ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, union,                   arginfo_Geometry_union,                   ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, convexHull,              arginfo_Geometry_convexHull,              ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, area,                    arginfo_Geometry_area,                    ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, length,                  arginfo_Geometry_length,                  ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, distance,                arginfo_Geometry_distance,                ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, covers,                  arginfo_Geometry_covers,                  ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, envelope,                arginfo_Geometry_envelope,                ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, envelope3D,              arginfo_Geometry_envelope3D,              ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, centroid,                arginfo_Geometry_centroid,                ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, centroid3D,              arginfo_Geometry_centroid3D,              ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, boundary,                arginfo_Geometry_boundary,                ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, orientation,             arginfo_Geometry_orientation,             ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, triangulate2DZ,          arginfo_Geometry_triangulate2DZ,          ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, offsetPolygon,           arginfo_Geometry_offsetPolygon,           ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, equals,                  arginfo_Geometry_equals,                  ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, almostEquals,            arginfo_Geometry_almostEquals,            ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, validityDetail,          arginfo_Geometry_validityDetail,          ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, simpleDetail,            arginfo_Geometry_simpleDetail,            ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, hasValidityFlag,         arginfo_Geometry_hasValidityFlag,         ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, forceValid,              arginfo_Geometry_forceValid,              ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, straightSkeletonDistanceInM, arginfo_Geometry_straightSkeletonDistanceInM, ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, straightSkeletonPartition,   arginfo_Geometry_straightSkeletonPartition,   ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, yMonotonePartition2,         arginfo_Geometry_yMonotonePartition2,         ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, approxConvexPartition2,      arginfo_Geometry_approxConvexPartition2,      ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, greeneApproxConvexPartition2,arginfo_Geometry_greeneApproxConvexPartition2,ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, optimalConvexPartition2,     arginfo_Geometry_optimalConvexPartition2,     ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, visibilityPoint,             arginfo_Geometry_visibilityPoint,             ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, visibilitySegment,           arginfo_Geometry_visibilitySegment,           ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, rotate,                  arginfo_Geometry_rotate,                  ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, rotate2D,                arginfo_Geometry_rotate2D,                ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, rotate3D,                arginfo_Geometry_rotate3D,                ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, rotate3DAroundCenter,    arginfo_Geometry_rotate3DAroundCenter,    ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, rotateX,                 arginfo_Geometry_rotateX,                 ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, rotateY,                 arginfo_Geometry_rotateY,                 ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, rotateZ,                 arginfo_Geometry_rotateZ,                 ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, scale,                   arginfo_Geometry_scale,                   ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, scale3DAroundCenter,     arginfo_Geometry_scale3DAroundCenter,     ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, translate2D,             arginfo_Geometry_translate2D,             ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, writeVtkFile,            arginfo_Geometry_writeVtkFile,            ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, writeObjFile,            arginfo_Geometry_writeObjFile,            ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, writeStlFile,            arginfo_Geometry_writeStlFile,            ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, numGeometries,           arginfo_Geometry_numGeometries,           ZEND_ACC_PUBLIC)
    PHP_ME(Geometry, geometryN,               arginfo_Geometry_geometryN,               ZEND_ACC_PUBLIC)
    PHP_FE_END
};

static const zend_function_entry PreparedGeometry_methods[] = {
    PHP_ME(PreparedGeometry, __construct, arginfo_PreparedGeometry___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(PreparedGeometry, geometry,    arginfo_PreparedGeometry_geometry,    ZEND_ACC_PUBLIC)
    PHP_ME(PreparedGeometry, srid,        arginfo_PreparedGeometry_srid,        ZEND_ACC_PUBLIC)
    PHP_ME(PreparedGeometry, setSrid,     arginfo_PreparedGeometry_setSrid,     ZEND_ACC_PUBLIC)
    PHP_ME(PreparedGeometry, asEWKT,      arginfo_PreparedGeometry_asEWKT,      ZEND_ACC_PUBLIC)
    PHP_ME(PreparedGeometry, setGeometry, arginfo_PreparedGeometry_setGeometry, ZEND_ACC_PUBLIC)
    PHP_ME(PreparedGeometry, asBinary,    arginfo_PreparedGeometry_asBinary,    ZEND_ACC_PUBLIC)
    PHP_ME(PreparedGeometry, fromBinary,  arginfo_PreparedGeometry_fromBinary,  ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};

static const zend_function_entry Point_methods[] = {
    PHP_ME(Point, create, arginfo_Point_create, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Point, x,      arginfo_Point_x,      ZEND_ACC_PUBLIC)
    PHP_ME(Point, y,      arginfo_Point_y,      ZEND_ACC_PUBLIC)
    PHP_ME(Point, z,      arginfo_Point_z,      ZEND_ACC_PUBLIC)
    PHP_ME(Point, m,      arginfo_Point_m,      ZEND_ACC_PUBLIC)
    PHP_FE_END
};

static const zend_function_entry LineString_methods[] = {
    PHP_ME(LineString, create,    arginfo_LineString_create,    ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(LineString, numPoints, arginfo_LineString_numPoints, ZEND_ACC_PUBLIC)
    PHP_ME(LineString, pointN,    arginfo_LineString_pointN,    ZEND_ACC_PUBLIC)
    PHP_ME(LineString, addPoint,  arginfo_LineString_addPoint,  ZEND_ACC_PUBLIC)
    PHP_FE_END
};

static const zend_function_entry Triangle_methods[] = {
    PHP_ME(Triangle, create,            arginfo_Triangle_create,            ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Triangle, createFromPoints,  arginfo_Triangle_createFromPoints,  ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Triangle, vertex,            arginfo_Triangle_vertex,            ZEND_ACC_PUBLIC)
    PHP_ME(Triangle, setVertex,         arginfo_Triangle_setVertex,         ZEND_ACC_PUBLIC)
    PHP_FE_END
};

static const zend_function_entry Polygon_methods[] = {
    PHP_ME(Polygon, create,                  arginfo_Polygon_create,                  ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Polygon, createFromExteriorRing,  arginfo_Polygon_createFromExteriorRing,  ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Polygon, exteriorRing,            arginfo_Polygon_exteriorRing,            ZEND_ACC_PUBLIC)
    PHP_ME(Polygon, numInteriorRings,        arginfo_Polygon_numInteriorRings,        ZEND_ACC_PUBLIC)
    PHP_ME(Polygon, interiorRingN,           arginfo_Polygon_interiorRingN,           ZEND_ACC_PUBLIC)
    PHP_ME(Polygon, addInteriorRing,         arginfo_Polygon_addInteriorRing,         ZEND_ACC_PUBLIC)
    PHP_FE_END
};

static const zend_function_entry GeometryCollection_methods[] = {
    PHP_ME(GeometryCollection, create,         arginfo_GeometryCollection_create,        ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(GeometryCollection, addGeometry,    arginfo_GeometryCollection_addGeometry,   ZEND_ACC_PUBLIC)
    PHP_ME(GeometryCollection, setGeometryN,   arginfo_GeometryCollection_setGeometryN,  ZEND_ACC_PUBLIC)
    PHP_FE_END
};

static const zend_function_entry MultiPoint_methods[] = {
    PHP_ME(MultiPoint, create, arginfo_GeometryCollection_create, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};
static const zend_function_entry MultiLineString_methods[] = {
    PHP_ME(MultiLineString, create, arginfo_GeometryCollection_create, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};
static const zend_function_entry MultiPolygon_methods[] = {
    PHP_ME(MultiPolygon, create, arginfo_GeometryCollection_create, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};
static const zend_function_entry MultiSolid_methods[] = {
    PHP_ME(MultiSolid, create, arginfo_GeometryCollection_create, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};

static const zend_function_entry PolyhedralSurface_methods[] = {
    PHP_ME(PolyhedralSurface, create,     arginfo_PolyhedralSurface_create,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(PolyhedralSurface, numPatches, arginfo_PolyhedralSurface_numPatches, ZEND_ACC_PUBLIC)
    PHP_ME(PolyhedralSurface, patchN,     arginfo_PolyhedralSurface_patchN,     ZEND_ACC_PUBLIC)
    PHP_ME(PolyhedralSurface, addPatch,   arginfo_PolyhedralSurface_addPatch,   ZEND_ACC_PUBLIC)
    PHP_FE_END
};

static const zend_function_entry TriangulatedSurface_methods[] = {
    PHP_ME(TriangulatedSurface, create,     arginfo_TriangulatedSurface_create,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(TriangulatedSurface, numPatches, arginfo_TriangulatedSurface_numPatches, ZEND_ACC_PUBLIC)
    PHP_ME(TriangulatedSurface, patchN,     arginfo_TriangulatedSurface_patchN,     ZEND_ACC_PUBLIC)
    PHP_ME(TriangulatedSurface, addPatch,   arginfo_TriangulatedSurface_addPatch,   ZEND_ACC_PUBLIC)
    PHP_FE_END
};

static const zend_function_entry Solid_methods[] = {
    PHP_ME(Solid, create,                   arginfo_Solid_create,                   ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Solid, createFromExteriorShell,  arginfo_Solid_createFromExteriorShell,  ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Solid, numShells,                arginfo_Solid_numShells,                ZEND_ACC_PUBLIC)
    PHP_ME(Solid, shellN,                   arginfo_Solid_shellN,                   ZEND_ACC_PUBLIC)
    PHP_ME(Solid, addInteriorShell,         arginfo_Solid_addInteriorShell,         ZEND_ACC_PUBLIC)
    PHP_FE_END
};

/* ============================================================
 * Module init / shutdown
 * ============================================================ */

PHP_GINIT_FUNCTION(sfcgal)
{
    sfcgal_globals->initialized = 0;
}

PHP_MINIT_FUNCTION(sfcgal)
{
    zend_class_entry ce;

    /* Exception class — global namespace, mirroring GEOSException-style. */
    INIT_CLASS_ENTRY(ce, "SFCGALException", NULL);
    Exception_ce = zend_register_internal_class_ex(&ce, zend_ce_exception);

    /* Base Geometry */
    INIT_CLASS_ENTRY(ce, "SFCGALGeometry", Geometry_methods);
    Geometry_ce = zend_register_internal_class(&ce);
    Geometry_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
    Geometry_ce->create_object = Geometry_create_object;

    memcpy(&Geometry_object_handlers, zend_get_std_object_handlers(),
           sizeof(zend_object_handlers));
    Geometry_object_handlers.offset    = XtOffsetOf(php_sfcgal_obj, std);
    Geometry_object_handlers.free_obj  = php_sfcgal_free_obj;
    Geometry_object_handlers.clone_obj = NULL; /* TODO: deep clone */

    /* Concrete subclasses — flat global names: SFCGALPoint, SFCGALLineString, ... */
#define SFCGAL_REG_SUBCLASS(name, methods, var)                              \
    do {                                                                     \
        INIT_CLASS_ENTRY(ce, name, methods);                                 \
        var = zend_register_internal_class_ex(&ce, Geometry_ce);             \
        var->create_object = Geometry_create_object;                         \
    } while (0)

    SFCGAL_REG_SUBCLASS("SFCGALPoint",               Point_methods,               Point_ce);
    SFCGAL_REG_SUBCLASS("SFCGALLineString",          LineString_methods,          LineString_ce);
    SFCGAL_REG_SUBCLASS("SFCGALPolygon",             Polygon_methods,             Polygon_ce);
    SFCGAL_REG_SUBCLASS("SFCGALGeometryCollection",  GeometryCollection_methods,  GeometryCollection_ce);
    SFCGAL_REG_SUBCLASS("SFCGALTriangle",            Triangle_methods,            Triangle_ce);
    SFCGAL_REG_SUBCLASS("SFCGALTriangulatedSurface", TriangulatedSurface_methods, TriangulatedSurface_ce);
    SFCGAL_REG_SUBCLASS("SFCGALPolyhedralSurface",   PolyhedralSurface_methods,   PolyhedralSurface_ce);
    SFCGAL_REG_SUBCLASS("SFCGALSolid",               Solid_methods,               Solid_ce);
#undef SFCGAL_REG_SUBCLASS

    /* Multi* extend SFCGALGeometryCollection (per OGC SF) so they inherit
     * addGeometry / setGeometryN.  Each defines its own ::create() that
     * constructs the typed empty container. */
#define SFCGAL_REG_MULTI(name, methods, var)                                 \
    do {                                                                     \
        INIT_CLASS_ENTRY(ce, name, methods);                                 \
        var = zend_register_internal_class_ex(&ce, GeometryCollection_ce);   \
        var->create_object = Geometry_create_object;                         \
    } while (0)

    SFCGAL_REG_MULTI("SFCGALMultiPoint",      MultiPoint_methods,      MultiPoint_ce);
    SFCGAL_REG_MULTI("SFCGALMultiLineString", MultiLineString_methods, MultiLineString_ce);
    SFCGAL_REG_MULTI("SFCGALMultiPolygon",    MultiPolygon_methods,    MultiPolygon_ce);
    SFCGAL_REG_MULTI("SFCGALMultiSolid",      MultiSolid_methods,      MultiSolid_ce);
#undef SFCGAL_REG_MULTI

    /* PreparedGeometry — separate handlers because dtor uses prepared API. */
    INIT_CLASS_ENTRY(ce, "SFCGALPreparedGeometry", PreparedGeometry_methods);
    PreparedGeometry_ce = zend_register_internal_class(&ce);
    PreparedGeometry_ce->create_object = PreparedGeometry_create_object;

    memcpy(&PreparedGeometry_object_handlers, zend_get_std_object_handlers(),
           sizeof(zend_object_handlers));
    PreparedGeometry_object_handlers.offset    = XtOffsetOf(php_sfcgal_obj, std);
    PreparedGeometry_object_handlers.free_obj  = php_sfcgal_free_obj;
    PreparedGeometry_object_handlers.clone_obj = NULL;

    /* Type-id class constants on Geometry */
#define SFCGAL_REG_LCONST(name, val) \
    zend_declare_class_constant_long(Geometry_ce, name, sizeof(name) - 1, (val))

    SFCGAL_REG_LCONST("TYPE_POINT",              SFCGAL_TYPE_POINT);
    SFCGAL_REG_LCONST("TYPE_LINESTRING",         SFCGAL_TYPE_LINESTRING);
    SFCGAL_REG_LCONST("TYPE_POLYGON",            SFCGAL_TYPE_POLYGON);
    SFCGAL_REG_LCONST("TYPE_MULTIPOINT",         SFCGAL_TYPE_MULTIPOINT);
    SFCGAL_REG_LCONST("TYPE_MULTILINESTRING",    SFCGAL_TYPE_MULTILINESTRING);
    SFCGAL_REG_LCONST("TYPE_MULTIPOLYGON",       SFCGAL_TYPE_MULTIPOLYGON);
    SFCGAL_REG_LCONST("TYPE_GEOMETRYCOLLECTION", SFCGAL_TYPE_GEOMETRYCOLLECTION);
    SFCGAL_REG_LCONST("TYPE_POLYHEDRALSURFACE",  SFCGAL_TYPE_POLYHEDRALSURFACE);
    SFCGAL_REG_LCONST("TYPE_TRIANGULATEDSURFACE",SFCGAL_TYPE_TRIANGULATEDSURFACE);
    SFCGAL_REG_LCONST("TYPE_TRIANGLE",           SFCGAL_TYPE_TRIANGLE);
    SFCGAL_REG_LCONST("TYPE_SOLID",              SFCGAL_TYPE_SOLID);
    SFCGAL_REG_LCONST("TYPE_MULTISOLID",         SFCGAL_TYPE_MULTISOLID);

    /* buffer3D() type-arg constants — match sfcgal_buffer3d_type_t. */
    SFCGAL_REG_LCONST("BUFFER3D_ROUND",           SFCGAL_BUFFER3D_ROUND);
    SFCGAL_REG_LCONST("BUFFER3D_CYLSPHERE",       SFCGAL_BUFFER3D_CYLSPHERE);
    SFCGAL_REG_LCONST("BUFFER3D_FLAT",            SFCGAL_BUFFER3D_FLAT);
#undef SFCGAL_REG_LCONST

    /* Initialize SFCGAL once per process. */
    if (!SFCGAL_G(initialized)) {
        sfcgal_init();
        sfcgal_set_error_handlers(php_sfcgal_warning_handler,
                                  php_sfcgal_error_handler);
        SFCGAL_G(initialized) = 1;
    }

    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(sfcgal)
{
    /* SFCGAL has no explicit shutdown. */
    return SUCCESS;
}

PHP_RINIT_FUNCTION(sfcgal)    { return SUCCESS; }
PHP_RSHUTDOWN_FUNCTION(sfcgal){ return SUCCESS; }

PHP_MINFO_FUNCTION(sfcgal)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "sfcgal support", "enabled");
    php_info_print_table_row(2, "php-sfcgal version", PHP_SFCGAL_VERSION);
    php_info_print_table_row(2, "SFCGAL library version", sfcgal_version());
    php_info_print_table_row(2, "SFCGAL full version", sfcgal_full_version());
    php_info_print_table_end();
}

/* ============================================================
 * Module-level functions
 * ============================================================ */

PHP_FUNCTION(SFCGALVersion)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    RETURN_STRING(sfcgal_version());
}

PHP_FUNCTION(SFCGALFullVersion)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    RETURN_STRING(sfcgal_full_version());
}

/* ============================================================
 * SFCGAL\Geometry — methods
 * ============================================================ */

PHP_METHOD(Geometry, __construct)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    /* Direct construction is not supported — use the static readers / factories. */
    zend_throw_exception(Exception_ce,
        "SFCGALGeometry cannot be constructed directly; use ::readWKT(), ::readWKB(), ::readEWKT(), ::readHexWKB() or ::fromGEOS()", 0);
}

PHP_METHOD(Geometry, __toString)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    char *buf = NULL; size_t len = 0;
    sfcgal_geometry_as_text(g, &buf, &len);
    if (!buf) RETURN_EMPTY_STRING();
    RETVAL_STRINGL(buf, len);
    free(buf);
}

PHP_METHOD(Geometry, typeId)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    RETURN_LONG((long) sfcgal_geometry_type_id(g));
}

PHP_METHOD(Geometry, geometryType)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    char *buf = NULL; size_t len = 0;
    sfcgal_geometry_type(g, &buf, &len);
    if (!buf) RETURN_EMPTY_STRING();
    RETVAL_STRINGL(buf, len);
    free(buf);
}

PHP_METHOD(Geometry, dimension)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    RETURN_LONG(sfcgal_geometry_dimension(g));
}

#define SFCGAL_BOOL_PREDICATE(name, capi)                                        \
    PHP_METHOD(Geometry, name)                                                   \
    {                                                                            \
        if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();            \
        sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());                   \
        if (!g) RETURN_THROWS();                                                 \
        RETURN_BOOL(capi(g) != 0);                                               \
    }

SFCGAL_BOOL_PREDICATE(is3D,       sfcgal_geometry_is_3d)
SFCGAL_BOOL_PREDICATE(isMeasured, sfcgal_geometry_is_measured)
SFCGAL_BOOL_PREDICATE(isEmpty,    sfcgal_geometry_is_empty)
SFCGAL_BOOL_PREDICATE(isSimple,   sfcgal_geometry_is_simple)
SFCGAL_BOOL_PREDICATE(isPlanar,   sfcgal_geometry_is_planar)
#undef SFCGAL_BOOL_PREDICATE

PHP_METHOD(Geometry, isValid)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    /* Caller may want a (bool, reason) pair later; for v1 return bool. */
    RETURN_BOOL(sfcgal_geometry_is_valid(g) != 0);
}

PHP_METHOD(Geometry, asText)
{
    zend_long decimals = -1;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &decimals) == FAILURE)
        RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    char *buf = NULL; size_t len = 0;
    if (decimals < 0) {
        sfcgal_geometry_as_text(g, &buf, &len);
    } else {
        sfcgal_geometry_as_text_decim(g, (int) decimals, &buf, &len);
    }
    if (!buf) RETURN_EMPTY_STRING();
    RETVAL_STRINGL(buf, len);
    free(buf);
}

#define SFCGAL_AS_BYTES(method, capi)                                            \
    PHP_METHOD(Geometry, method)                                                 \
    {                                                                            \
        if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();            \
        sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());                   \
        if (!g) RETURN_THROWS();                                                 \
        char *buf = NULL; size_t len = 0;                                        \
        capi(g, &buf, &len);                                                     \
        if (!buf) RETURN_EMPTY_STRING();                                         \
        RETVAL_STRINGL(buf, len);                                                \
        free(buf);                                                               \
    }

SFCGAL_AS_BYTES(asWkb,    sfcgal_geometry_as_wkb)
SFCGAL_AS_BYTES(asHexWkb, sfcgal_geometry_as_hexwkb)
SFCGAL_AS_BYTES(asObj,    sfcgal_geometry_as_obj)
SFCGAL_AS_BYTES(asStl,    sfcgal_geometry_as_stl)
SFCGAL_AS_BYTES(asVtk,    sfcgal_geometry_as_vtk)
#undef SFCGAL_AS_BYTES

/* -- Static factories -- */

PHP_METHOD(Geometry, readWKT)
{
    char *s; size_t s_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &s, &s_len) == FAILURE)
        RETURN_THROWS();
    sfcgal_geometry_t *g = sfcgal_io_read_wkt(s, s_len);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, g) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Geometry, readWKB)
{
    char *s; size_t s_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &s, &s_len) == FAILURE)
        RETURN_THROWS();
    sfcgal_geometry_t *g = sfcgal_io_read_wkb(s, s_len);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, g) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Geometry, readHexWKB)
{
    /* SFCGAL has no native hex-WKB reader; decode in PHP land for now. */
    char *s; size_t s_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &s, &s_len) == FAILURE)
        RETURN_THROWS();
    if (s_len % 2 != 0) {
        zend_throw_exception(Exception_ce, "Hex-WKB length must be even", 0);
        RETURN_THROWS();
    }
    size_t out_len = s_len / 2;
    char *out = emalloc(out_len + 1);
    for (size_t i = 0; i < out_len; i++) {
        unsigned int byte;
        if (sscanf(s + 2 * i, "%2x", &byte) != 1) {
            efree(out);
            zend_throw_exception(Exception_ce, "Invalid hex-WKB digit", 0);
            RETURN_THROWS();
        }
        out[i] = (char) byte;
    }
    out[out_len] = '\0';
    sfcgal_geometry_t *g = sfcgal_io_read_wkb(out, out_len);
    efree(out);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, g) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Geometry, readEWKT)
{
    char *s; size_t s_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &s, &s_len) == FAILURE)
        RETURN_THROWS();
    sfcgal_prepared_geometry_t *p = sfcgal_io_read_ewkt(s, s_len);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_prepared(return_value, p) == FAILURE) RETURN_THROWS();
}

/* -- Binary predicates / operations against another geometry -- */

#define SFCGAL_BINARY_PREDICATE(name, capi)                                      \
    PHP_METHOD(Geometry, name)                                                   \
    {                                                                            \
        zval *other_zv;                                                          \
        if (zend_parse_parameters(ZEND_NUM_ARGS(), "O",                          \
                &other_zv, Geometry_ce) == FAILURE) RETURN_THROWS();             \
        sfcgal_geometry_t *self = php_sfcgal_get_self(getThis());                \
        sfcgal_geometry_t *o    = php_sfcgal_get_geom(other_zv);                 \
        if (!self || !o) RETURN_THROWS();                                        \
        int rv = capi(self, o);                                                  \
        if (EG(exception)) RETURN_THROWS();                                      \
        RETURN_BOOL(rv != 0);                                                    \
    }

SFCGAL_BINARY_PREDICATE(intersects3D, sfcgal_geometry_intersects_3d)
SFCGAL_BINARY_PREDICATE(covers3D,     sfcgal_geometry_covers_3d)
#undef SFCGAL_BINARY_PREDICATE

PHP_METHOD(Geometry, distance3D)
{
    zval *other_zv;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &other_zv, Geometry_ce) == FAILURE)
        RETURN_THROWS();
    sfcgal_geometry_t *self = php_sfcgal_get_self(getThis());
    sfcgal_geometry_t *o    = php_sfcgal_get_geom(other_zv);
    if (!self || !o) RETURN_THROWS();
    double d = sfcgal_geometry_distance_3d(self, o);
    if (EG(exception)) RETURN_THROWS();
    RETURN_DOUBLE(d);
}

#define SFCGAL_DOUBLE_MEASURE(name, capi)                                        \
    PHP_METHOD(Geometry, name)                                                   \
    {                                                                            \
        if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();            \
        sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());                   \
        if (!g) RETURN_THROWS();                                                 \
        double v = capi(g);                                                      \
        if (EG(exception)) RETURN_THROWS();                                      \
        RETURN_DOUBLE(v);                                                        \
    }

SFCGAL_DOUBLE_MEASURE(area3D,   sfcgal_geometry_area_3d)
SFCGAL_DOUBLE_MEASURE(length3D, sfcgal_geometry_length_3d)
SFCGAL_DOUBLE_MEASURE(volume,   sfcgal_geometry_volume)
#undef SFCGAL_DOUBLE_MEASURE

/* -- Set operations (3D) and other "returns a new geometry" patterns -- */

#define SFCGAL_BINARY_GEOMOP(name, capi)                                         \
    PHP_METHOD(Geometry, name)                                                   \
    {                                                                            \
        zval *other_zv;                                                          \
        if (zend_parse_parameters(ZEND_NUM_ARGS(), "O",                          \
                &other_zv, Geometry_ce) == FAILURE) RETURN_THROWS();             \
        sfcgal_geometry_t *self = php_sfcgal_get_self(getThis());                \
        sfcgal_geometry_t *o    = php_sfcgal_get_geom(other_zv);                 \
        if (!self || !o) RETURN_THROWS();                                        \
        sfcgal_geometry_t *r = capi(self, o);                                    \
        if (EG(exception)) RETURN_THROWS();                                      \
        if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE)                \
            RETURN_THROWS();                                                     \
    }

SFCGAL_BINARY_GEOMOP(intersection3D, sfcgal_geometry_intersection_3d)
SFCGAL_BINARY_GEOMOP(difference3D,   sfcgal_geometry_difference_3d)
SFCGAL_BINARY_GEOMOP(union3D,        sfcgal_geometry_union_3d)
SFCGAL_BINARY_GEOMOP(minkowskiSum,   sfcgal_geometry_minkowski_sum)
#undef SFCGAL_BINARY_GEOMOP

#define SFCGAL_UNARY_GEOMOP(name, capi)                                          \
    PHP_METHOD(Geometry, name)                                                   \
    {                                                                            \
        if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();            \
        sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());                   \
        if (!g) RETURN_THROWS();                                                 \
        sfcgal_geometry_t *r = capi(g);                                          \
        if (EG(exception)) RETURN_THROWS();                                      \
        if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE)                \
            RETURN_THROWS();                                                     \
    }

SFCGAL_UNARY_GEOMOP(convexHull3D,          sfcgal_geometry_convexhull_3d)
SFCGAL_UNARY_GEOMOP(tesselate,             sfcgal_geometry_tesselate)
SFCGAL_UNARY_GEOMOP(makeSolid,             sfcgal_geometry_make_solid)
SFCGAL_UNARY_GEOMOP(straightSkeleton,      sfcgal_geometry_straight_skeleton)
SFCGAL_UNARY_GEOMOP(approximateMedialAxis, sfcgal_geometry_approximate_medial_axis)
#undef SFCGAL_UNARY_GEOMOP

PHP_METHOD(Geometry, extrude)
{
    double dx, dy, dz;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "ddd", &dx, &dy, &dz) == FAILURE)
        RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    sfcgal_geometry_t *r = sfcgal_geometry_extrude(g, dx, dy, dz);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Geometry, extrudeStraightSkeleton)
{
    double height;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &height) == FAILURE)
        RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    sfcgal_geometry_t *r = sfcgal_geometry_extrude_straight_skeleton(g, height);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Geometry, extrudePolygonStraightSkeleton)
{
    double building_h, roof_h;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "dd", &building_h, &roof_h) == FAILURE)
        RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    sfcgal_geometry_t *r =
        sfcgal_geometry_extrude_polygon_straight_skeleton(g, building_h, roof_h);
    if (EG(exception)) RETURN_THROWS();
    if (!r) {
        zend_throw_exception(Exception_ce,
            "extrudePolygonStraightSkeleton returned NULL", 0);
        RETURN_THROWS();
    }
    /* Upstream returns a PolyhedralSurface, but the operation semantically
     * produces a Solid (a closed building with a roof — non-zero volume).
     * Wrap with makeSolid so the binding's documented return type
     * (SFCGALSolid) is honoured.  makeSolid validates that the surface is
     * a closed manifold; if it isn't, propagate the original surface. */
    sfcgal_geometry_t *solid = sfcgal_geometry_make_solid(r);
    sfcgal_geometry_delete(r);
    if (EG(exception)) RETURN_THROWS();
    if (!solid) {
        zend_throw_exception(Exception_ce,
            "extrudePolygonStraightSkeleton: makeSolid wrap failed", 0);
        RETURN_THROWS();
    }
    if (php_sfcgal_wrap_geometry(return_value, solid) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Geometry, alphaShapes)
{
    double alpha = 1.0;
    zend_bool allow_holes = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|db",
            &alpha, &allow_holes) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    sfcgal_geometry_t *r = sfcgal_geometry_alpha_shapes(g, alpha, allow_holes);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Geometry, optimalAlphaShapes)
{
    zend_bool allow_holes = 0;
    zend_long nb_components = 1;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|bl",
            &allow_holes, &nb_components) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    sfcgal_geometry_t *r = sfcgal_geometry_optimal_alpha_shapes(
        g, allow_holes, (size_t) nb_components);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Geometry, alphaWrapping3D)
{
    zend_long relative_alpha;
    zend_long relative_offset = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l",
            &relative_alpha, &relative_offset) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    sfcgal_geometry_t *r = sfcgal_geometry_alpha_wrapping_3d(
        g, (size_t) relative_alpha, (size_t) relative_offset);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Geometry, buffer3D)
{
    double radius;
    zend_long segments;
    zend_long type = 0; /* SFCGAL_BUFFER3D_ROUND default */
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "dl|l",
            &radius, &segments, &type) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    sfcgal_geometry_t *r = sfcgal_geometry_buffer3d(
        g, radius, (int) segments, (sfcgal_buffer3d_type_t) type);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Geometry, simplify)
{
    double threshold;
    zend_bool preserve_topology = 1;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "d|b",
            &threshold, &preserve_topology) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    sfcgal_geometry_t *r = sfcgal_geometry_simplify(g, threshold, preserve_topology);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Geometry, lineSubstring)
{
    double start, end;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "dd", &start, &end) == FAILURE)
        RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    sfcgal_geometry_t *r = sfcgal_geometry_line_sub_string(g, start, end);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();
}

/* -- Transforms (mutating in place — sfcgal_*) -- */

PHP_METHOD(Geometry, forceZ)
{
    double defaultZ = 0.0;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|d", &defaultZ) == FAILURE)
        RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    sfcgal_geometry_force_z(g, defaultZ);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Geometry, forceM)
{
    double defaultM = 0.0;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|d", &defaultM) == FAILURE)
        RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    sfcgal_geometry_force_m(g, defaultM);
    RETURN_ZVAL(getThis(), 1, 0);
}

#define SFCGAL_INPLACE_VOID(name, capi)                                          \
    PHP_METHOD(Geometry, name)                                                   \
    {                                                                            \
        if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();            \
        sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());                   \
        if (!g) RETURN_THROWS();                                                 \
        capi(g);                                                                 \
        RETURN_ZVAL(getThis(), 1, 0);                                            \
    }

SFCGAL_INPLACE_VOID(dropZ,    sfcgal_geometry_drop_z)
SFCGAL_INPLACE_VOID(dropM,    sfcgal_geometry_drop_m)
SFCGAL_INPLACE_VOID(swapXY,   sfcgal_geometry_swap_xy)
#undef SFCGAL_INPLACE_VOID

/* sfcgal_geometry_force_lhr / _force_rhr take a const pointer and
 * return a freshly cloned geometry — they are NOT in-place mutators
 * (despite the name).  In php-sfcgal 1.0.1 these were mis-wrapped via
 * SFCGAL_INPLACE_VOID, which discarded the return value, leaked the
 * clone, and produced a no-op visible to callers.  Bug fix in 1.0.2:
 * wrap the returned geometry like rotate* / scale* / translate*. */
PHP_METHOD(Geometry, forceLHR)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    sfcgal_geometry_t *r = sfcgal_geometry_force_lhr(g);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Geometry, forceRHR)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    sfcgal_geometry_t *r = sfcgal_geometry_force_rhr(g);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Geometry, round)
{
    zend_long scale;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &scale) == FAILURE)
        RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    sfcgal_geometry_round(g, (int) scale);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Geometry, translate3D)
{
    double dx, dy, dz;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "ddd", &dx, &dy, &dz) == FAILURE)
        RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    sfcgal_geometry_t *r = sfcgal_geometry_translate_3d(g, dx, dy, dz);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Geometry, scale3D)
{
    double sx, sy, sz;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "ddd", &sx, &sy, &sz) == FAILURE)
        RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    sfcgal_geometry_t *r = sfcgal_geometry_scale_3d(g, sx, sy, sz);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();
}

/* -- GEOS bridge --------------------------------------------------------- */

PHP_METHOD(Geometry, fromGEOS)
{
    zval *geos_zv;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &geos_zv) == FAILURE)
        RETURN_THROWS();

    /* Resolve \GEOS\Geometry / GEOSGeometry at runtime so we don't hard-link
     * against the geos extension. Both names are tolerated to track future
     * namespacing. */
    zend_class_entry *geos_ce = zend_lookup_class(zend_string_init("GEOSGeometry", sizeof("GEOSGeometry") - 1, 0));
    if (!geos_ce) {
        zend_throw_exception(Exception_ce,
            "php-geos extension is not loaded; cannot bridge from GEOS", 0);
        RETURN_THROWS();
    }
    if (!instanceof_function(Z_OBJCE_P(geos_zv), geos_ce)) {
        zend_throw_exception(Exception_ce,
            "Argument must be a GEOSGeometry instance", 0);
        RETURN_THROWS();
    }

    /* Reject curved geometry types (CIRCSTRING / COMPOUNDCURVE / CURVEPOLYGON
     * / MULTICURVE / MULTISURFACE) — mirroring PostGIS LWGEOM2SFCGAL.
     * GEOS exposes the type-id via typeName(). Cheaper: WKB tag inspection. */
    zval typeName_rv, *type_zv;
    zval method;
    ZVAL_STRING(&method, "typeName");
    if (call_user_function(NULL, geos_zv, &method, &typeName_rv, 0, NULL) != SUCCESS) {
        zval_ptr_dtor(&method);
        zend_throw_exception(Exception_ce, "GEOS::typeName() call failed", 0);
        RETURN_THROWS();
    }
    zval_ptr_dtor(&method);
    type_zv = &typeName_rv;
    if (Z_TYPE_P(type_zv) == IS_STRING) {
        const char *n = Z_STRVAL_P(type_zv);
        if (!strcmp(n, "CircularString")
         || !strcmp(n, "CompoundCurve")
         || !strcmp(n, "CurvePolygon")
         || !strcmp(n, "MultiCurve")
         || !strcmp(n, "MultiSurface")) {
            zend_throw_exception_ex(Exception_ce, 0,
                "SFCGAL does not support curved type '%s'; "
                "call \\GEOS\\Geometry::curveToLine() (or GEOSGeometry::curveToLine()) first", n);
            zval_ptr_dtor(type_zv);
            RETURN_THROWS();
        }
    }
    zval_ptr_dtor(type_zv);

    /* Use WKB round-trip: GEOS->WKB->SFCGAL. */
    zval writer, args[1], wkb_rv;
    /* Build a GEOSWKBWriter, call write($geos), then read into SFCGAL. */
    zend_class_entry *wkbw_ce = zend_lookup_class(
        zend_string_init("GEOSWKBWriter", sizeof("GEOSWKBWriter") - 1, 0));
    if (!wkbw_ce) {
        zend_throw_exception(Exception_ce, "GEOSWKBWriter not available", 0);
        RETURN_THROWS();
    }
    object_init_ex(&writer, wkbw_ce);
    if (php_sfcgal_call_default_ctor(&writer) != SUCCESS) {
        zval_ptr_dtor(&writer);
        zend_throw_exception(Exception_ce, "GEOSWKBWriter constructor failed", 0);
        RETURN_THROWS();
    }
    /* SFCGAL only accepts ISO WKB (type IDs in the 1000s for Z, 2000s
     * for M, 3000s for ZM); GEOS defaults to EWKB (high-bit flags).
     * Force the ISO flavor. */
    {
        zval flavor_method, flavor_arg, flavor_rv;
        ZVAL_STRING(&flavor_method, "setFlavor");
        ZVAL_LONG(&flavor_arg, 2);   /* GEOSWKB_ISO */
        if (call_user_function(NULL, &writer, &flavor_method,
                               &flavor_rv, 1, &flavor_arg) == SUCCESS) {
            zval_ptr_dtor(&flavor_rv);
        }
        zval_ptr_dtor(&flavor_method);
        zval_ptr_dtor(&flavor_arg);
    }
    /* Set output dim 3 if available — best-effort. */
    {
        zval setdim_method, dim_arg, dim_rv;
        ZVAL_STRING(&setdim_method, "setOutputDimension");
        ZVAL_LONG(&dim_arg, 3);
        if (call_user_function(NULL, &writer, &setdim_method, &dim_rv, 1, &dim_arg) == SUCCESS) {
            zval_ptr_dtor(&dim_rv);
        }
        zval_ptr_dtor(&setdim_method);
        zval_ptr_dtor(&dim_arg);
    }

    ZVAL_COPY(&args[0], geos_zv);
    zval write_method;
    ZVAL_STRING(&write_method, "write");
    int r = call_user_function(NULL, &writer, &write_method, &wkb_rv, 1, args);
    zval_ptr_dtor(&write_method);
    zval_ptr_dtor(&args[0]);
    zval_ptr_dtor(&writer);
    if (r != SUCCESS || Z_TYPE(wkb_rv) != IS_STRING) {
        zval_ptr_dtor(&wkb_rv);
        zend_throw_exception(Exception_ce, "GEOSWKBWriter::write() failed", 0);
        RETURN_THROWS();
    }
    sfcgal_geometry_t *g = sfcgal_io_read_wkb(Z_STRVAL(wkb_rv), Z_STRLEN(wkb_rv));
    zval_ptr_dtor(&wkb_rv);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, g) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Geometry, toGEOS)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();

    zend_class_entry *wkbr_ce = zend_lookup_class(
        zend_string_init("GEOSWKBReader", sizeof("GEOSWKBReader") - 1, 0));
    if (!wkbr_ce) {
        zend_throw_exception(Exception_ce,
            "php-geos extension is not loaded; cannot bridge to GEOS", 0);
        RETURN_THROWS();
    }

    /* SFCGAL refuses to emit WKB for Solid; GEOS rejects WKB types 15/16
     * (PolyhedralSurface / TriangulatedSurface).  For those, decompose to
     * MultiPolygon Z of all face polygons before serialising — lossy on
     * shell / solid identity but the only OGC-portable representation. */
    sfcgal_geometry_type_t t = sfcgal_geometry_type_id(g);
    sfcgal_geometry_t *to_serialize = g;
    sfcgal_geometry_t *decomposed = NULL;
    if (t == SFCGAL_TYPE_SOLID
     || t == SFCGAL_TYPE_POLYHEDRALSURFACE
     || t == SFCGAL_TYPE_TRIANGULATEDSURFACE
     || t == SFCGAL_TYPE_MULTISOLID
     || t == SFCGAL_TYPE_TRIANGLE) {
        decomposed = build_face_multipolygon(g);
        if (EG(exception)) {
            if (decomposed) sfcgal_geometry_delete(decomposed);
            RETURN_THROWS();
        }
        to_serialize = decomposed;
    }

    char *buf = NULL; size_t len = 0;
    sfcgal_geometry_as_wkb(to_serialize, &buf, &len);
    if (decomposed) sfcgal_geometry_delete(decomposed);
    if (EG(exception)) {
        if (buf) free(buf);
        RETURN_THROWS();
    }
    if (!buf) {
        zend_throw_exception(Exception_ce, "SFCGAL produced empty WKB", 0);
        RETURN_THROWS();
    }

    zval reader, args[1], rv, method;
    object_init_ex(&reader, wkbr_ce);
    if (php_sfcgal_call_default_ctor(&reader) != SUCCESS) {
        zval_ptr_dtor(&reader);
        free(buf);
        zend_throw_exception(Exception_ce, "GEOSWKBReader constructor failed", 0);
        RETURN_THROWS();
    }
    ZVAL_STRINGL(&args[0], buf, len);
    free(buf);
    ZVAL_STRING(&method, "read");
    int r = call_user_function(NULL, &reader, &method, &rv, 1, args);
    zval_ptr_dtor(&method);
    zval_ptr_dtor(&args[0]);
    zval_ptr_dtor(&reader);
    if (r != SUCCESS) {
        zval_ptr_dtor(&rv);
        zend_throw_exception(Exception_ce, "GEOSWKBReader::read() failed", 0);
        RETURN_THROWS();
    }
    RETURN_ZVAL(&rv, 1, 1);
}

/* ============================================================
 * SFCGAL\PreparedGeometry — methods
 * ============================================================ */

PHP_METHOD(PreparedGeometry, __construct)
{
    zval *geom_zv = NULL;
    zend_long srid = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|O!l",
            &geom_zv, Geometry_ce, &srid) == FAILURE) RETURN_THROWS();

    php_sfcgal_obj *intern = Z_SFCGAL_OBJ_P(getThis());
    if (intern->relay) {
        return; /* already wrapped via factory */
    }

    sfcgal_prepared_geometry_t *p;
    if (geom_zv == NULL) {
        p = sfcgal_prepared_geometry_create();
    } else {
        sfcgal_geometry_t *src = php_sfcgal_get_geom(geom_zv);
        if (!src) RETURN_THROWS();
        sfcgal_geometry_t *cloned = sfcgal_geometry_clone(src);
        p = sfcgal_prepared_geometry_create_from_geometry(cloned, (srid_t) srid);
    }
    if (EG(exception)) RETURN_THROWS();
    intern->relay = p;
    intern->owns_relay = 1;
}

PHP_METHOD(PreparedGeometry, geometry)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    php_sfcgal_obj *intern = Z_SFCGAL_OBJ_P(getThis());
    if (!intern->relay) RETURN_NULL();
    const sfcgal_geometry_t *cg =
        sfcgal_prepared_geometry_geometry((sfcgal_prepared_geometry_t *) intern->relay);
    if (!cg) RETURN_NULL();
    /* Inner geometry is owned by the prepared wrapper — clone for safety. */
    sfcgal_geometry_t *clone = sfcgal_geometry_clone(cg);
    if (php_sfcgal_wrap_geometry(return_value, clone) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(PreparedGeometry, srid)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    php_sfcgal_obj *intern = Z_SFCGAL_OBJ_P(getThis());
    if (!intern->relay) RETURN_LONG(0);
    RETURN_LONG(sfcgal_prepared_geometry_srid(
        (sfcgal_prepared_geometry_t *) intern->relay));
}

PHP_METHOD(PreparedGeometry, setSrid)
{
    zend_long srid;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &srid) == FAILURE)
        RETURN_THROWS();
    php_sfcgal_obj *intern = Z_SFCGAL_OBJ_P(getThis());
    if (!intern->relay) RETURN_THROWS();
    sfcgal_prepared_geometry_set_srid(
        (sfcgal_prepared_geometry_t *) intern->relay, (srid_t) srid);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(PreparedGeometry, asEWKT)
{
    zend_long decimals = -1;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &decimals) == FAILURE)
        RETURN_THROWS();
    php_sfcgal_obj *intern = Z_SFCGAL_OBJ_P(getThis());
    if (!intern->relay) RETURN_EMPTY_STRING();
    char *buf = NULL; size_t len = 0;
    sfcgal_prepared_geometry_as_ewkt(
        (sfcgal_prepared_geometry_t *) intern->relay,
        decimals < 0 ? -1 : (int) decimals,
        &buf, &len);
    if (!buf) RETURN_EMPTY_STRING();
    RETVAL_STRINGL(buf, len);
    free(buf);
}

/* ============================================================
 * v1 expansion — bodies
 * ============================================================ */

/* -- 2D twins of the 3D ops we already have -- */

#define SFCGAL_BINARY_PREDICATE(name, capi)                                      \
    PHP_METHOD(Geometry, name)                                                   \
    {                                                                            \
        zval *other_zv;                                                          \
        if (zend_parse_parameters(ZEND_NUM_ARGS(), "O",                          \
                &other_zv, Geometry_ce) == FAILURE) RETURN_THROWS();             \
        sfcgal_geometry_t *self = php_sfcgal_get_self(getThis());                \
        sfcgal_geometry_t *o    = php_sfcgal_get_geom(other_zv);                 \
        if (!self || !o) RETURN_THROWS();                                        \
        int rv = capi(self, o);                                                  \
        if (EG(exception)) RETURN_THROWS();                                      \
        RETURN_BOOL(rv != 0);                                                    \
    }

SFCGAL_BINARY_PREDICATE(intersects, sfcgal_geometry_intersects)
SFCGAL_BINARY_PREDICATE(covers,     sfcgal_geometry_covers)
SFCGAL_BINARY_PREDICATE(equals,     sfcgal_geometry_is_equals)
#undef SFCGAL_BINARY_PREDICATE

PHP_METHOD(Geometry, almostEquals)
{
    zval *other_zv; double tol;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Od",
            &other_zv, Geometry_ce, &tol) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *self = php_sfcgal_get_self(getThis());
    sfcgal_geometry_t *o    = php_sfcgal_get_geom(other_zv);
    if (!self || !o) RETURN_THROWS();
    int rv = sfcgal_geometry_is_almost_equals(self, o, tol);
    if (EG(exception)) RETURN_THROWS();
    RETURN_BOOL(rv != 0);
}

#define SFCGAL_BINARY_GEOMOP(name, capi)                                         \
    PHP_METHOD(Geometry, name)                                                   \
    {                                                                            \
        zval *other_zv;                                                          \
        if (zend_parse_parameters(ZEND_NUM_ARGS(), "O",                          \
                &other_zv, Geometry_ce) == FAILURE) RETURN_THROWS();             \
        sfcgal_geometry_t *self = php_sfcgal_get_self(getThis());                \
        sfcgal_geometry_t *o    = php_sfcgal_get_geom(other_zv);                 \
        if (!self || !o) RETURN_THROWS();                                        \
        sfcgal_geometry_t *r = capi(self, o);                                    \
        if (EG(exception)) RETURN_THROWS();                                      \
        if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE)                \
            RETURN_THROWS();                                                     \
    }

SFCGAL_BINARY_GEOMOP(intersection, sfcgal_geometry_intersection)
SFCGAL_BINARY_GEOMOP(difference,   sfcgal_geometry_difference)
SFCGAL_BINARY_GEOMOP(union,        sfcgal_geometry_union)
#undef SFCGAL_BINARY_GEOMOP

#define SFCGAL_UNARY_GEOMOP(name, capi)                                          \
    PHP_METHOD(Geometry, name)                                                   \
    {                                                                            \
        if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();            \
        sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());                   \
        if (!g) RETURN_THROWS();                                                 \
        sfcgal_geometry_t *r = capi(g);                                          \
        if (EG(exception)) RETURN_THROWS();                                      \
        if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE)                \
            RETURN_THROWS();                                                     \
    }

SFCGAL_UNARY_GEOMOP(convexHull,                  sfcgal_geometry_convexhull)
SFCGAL_UNARY_GEOMOP(envelope,                    sfcgal_geometry_envelope)
SFCGAL_UNARY_GEOMOP(envelope3D,                  sfcgal_geometry_envelope_3d)
SFCGAL_UNARY_GEOMOP(centroid,                    sfcgal_geometry_centroid)
SFCGAL_UNARY_GEOMOP(centroid3D,                  sfcgal_geometry_centroid_3d)
SFCGAL_UNARY_GEOMOP(boundary,                    sfcgal_geometry_boundary)
SFCGAL_UNARY_GEOMOP(triangulate2DZ,              sfcgal_geometry_triangulate_2dz)
SFCGAL_UNARY_GEOMOP(straightSkeletonDistanceInM, sfcgal_geometry_straight_skeleton_distance_in_m)
SFCGAL_UNARY_GEOMOP(yMonotonePartition2,         sfcgal_y_monotone_partition_2)
SFCGAL_UNARY_GEOMOP(approxConvexPartition2,      sfcgal_approx_convex_partition_2)
SFCGAL_UNARY_GEOMOP(greeneApproxConvexPartition2,sfcgal_greene_approx_convex_partition_2)
SFCGAL_UNARY_GEOMOP(optimalConvexPartition2,     sfcgal_optimal_convex_partition_2)
#undef SFCGAL_UNARY_GEOMOP

PHP_METHOD(Geometry, distance)
{
    zval *other_zv;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &other_zv, Geometry_ce) == FAILURE)
        RETURN_THROWS();
    sfcgal_geometry_t *self = php_sfcgal_get_self(getThis());
    sfcgal_geometry_t *o    = php_sfcgal_get_geom(other_zv);
    if (!self || !o) RETURN_THROWS();
    double d = sfcgal_geometry_distance(self, o);
    if (EG(exception)) RETURN_THROWS();
    RETURN_DOUBLE(d);
}

#define SFCGAL_DOUBLE_MEASURE(name, capi)                                        \
    PHP_METHOD(Geometry, name)                                                   \
    {                                                                            \
        if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();            \
        sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());                   \
        if (!g) RETURN_THROWS();                                                 \
        double v = capi(g);                                                      \
        if (EG(exception)) RETURN_THROWS();                                      \
        RETURN_DOUBLE(v);                                                        \
    }

SFCGAL_DOUBLE_MEASURE(area,   sfcgal_geometry_area)
SFCGAL_DOUBLE_MEASURE(length, sfcgal_geometry_length)
#undef SFCGAL_DOUBLE_MEASURE

PHP_METHOD(Geometry, orientation)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    int v = sfcgal_geometry_orientation(g);
    if (EG(exception)) RETURN_THROWS();
    RETURN_LONG(v);
}

PHP_METHOD(Geometry, offsetPolygon)
{
    double radius;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &radius) == FAILURE)
        RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    sfcgal_geometry_t *r = sfcgal_geometry_offset_polygon(g, radius);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();
}

/* -- Validity detail / flags -- */

PHP_METHOD(Geometry, validityDetail)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    char *reason = NULL;
    sfcgal_geometry_t *loc = NULL;
    int valid = sfcgal_geometry_is_valid_detail(g, &reason, &loc);
    if (EG(exception)) {
        if (reason) free(reason);
        if (loc) sfcgal_geometry_delete(loc);
        RETURN_THROWS();
    }
    array_init(return_value);
    add_assoc_bool(return_value, "valid", valid != 0);
    if (reason) {
        add_assoc_string(return_value, "reason", reason);
        free(reason);
    } else {
        add_assoc_null(return_value, "reason");
    }
    if (loc) {
        zval z;
        if (php_sfcgal_wrap_geometry(&z, loc) == SUCCESS) {
            add_assoc_zval(return_value, "location", &z);
        }
    } else {
        add_assoc_null(return_value, "location");
    }
}

PHP_METHOD(Geometry, simpleDetail)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    char *reason = NULL;
    int simple = sfcgal_geometry_is_simple_detail(g, &reason);
    if (EG(exception)) { if (reason) free(reason); RETURN_THROWS(); }
    array_init(return_value);
    add_assoc_bool(return_value, "simple", simple != 0);
    if (reason) {
        add_assoc_string(return_value, "reason", reason);
        free(reason);
    } else {
        add_assoc_null(return_value, "reason");
    }
}

PHP_METHOD(Geometry, hasValidityFlag)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    RETURN_BOOL(sfcgal_geometry_has_validity_flag(g) != 0);
}

PHP_METHOD(Geometry, forceValid)
{
    zend_bool valid = 1;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &valid) == FAILURE)
        RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    sfcgal_geometry_force_valid(g, valid ? 1 : 0);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Geometry, straightSkeletonPartition)
{
    zend_bool autoOrient = 1;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &autoOrient) == FAILURE)
        RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    sfcgal_geometry_t *r = sfcgal_geometry_straight_skeleton_partition(g, autoOrient);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Geometry, visibilityPoint)
{
    zval *pt_zv;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &pt_zv, Point_ce) == FAILURE)
        RETURN_THROWS();
    sfcgal_geometry_t *self = php_sfcgal_get_self(getThis());
    sfcgal_geometry_t *p    = php_sfcgal_get_geom(pt_zv);
    if (!self || !p) RETURN_THROWS();
    sfcgal_geometry_t *r = sfcgal_geometry_visibility_point(self, p);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Geometry, visibilitySegment)
{
    zval *a_zv, *b_zv;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "OO",
            &a_zv, Point_ce, &b_zv, Point_ce) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *self = php_sfcgal_get_self(getThis());
    sfcgal_geometry_t *a    = php_sfcgal_get_geom(a_zv);
    sfcgal_geometry_t *b    = php_sfcgal_get_geom(b_zv);
    if (!self || !a || !b) RETURN_THROWS();
    sfcgal_geometry_t *r = sfcgal_geometry_visibility_segment(self, a, b);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();
}

/* -- Rotate / scale / translate (all return new geometries) -- */

PHP_METHOD(Geometry, rotate)
{
    double a;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &a) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis()); if (!g) RETURN_THROWS();
    sfcgal_geometry_t *r = sfcgal_geometry_rotate(g, a);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Geometry, rotate2D)
{
    double a, cx, cy;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "ddd", &a, &cx, &cy) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis()); if (!g) RETURN_THROWS();
    sfcgal_geometry_t *r = sfcgal_geometry_rotate_2d(g, a, cx, cy);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Geometry, rotate3D)
{
    double a, ax, ay, az;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "dddd", &a, &ax, &ay, &az) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis()); if (!g) RETURN_THROWS();
    sfcgal_geometry_t *r = sfcgal_geometry_rotate_3d(g, a, ax, ay, az);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Geometry, rotate3DAroundCenter)
{
    double a, ax, ay, az, cx, cy, cz;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "ddddddd",
            &a, &ax, &ay, &az, &cx, &cy, &cz) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis()); if (!g) RETURN_THROWS();
    sfcgal_geometry_t *r = sfcgal_geometry_rotate_3d_around_center(g, a, ax, ay, az, cx, cy, cz);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();
}

#define SFCGAL_ROT_AXIS(name, capi)                                              \
    PHP_METHOD(Geometry, name)                                                   \
    {                                                                            \
        double a;                                                                \
        if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &a) == FAILURE) RETURN_THROWS(); \
        sfcgal_geometry_t *g = php_sfcgal_get_self(getThis()); if (!g) RETURN_THROWS(); \
        sfcgal_geometry_t *r = capi(g, a);                                       \
        if (EG(exception)) RETURN_THROWS();                                      \
        if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();\
    }

SFCGAL_ROT_AXIS(rotateX, sfcgal_geometry_rotate_x)
SFCGAL_ROT_AXIS(rotateY, sfcgal_geometry_rotate_y)
SFCGAL_ROT_AXIS(rotateZ, sfcgal_geometry_rotate_z)
#undef SFCGAL_ROT_AXIS

PHP_METHOD(Geometry, scale)
{
    double s;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &s) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis()); if (!g) RETURN_THROWS();
    sfcgal_geometry_t *r = sfcgal_geometry_scale(g, s);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Geometry, scale3DAroundCenter)
{
    double sx, sy, sz, cx, cy, cz;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "dddddd",
            &sx, &sy, &sz, &cx, &cy, &cz) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis()); if (!g) RETURN_THROWS();
    sfcgal_geometry_t *r = sfcgal_geometry_scale_3d_around_center(g, sx, sy, sz, cx, cy, cz);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Geometry, translate2D)
{
    double dx, dy;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "dd", &dx, &dy) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis()); if (!g) RETURN_THROWS();
    sfcgal_geometry_t *r = sfcgal_geometry_translate_2d(g, dx, dy);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_geometry(return_value, r) == FAILURE) RETURN_THROWS();
}

/* -- File-output writers -- */

#define SFCGAL_FILE_WRITE(name, capi)                                            \
    PHP_METHOD(Geometry, name)                                                   \
    {                                                                            \
        char *fn; size_t fn_len;                                                 \
        if (zend_parse_parameters(ZEND_NUM_ARGS(), "p", &fn, &fn_len) == FAILURE)\
            RETURN_THROWS();                                                     \
        sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());                   \
        if (!g) RETURN_THROWS();                                                 \
        capi(g, fn);                                                             \
        if (EG(exception)) RETURN_THROWS();                                      \
        RETURN_TRUE;                                                             \
    }

SFCGAL_FILE_WRITE(writeVtkFile, sfcgal_geometry_as_vtk_file)
SFCGAL_FILE_WRITE(writeObjFile, sfcgal_geometry_as_obj_file)
SFCGAL_FILE_WRITE(writeStlFile, sfcgal_geometry_as_stl_file)
#undef SFCGAL_FILE_WRITE

/* -- Hierarchy: numGeometries / geometryN — base-class polymorphic accessor -- */

PHP_METHOD(Geometry, numGeometries)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    sfcgal_geometry_type_t t = sfcgal_geometry_type_id(g);
    size_t n = 0;
    switch (t) {
    case SFCGAL_TYPE_LINESTRING:
        n = sfcgal_linestring_num_points(g); break;
    case SFCGAL_TYPE_POLYGON:
        /* exterior ring + N interior rings */
        n = 1 + sfcgal_polygon_num_interior_rings(g); break;
    case SFCGAL_TYPE_TRIANGULATEDSURFACE:
        n = sfcgal_triangulated_surface_num_patches(g); break;
    case SFCGAL_TYPE_POLYHEDRALSURFACE:
        n = sfcgal_polyhedral_surface_num_patches(g); break;
    case SFCGAL_TYPE_SOLID:
        n = sfcgal_solid_num_shells(g); break;
    case SFCGAL_TYPE_MULTIPOINT:
    case SFCGAL_TYPE_MULTILINESTRING:
    case SFCGAL_TYPE_MULTIPOLYGON:
    case SFCGAL_TYPE_MULTISOLID:
    case SFCGAL_TYPE_GEOMETRYCOLLECTION:
        n = sfcgal_geometry_num_geometries(g); break;
    default:
        n = 0;
    }
    if (EG(exception)) RETURN_THROWS();
    RETURN_LONG((zend_long) n);
}

PHP_METHOD(Geometry, geometryN)
{
    zend_long i;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &i) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    if (i < 0) {
        zend_throw_exception(Exception_ce, "Index must be >= 0", 0);
        RETURN_THROWS();
    }
    sfcgal_geometry_type_t t = sfcgal_geometry_type_id(g);
    const sfcgal_geometry_t *child = NULL;
    switch (t) {
    case SFCGAL_TYPE_LINESTRING:
        child = sfcgal_linestring_point_n(g, (size_t) i); break;
    case SFCGAL_TYPE_POLYGON:
        if (i == 0) child = sfcgal_polygon_exterior_ring(g);
        else        child = sfcgal_polygon_interior_ring_n(g, (size_t)(i - 1));
        break;
    case SFCGAL_TYPE_TRIANGULATEDSURFACE:
        child = sfcgal_triangulated_surface_patch_n(g, (size_t) i); break;
    case SFCGAL_TYPE_POLYHEDRALSURFACE:
        child = sfcgal_polyhedral_surface_patch_n(g, (size_t) i); break;
    case SFCGAL_TYPE_SOLID:
        child = sfcgal_solid_shell_n(g, (size_t) i); break;
    case SFCGAL_TYPE_MULTIPOINT:
    case SFCGAL_TYPE_MULTILINESTRING:
    case SFCGAL_TYPE_MULTIPOLYGON:
    case SFCGAL_TYPE_MULTISOLID:
    case SFCGAL_TYPE_GEOMETRYCOLLECTION:
        child = sfcgal_geometry_collection_geometry_n(g, (size_t) i); break;
    default:
        zend_throw_exception(Exception_ce, "geometryN: unsupported geometry kind", 0);
        RETURN_THROWS();
    }
    if (EG(exception)) RETURN_THROWS();
    if (!child) {
        zend_throw_exception(Exception_ce, "geometryN: index out of range", 0);
        RETURN_THROWS();
    }
    /* Children are owned by the parent — clone before wrapping. */
    sfcgal_geometry_t *clone = sfcgal_geometry_clone(child);
    if (php_sfcgal_wrap_geometry(return_value, clone) == FAILURE) RETURN_THROWS();
}

/* ============================================================
 * Typed factories — Point / LineString / Triangle / Polygon /
 * GeometryCollection / Multi* / *Surface / Solid
 *
 * For factories that accept a child geometry argument, the SFCGAL C API
 * takes ownership of that child. We therefore CLONE the caller's geometry
 * before handing it in, so the PHP-side parent object is unaffected.
 * ============================================================ */

/* Helper: wrap a child sfcgal_geometry_t* under the right concrete CE. */
static int php_sfcgal_wrap_into(zval *out, zend_class_entry *ce,
                                sfcgal_geometry_t *g)
{
    if (!g) {
        zend_throw_exception(Exception_ce, "SFCGAL returned NULL geometry", 0);
        return FAILURE;
    }
    object_init_ex(out, ce);
    php_sfcgal_obj *intern = Z_SFCGAL_OBJ_P(out);
    intern->relay = g;
    intern->owns_relay = 1;
    intern->kind = 0;
    return SUCCESS;
}

/* Helper: clone the caller's geometry and pass ownership; returns the clone. */
static sfcgal_geometry_t *
php_sfcgal_clone_arg(zval *zv, zend_class_entry *expected_ce)
{
    sfcgal_geometry_t *g = php_sfcgal_get_geom(zv);
    if (!g) return NULL;
    if (expected_ce && !instanceof_function(Z_OBJCE_P(zv), expected_ce)) {
        zend_throw_exception(Exception_ce, "Argument has wrong geometry type", 0);
        return NULL;
    }
    return sfcgal_geometry_clone(g);
}

/* -- Point -- */

PHP_METHOD(Point, create)
{
    double x, y, z, m;
    int nargs = ZEND_NUM_ARGS();
    sfcgal_geometry_t *g = NULL;
    if (nargs == 0) {
        g = sfcgal_point_create();
    } else if (nargs == 2) {
        if (zend_parse_parameters(2, "dd", &x, &y) == FAILURE) RETURN_THROWS();
        g = sfcgal_point_create_from_xy(x, y);
    } else if (nargs == 3) {
        if (zend_parse_parameters(3, "ddd", &x, &y, &z) == FAILURE) RETURN_THROWS();
        g = sfcgal_point_create_from_xyz(x, y, z);
    } else if (nargs == 4) {
        if (zend_parse_parameters(4, "dddd", &x, &y, &z, &m) == FAILURE) RETURN_THROWS();
        g = sfcgal_point_create_from_xyzm(x, y, z, m);
    } else {
        zend_throw_exception(Exception_ce,
            "SFCGALPoint::create takes 0, 2, 3 or 4 arguments", 0);
        RETURN_THROWS();
    }
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_into(return_value, Point_ce, g) == FAILURE) RETURN_THROWS();
}

#define SFCGAL_POINT_COORD(name, capi)                                           \
    PHP_METHOD(Point, name)                                                      \
    {                                                                            \
        if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();            \
        sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());                   \
        if (!g) RETURN_THROWS();                                                 \
        if (sfcgal_geometry_is_empty(g)) {                                       \
            zend_throw_exception(Exception_ce, "Empty point has no coord", 0);   \
            RETURN_THROWS();                                                     \
        }                                                                        \
        RETURN_DOUBLE(capi(g));                                                  \
    }

SFCGAL_POINT_COORD(x, sfcgal_point_x)
SFCGAL_POINT_COORD(y, sfcgal_point_y)
SFCGAL_POINT_COORD(z, sfcgal_point_z)
SFCGAL_POINT_COORD(m, sfcgal_point_m)
#undef SFCGAL_POINT_COORD

/* -- LineString -- */

PHP_METHOD(LineString, create)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = sfcgal_linestring_create();
    if (php_sfcgal_wrap_into(return_value, LineString_ce, g) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(LineString, numPoints)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis()); if (!g) RETURN_THROWS();
    RETURN_LONG((zend_long) sfcgal_linestring_num_points(g));
}

PHP_METHOD(LineString, pointN)
{
    zend_long n;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &n) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis()); if (!g) RETURN_THROWS();
    if (n < 0 || (size_t) n >= sfcgal_linestring_num_points(g)) {
        zend_throw_exception(Exception_ce, "pointN index out of range", 0);
        RETURN_THROWS();
    }
    const sfcgal_geometry_t *p = sfcgal_linestring_point_n(g, (size_t) n);
    sfcgal_geometry_t *cl = sfcgal_geometry_clone(p);
    if (php_sfcgal_wrap_into(return_value, Point_ce, cl) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(LineString, addPoint)
{
    zval *p_zv;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &p_zv, Point_ce) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *self = php_sfcgal_get_self(getThis()); if (!self) RETURN_THROWS();
    sfcgal_geometry_t *cloned = php_sfcgal_clone_arg(p_zv, Point_ce);
    if (!cloned) RETURN_THROWS();
    sfcgal_linestring_add_point(self, cloned);
    if (EG(exception)) RETURN_THROWS();
    RETURN_ZVAL(getThis(), 1, 0);
}

/* -- Triangle -- */

PHP_METHOD(Triangle, create)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = sfcgal_triangle_create();
    if (php_sfcgal_wrap_into(return_value, Triangle_ce, g) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Triangle, createFromPoints)
{
    zval *a_zv, *b_zv, *c_zv;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "OOO",
            &a_zv, Point_ce, &b_zv, Point_ce, &c_zv, Point_ce) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *a = php_sfcgal_get_geom(a_zv);
    sfcgal_geometry_t *b = php_sfcgal_get_geom(b_zv);
    sfcgal_geometry_t *c = php_sfcgal_get_geom(c_zv);
    if (!a || !b || !c) RETURN_THROWS();
    sfcgal_geometry_t *g = sfcgal_triangle_create_from_points(a, b, c);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_into(return_value, Triangle_ce, g) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Triangle, vertex)
{
    zend_long n;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &n) == FAILURE) RETURN_THROWS();
    if (n < 0 || n > 3) {
        zend_throw_exception(Exception_ce, "Triangle vertex index must be 0..3", 0);
        RETURN_THROWS();
    }
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis()); if (!g) RETURN_THROWS();
    const sfcgal_geometry_t *p = sfcgal_triangle_vertex(g, (int) n);
    sfcgal_geometry_t *cl = sfcgal_geometry_clone(p);
    if (php_sfcgal_wrap_into(return_value, Point_ce, cl) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Triangle, setVertex)
{
    zend_long n;
    zval *p_zv;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "lO", &n, &p_zv, Point_ce) == FAILURE) RETURN_THROWS();
    if (n < 0 || n > 3) {
        zend_throw_exception(Exception_ce, "Triangle vertex index must be 0..3", 0);
        RETURN_THROWS();
    }
    sfcgal_geometry_t *self = php_sfcgal_get_self(getThis()); if (!self) RETURN_THROWS();
    sfcgal_geometry_t *p = php_sfcgal_get_geom(p_zv); if (!p) RETURN_THROWS();
    sfcgal_triangle_set_vertex(self, (int) n, p);
    RETURN_ZVAL(getThis(), 1, 0);
}

/* -- Polygon -- */

PHP_METHOD(Polygon, create)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = sfcgal_polygon_create();
    if (php_sfcgal_wrap_into(return_value, Polygon_ce, g) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Polygon, createFromExteriorRing)
{
    zval *ring_zv;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &ring_zv, LineString_ce) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *cloned = php_sfcgal_clone_arg(ring_zv, LineString_ce);
    if (!cloned) RETURN_THROWS();
    sfcgal_geometry_t *g = sfcgal_polygon_create_from_exterior_ring(cloned);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_into(return_value, Polygon_ce, g) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Polygon, exteriorRing)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis()); if (!g) RETURN_THROWS();
    const sfcgal_geometry_t *r = sfcgal_polygon_exterior_ring(g);
    sfcgal_geometry_t *cl = sfcgal_geometry_clone(r);
    if (php_sfcgal_wrap_into(return_value, LineString_ce, cl) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Polygon, numInteriorRings)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis()); if (!g) RETURN_THROWS();
    RETURN_LONG((zend_long) sfcgal_polygon_num_interior_rings(g));
}

PHP_METHOD(Polygon, interiorRingN)
{
    zend_long n;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &n) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis()); if (!g) RETURN_THROWS();
    if (n < 0 || (size_t) n >= sfcgal_polygon_num_interior_rings(g)) {
        zend_throw_exception(Exception_ce, "interiorRingN index out of range", 0);
        RETURN_THROWS();
    }
    const sfcgal_geometry_t *r = sfcgal_polygon_interior_ring_n(g, (size_t) n);
    sfcgal_geometry_t *cl = sfcgal_geometry_clone(r);
    if (php_sfcgal_wrap_into(return_value, LineString_ce, cl) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Polygon, addInteriorRing)
{
    zval *r_zv;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &r_zv, LineString_ce) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *self = php_sfcgal_get_self(getThis()); if (!self) RETURN_THROWS();
    sfcgal_geometry_t *cloned = php_sfcgal_clone_arg(r_zv, LineString_ce);
    if (!cloned) RETURN_THROWS();
    sfcgal_polygon_add_interior_ring(self, cloned);
    RETURN_ZVAL(getThis(), 1, 0);
}

/* -- Collection-style factories: GeometryCollection + Multi* -- */

#define SFCGAL_EMPTY_FACTORY(klass, capi, ce_var)                                \
    PHP_METHOD(klass, create)                                                    \
    {                                                                            \
        if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();            \
        sfcgal_geometry_t *g = capi();                                           \
        if (php_sfcgal_wrap_into(return_value, ce_var, g) == FAILURE)            \
            RETURN_THROWS();                                                     \
    }

SFCGAL_EMPTY_FACTORY(GeometryCollection,  sfcgal_geometry_collection_create, GeometryCollection_ce)
SFCGAL_EMPTY_FACTORY(MultiPoint,          sfcgal_multi_point_create,          MultiPoint_ce)
SFCGAL_EMPTY_FACTORY(MultiLineString,     sfcgal_multi_linestring_create,     MultiLineString_ce)
SFCGAL_EMPTY_FACTORY(MultiPolygon,        sfcgal_multi_polygon_create,        MultiPolygon_ce)
SFCGAL_EMPTY_FACTORY(MultiSolid,          sfcgal_multi_solid_create,          MultiSolid_ce)
SFCGAL_EMPTY_FACTORY(PolyhedralSurface,   sfcgal_polyhedral_surface_create,   PolyhedralSurface_ce)
SFCGAL_EMPTY_FACTORY(TriangulatedSurface, sfcgal_triangulated_surface_create, TriangulatedSurface_ce)
SFCGAL_EMPTY_FACTORY(Solid,               sfcgal_solid_create,                Solid_ce)
#undef SFCGAL_EMPTY_FACTORY

PHP_METHOD(GeometryCollection, addGeometry)
{
    zval *zv;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &zv, Geometry_ce) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *self = php_sfcgal_get_self(getThis()); if (!self) RETURN_THROWS();
    sfcgal_geometry_t *cloned = php_sfcgal_clone_arg(zv, Geometry_ce);
    if (!cloned) RETURN_THROWS();
    sfcgal_geometry_collection_add_geometry(self, cloned);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(GeometryCollection, setGeometryN)
{
    zend_long n; zval *zv;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "lO", &n, &zv, Geometry_ce) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *self = php_sfcgal_get_self(getThis()); if (!self) RETURN_THROWS();
    sfcgal_geometry_t *cloned = php_sfcgal_clone_arg(zv, Geometry_ce);
    if (!cloned) RETURN_THROWS();
    sfcgal_geometry_collection_set_geometry_n(self, cloned, (size_t) n);
    RETURN_ZVAL(getThis(), 1, 0);
}

/* -- PolyhedralSurface -- */

PHP_METHOD(PolyhedralSurface, numPatches)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis()); if (!g) RETURN_THROWS();
    RETURN_LONG((zend_long) sfcgal_polyhedral_surface_num_patches(g));
}

PHP_METHOD(PolyhedralSurface, patchN)
{
    zend_long n;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &n) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis()); if (!g) RETURN_THROWS();
    if (n < 0 || (size_t) n >= sfcgal_polyhedral_surface_num_patches(g)) {
        zend_throw_exception(Exception_ce, "patchN index out of range", 0);
        RETURN_THROWS();
    }
    const sfcgal_geometry_t *p = sfcgal_polyhedral_surface_patch_n(g, (size_t) n);
    sfcgal_geometry_t *cl = sfcgal_geometry_clone(p);
    if (php_sfcgal_wrap_into(return_value, Polygon_ce, cl) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(PolyhedralSurface, addPatch)
{
    zval *zv;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &zv, Polygon_ce) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *self = php_sfcgal_get_self(getThis()); if (!self) RETURN_THROWS();
    sfcgal_geometry_t *cloned = php_sfcgal_clone_arg(zv, Polygon_ce);
    if (!cloned) RETURN_THROWS();
    sfcgal_polyhedral_surface_add_patch(self, cloned);
    RETURN_ZVAL(getThis(), 1, 0);
}

/* -- TriangulatedSurface -- */

PHP_METHOD(TriangulatedSurface, numPatches)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis()); if (!g) RETURN_THROWS();
    RETURN_LONG((zend_long) sfcgal_triangulated_surface_num_patches(g));
}

PHP_METHOD(TriangulatedSurface, patchN)
{
    zend_long n;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &n) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis()); if (!g) RETURN_THROWS();
    if (n < 0 || (size_t) n >= sfcgal_triangulated_surface_num_patches(g)) {
        zend_throw_exception(Exception_ce, "patchN index out of range", 0);
        RETURN_THROWS();
    }
    const sfcgal_geometry_t *p = sfcgal_triangulated_surface_patch_n(g, (size_t) n);
    sfcgal_geometry_t *cl = sfcgal_geometry_clone(p);
    if (php_sfcgal_wrap_into(return_value, Triangle_ce, cl) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(TriangulatedSurface, addPatch)
{
    zval *zv;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &zv, Triangle_ce) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *self = php_sfcgal_get_self(getThis()); if (!self) RETURN_THROWS();
    sfcgal_geometry_t *cloned = php_sfcgal_clone_arg(zv, Triangle_ce);
    if (!cloned) RETURN_THROWS();
    sfcgal_triangulated_surface_add_patch(self, cloned);
    RETURN_ZVAL(getThis(), 1, 0);
}

/* -- Solid -- */

PHP_METHOD(Solid, createFromExteriorShell)
{
    zval *zv;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "O",
            &zv, PolyhedralSurface_ce) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *cloned = php_sfcgal_clone_arg(zv, PolyhedralSurface_ce);
    if (!cloned) RETURN_THROWS();
    sfcgal_geometry_t *g = sfcgal_solid_create_from_exterior_shell(cloned);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_into(return_value, Solid_ce, g) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Solid, numShells)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis()); if (!g) RETURN_THROWS();
    RETURN_LONG((zend_long) sfcgal_solid_num_shells(g));
}

PHP_METHOD(Solid, shellN)
{
    zend_long n;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &n) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis()); if (!g) RETURN_THROWS();
    if (n < 0 || (size_t) n >= sfcgal_solid_num_shells(g)) {
        zend_throw_exception(Exception_ce, "shellN index out of range", 0);
        RETURN_THROWS();
    }
    const sfcgal_geometry_t *p = sfcgal_solid_shell_n(g, (size_t) n);
    sfcgal_geometry_t *cl = sfcgal_geometry_clone(p);
    if (php_sfcgal_wrap_into(return_value, PolyhedralSurface_ce, cl) == FAILURE) RETURN_THROWS();
}

PHP_METHOD(Solid, addInteriorShell)
{
    zval *zv;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &zv, PolyhedralSurface_ce) == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *self = php_sfcgal_get_self(getThis()); if (!self) RETURN_THROWS();
    sfcgal_geometry_t *cloned = php_sfcgal_clone_arg(zv, PolyhedralSurface_ce);
    if (!cloned) RETURN_THROWS();
    sfcgal_solid_add_interior_shell(self, cloned);
    RETURN_ZVAL(getThis(), 1, 0);
}

/* -- PreparedGeometry: setGeometry, asBinary, fromBinary -- */

PHP_METHOD(PreparedGeometry, setGeometry)
{
    zval *zv;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &zv, Geometry_ce) == FAILURE) RETURN_THROWS();
    php_sfcgal_obj *intern = Z_SFCGAL_OBJ_P(getThis());
    if (!intern->relay) {
        zend_throw_exception(Exception_ce, "PreparedGeometry not initialised", 0);
        RETURN_THROWS();
    }
    sfcgal_geometry_t *cloned = php_sfcgal_clone_arg(zv, Geometry_ce);
    if (!cloned) RETURN_THROWS();
    sfcgal_prepared_geometry_set_geometry(
        (sfcgal_prepared_geometry_t *) intern->relay, cloned);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(PreparedGeometry, asBinary)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    php_sfcgal_obj *intern = Z_SFCGAL_OBJ_P(getThis());
    if (!intern->relay) RETURN_EMPTY_STRING();
    char *buf = NULL; size_t len = 0;
    sfcgal_io_write_binary_prepared(
        (sfcgal_prepared_geometry_t *) intern->relay, &buf, &len);
    if (!buf) RETURN_EMPTY_STRING();
    RETVAL_STRINGL(buf, len);
    free(buf);
}

PHP_METHOD(PreparedGeometry, fromBinary)
{
    char *s; size_t s_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &s, &s_len) == FAILURE)
        RETURN_THROWS();
    sfcgal_prepared_geometry_t *p = sfcgal_io_read_binary_prepared(s, s_len);
    if (EG(exception)) RETURN_THROWS();
    if (php_sfcgal_wrap_prepared(return_value, p) == FAILURE) RETURN_THROWS();
}

/* ============================================================
 * decomposeToFaces — flatten any 3D-surface geometry to a
 * MultiPolygon Z whose members are the individual face polygons.
 *
 * SFCGAL refuses to emit WKB for SOLID, and GEOS does not understand
 * WKB type 15 (PolyhedralSurface) or type 16 (TriangulatedSurface).
 * MultiPolygon Z (WKB type 6 with hasZ) is the lowest-common-denominator
 * carrier — GEOS reads it cleanly, and from a SFCGAL perspective each
 * polygon-face simply becomes a member of the MP.
 *
 * Conversion table:
 *   Solid              → MP of every patch in every shell
 *   PolyhedralSurface  → MP of every patch
 *   TriangulatedSurface→ MP of every triangle (each promoted to a
 *                        Polygon with the closed exterior ring)
 *   MultiSolid         → MP of every patch in every shell of every solid
 *   Polygon            → MP containing the polygon
 *   MultiPolygon       → clone (already a MP)
 *   GeometryCollection → MP of any Polygon / *Surface / Solid members,
 *                        flattened recursively; non-areal members are
 *                        skipped (GEOS handles them via the WKB path).
 *
 * Returns NULL on malloc failure (caller throws).
 * ============================================================ */

/* Convert a Triangle to a freshly-allocated Polygon (caller owns). */
static sfcgal_geometry_t *
triangle_to_polygon(const sfcgal_geometry_t *tri)
{
    sfcgal_geometry_t *ring = sfcgal_linestring_create();
    for (int i = 0; i < 4; i++) {
        const sfcgal_geometry_t *v = sfcgal_triangle_vertex(tri, i);
        sfcgal_linestring_add_point(ring, sfcgal_geometry_clone(v));
    }
    return sfcgal_polygon_create_from_exterior_ring(ring);
}

/* Append every face polygon of `g` to `mp`. Recurses on collections. */
static void
append_faces_to_mp(sfcgal_geometry_t *mp, const sfcgal_geometry_t *g)
{
    if (!g) return;
    sfcgal_geometry_type_t t = sfcgal_geometry_type_id(g);
    size_t n;
    switch (t) {
    case SFCGAL_TYPE_POLYGON:
        sfcgal_geometry_collection_add_geometry(mp, sfcgal_geometry_clone(g));
        return;
    case SFCGAL_TYPE_TRIANGLE:
        sfcgal_geometry_collection_add_geometry(mp, triangle_to_polygon(g));
        return;
    case SFCGAL_TYPE_POLYHEDRALSURFACE:
        n = sfcgal_polyhedral_surface_num_patches(g);
        for (size_t i = 0; i < n; i++) {
            sfcgal_geometry_collection_add_geometry(
                mp, sfcgal_geometry_clone(
                    sfcgal_polyhedral_surface_patch_n(g, i)));
        }
        return;
    case SFCGAL_TYPE_TRIANGULATEDSURFACE:
        n = sfcgal_triangulated_surface_num_patches(g);
        for (size_t i = 0; i < n; i++) {
            sfcgal_geometry_collection_add_geometry(
                mp, triangle_to_polygon(
                    sfcgal_triangulated_surface_patch_n(g, i)));
        }
        return;
    case SFCGAL_TYPE_SOLID:
        n = sfcgal_solid_num_shells(g);
        for (size_t i = 0; i < n; i++) {
            append_faces_to_mp(mp, sfcgal_solid_shell_n(g, i));
        }
        return;
    case SFCGAL_TYPE_MULTIPOLYGON:
    case SFCGAL_TYPE_MULTISOLID:
    case SFCGAL_TYPE_GEOMETRYCOLLECTION:
        n = sfcgal_geometry_num_geometries(g);
        for (size_t i = 0; i < n; i++) {
            append_faces_to_mp(mp, sfcgal_geometry_collection_geometry_n(g, i));
        }
        return;
    default:
        /* Non-areal types (Point, LineString, MultiPoint, …) are dropped
         * by design — they have no face contribution.  Caller should
         * use the regular WKB path for those. */
        return;
    }
}

/* Build a MultiPolygon from the faces of `g`. Caller owns. */
static sfcgal_geometry_t *
build_face_multipolygon(const sfcgal_geometry_t *g)
{
    sfcgal_geometry_t *mp = sfcgal_multi_polygon_create();
    append_faces_to_mp(mp, g);
    return mp;
}

PHP_METHOD(Geometry, decomposeToFaces)
{
    if (zend_parse_parameters_none() == FAILURE) RETURN_THROWS();
    sfcgal_geometry_t *g = php_sfcgal_get_self(getThis());
    if (!g) RETURN_THROWS();
    sfcgal_geometry_t *mp = build_face_multipolygon(g);
    if (EG(exception)) {
        if (mp) sfcgal_geometry_delete(mp);
        RETURN_THROWS();
    }
    if (php_sfcgal_wrap_geometry(return_value, mp) == FAILURE) RETURN_THROWS();
}
