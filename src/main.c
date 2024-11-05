#include <stdio.h>
#include <obscapp.h>

#include "linmath.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

int main(void)
{
    struct obsc_app app;
    if (obsc_app__init(&app) ||
        obsc_app__run(&app))
    {
        return 1;
    }

    obsc_app__close(&app);
    return 0;
}