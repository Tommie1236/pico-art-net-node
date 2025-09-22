// menu.c

#include "menu.h"


goid menu_enter_menu(const menu_t *menu) {
    menu_stack_push(menu);
    menu_state.edit = 0;
}

void menu_back() {
    menu_stack_pop();
    menu_state.edit = 0;
}

void menu_reset() {
    menu_state.stack_pointer = 0;

    // TODO: change NULL to the root menu! ------ v
    menu_state.stack[menu_state.stack_pointer] = NULL;
    menu_state.selected_index[menu_state.stack_pointer] = 0;
    menu_state.edit = 0;
}

void menu_stack_push(const menu_t *menu) {
    if (menu_state.stack_pointer < MENU_STACK_SIZE - 1) {
        menu_state.stack[++menu_state.stack_pointer] = menu;
    }
}

const menu_t* menu_stack_pop() {
    if (menu_state.stack_pointer > 0) {

        // reset selected index shen moving back up
        menu_state.selected_index[menu_state.stack_pointer] = 0;

        return menu_state.stack[menu_state.stack_pointer--];
    }
    return menu_state.stack[menu_state.stack_pointer];
}

const menu_t* menu_stack_current() {

    return menu_state.stack[menu_state.stack_pointer];
};

void menu_button_menu() {

    const menu_t *menu = menu_stack_current();
    const menu_item_t *selected_item = menu->items + menu_state.selected_index[menu_state.stack_pointer];
    const menu_item_type_t item_type = selected_item->item_type;

    switch(item_type) {
        case MENU_I_SUBMENU: 
            menu_enter_menu(selected_item->submenu);
            break;

        case MENU_I_TOGGLE:
            // selected_item->value = !selected_item->value;
            break;

        case MENU_I_RADIO:
            // uint8_t xxx = menu_state.selected_index[menu_state.stack_pointer];
            break;

        case MENU_I_INT:
            if (menu_state.edit == 0) {
                menu_state.edit = 1;
            } else {
                menu_state.edit = 0;
            }
            break;

        case MENU_I_MULTI_INT:
            break;

        case MENU_I_STRING:
            break;
    }
}


void menu_button_exit() {
    if (menu_state.edit) {
        
        // TODO: fine with single value edits but needs other option for
        // multi int and or string edit.
        menu_state.edit = 0;
    } else {
        menu_back();
        return;        
    }
}

void menu_button_up() {
    if (menu_state.edit) {
        // change value
    } else {
        if (menu_state.selected_index[menu_state.stack_pointer] <= 1) {
            menu_state.selected_index[menu_state.stack_pointer] = menu_state.stack[menu_state.stack_pointer]->num_items - 1;
        } else menu_state.selected_index[menu_state.stack_pointer]--;
    }
}

void menu_button_down() {
    if (menu_state.edit) {
        // change value
    } else {
        if (menu_state.selected_index[menu_state.stack_pointer] >= 2) {
            menu_state.selected_index[menu_state.stack_pointer] = 0;
        } else menu_state.selected_index[menu_state.stack_pointer]++;
    }
}

void menu_draw_menu() {

}
