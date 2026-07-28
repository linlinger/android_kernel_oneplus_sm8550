/*
 * Copyright (C) 2026 The AviumUI Project
 * SPDX-License-Identifier: GPL-2.0
 *
 * /proc/maps path hiding — hides sensitive ROM identifier strings
 * from memory maps visible to userspace applications.
 *
 * Configured via defconfig fragment:
 *   CONFIG_PROC_MAPS_HIDE=y
 *   CONFIG_PROC_MAPS_HIDE_PATTERNS="lineage,crDroid,avium,rising"
 *   CONFIG_PROC_MAPS_HIDE_REDIRECT="/system/framework/framework-res.apk"
 */

#include <linux/kernel.h>

/*
 * Patterns to match against d_name of mapped files.
 * Any file whose basename contains one of these strings
 * will have its path replaced with proc_maps_hide_redirect
 * in /proc/self/maps and /proc/self/map_files/.
 */
const char *proc_maps_hide_patterns[] = {
	CONFIG_PROC_MAPS_HIDE_PATTERNS,
	NULL,
};

/*
 * Fallback path shown when a mapped file's name matches
 * one of the patterns above.
 */
const char *proc_maps_hide_redirect = CONFIG_PROC_MAPS_HIDE_REDIRECT;
