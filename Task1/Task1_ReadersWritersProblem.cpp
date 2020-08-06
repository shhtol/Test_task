#include <time.h>
#include <Windows.h>
#include <iostream>
#define SLEEP_TIME 1000
#define WRITERS_NUMBER 2
#define READERS_NUMBER 7
#define REITERATION_NUMBER 2

using namespace std;

HANDLE mutex;

HANDLE writers[WRITERS_NUMBER];
HANDLE readers[READERS_NUMBER];

HANDLE autorisedToWrite;
HANDLE autorisedToRead;
LONG lockedReaders = 0;
LONG lockedWriters = 0;
LONG activeReaders = 0;

BOOL isWriting = FALSE;

INT value = 0;

DWORD WINAPI Writer(LPVOID)
{
    for (INT i = 0; i < REITERATION_NUMBER; i++) {
      InterlockedIncrement(&lockedWriters);

      if (isWriting || (activeReaders > 0)) {
             WaitForSingleObject(autorisedToWrite, INFINITE);
         }
		 
      WaitForSingleObject(mutex, INFINITE);
      InterlockedDecrement(&lockedWriters);
      ResetEvent(autorisedToWrite);
      isWriting = TRUE;

      ReleaseMutex(mutex);
	
	  srand(time(NULL) % GetCurrentThreadId());
	  cout << "Writer №" << GetCurrentThreadId() << " writes " << ++value << endl;

      isWriting = FALSE;

      if (lockedReaders > 0){ 
	    SetEvent(autorisedToRead);
      }else{
        SetEvent(autorisedToWrite);
      }

      Sleep(SLEEP_TIME);
    }

    return 0;
}

DWORD WINAPI Reader(LPVOID)
{
	BOOL isWritingCompleted = FALSE;
	
    while (!isWritingCompleted) {
        InterlockedIncrement(&lockedReaders);

        if (isWriting || (lockedWriters > 0)) {
			 
	WaitForSingleObject(autorisedToRead, INFINITE);
				
        }

        InterlockedDecrement(&lockedReaders);
        InterlockedIncrement(&activeReaders);

        SetEvent(autorisedToRead);
		
		srand(time(NULL) % GetCurrentThreadId());
		
		cout << "Reader №" << GetCurrentThreadId() << " reads " << value << endl;

        if(value >= WRITERS_NUMBER * REITERATION_NUMBER) { isWritingCompleted = TRUE; }

        InterlockedDecrement(&activeReaders);

        if (activeReaders == 0) { SetEvent(autorisedToWrite); }

        Sleep(SLEEP_TIME);
    }

    return 0;
}

int main()
{   INT i;
	autorisedToWrite = CreateEvent(NULL, TRUE, TRUE, NULL);
    if (autorisedToWrite == NULL) {
        cerr << "Error with creating event" << endl;
        exit (EXIT_FAILURE);
    }

    autorisedToRead = CreateEvent(NULL, FALSE, TRUE, NULL);
    if (autorisedToRead == NULL){
        cerr << "Error with creating event" << endl;
        exit (EXIT_FAILURE);
    }
	
	 if (!(mutex = CreateMutex(NULL, FALSE, NULL))){
        cerr << "Error with creating mutex" << endl;
        exit (EXIT_FAILURE);
    }
    
    for (i = 0; i < WRITERS_NUMBER; i++){
        writers[i] = CreateThread(NULL, 0, &Writer, NULL, 0, NULL);
        if (writers[i] == NULL) { exit (EXIT_FAILURE); }
    }

    for (i = 0; i < READERS_NUMBER; i++){
        readers[i] = CreateThread(NULL, 0, &Reader, NULL, 0, NULL);
        if (readers[i] == NULL) { exit (EXIT_FAILURE); }
    }

    WaitForMultipleObjects(WRITERS_NUMBER, writers, TRUE, INFINITE);
    WaitForMultipleObjects(READERS_NUMBER, readers, TRUE, INFINITE);

    if (!CloseHandle(autorisedToWrite)){
        cerr << "Error with closing object" << endl;
        exit (EXIT_FAILURE);
    }

    if (!CloseHandle(autorisedToRead)){
        cerr << "Error with closing object" << endl;
        exit (EXIT_FAILURE);
    }

    CloseHandle(mutex);
	system("pause");
    return 0;
}



