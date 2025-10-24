#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "inventory_management.h"

#define INVENTORY_FILENAME "inventory.dat"

void clear_input_buffer_inv() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int read_until_delimiter_inv(FILE *file, char *buffer, int buffer_size, char delimiter) {
    int i = 0, ch;
    while ((ch = fgetc(file)) != EOF && ch != delimiter && ch != '\n') {
        if (i < buffer_size - 1) {
            buffer[i++] = (char)ch;
        } else {
            while ((ch = fgetc(file)) != EOF && ch != delimiter && ch != '\n');
            buffer[i] = '\0';
            return 0;
        }
    }
    buffer[i] = '\0';
    if (ch == EOF && i == 0) return 0;
    return 1;
}

void saveInventoryToFile(const char *filename, InventoryItem inventory[], int inventoryCount) {
    FILE *file = fopen(filename, "w");
    if (!file) return;

    for (int i = 0; i < inventoryCount; ++i) {
        fprintf(file, "%s|%.2f|%.2f\n", inventory[i].name, inventory[i].quantity, inventory[i].lowLimit);
    }

    fclose(file);
}

void loadInventoryFromFile(const char *filename, InventoryItem inventory[], int *inventoryCount, int maxInventorySize) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        *inventoryCount = 0;
        return;
    }

    *inventoryCount = 0;
    char temp_buffer[256];

    while (*inventoryCount < maxInventorySize && !feof(file)) {
        InventoryItem currentItem;
        int successful_read = 1;

        if (!read_until_delimiter_inv(file, currentItem.name, MAX_NAME_LEN, '|')) {
            if (feof(file) && strlen(currentItem.name) == 0) break;
            successful_read = 0;
        }

        if (successful_read && read_until_delimiter_inv(file, temp_buffer, sizeof(temp_buffer), '|')) {
            currentItem.quantity = atof(temp_buffer);
        } else {
            successful_read = 0;
        }

        if (successful_read && read_until_delimiter_inv(file, temp_buffer, sizeof(temp_buffer), '\n')) {
            currentItem.lowLimit = atof(temp_buffer);
        } else {
            successful_read = 0;
        }

        if (feof(file) && !successful_read && *inventoryCount == 0 && strlen(currentItem.name) == 0) {
            break;
        }

        if (successful_read && strlen(currentItem.name) > 0) {
            inventory[*inventoryCount] = currentItem;
            (*inventoryCount)++;
        } else {
            if (!feof(file)) {
                int c;
                while ((c = fgetc(file)) != '\n' && c != EOF);
            }
        }

        if (feof(file)) break;
    }

    fclose(file);
}

void restockIngredient(InventoryItem inventory[], int *inventoryCount, int maxInventorySize) {
    char name[MAX_NAME_LEN];
    float quantity, lowLimit;

    printf("Enter ingredient name to restock/add: ");
    scanf(" %49[^\n]", name);
    clear_input_buffer_inv();

    int foundIndex = -1;
    for (int i = 0; i < *inventoryCount; ++i) {
        if (strcmp(inventory[i].name, name) == 0) {
            foundIndex = i;
            break;
        }
    }

    if (foundIndex != -1) {
        printf("Ingredient '%s' found. Current stock: %.2f\n", name, inventory[foundIndex].quantity);
        printf("Enter quantity to add: ");
        scanf("%f", &quantity);
        clear_input_buffer_inv();
        inventory[foundIndex].quantity += quantity;
        printf("New stock for '%s': %.2f\n", name, inventory[foundIndex].quantity);
    } else {
        if (*inventoryCount >= maxInventorySize) {
            printf("Inventory is full. Cannot add new ingredient.\n");
            return;
        }
        strcpy(inventory[*inventoryCount].name, name);

        printf("Enter quantity for new ingredient '%s': ", name);
        scanf("%f", &inventory[*inventoryCount].quantity);
        clear_input_buffer_inv();

        printf("Enter low stock limit for '%s': ", name);
        scanf("%f", &inventory[*inventoryCount].lowLimit);
        clear_input_buffer_inv();

        (*inventoryCount)++;
        printf("Ingredient '%s' added to inventory.\n", name);
    }

    saveInventoryToFile(INVENTORY_FILENAME, inventory, *inventoryCount);
}

void checkLowStock(InventoryItem inventory[], int inventoryCount) {
    if (inventoryCount == 0) {
        printf("Inventory is empty.\n");
        return;
    }

    int lowStockCount = 0;
    printf("\n--- Low Stock Alerts ---\n");
    for (int i = 0; i < inventoryCount; ++i) {
        if (inventory[i].quantity < inventory[i].lowLimit) {
            printf("ALERT: '%s' is low! Current: %.2f, Limit: %.2f\n",
                   inventory[i].name, inventory[i].quantity, inventory[i].lowLimit);
            lowStockCount++;
        }
    }

    if (lowStockCount == 0) {
        printf("No ingredients are currently below their low stock limit.\n");
    }

    printf("------------------------\n");
}

void updateStockAfterOrder(InventoryItem inventory[], int invCount,
                           MenuItem menu[], int menuItemIndex, int quantityOrdered) {
    if (menuItemIndex < 0 || !menu) return;

    MenuItem orderedItem = menu[menuItemIndex];
    printf("Updating stock for %d x '%s'.\n", quantityOrdered, orderedItem.name);

    int stockChanged = 0;

    for (int i = 0; i < orderedItem.ingredientCount; ++i) {
        Ingredient *dishIngredient = &orderedItem.ingredients[i];
        int foundInInventory = 0;

        for (int j = 0; j < invCount; ++j) {
            if (strcmp(inventory[j].name, dishIngredient->name) == 0) {
                float quantityToDeduct = dishIngredient->quantity * quantityOrdered;
                inventory[j].quantity -= quantityToDeduct;
                printf("Deducted %.2f of '%s'. New stock: %.2f\n",
                       quantityToDeduct, inventory[j].name, inventory[j].quantity);
                if (inventory[j].quantity < inventory[j].lowLimit) {
                    printf("ALERT: '%s' is now low on stock!\n", inventory[j].name);
                }
                foundInInventory = 1;
                stockChanged = 1;
                break;
            }
        }

        if (!foundInInventory) {
            printf("Warning: Ingredient '%s' for dish '%s' not found in inventory.\n",
                   dishIngredient->name, orderedItem.name);
        }
    }

    if (stockChanged) {
        saveInventoryToFile(INVENTORY_FILENAME, inventory, invCount);
    }
}
