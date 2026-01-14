/* r2mcp - MIT - Copyright 2025 - pancake */

#define R_LOG_ORIGIN "core.r2mcp"

#include <r_core.h>
#include "r2mcp.h"
#include "tools.h"

int r2mcp_run_dsl_tests(ServerState *ss, const char *dsl, RCore *core);

typedef struct r2mcp_data_t {
	ServerState *ss;
} R2mcpData;

static R2mcpData g_data;

static int r2mcp_call(void *user, const char *input) {
	RCore *core = (RCore *)user;
	R2mcpData *data = &g_data;

	if (!r_str_startswith (input, "r2mcp")) {
		return false;
	}

	// Skip "r2mcp" command name
	const char *args = r_str_trim_head_ro (input + strlen ("r2mcp"));

	// Initialize server state if not already done
	if (!data->ss) {
		data->ss = R_NEW0 (ServerState);
		// Initialize the tools registry
		// Set up the core reference
		data->ss->rstate.core = core;
		data->ss->rstate.file_opened = true; // We're already in r2 with a file
	}

	if (R_STR_ISEMPTY (args)) {
		tools_print_table (data->ss);
	} else {
		if (r2mcp_run_dsl_tests (data->ss, args, core) != 0) {
			R_LOG_ERROR ("Error executing r2mcp command");
		}
	}

	return true;
}

static int r2mcp_init(void *user, const char *input) {
	(void)user;
	(void)input;
	g_data.ss = NULL;
	return true;
}

static int r2mcp_fini(void *user, const char *input) {
	(void)user;
	(void)input;
	if (g_data.ss) {
		r_strbuf_free (g_data.ss->sb);
		g_data.ss->sb = NULL;
		free (g_data.ss);
		g_data.ss = NULL;
	}
	return true;
}

// PLUGIN Definition Info
RCorePlugin r_core_plugin_r2mcp = {
	.name = "core-r2mcp",
	.desc = "r2mcp command integration for radare2",
	.license = "MIT",
	.author = "pancake",
	.version = R2_VERSION,
	.call = r2mcp_call,
	.init = r2mcp_init,
	.fini = r2mcp_fini,
};

#ifndef R2_PLUGIN_INCORE
R_API RLibStruct radare_plugin = {
	.type = R_LIB_TYPE_CORE,
	.data = &r_core_plugin_r2mcp,
	.version = R2_VERSION
};
#endif
