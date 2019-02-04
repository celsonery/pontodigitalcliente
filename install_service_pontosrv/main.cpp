#include <windows.h>

const char SERVICO_NOME[] = "pontodigital";
const char SERVICO_TITULO[] = "Serviço de Controle de Ponto Digital";
const char SERVICO_ARQUIVO[] = "C:\\Arquivos de Programas\\Osiris TI\\Ponto Digital\\pontodigitalsrv.exe";

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
			SC_HANDLE hSCM, hSCS;

			if(!(hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE)))
			{
				MessageBox(NULL,"Não foi possível gerenciar servico!","Ponto Digital", MB_OK);
			}
/*
			if(hSCS = OpenService(hSCM, SERVICO_NOME, SERVICE_ALL_ACCESS))
			{
				MessageBox(NULL, "Serviço especificado encontrado. Deseja realmente apaga-lo?", "Ponto Digital", MB_YESNO | MB_ICONQUESTION);
				DeleteService(hSCS);
				CloseServiceHandle(hSCM);
				CloseServiceHandle(hSCS);
			}else{
				MessageBox(NULL, "Serviço especificado inexistente!","Ponto Digital", MB_OK | MB_ICONERROR);

			}
*/
			if(hSCS = CreateService(hSCM,
							SERVICO_NOME,
							SERVICO_TITULO,
							SERVICE_ALL_ACCESS,
							SERVICE_WIN32_OWN_PROCESS,
							SERVICE_AUTO_START,
							SERVICE_ERROR_NORMAL,
							SERVICO_ARQUIVO,
							NULL,
							NULL,
							NULL,
							NULL,
							NULL))
			{
				MessageBox(NULL, "Criei Servico", "Ponto Digital", MB_OK);
			}
			
	return 0;

}
