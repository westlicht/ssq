/** @file log.h
 *
 * This include file defines a macro to log message using the linux standard
 * loglevels. The output can either be sent to stdout, or to the syslog daemon.
 *
 * Copyright 2004, Fela Management AG
 *
 * @author   Simon Kallweit
 * @version  $Id: bs.h 399 2005-01-05 15:35:56Z ska $
 */
 
#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <syslog.h>

#define NO_SYSLOG

#ifdef NO_SYSLOG
        #define LOG(level, msg, args...)  printf(msg "\n", ##args)
#else
        #define LOG(level, msg, args...)  syslog((level), msg, ##args)
#endif

#endif /* __LOG_H__ */
