#include <stdio.h>
#include <stdlib.h>

#include "type/data-page.h"

DataPage createDataPage() {
    DataPage dataPage;
    dataPage.start = malloc(PAGE_SIZE);
    dataPage.freeSpace = dataPage.start;
    dataPage.spaceLeft = PAGE_SIZE;

    return dataPage;
}
