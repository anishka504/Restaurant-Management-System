#include <stdio.h>
#include <string.h>
#include "bill_generation.h"

float calculateBill(TotalOrder order, MenuItem menu[], int menuCount) {
    float subtotal = 0.0;
    for (int i = 0; i < order.itemCount; i++) {
        if (order.items[i].menuItemIndex >= 0 && order.items[i].menuItemIndex < menuCount) {
            float price = menu[order.items[i].menuItemIndex].price;
            subtotal += price * order.items[i].quantity;
        } else {
            printf("Warning: Invalid menu item index %d in order for table %d. Item skipped in subtotal.\n",
                   order.items[i].menuItemIndex, order.tableNumber);
        }
    }
    float tax = subtotal * 0.1f;
    return subtotal + tax;
}

void printBill(TotalOrder order, MenuItem menu[], int menuCount) {
    float subtotal = 0.0;
    printf("\nBill for Table #%d\n", order.tableNumber);
    printf("--------------------------------------------------\n");
    printf("%-20s %5s %10s %10s\n", "Item", "Qty", "Price", "Subtotal");
    printf("--------------------------------------------------\n");

    for (int i = 0; i < order.itemCount; i++) {
        if (order.items[i].menuItemIndex >= 0 && order.items[i].menuItemIndex < menuCount) {
            const char* itemName = menu[order.items[i].menuItemIndex].name;
            float price = menu[order.items[i].menuItemIndex].price;
            float itemTotal = price * order.items[i].quantity;
            subtotal += itemTotal;
            printf("%-20s %5d ₹%9.2f ₹%9.2f\n", itemName, order.items[i].quantity, price, itemTotal);
        } else {
            printf("%-20s %5d %10s %10s\n", "Unknown Item", order.items[i].quantity, "N/A", "N/A");
        }
    }

    float tax = subtotal * 0.1f;
    float total = subtotal + tax;

    printf("--------------------------------------------------\n");
    printf("%35s ₹%.2f\n", "Subtotal:", subtotal);
    printf("%35s ₹%.2f\n", "Tax (10%):", tax);
    printf("%35s ₹%.2f\n", "Total:", total);
    printf("--------------------------------------------------\n");
    printf("Thank you!\n\n");
}
