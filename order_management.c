#include <stdio.h>
#include <string.h>
#include "order_management.h"

void clear_input_buffer_order() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void placeOrder(Order orders[], int *orderCount, int tableNumber,
                MenuItem menu[], int menuCount, int maxIndividualOrders) {
    if (*orderCount >= maxIndividualOrders) {
        printf("Maximum number of pending orders reached. Cannot place new order now.\n");
        return;
    }
    if (menuCount == 0) {
        printf("Menu is empty. Cannot place an order.\n");
        return;
    }

    Order *newOrder = &orders[*orderCount];
    newOrder->tableNumber = tableNumber;
    newOrder->itemCount = 0;

    int itemIndex, quantity;
    char more_char = 'y';

    printf("\n--- Placing Order for Table %d ---\n", tableNumber);
    displayMenu(menu, menuCount);

    while ((more_char == 'y' || more_char == 'Y') && newOrder->itemCount < MAX_ORDER_ITEMS) {
        printf("Enter menu item index (0 to %d) to add: ", menuCount - 1);
        if (scanf("%d", &itemIndex) != 1) {
            clear_input_buffer_order();
            continue;
        }
        clear_input_buffer_order();
        itemIndex--;
        if (itemIndex < 0 || itemIndex >= menuCount) continue;

        printf("Enter quantity for '%s': ", menu[itemIndex].name);
        if (scanf("%d", &quantity) != 1) {
            clear_input_buffer_order();
            continue;
        }
        clear_input_buffer_order();

        if (quantity <= 0) continue;

        newOrder->items[newOrder->itemCount].menuItemIndex = itemIndex;
        newOrder->items[newOrder->itemCount].quantity = quantity;
        newOrder->itemCount++;

        if (newOrder->itemCount >= MAX_ORDER_ITEMS) break;

        printf("Add more items to this order? (y/n): ");
        scanf(" %c", &more_char);
        clear_input_buffer_order();
    }

    if (newOrder->itemCount > 0) {
        (*orderCount)++;
        printf("Order for table %d finalized with %d item type(s).\n", tableNumber, newOrder->itemCount);
    } else {
        printf("No items added to the order for table %d. Order not placed.\n", tableNumber);
    }
}

void mergeOrderWithTotal(TotalOrder totalOrders[], int *totalOrderCount, Order newOrder, int maxTableOrders) {
    int foundTableIndex = -1;

    for (int i = 0; i < *totalOrderCount; i++) {
        if (totalOrders[i].tableNumber == newOrder.tableNumber) {
            foundTableIndex = i;
            break;
        }
    }

    if (foundTableIndex != -1) {
        TotalOrder *existingTotalOrder = &totalOrders[foundTableIndex];

        for (int j = 0; j < newOrder.itemCount; j++) {
            int newItemIndexInMenu = newOrder.items[j].menuItemIndex;
            int newQuantity = newOrder.items[j].quantity;
            int mergedInExisting = 0;

            for (int k = 0; k < existingTotalOrder->itemCount; k++) {
                if (existingTotalOrder->items[k].menuItemIndex == newItemIndexInMenu) {
                    existingTotalOrder->items[k].quantity += newQuantity;
                    mergedInExisting = 1;
                    break;
                }
            }

            if (!mergedInExisting) {
                if (existingTotalOrder->itemCount < MAX_TOTAL_ITEMS) {
                    existingTotalOrder->items[existingTotalOrder->itemCount].menuItemIndex = newItemIndexInMenu;
                    existingTotalOrder->items[existingTotalOrder->itemCount].quantity = newQuantity;
                    existingTotalOrder->itemCount++;
                }
            }
        }
    } else {
        if (*totalOrderCount < maxTableOrders) {
            TotalOrder *newTotalTableOrder = &totalOrders[*totalOrderCount];
            newTotalTableOrder->tableNumber = newOrder.tableNumber;
            newTotalTableOrder->itemCount = 0;

            for (int i = 0; i < newOrder.itemCount && newTotalTableOrder->itemCount < MAX_TOTAL_ITEMS; i++) {
                newTotalTableOrder->items[newTotalTableOrder->itemCount] = newOrder.items[i];
                newTotalTableOrder->itemCount++;
            }
            (*totalOrderCount)++;
        }
    }
}

void printTotalOrders(const TotalOrder totalOrders[], int totalCount, const MenuItem menu[], int menuCount) {
    printf("\n--- All Active Total Orders ---\n");
    if (totalCount == 0) {
        printf("No total orders recorded yet.\n");
        return;
    }

    for (int i = 0; i < totalCount; i++) {
        printf("Table %d:\n", totalOrders[i].tableNumber);
        if (totalOrders[i].itemCount == 0) {
            printf("  No items in this total order.\n");
        }

        for (int j = 0; j < totalOrders[i].itemCount; j++) {
            int itemIdx = totalOrders[i].items[j].menuItemIndex;
            const char* itemName = "Unknown Item";
            if (itemIdx >= 0 && itemIdx < menuCount) {
                itemName = menu[itemIdx].name;
            }
            printf("  - %s (Index: %d): Quantity %d\n",
                   itemName,
                   itemIdx,
                   totalOrders[i].items[j].quantity);
        }
        printf("\n");
    }
}
