/**
 * \file   os_copyfile.c
 * \brief  Copy a file from one location to another.
 * \author Copyright (c) 2002-2008 Jason Perkins and the Premake project
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "premake.h"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

static int compare_file(const char* content, size_t length, const char* dst)
{
	FILE* file = fopen(dst, "rb");
	long size;
	char buffer[4096];
	int num;

	if (file == NULL)
	{
		return FALSE;
	}

	// check sizes.
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (length != size)
	{
		fclose(file);
		return FALSE;
	}

	while (size > 0)
	{
		num = size > 4096 ? 4096 : size;

		fread(buffer, 1, num, file);

		if (memcmp(content, buffer, num) != 0)
		{
			fclose(file);
			return FALSE;
		}

		size    -= num;
		content += num;
	}

	fclose(file);
	return TRUE;
}


int os_writefile_ifnotequal(lua_State* L)
{
	FILE* file;
	size_t length;
	const char* content = luaL_checklstring(L, 1, &length);
	const char* dst     = luaL_checkstring(L, 2);

	// if destination exist, and they are the same, no need to copy.
	if (do_isfile(dst) && compare_file(content, length, dst))
	{
		lua_pushinteger(L, 0);
		return 1;
	}


	file = fopen(dst, "wb");
	if (file != NULL)
	{
		fwrite(content, 1, length, file);
		fclose(file);

		lua_pushinteger(L, 1);
		return 1;
	}

	lua_pushinteger(L, -1);
	lua_pushfstring(L, "unable to write file to '%s'", dst);
	return 2;
}