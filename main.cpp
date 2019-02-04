#include <windows.h>
#include <mysql.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#define WIN32_LEAN_AND_MEAN

#pragma comment(lib, "libmysql.lib")


TCHAR* serviceName = TEXT("PontoDigital");
TCHAR* serviceTitle = TEXT("Serviço de Ponto Digital");
TCHAR* servicePath = TEXT("C:\\Arquivos de programas\\Osiris TI\\Ponto Digital\\PontoDigitalCliente.exe");
TCHAR* serviceDeps = TEXT("Netman");
TCHAR* serviceDesc = TEXT("Serviço para Controle de Ponto Digital - by Neo\0");
TCHAR* server = NULL;

HKEY hKey = NULL;

SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE serviceStatusHandle = 0;
HANDLE stopServiceEvent = 0;
HANDLE hf = INVALID_HANDLE_VALUE;

MYSQL con;
char query[1024];
DWORD qry_tam=0;
char  ComputerName [MAX_COMPUTERNAME_LENGTH + 1];
DWORD cbComputerName = sizeof ( ComputerName );

void log(const char*txt)
{
	char buf[1024];
	DWORD mx=0;
	SYSTEMTIME st;
	GetLocalTime(&st);

	mx=wsprintf(buf, "[%04d/%02d/%02d %02d:%02d.%03d] %s\r\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, txt);

	WriteFile(hf, &buf, mx, &mx, NULL);
}


void WINAPI ServiceControlHandler( DWORD controlCode )
{
	switch ( controlCode )
	{
		case SERVICE_CONTROL_INTERROGATE:
			break;

		case SERVICE_CONTROL_SHUTDOWN:
		case SERVICE_CONTROL_STOP:
			{
				log("Parando servico");
				if(NULL == mysql_real_connect(&con, server,"uponto","senha_bd","ponto",0,NULL,0))
				{
					log("Erro ao conectar com MySQL!");
				}
				else
				{
					qry_tam=wsprintf(query,"update controle set saida = now() where host = '%s' and chegada > CURDATE();", ComputerName);

					if(mysql_query(&con, query))
					{
						log("Update realizado com sucesso!");
					}
				}
				mysql_close(&con);

				serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
				SetServiceStatus( serviceStatusHandle, &serviceStatus );

				SetEvent( stopServiceEvent );
			}return;

		case SERVICE_CONTROL_PAUSE:
			log("Servico em pausa!");
			break;

		case SERVICE_CONTROL_CONTINUE:
			log("Continuando servico!");
			break;

		default:
			if ( controlCode >= 128 && controlCode <= 255 )
				// user defined control code
				break;
			else
				// unrecognised control code
				break;
	}

	SetServiceStatus( serviceStatusHandle, &serviceStatus );
}

void WINAPI ServiceMain( DWORD /*argc*/, TCHAR* /*argv*/[] )
{
	mysql_init(&con);
	// initialise service status
	serviceStatus.dwServiceType = SERVICE_WIN32;
	serviceStatus.dwCurrentState = SERVICE_STOPPED;
	serviceStatus.dwControlsAccepted = 0;
	serviceStatus.dwWin32ExitCode = NO_ERROR;
	serviceStatus.dwServiceSpecificExitCode = NO_ERROR;
	serviceStatus.dwCheckPoint = 0;
	serviceStatus.dwWaitHint = 0;

	serviceStatusHandle = RegisterServiceCtrlHandler( serviceName, ServiceControlHandler );

	if ( serviceStatusHandle )
	{
		// service is starting
		serviceStatus.dwCurrentState = SERVICE_START_PENDING;
		SetServiceStatus( serviceStatusHandle, &serviceStatus );

		// do initialisation here
		stopServiceEvent = CreateEvent( 0, FALSE, FALSE, 0 );

		// running
		serviceStatus.dwControlsAccepted |= (SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
		serviceStatus.dwCurrentState = SERVICE_RUNNING;
		SetServiceStatus( serviceStatusHandle, &serviceStatus );

		log("Iniciando servico");

		GetComputerName ( ComputerName, &cbComputerName );

		if(NULL == mysql_real_connect(&con, server, "uponto", "senha_bd","ponto",0,NULL,0))
			{
				log("Erro ao conectar com MySQL!");
			}
		qry_tam=wsprintf(query, "insert into controle (`host`, `chegada`) values('%s', now());", ComputerName);

		if(mysql_query(&con, query))
			{
				log("Registro inserido com sucesso!");
			}
		
		mysql_close(&con);

		/*do
		{
			Beep( 1000, 100 );
		}
		while ( WaitForSingleObject( stopServiceEvent, 5000 ) == WAIT_TIMEOUT );
		*/
		while( WaitForSingleObject( stopServiceEvent, 5000 ) == WAIT_TIMEOUT )
		{
		}

		// service was stopped
		serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		SetServiceStatus( serviceStatusHandle, &serviceStatus );

		// do cleanup here
		CloseHandle( stopServiceEvent );
		stopServiceEvent = 0;

		// service is now stopped
		serviceStatus.dwControlsAccepted &= ~(SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
		serviceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus( serviceStatusHandle, &serviceStatus );
	}
}

void RunService()
{
	BYTE SrvValor[15];
	DWORD tam;

	RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\PontoDigital", NULL, KEY_ALL_ACCESS, &hKey);
	RegQueryValueEx(hKey, "Server", NULL, NULL, SrvValor, &tam);
	RegCloseKey(hKey);

	server = (TCHAR*)SrvValor;

	RegCloseKey(hKey);

	hf=CreateFile("C:\\Arquivos de programas\\Osiris TI\\Ponto Digital\\pontodigital.log", GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	SetFilePointer(hf, 0, NULL, FILE_END);

	log("Abriu arquivo!");

	SERVICE_TABLE_ENTRY serviceTable[] =
	{
		{ serviceName, ServiceMain },
		{ 0, 0 }
	};

	StartServiceCtrlDispatcher( serviceTable );
}

void InstallService(TCHAR* server)
{
	log("Instalando servico!");
	SC_HANDLE serviceControlManager = OpenSCManager( 0, 0, SC_MANAGER_CREATE_SERVICE );

	if ( serviceControlManager )
	{
		TCHAR path[ _MAX_PATH + 1 ];
		if ( GetModuleFileName( 0, path, sizeof(path)/sizeof(path[0]) ) > 0 )
		{
			SC_HANDLE service = CreateService( serviceControlManager,
							serviceName, serviceTitle,
							SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
							SERVICE_AUTO_START, SERVICE_ERROR_IGNORE, servicePath,
							0, 0, serviceDeps, 0, 0 );
			if ( service )
				CloseServiceHandle( service );
		}

		CloseServiceHandle( serviceControlManager );
	}
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\PontoDigital",	NULL, KEY_ALL_ACCESS, &hKey); 
	RegSetValueEx(hKey, "Description", 0, REG_SZ, (BYTE *)serviceDesc, 50);
	RegSetValueEx(hKey, "Server", 0, REG_SZ, (BYTE *)server, 50);
	RegCloseKey(hKey);

}

void UninstallService()
{
	log("Desinstalando servico!");
	SC_HANDLE serviceControlManager = OpenSCManager( 0, 0, SC_MANAGER_CONNECT );

	if ( serviceControlManager )
	{
		SC_HANDLE service = OpenService( serviceControlManager,
			serviceName, SERVICE_QUERY_STATUS | DELETE );
		if ( service )
		{
			SERVICE_STATUS serviceStatus;
			if ( QueryServiceStatus( service, &serviceStatus ) )
			{
				if ( serviceStatus.dwCurrentState == SERVICE_STOPPED )
					DeleteService( service );
			}

			CloseServiceHandle( service );
		}

		CloseServiceHandle( serviceControlManager );
	}
}

int _tmain( int argc, TCHAR* argv[] )
{
	if ( argc > 1 && lstrcmpi( argv[1], TEXT("install") ) == 0 )
	{
		InstallService(argv[2]);
	}
	else if ( argc > 1 && lstrcmpi( argv[1], TEXT("uninstall") ) == 0 )
	{
		UninstallService();
	}
	else
	{
		RunService();
	}

	return 0;
}
