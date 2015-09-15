/*
 * Copyright (c) 2004-2006 Maxim Sobolev <sobomax@FreeBSD.org>
 * Copyright (c) 2006-2015 Sippy Software, Inc., http://www.sippysoft.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <sys/stat.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <pthread.h>

#include "rtpp_log.h"
#include "rtpp_cfg_stable.h"
#include "rtpp_sessinfo.h"
#include "rtpp_util.h"

struct rtpp_sessinfo *
rtpp_sessinfo_ctor(struct rtpp_cfg_stable *cfsp)
{
    struct rtpp_sessinfo *sessinfo;

    sessinfo = rtpp_zmalloc(sizeof(struct rtpp_sessinfo));
    if (sessinfo == NULL) {
        return (NULL);
    }
    sessinfo->sessions = rtpp_zmalloc((sizeof sessinfo->sessions[0]) *
      (((cfsp->port_max - cfsp->port_min + 1)) + 1));
    if (sessinfo->sessions == NULL) {
        goto e0;
    }
    sessinfo->rtp_servers =  rtpp_zmalloc((sizeof sessinfo->rtp_servers[0]) *
      (((cfsp->port_max - cfsp->port_min + 1) * 2) + 1));
    if (sessinfo->rtp_servers == NULL) {
        goto e1;
    }
    sessinfo->pfds_rtp = rtpp_zmalloc((sizeof sessinfo->pfds_rtp[0]) *
      (((cfsp->port_max - cfsp->port_min + 1)) + 1));
    if (sessinfo->pfds_rtp == NULL) {
        goto e2;
    }
    sessinfo->pfds_rtcp = rtpp_zmalloc((sizeof sessinfo->pfds_rtcp[0]) *
      (((cfsp->port_max - cfsp->port_min + 1)) + 1));
    if (sessinfo->pfds_rtcp == NULL) {
        goto e3;
    }
    pthread_mutex_init(&sessinfo->lock, NULL);

    sessinfo->sessions[0] = NULL;
    sessinfo->nsessions = 0;
    sessinfo->rtp_nsessions = 0;

    return (sessinfo);

e3:
    free(sessinfo->pfds_rtp);
e2:
    free(sessinfo->rtp_servers);
e1:
    free(sessinfo->sessions);
e0:
    free(sessinfo);
    return (NULL);
}