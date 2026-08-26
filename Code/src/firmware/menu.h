// menu.h

#ifndef MENU_H
#define MENU_H

#include "pico/stdlib.h"

// Menu types:


// Menu item types:
typedef enum {
    MENU_I_SUBMENU,     // open submenu
    MENU_I_SELECT,      // inline option selection
    MENU_I_RADIO,       // item of multiple choice
    MENU_I_MULTI_INT,   // ip's etc
    MENU_I_STRING,      // text
} menu_item_type_t;

struct menu_t;

// Menu item, the individual items in a visible menu:
typedef struct menu_item_t {

    // Could fit 10 but menu items are indented and or
    // have a icon in front of it.
    const char* label[9];
    const menu_item_type_t item_type;

    // Only for type Submenu. else leave empty
    const struct menu_t* submenu;
    // TODO: support radio menu

    void* value;

    const void* callback;
    const void* callback_args;

    // Show menu item striketought and don't allow interaction
    // when not active. looke like not implemented yet
    bool active;
    
} menu_item_t;

// Menu, what you see and has items.
typedef struct menu_t {
    const char* title[10];      // 12x16 font. 128 // 12 = 10
    const menu_item_t* items;   // Pointer to array of menu items.
    const uint8_t num_items;    // number of items in said array.
    const uint8_t port;         // 0 if not applicable, 1/2 for A/B (and other expansions later)
} menu_t;



// Menu state machine:

#define MENU_STACK_SIZE 10

typedef struct {
    // menu nesting stack
    const menu_t* stack[MENU_STACK_SIZE];
    uint8_t stack_pointer;

    // save the index of the selected item for each of the menu's
    // visited
    uint8_t selected_index[MENU_STACK_SIZE];

    // 0 = browse
    // 1 = edit current item 
    // 1(+) = edit item + index. eg part of ip addr.
    uint8_t edit;
} menu_state_t;

extern menu_state_t menu_state;


// enter submenu / enter arbirary menu and add to menu stack
void menu_enter_menu(const menu_t *menu);

// go one menu back
void menu_back();

// go back to root menu
void menu_reset();

// push menu onto stack / set current menu
void menu_stack_push(const menu_t *menu);

// pop menu from stack / go one menu back
const menu_t* menu_stack_pop();

// get current menu from stack
const menu_t* menu_stack_current();

// trigger action linked to [menu] button
void menu_button_menu();

// trigger action linked to [up] button
void menu_button_up();

// trigger action linked to [down] button
void menu_button_down();

// trigger action linked to [exit] button
void menu_button_exit();



// menu state machine.
void menu_draw_menu();

void menu_process_menu();

#endif // MENU_H

