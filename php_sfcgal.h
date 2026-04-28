/*
 * php-sfcgal — PHP bindings for SFCGAL.
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 * Copyright (C) 2026 Abraham van der Merwe <abz@frogfoot.com>
 */

#ifndef PHP_SFCGAL_H
#define PHP_SFCGAL_H

#define PHP_SFCGAL_VERSION  "1.0.2"
#define PHP_SFCGAL_EXTNAME  "sfcgal"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#ifdef ZTS
#include "TSRM.h"
#endif

extern zend_module_entry sfcgal_module_entry;
#define phpext_sfcgal_ptr &sfcgal_module_entry

#ifdef ZTS
#define SFCGAL_G(v) TSRMG(sfcgal_globals_id, zend_sfcgal_globals *, v)
#else
#define SFCGAL_G(v) (sfcgal_globals.v)
#endif

/* PHP 7+ TSRMLS shims (PHP 8 has no TSRMLS_* args anywhere; keep for ZTS macros). */
#ifndef TSRMLS_D
#define TSRMLS_D    void
#define TSRMLS_DC
#define TSRMLS_C
#define TSRMLS_CC
#define TSRMLS_FETCH()
#endif

ZEND_BEGIN_MODULE_GLOBALS(sfcgal)
    int initialized;            /* 1 once sfcgal_init() has run */
ZEND_END_MODULE_GLOBALS(sfcgal)

#endif /* PHP_SFCGAL_H */
