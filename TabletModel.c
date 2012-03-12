#include "TabletModel.h"

#include <stdlib.h>
#include <string.h>

TabletModel tabletModel()
{
    const char* productHardware = getenv("OSSO_PRODUCT_HARDWARE");

    if (productHardware == 0)
    {
        return N770;
    }

    if (strcmp(productHardware, "RX-34") == 0)
    {
        return N800;
    }
    if (strcmp(productHardware, "RX-44") == 0)
    {
        return N810;
    }
    if (strcmp(productHardware, "RX-48") == 0)
    {
        return N810;//WiMax
    }
    if (strcmp(productHardware, "RX-51") == 0)
    {
        return N900;
    }

    // Ok, lets guess that it is 700
    return N770;
}

