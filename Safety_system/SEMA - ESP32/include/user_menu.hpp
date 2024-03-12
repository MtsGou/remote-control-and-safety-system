#include <sys_def.h>

///////////////////////////////////////////////////////////////////////////////////////
/* /////////////// MENU DO USUARIO PARA CONFIGURACAO DOS PARAMETROS /////////////////*/

void reset_dados(void);
void Set_initial_config(void);
void Print_menu(void);
void Print_menu_NET(void);
void Print_parameters(void);
void Menu(void);
void Menu_NET(void);
float Change_parameters_float(float var);
int Change_parameters_int(int variable);
String Change_parameters_char(String enter);
int32_t Change_parameters_simple(int ID, int STANDARD);
void user_interface(int ident);
String BufferToSave(void);