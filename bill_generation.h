#ifndef BILL_GENERATION_H
#define BILL_GENERATION_H

#include "menu_management.h"
#include "order_management.h"

float calculateBill(TotalOrder order, MenuItem menu[], int menuCount);
void printBill(TotalOrder order, MenuItem menu[], int menuCount);

#endif
