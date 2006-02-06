/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   xrdp: A Remote Desktop Protocol server.
   Copyright (C) Jay Sorg 2005

   authenticate user

*/

#include "sesman.h"

#define _XOPEN_SOURCE
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

extern struct config_sesman g_cfg;

/******************************************************************************/
int DEFAULT_CC
access_login_allowed(char* user)
{
  int i;
  struct group* groups;
  struct passwd* pwd;

  if ((0==g_strncmp(user, "root",5)) && (0==g_cfg.sec.allow_root)) 
  {
    log_message(LOG_LEVEL_WARNING, "ROOT login attempted, but root login is disabled");
    return 0;
  }
 
  if (0==g_cfg.sec.ts_users_enable) 
  {
    LOG_DBG("Terminal Server Users group is disabled, allowing authentication",1);
    return 1;
  }
  
  groups = getgrgid(g_cfg.sec.ts_users);

  if (0==groups)
  {
    log_message(LOG_LEVEL_ERROR,"Cannot read group info! - login denied"); 
    return 0;
  }
  
  pwd = getpwnam(user);
  if (0==pwd)
  {
    log_message(LOG_LEVEL_ERROR, "Cannot read user info! - login denied");
    return 0;
  }
  
  if (g_cfg.sec.ts_users==pwd->pw_gid)
  {
    LOG_DBG("ts_users is user's primary group",1);
    return 1;
  }
  
  i=0;
  while (0!=groups->gr_mem[i])
  {
    LOG_DBG("user: %s", groups->gr_mem[i]);
    if (0==g_strcmp(groups->gr_mem[i], user)) return 1;
    i++;
  }
  
  log_message(LOG_LEVEL_INFO, "login denied for user %s", user);
  
  return 0;
}