// Race_condition_example.cpp : Defines the entry point for the console application.
#include "stdafx.h"

int commonVariable = 0;

DWORD WINAPI Thread_1( LPVOID lpParam )
{
  for(int i=0; i<1000*1000; i++)
  {
    commonVariable++;
  }
  
  return 0;
}

DWORD WINAPI Thread_2( LPVOID lpParam )
{
  for(int i=0; i<1000*1000; i++)
  {
    commonVariable++;
  }
  return 0;
}

DWORD WINAPI Thread_3( LPVOID lpParam )
{
  for(int i=0; i<1000*1000; i++)
  {
    commonVariable++;
  }
  return 0;
}

DWORD WINAPI Thread_4( LPVOID lpParam )
{
  for(int i=0; i<1000*1000; i++)
  {
    commonVariable++;
  }
  return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
  printf("Entered In Main\n");
  HANDLE hThread_1;
  DWORD threadID_1;
  
  HANDLE hThread_2;
  DWORD threadID_2;
  
  HANDLE hThread_3;
  DWORD threadID_3;
  
  HANDLE hThread_4;
  DWORD threadID_4;
  
  // создадим 4 потока
    
  hThread_1 = CreateThread(NULL, // security attributes ( default if NULL )
                           0, // stack SIZE default if 0
                           Thread_1, // Start Address
                           NULL, // input data
                           0, // creational flag ( start if  0 )
                           &threadID_1); // thread ID
  
  hThread_2 = CreateThread(NULL, // security attributes ( default if NULL )
                           0, // stack SIZE default if 0
                           Thread_2, // Start Address
                           NULL, // input data
                           0, // creational flag ( start if  0 )
                           &threadID_2); // thread ID
  
  hThread_3 = CreateThread(NULL, // security attributes ( default if NULL )
                           0, // stack SIZE default if 0
                           Thread_3, // Start Address
                           NULL, // input data
                           0, // creational flag ( start if  0 )
                           &threadID_3); // thread ID
  
  hThread_4 = CreateThread(NULL, // security attributes ( default if NULL )
                           0, // stack SIZE default if 0
                           Thread_4, // Start Address
                           NULL, // input data
                           0, // creational flag ( start if  0 )
                           &threadID_4); // thread ID
  
  
  // ждем, пока все потоки завершатся
  WaitForSingleObject(hThread_1, INFINITE);
  WaitForSingleObject(hThread_2, INFINITE);
  WaitForSingleObject(hThread_3, INFINITE);
  WaitForSingleObject(hThread_4, INFINITE);
  
  // выводим результат
  printf("Final value = %d\n", commonVariable);   
  
  
  CloseHandle(hThread_1);
  CloseHandle(hThread_2);
  CloseHandle(hThread_3);
  CloseHandle(hThread_4);
  
  
  getchar();
  
  
  return 0;
}

