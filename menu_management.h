#ifndef MENU_MANAGEMENT_H
#define MENU_MANAGEMENT_H

#define MAX_NAME_LEN 50
#define MAX_INGREDIENTS 10

typedef struct {
    char name[MAX_NAME_LEN];
    float quantity;
} Ingredient;

typedef struct {
    char name[MAX_NAME_LEN];
    float price;
    Ingredient ingredients[MAX_INGREDIENTS];
    int ingredientCount;
} MenuItem;

void addMenuItem(MenuItem menu[], int *menuCount, int maxMenuSize);
void editMenuItem(MenuItem menu[], int menuCount);
void removeMenuItem(MenuItem menu[], int *menuCount);
void displayMenu(MenuItem menu[], int menuCount);
void saveMenuToFile(const char *filename, MenuItem menu[], int menuCount);
void loadMenuFromFile(const char *filename, MenuItem menu[], int *menuCount, int maxMenuSize);

#endif
