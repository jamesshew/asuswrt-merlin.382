/*
 * iplink_vlan.c	VLAN device support
 *
 *              This program is free software; you can redistribute it and/or
 *              modify it under the terms of the GNU General Public License
 *              as published by the Free Software Foundation; either version
 *              2 of the License, or (at your option) any later version.
 *
 * Authors:     Patrick McHardy <kaber@trash.net>
 *		Arnd Bergmann <arnd@arndb.de>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/if_link.h>

#include "rt_names.h"
#include "utils.h"
#include "ip_common.h"

static void print_explain(FILE *f)
{
	fprintf(f,
		"Usage: ... macvlan mode { private | vepa | bridge | passthru }\n"
	);
}

static void explain(void)
{
	print_explain(stderr);
}

static int mode_arg(void)
{
        fprintf(stderr, "Error: argument of \"mode\" must be \"private\", "
		"\"vepa\", \"bridge\" or \"passthru\" \n");
        return -1;
}

static int macvlan_parse_opt(struct link_util *lu, int argc, char **argv,
			  struct nlmsghdr *n)
{
	while (argc > 0) {
		if (matches(*argv, "mode") == 0) {
			__u32 mode = 0;
			NEXT_ARG();

			if (strcmp(*argv, "private") == 0)
				mode = MACVLAN_MODE_PRIVATE;
			else if (strcmp(*argv, "vepa") == 0)
				mode = MACVLAN_MODE_VEPA;
			else if (strcmp(*argv, "bridge") == 0)
				mode = MACVLAN_MODE_BRIDGE;
			else if (strcmp(*argv, "passthru") == 0)
				mode = MACVLAN_MODE_PASSTHRU;
			else
				return mode_arg();

			addattr32(n, 1024, IFLA_MACVLAN_MODE, mode);
		} else if (matches(*argv, "help") == 0) {
			explain();
			return -1;
		} else {
			fprintf(stderr, "macvlan: unknown option \"%s\"?\n", *argv);
			explain();
			return -1;
		}
		argc--, argv++;
	}

	return 0;
}

static void macvlan_print_opt(struct link_util *lu, FILE *f, struct rtattr *tb[])
{
	__u32 mode;

	if (!tb)
		return;

	if (!tb[IFLA_MACVLAN_MODE] ||
	    RTA_PAYLOAD(tb[IFLA_MACVLAN_MODE]) < sizeof(__u32))
		return;

	mode = rta_getattr_u32(tb[IFLA_MACVLAN_MODE]);
	fprintf(f, " mode %s ",
		  mode == MACVLAN_MODE_PRIVATE ? "private"
		: mode == MACVLAN_MODE_VEPA    ? "vepa"
		: mode == MACVLAN_MODE_BRIDGE  ? "bridge"
		: mode == MACVLAN_MODE_PASSTHRU  ? "passthru"
		:				 "unknown");
}

static void macvlan_print_help(struct link_util *lu, int argc, char **argv,
	FILE *f)
{
	print_explain(f);
}

struct link_util macvlan_link_util = {
	.id		= "macvlan",
	.maxattr	= IFLA_MACVLAN_MAX,
	.parse_opt	= macvlan_parse_opt,
	.print_opt	= macvlan_print_opt,
	.print_help	= macvlan_print_help,
};
