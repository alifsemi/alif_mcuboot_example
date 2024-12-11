#ifndef MCUMGR_OS_PORT_H
#define MCUMGR_OS_PORT_H

typedef void(*reset_cb)(void);

int32_t os_mgmt_impl_init(reset_cb cb);

#endif
