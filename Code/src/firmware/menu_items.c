
#include "menu.h"






menu_t Menu_Network;
menu_t Menu_Ip_config;
menu_t Menu_Ip;
menu_t Menu_Sn;

menu_t Menu_Ports;
menu_t Menu_Port_X;
menu_t Menu_Status_X;
menu_t Menu_Universe_X;

menu_t Menu_status;

// =====

menu_t Menu_Status;

menu_t Menu_Root;
    menu_t Menu_Port_Config;
        menu_t Menu_Port_A;
            menu_t Menu_PA_Direction;
            menu_t Menu_PA_Source;
            menu_t Menu_PA_Uni_Artnet;
            menu_t Menu_PA_Uni_Sacn;
            menu_t Menu_PA_Uni_USB;
        menu_t Menu_Port_B;
            menu_t Menu_PB_Direction;
            menu_t Menu_PB_Source;
            menu_t Menu_PB_Uni_Artnet;
            menu_t Menu_PB_Uni_Sacn;
            menu_t Menu_PB_Uni_USB;
    menu_t Menu_Network_Config;
        menu_t Menu_IP_Info;
        menu_t Menu_IP_Config;
    menu_t Menu_Node_Info;
    menu_t Menu_Dmx_Config;
        menu_t Menu_Dmx_Timeout;
            menu_t Menu_Dmx_Timeout_Action;
        menu_t Menu_Dmx_Merge;
            menu_t Menu_Dmx_Merge_A;
                menu_t Menu_Dmx_Merge_A1;
            menu_t Menu_Dmx_Merge_B;

        menu_t Menu_Dmx_Refresh;
    menu_t Menu_Factory_Reset;
