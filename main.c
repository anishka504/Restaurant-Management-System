#include <stdio.h>
#include <stdlib.h>

#include "menu_management.h"
#include "order_management.h"
#include "bill_generation.h"
#include "inventory_management.h"

#define MAX_MENU_ITEMS_IN_RESTAURANT 20
#define MAX_PENDING_ORDERS_MAIN 50
#define MAX_TABLES_FOR_TOTAL_ORDERS_MAIN 10
#define MAX_INVENTORY_ITEMS_IN_STOCK 50

#define MENU_DATA_FILE "menu.dat"
#define INVENTORY_DATA_FILE "inventory.dat"

MenuItem menu[MAX_MENU_ITEMS_IN_RESTAURANT];
int menuCount = 0;

Order pendingOrders[MAX_PENDING_ORDERS_MAIN];
int pendingOrderCount = 0;

TotalOrder totalOrders[MAX_TABLES_FOR_TOTAL_ORDERS_MAIN];
int totalOrderCount = 0;

InventoryItem inventory[MAX_INVENTORY_ITEMS_IN_STOCK];
int inventoryCount = 0;

void clear_input_buffer_main() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    int choice;

    printf("Welcome to the Simple Restaurant Management System!\n");

    loadMenuFromFile(MENU_DATA_FILE, menu, &menuCount, MAX_MENU_ITEMS_IN_RESTAURANT);
    loadInventoryFromFile(INVENTORY_DATA_FILE, inventory, &inventoryCount, MAX_INVENTORY_ITEMS_IN_STOCK);

    do {
        printf("\n========= Restaurant Management System =========\n");
		printf("-------------- Menu Operations --------------\n");
		printf(" 1. Add Menu Item\n");
		printf(" 2. Edit Menu Item\n");
		printf(" 3. Remove Menu Item\n");
		printf(" 4. Display Full Menu\n");
		printf("------------- Order Operations --------------\n");
		printf(" 5. Place New Order for a Table\n");
		printf(" 6. View All Total Orders\n");
		printf("----------- Inventory Operations ------------\n");
		printf(" 7. Restock/Add Ingredient to Inventory\n");
		printf(" 8. Check Low Stock Ingredients\n");
		printf("------------ Billing Operations -------------\n");
		printf(" 9. Generate Bill for a Table\n");
		printf("---------------------------------------------\n");
	    printf(" 0. Exit System\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            clear_input_buffer_main();
            choice = -1;
            continue;
        }
        clear_input_buffer_main();

        switch (choice) {
            case 1: addMenuItem(menu, &menuCount, MAX_MENU_ITEMS_IN_RESTAURANT); break;
            case 2: editMenuItem(menu, menuCount); break;
            case 3: removeMenuItem(menu, &menuCount); break;
            case 4: displayMenu(menu, menuCount); break;
            case 5: {
                if (menuCount == 0) break;

                int tableNum;
                printf("Enter table number for the order: ");
                if (scanf("%d", &tableNum) != 1) {
                    clear_input_buffer_main();
                    break;
                }
                clear_input_buffer_main();

                int initialPendingOrderCount = pendingOrderCount;

                placeOrder(pendingOrders, &pendingOrderCount, tableNum,
                           menu, menuCount, MAX_PENDING_ORDERS_MAIN);

                if (pendingOrderCount > initialPendingOrderCount) {
                    Order newOrderPlaced = pendingOrders[initialPendingOrderCount];

                    mergeOrderWithTotal(totalOrders, &totalOrderCount, newOrderPlaced, MAX_TABLES_FOR_TOTAL_ORDERS_MAIN);

                    for (int i = 0; i < newOrderPlaced.itemCount; ++i) {
                        if (newOrderPlaced.items[i].menuItemIndex >= 0 && newOrderPlaced.items[i].menuItemIndex < menuCount) {
                            updateStockAfterOrder(inventory, inventoryCount, menu,
                                                  newOrderPlaced.items[i].menuItemIndex,
                                                  newOrderPlaced.items[i].quantity);
                        }
                    }
                }
                break;
            }
            case 6:
                printTotalOrders(totalOrders, totalOrderCount, menu, menuCount);
                break;
            case 7:
                restockIngredient(inventory, &inventoryCount, MAX_INVENTORY_ITEMS_IN_STOCK);
                break;
            case 8:
                checkLowStock(inventory, inventoryCount);
                break;
            case 9: {
                if (totalOrderCount == 0) break;

                int tableNumToBill;
                printf("Enter table number to generate bill for: ");
                if (scanf("%d", &tableNumToBill) != 1) {
                    clear_input_buffer_main();
                    break;
                }
                clear_input_buffer_main();

                int tableOrderIndex = -1;
                for (int i = 0; i < totalOrderCount; ++i) {
                    if (totalOrders[i].tableNumber == tableNumToBill) {
                        tableOrderIndex = i;
                        break;
                    }
                }
                if (tableOrderIndex != -1) {
                    printBill(totalOrders[tableOrderIndex], menu, menuCount);
                }
                break;
            }
            case 0: break;
            default: break;
        }
    } while (choice != 0);

    return 0;
}
