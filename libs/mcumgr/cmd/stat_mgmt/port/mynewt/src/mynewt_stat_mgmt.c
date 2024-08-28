/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */


#include "stats/stats.h"
#include "mgmt/mgmt.h"
#include "sysinit/sysinit.h"
#include "stat_mgmt/stat_mgmt.h"
#include "stat_mgmt/stat_mgmt_impl.h"
#include "stat_mgmt/stat_mgmt_config.h"

struct mynewt_stat_mgmt_walk_arg {
    stat_mgmt_foreach_entry_fn *cb;
    void *arg;
};

int
stat_mgmt_impl_get_group(int idx, const char **out_name)
{
    const struct stats_hdr *cur;
    int i;
    int rc;

    rc = MGMT_ERR_ENOENT;

    cur = NULL;
    i = 0;
    STAILQ_FOREACH(cur, &g_stats_registry, s_next) {
        if (i == idx) {
            rc = 0;
            break;
        }
        i++;
    }

    if (!rc) {
        *out_name = cur->s_name;
    }

    return rc;
}

static int
mynewt_stat_mgmt_walk_cb(struct stats_hdr *hdr, void *arg,
                         char *name, uint16_t off)
{
    struct mynewt_stat_mgmt_walk_arg *walk_arg;
    struct stat_mgmt_entry entry;
    void *stat_val;

    walk_arg = arg;

    stat_val = (uint8_t *)hdr + off;
    switch (hdr->s_size) {
    case sizeof (uint16_t):
        entry.value = *(uint16_t *) stat_val;
        break;
    case sizeof (uint32_t):
        entry.value = *(uint32_t *) stat_val;
        break;
    case sizeof (uint64_t):
        entry.value = *(uint64_t *) stat_val;
        break;
    default:
        return MGMT_ERR_EUNKNOWN;
    }
    entry.name = name;

    return walk_arg->cb(&entry, walk_arg->arg);
}

int
stat_mgmt_impl_foreach_entry(const char *group_name,
                             stat_mgmt_foreach_entry_fn *cb,
                             void *arg)
{
    struct mynewt_stat_mgmt_walk_arg walk_arg;
    struct stats_hdr *hdr;

    hdr = stats_group_find(group_name);
    if (hdr == NULL) {
        return MGMT_ERR_ENOENT;
    }

    walk_arg = (struct mynewt_stat_mgmt_walk_arg) {
        .cb = cb,
        .arg = arg,
    };

    return stats_walk(hdr, mynewt_stat_mgmt_walk_cb, &walk_arg);
}

void
stat_mgmt_module_init(void)
{
    /* Ensure this function only gets called by sysinit. */
    SYSINIT_ASSERT_ACTIVE();

    stat_mgmt_register_group();
}
