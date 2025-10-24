#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "menu_management.h"

#define MENU_FILENAME "menu.dat"

void clear_input_buffer_menu() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int read_until_delimiter(FILE *file, char *buffer, int buffer_size, char delimiter) {
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

void saveMenuToFile(const char *filename, MenuItem menu[], int menuCount) {
    FILE *file = fopen(filename, "w");
    if (!file) return;

    for (int i = 0; i < menuCount; ++i) {
        fprintf(file, "%s|%.2f|%d", menu[i].name, menu[i].price, menu[i].ingredientCount);
        for (int j = 0; j < menu[i].ingredientCount; ++j) {
            fprintf(file, "|%s|%.2f", menu[i].ingredients[j].name, menu[i].ingredients[j].quantity);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

void loadMenuFromFile(const char *filename, MenuItem menu[], int *menuCount, int maxMenuSize) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        *menuCount = 0;
        return;
    }

    *menuCount = 0;
    char temp_buffer[256];

    while (*menuCount < maxMenuSize && !feof(file)) {
        MenuItem currentItem;
        int successful_read = 1;

        if (!read_until_delimiter(file, currentItem.name, MAX_NAME_LEN, '|')) {
            if (feof(file) && strlen(currentItem.name) == 0) break;
            successful_read = 0;
        }

        if (successful_read && read_until_delimiter(file, temp_buffer, sizeof(temp_buffer), '|')) {
            currentItem.price = atof(temp_buffer);
        } else {
            successful_read = 0;
        }

        if (successful_read && read_until_delimiter(file, temp_buffer, sizeof(temp_buffer), '|')) {
            currentItem.ingredientCount = atoi(temp_buffer);
            if (currentItem.ingredientCount > MAX_INGREDIENTS) currentItem.ingredientCount = MAX_INGREDIENTS;
            if (currentItem.ingredientCount < 0) currentItem.ingredientCount = 0;
        } else {
            successful_read = 0;
        }

        for (int i = 0; i < currentItem.ingredientCount && successful_read; ++i) {
            if (!read_until_delimiter(file, currentItem.ingredients[i].name, MAX_NAME_LEN, '|')) {
                successful_read = 0; break;
            }
            if (read_until_delimiter(file, temp_buffer, sizeof(temp_buffer), (i == currentItem.ingredientCount - 1) ? '\n' : '|')) {
                currentItem.ingredients[i].quantity = atof(temp_buffer);
            } else {
                successful_read = 0; break;
            }
        }

        if (successful_read && strlen(currentItem.name) > 0) {
            menu[*menuCount] = currentItem;
            (*menuCount)++;
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

void addMenuItem(MenuItem menu[], int *menuCount, int maxMenuSize) {
    if (*menuCount >= maxMenuSize) return;

    MenuItem newItem;
    printf("Enter new menu item name: ");
    scanf(" %49[^\n]", newItem.name);
    clear_input_buffer_menu();

    printf("Enter price: ");
    scanf("%f", &newItem.price);
    clear_input_buffer_menu();

    printf("Enter number of ingredients (max %d): ", MAX_INGREDIENTS);
    scanf("%d", &newItem.ingredientCount);
    clear_input_buffer_menu();

    if (newItem.ingredientCount > MAX_INGREDIENTS) newItem.ingredientCount = MAX_INGREDIENTS;
    if (newItem.ingredientCount < 0) newItem.ingredientCount = 0;

    for (int i = 0; i < newItem.ingredientCount; ++i) {
        printf("Enter ingredient %d name: ", i + 1);
        scanf(" %49[^\n]", newItem.ingredients[i].name);
        clear_input_buffer_menu();
        printf("Enter quantity for %s: ", newItem.ingredients[i].name);
        scanf("%f", &newItem.ingredients[i].quantity);
        clear_input_buffer_menu();
    }

    menu[*menuCount] = newItem;
    (*menuCount)++;
    saveMenuToFile(MENU_FILENAME, menu, *menuCount);
}

void editMenuItem(MenuItem menu[], int menuCount) {
    if (menuCount == 0) return;

    int index;
    displayMenu(menu, menuCount);
    printf("Enter index of item to edit (0 to %d): ", menuCount - 1);
    scanf("%d", &index);
    clear_input_buffer_menu();
    index--;
    if (index < 0 || index >= menuCount) return;

    MenuItem *itemToEdit = &menu[index];
    
    printf("Enter new name (or press Enter to keep): ");
    char newName[MAX_NAME_LEN];
    fgets(newName, MAX_NAME_LEN, stdin); 
    if (newName[0] != '\n') {
        newName[strcspn(newName, "\n")] = 0; 
        strcpy(itemToEdit->name, newName);
    }

    float newPrice;
    printf("Enter new price (or -1 to keep): ");
    scanf("%f", &newPrice);
    clear_input_buffer_menu();
    if (newPrice != -1) itemToEdit->price = newPrice;

    char choice;
    printf("Edit ingredients? (y/n): ");
    scanf(" %c", &choice);
    clear_input_buffer_menu();

    if (choice == 'y' || choice == 'Y') {
        printf("Enter new number of ingredients (max %d): ", MAX_INGREDIENTS);
        int newIngCount;
        scanf("%d", &newIngCount);
        clear_input_buffer_menu();

        if (newIngCount > MAX_INGREDIENTS) newIngCount = MAX_INGREDIENTS;
        if (newIngCount < 0) newIngCount = 0;
        itemToEdit->ingredientCount = newIngCount;

        for (int i = 0; i < newIngCount; ++i) {
            printf("Enter ingredient %d name: ", i + 1);
            scanf(" %49[^\n]", itemToEdit->ingredients[i].name);
            clear_input_buffer_menu();
            printf("Enter quantity for %s: ", itemToEdit->ingredients[i].name);
            scanf("%f", &itemToEdit->ingredients[i].quantity);
            clear_input_buffer_menu();
        }
    }

    saveMenuToFile(MENU_FILENAME, menu, menuCount);
}

void removeMenuItem(MenuItem menu[], int *menuCount) {
    if (*menuCount == 0) return;

    int index;
    displayMenu(menu, *menuCount);
    printf("Enter index of item to remove (0 to %d): ", *menuCount - 1);
    scanf("%d", &index);
    clear_input_buffer_menu();
    index--;

    if (index < 0 || index >= *menuCount) return;

    for (int i = index; i < *menuCount - 1; ++i) {
        menu[i] = menu[i + 1];
    }
    (*menuCount)--;
    saveMenuToFile(MENU_FILENAME, menu, *menuCount);
}

void displayMenu(MenuItem menu[], int menuCount) {
    if (menuCount == 0) {
        printf("Menu is currently empty.\n");
        return;
    }

    printf("\n--- Restaurant Menu ---\n");
    printf("Idx | Name                 | Price\n");
    printf("----|----------------------|--------\n");
    for (int i = 0; i < menuCount; ++i) {
        printf("%-3d | %-20s | ₹%.2f\n", i+1, menu[i].name, menu[i].price);
    }
    printf("-----------------------\n");
}
