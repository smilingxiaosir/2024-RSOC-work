#include "wifi_connection.h"
#include <msh.h>

#define WIFI_SSID "xiao"                    /*WIFI 名称*/
#define WIFI_PASSWORD "xiao12345"           /*WIFI 密码*/

void wifi_connection(void *args)
{
    char wifi_scan_command[] = "wifi scan";
    char wifi_connection_command[] = "wifi join " WIFI_SSID " " WIFI_PASSWORD;
    msh_exec(wifi_scan_command, rt_strlen(wifi_scan_command));
    msh_exec(wifi_connection_command, rt_strlen(wifi_connection_command));
}
