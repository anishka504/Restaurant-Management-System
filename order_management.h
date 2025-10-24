#ifndef ORDER_MANAGEMENT_H
#define ORDER_MANAGEMENT_H

#include "menu_management.h"

#define MAX_ORDER_ITEMS 50
#define MAX_TOTAL_ITEMS 50
#define MAX_TOTAL_ORDERS_PENDING 50
#define MAX_TABLES_WITH_ORDERS 10

typedef struct {
    int menuItemIndex;
    int quantity;
} OrderItem;

typedef struct {
    int tableNumber;
    OrderItem items[MAX_ORDER_ITEMS];
    int itemCount;
} Order;

typedef struct {
    int tableNumber;
    OrderItem items[MAX_TOTAL_ITEMS];
    int itemCount;
} TotalOrder;

void placeOrder(Order orders[], int *orderCount, int tableNumber,
                MenuItem menu[], int menuCount, int maxIndividualOrders);

void mergeOrderWithTotal(TotalOrder totalOrders[], int *totalOrderCount, Order newOrder, int maxTableOrders);

void printTotalOrders(const TotalOrder totalOrders[], int totalCount, const MenuItem menu[], int menuCount);

#endif
