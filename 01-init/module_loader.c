#include "module_loader.h"

void *dl_unload(void *a)
{
	char *error;
	if((dlclose(a))&&((error = dlerror()) != NULL))
	{
		log_write(error, LOG_ERR);
		nfree(error);
	}
	return NULL;
}

int lua_answer_request(void *md, request *client_request)
{
	int ret_code=0;
	pthread_mutex_lock(&((struct lua_data*)md)->lua_lock);

	lua_State *L=((struct lua_data*)md)->L;

	lua_getfield(L, LUA_GLOBALSINDEX, "ModuleAnswerRequest");

	lua_pushinteger(L, client_request->user);
	lua_pushinteger(L, client_request->group);
	lua_pushinteger(L, client_request->session1);
	lua_pushinteger(L, client_request->session2);
	lua_pushinteger(L, client_request->project);
	lua_pushinteger(L, client_request->module);

	lua_pushstring(L, client_request->module_request);

	lua_call(L, 7, 3);

	if(!lua_isnumber(L, -3))
	{
		ret_code=1;
	}
	else
	{
		ret_code=lua_tonumber(L, -3);
	}

	if(ret_code!=0)
	{
		void *n;
		n=html_flush(((struct html_ui*)client_request->answer)->base, 1);
		nfree(n);

		html_add_tag(&((struct html_ui*)client_request->answer)->base, lua_tostring(L, -2), NULL, NULL);
		html_add_tag(&((struct html_ui*)client_request->answer)->base, lua_tostring(L, -1), NULL, NULL);
	}
	else
	{
		html_add_tag(&((struct html_ui*)client_request->answer)->sidebar, lua_tostring(L, -2), NULL, NULL);
		html_add_tag(&((struct html_ui*)client_request->answer)->main, lua_tostring(L, -1), NULL, NULL);
	}

	lua_pop(L,3);

	pthread_mutex_unlock(&((struct lua_data*)md)->lua_lock);

	return ret_code;
}

void *lua_unload(void *a)
{
	lua_close(((struct lua_data*)a)->L);
	nfree(a);
	return NULL;
}

void load_module(const char *varName, int x)
{
	void *lib_handle;
	int (*fn)(int , struct module_info*);
	char *error;


	//Script or Binary
	if(strend(".lua", varName)==0)
	{
		lua_State *L=NULL;
		L=(lua_State*)luaL_newstate();
		luaL_openlibs(L);

		if(L!=NULL && luaL_loadfile(L, varName)==0 && lua_pcall(L, 0, 0, 0)==0)
		{
			log_write("Loading %s", LOG_INFO, varName);

			Modules[x+1].id=x+1;

			lua_getglobal(L,"ModuleName");
			if(!lua_isnil(L,-1))
			{
				Modules[x+1].name=strdup(lua_tostring(L,-1));
				sighndlr_add(sighndlr_free, Modules[x+1].name);
			}
			else
			{
				Modules[x+1].name="Unknown LUA-Script";
			}
			lua_pop(L, 1);

			lua_getglobal(L,"ModuleDescription");
			if(!lua_isnil(L,-1))
			{
				Modules[x+1].description=strdup(lua_tostring(L,-1));
				sighndlr_add(sighndlr_free, Modules[x+1].description);
			}
			else
			{
				Modules[x+1].description="Unknown LUA-Script";
			}
			lua_pop(L, 1);

			Modules[x+1].data=malloc(sizeof(struct lua_data));
			Modules[x+1].func=lua_answer_request;
			((struct lua_data*)Modules[x+1].data)->L=L;
			pthread_mutex_init(&((struct lua_data*)Modules[x+1].data)->lua_lock,NULL);

			sighndlr_add(lua_unload, Modules[x+1].data);
		}
		else
		{
			if(L!=NULL) lua_close(L);
			log_write("Invalid LUA script %s", LOG_ERR, varName);
		}
	}
	else
	{
		lib_handle = dlopen(varName, RTLD_NOW);
		if(!lib_handle)
		{
			log_write(dlerror(), LOG_FATAL);
			exit(1);
		}

		fn = (int(*)(int, struct module_info *))dlsym(lib_handle, "init_module");
		if((error = dlerror()) != NULL)
		{
			log_write(error, LOG_FATAL);
			exit(1);
		}
		(*fn)(x+1,&Modules[x+1]);
		sighndlr_add(dl_unload, lib_handle);
	}
}
