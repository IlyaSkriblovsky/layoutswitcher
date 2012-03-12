#ifndef __TABLETVERSION_H
#define __TABLETVERSION_H

typedef enum _TabletModel
{
    Unknown,
    N770, // Yeah, i know that it is not N
    N800,
    N810,
    N900
} TabletModel;

TabletModel tabletModel();

#endif
