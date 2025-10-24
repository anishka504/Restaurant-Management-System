#ifndef INVENTORY_MANAGEMENT_H
#define INVENTORY_MANAGEMENT_H

#include "menu_management.h"

typedef struct {
    char name[MAX_NAME_LEN];
    float quantity;
    float lowLimit;
} InventoryItem;

void restockIngredient(InventoryItem inventory[], int *inventoryCount, int maxInventorySize);
void checkLowStock(InventoryItem inventory[], int inventoryCount);
void updateStockAfterOrder(InventoryItem inventory[], int invCount,
                           MenuItem menu[], int menuItemIndex, int quantityOrdered);
void saveInventoryToFile(const char *filename, InventoryItem inventory[], int inventoryCount);
void loadInventoryFromFile(const char *filename, InventoryItem inventory[], int *inventoryCount, int maxInventorySize);

#endif
