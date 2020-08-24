extern "C" {
#include "motor/plugins/plugin.h"

#include "private_api.h"
}

#include <cstdio>

#define CR_HOST CR_SAFEST // try to best manage static states
#define CR_DEBUG
#include "cr.h"
