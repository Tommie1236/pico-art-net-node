// menu.h

#ifndef MENU_H
#define MENU_H

#include "pico/stdlib.h"

// Menu types:


// Menu item types:
typedef enum {
    MENU_SUBMENU,
    MENU_TOGGLE,
    MENU_RADIO,
    MENU_INT,
    MENU_MULTI_INT,
    MENU_STRING,
} menu_item_type_t;

struct menu_t;

// Menu item:
typedef struct {
    const char* label;
    const menu_item_type_t item_type;

    const struct menu_t* submenu; // Type: Submenu.
    // Radio menu special type?
    const void *value; 
    bool active;
    
} menu_item_t;

// Menu:
typedef struct {
    const char* title;
    const menu_item_t* items;
    const uint8_t num_items;
} menu_t;



// Menu state machine:

#define MENU_STACK_SIZE 10

typedef struct {
    menu_t* stack[MENU_STACK_SIZE];
    uint8_t stack_pointer;
    uint8_t selected_index[MENU_STACK_SIZE];
    uint8_t edit;   // 0 = browse, 1(+) edit field index
} menu_state_t;

extern menu_state_t menu_state;

void menu_task();

void menu_enter_menu(menu_t *menu);

void menu_back();

void menu_reset();

void menu_stack_push(menu_t *menu);

void menu_stack_pop();

menu_t* menu_stack_current();

void menu_button_menu();

void menu_button_up();

void menu_button_down();

void menu_button_exit();


#endif // MENU_H
