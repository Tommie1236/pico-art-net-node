
#include "menu.h"




// Main (root)
//  - Network
//      - IP (config)
//          ip: (edit)
//          sn: (edit)
//  - Ports
//      - A
//      - B
//          - status (mode)
//              output
//              input
//              disabled
//          - universe
//              net
//              subnet
//              universe
//              (or just a 16bit int)
//  - Stutus (info page)

menu_t Menu_Main;

menu_t Menu_Network;
menu_t Menu_Ip_config;
menu_t Menu_Ip;
menu_t Menu_Sn;

menu_t Menu_Ports;
menu_t Menu_Port_X;
menu_t Menu_Status_X;
menu_t Menu_Universe_X;

menu_t Menu_status;
