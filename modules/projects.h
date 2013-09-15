#ifndef __PROJECTS_H__
#define __PROJECTS_H__

#include "modules.h"

/** \brief Data passed to all request calls*/
struct projects_data
{
	int i;
};

/** \brief Exits module */
void *projects_close_module(void *arg);

/** \brief Initalizes module */
int projects_init_module(int id);

/** \brief Request handler */
int projects_request(void *module_data, request *client_request);

#endif