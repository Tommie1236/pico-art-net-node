// menu.c

#include "menu.h"


void menu_enter_menu(menu_t *menu) {
    menu_stack_push(menu);
    menu_state.edit = 0;
}

void menu_back() {
    menu_stack_pop();
    menu_state.edit = 0;
}

void menu_reset() {
    menu_state.stack_pointer = 0;
    // TODO: change NULL to the root menu!
    menu_state.stack[menu_state.stack_pointer] = NULL;
    menu_state.selected_index[menu_state.stack_pointer] = 0;
    menu_state.edit = 0;
}

void menu_stack_push(menu_t *menu) {
    if (menu_state.stack_pointer < MENU_STACK_SIZE - 1) {
        menu_state.stack[++menu_state.stack_pointer] = menu;
    }
}

void menu_stack_pop() {
    if (menu_state.stack_pointer > 0) {
        menu_state.stack_pointer--;
    }
}

menu_t* menu_stack_current() {
    return menu_state.stack[menu_state.stack_pointer];
};
