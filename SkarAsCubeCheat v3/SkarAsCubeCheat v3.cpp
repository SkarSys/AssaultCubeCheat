#include <windows.h>
#include <iostream>
#include <tlhelp32.h> 
#include <tchar.h> 
#include <vector> 
#include <stdlib.h> 
#include <dwmapi.h>
#include <cmath>


using namespace std;

void DrawRedDot(HDC hdc, int x, int y) {
    HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0)); 
    SelectObject(hdc, redBrush);
    Ellipse(hdc, x - 5, y - 5, x + 5, y + 5); 
    DeleteObject(redBrush);
}

DWORD GetModuleBaseAddress(TCHAR* lpszModuleName, DWORD pID) {
    DWORD dwModuleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID); // make snapshot of all modules within process
    MODULEENTRY32 ModuleEntry32 = { 0 };
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &ModuleEntry32)) //store first Module in ModuleEntry32
    {
        do {
            if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0) // if Found Module matches Module we look for -> done!
            {
                dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapshot, &ModuleEntry32)); // go through Module entries in Snapshot and store in ModuleEntry32
    }
    CloseHandle(hSnapshot);
    return dwModuleBaseAddress;
}

DWORD GetPointerAddress(HWND hwnd, DWORD gameBaseAddr, DWORD address, vector<DWORD> offsets)
{
    DWORD pID = NULL; // Game process ID
    GetWindowThreadProcessId(hwnd, &pID);
    HANDLE phandle = NULL;
    phandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
    if (phandle == INVALID_HANDLE_VALUE || phandle == NULL);

    DWORD offset_null = NULL;
    ReadProcessMemory(phandle, (LPVOID*)(gameBaseAddr + address), &offset_null, sizeof(offset_null), 0);
    DWORD pointeraddress = offset_null; // the address we need
    for (int i = 0; i < offsets.size() - 1; i++) // we dont want to change the last offset value so we do -1
    {
        ReadProcessMemory(phandle, (LPVOID*)(pointeraddress + offsets.at(i)), &pointeraddress, sizeof(pointeraddress), 0);
    }
    return pointeraddress += offsets.at(offsets.size() - 1); // adding the last offset
}

struct Vector3D {
    float x, y, z;
};

Vector3D ReadPlayerCoordinates(HANDLE processHandle, uintptr_t xAddress, uintptr_t yAddress, uintptr_t zAddress) {
    Vector3D playerCoords;

    ReadProcessMemory(processHandle, (LPVOID)xAddress, &playerCoords.x, sizeof(playerCoords.x), 0);
    ReadProcessMemory(processHandle, (LPVOID)yAddress, &playerCoords.y, sizeof(playerCoords.y), 0);
    ReadProcessMemory(processHandle, (LPVOID)zAddress, &playerCoords.z, sizeof(playerCoords.z), 0);

    return playerCoords;
}

Vector3D WorldToScreen(Vector3D playerCoords, float viewMatrix[16], float screenWidth, float screenHeight) {
    Vector3D screenPosition;

    screenPosition.x = playerCoords.x * viewMatrix[0] + playerCoords.y * viewMatrix[1] + playerCoords.z * viewMatrix[2] + viewMatrix[3];
    screenPosition.y = playerCoords.x * viewMatrix[4] + playerCoords.y * viewMatrix[5] + playerCoords.z * viewMatrix[6] + viewMatrix[7];
    float w = playerCoords.x * viewMatrix[12] + playerCoords.y * viewMatrix[13] + playerCoords.z * viewMatrix[14] + viewMatrix[15];

    screenPosition.x /= w;
    screenPosition.y /= w;

    screenPosition.x = (screenPosition.x + 1.0f) * 0.5f * screenWidth;
    screenPosition.y = (1.0f - screenPosition.y) * 0.5f * screenHeight;

    return screenPosition;
}
void DrawDot(HDC hdc, int x, int y) {
    SetPixel(hdc, x, y, RGB(255, 0, 0));
}

int main()
{


    HWND hwnd_AC = FindWindowA(NULL, "AssaultCube"); //getting the handle to window

    if (hwnd_AC != FALSE) {
        DWORD pID = NULL;
        GetWindowThreadProcessId(hwnd_AC, &pID);
        HANDLE phandle = NULL;
        phandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
        if (phandle == INVALID_HANDLE_VALUE || phandle == NULL);

        TCHAR gamemodule1[] = _T("ac_client.exe");
        DWORD gamebaseaddress1 = GetModuleBaseAddress(gamemodule1, pID);


        //getting matrix ptr addy
        DWORD matrixAddress = 0x0005AB58; // address for matirx
        vector<DWORD> matrixOffsets{ 0x34 }; // offsets for matrix
        DWORD viewMatrixPtrAddr = GetPointerAddress(hwnd_AC, gamebaseaddress1, matrixAddress, matrixOffsets);
       
       


        //define entity pointer/addy, entity number, offset of value to read
         
        DWORD entityAddr = 0x18AC04; // address for the entity list


        vector<DWORD> healthOffset1{ 0x4, 0xEC }; // 0x4 is first entity, 0xES health address
        vector<DWORD> eastwestoffset1{ 0x4, 0x28 }; // 0x4 is first entity, 0xES health address
        vector<DWORD> hightoffset1{ 0x4, 0x30 };
        vector<DWORD> northsouthoffset1{ 0x4, 0x2C };
        DWORD entity1northsouth = GetPointerAddress(hwnd_AC, gamebaseaddress1, entityAddr, northsouthoffset1);
        DWORD entity1eastwest = GetPointerAddress(hwnd_AC, gamebaseaddress1, entityAddr, eastwestoffset1);
        DWORD entity1health = GetPointerAddress(hwnd_AC, gamebaseaddress1, entityAddr, healthOffset1);
        DWORD entity1hight = GetPointerAddress(hwnd_AC, gamebaseaddress1, entityAddr, hightoffset1);

        vector<DWORD> healthOffset2{ 0x8, 0xEC };
        vector<DWORD> eastwestoffset2{ 0x8, 0x28 }; // 0x4 is first entity, 0xES health address
        vector<DWORD> hightoffset2{ 0x8, 0x30 };
        vector<DWORD> northsouthoffset2{ 0x8, 0x2C };
        DWORD entity2northsouth = GetPointerAddress(hwnd_AC, gamebaseaddress1, entityAddr, northsouthoffset2);
        DWORD entity2eastwest = GetPointerAddress(hwnd_AC, gamebaseaddress1, entityAddr, eastwestoffset2);
        DWORD entity2health = GetPointerAddress(hwnd_AC, gamebaseaddress1, entityAddr, healthOffset2);
        DWORD entity2hight = GetPointerAddress(hwnd_AC, gamebaseaddress1, entityAddr, hightoffset2);

        vector<DWORD> healthOffset3{ 0xC, 0xEC };
        vector<DWORD> eastwestoffset3{ 0xC, 0x28 }; // 0x4 is first entity, 0xES health address
        vector<DWORD> hightoffset3{ 0xC, 0x30 };
        vector<DWORD> northsouthoffset3{ 0xC, 0x2C };
        DWORD entity3northsouth = GetPointerAddress(hwnd_AC, gamebaseaddress1, entityAddr, northsouthoffset3);
        DWORD entity3eastwest = GetPointerAddress(hwnd_AC, gamebaseaddress1, entityAddr, eastwestoffset3);
        DWORD entity3health = GetPointerAddress(hwnd_AC, gamebaseaddress1, entityAddr, healthOffset3);
        DWORD entity3hight = GetPointerAddress(hwnd_AC, gamebaseaddress1, entityAddr, hightoffset3);


        //my own hight to read
        DWORD hightAddr = 0x0018AC00;
        vector<DWORD> hightOffsets{ 0x30 };
        DWORD hightPtrAddr = GetPointerAddress(hwnd_AC, gamebaseaddress1, hightAddr, hightOffsets);
        //my eastwest to read
        DWORD myEastWestAddr = 0x0017E0A8;
        vector<DWORD> myEastWestOffset{ 0x28 };
        DWORD myEastWestPtrAddr = GetPointerAddress(hwnd_AC, gamebaseaddress1, myEastWestAddr, myEastWestOffset);
        //my north south to read
        DWORD myNorthSouthAddr = 0x0017E0A8;
        vector<DWORD> myNorthSouthOffset{ 0x2C };
        DWORD myNorthSouthPtrAddr = GetPointerAddress(hwnd_AC, gamebaseaddress1, myNorthSouthAddr, myNorthSouthOffset);
        //reading our view matirx
      /*  DWORD mtrxaddr = 0x0005AB58;
        vector<DWORD> mtrxoffset{ 0x34 };
        DWORD mtrxPtrAddr = GetPointerAddress(hwnd_AC, gamebaseaddress1, mtrxaddr, mtrxoffset);*/
        
        //Hack Loop
        while (true)
        {
         
            //read and print entity healths/y coord/z coord/x
            int health1;
            ReadProcessMemory(phandle, (LPVOID)entity1health, &health1, sizeof(health1), 0);
            std::cout << "Current Health Entity [1] " << health1 << std::endl;
            float eastwest1;
            ReadProcessMemory(phandle, (LPVOID)entity1eastwest, &eastwest1, sizeof(eastwest1), 0);
            std::cout << "Current east/west Entity [1] " << eastwest1 << std::endl;
            float hight1;
            ReadProcessMemory(phandle, (LPVOID)entity1hight, &hight1, sizeof(hight1), 0);
            std::cout << "Current hight Entity [1] " << hight1 << std::endl;
            float northsouth1;
            ReadProcessMemory(phandle, (LPVOID)entity1northsouth, &northsouth1, sizeof(northsouth1), 0);
            std::cout << "Current norht/south Entity [1] " << northsouth1 << std::endl;

            std::cout << "------------------ " << std::endl;

            int health2;
            ReadProcessMemory(phandle, (LPVOID)entity2health, &health2, sizeof(health2), 0);
            std::cout << "Current Health Entity [2] " << health2 << std::endl;
            float eastwest2;
            ReadProcessMemory(phandle, (LPVOID)entity2eastwest, &eastwest2, sizeof(eastwest2), 0);
            std::cout << "Current east/west Entity [2] " << eastwest2 << std::endl;
            float hight2;
            ReadProcessMemory(phandle, (LPVOID)entity2hight, &hight2, sizeof(hight2), 0);
            std::cout << "Current hight Entity [2] " << hight2 << std::endl;
            float northsouth2;
            ReadProcessMemory(phandle, (LPVOID)entity2northsouth, &northsouth2, sizeof(northsouth2), 0);
            std::cout << "Current norht/south Entity [2] " << northsouth2 << std::endl;

            std::cout << "------------------ " << std::endl;


            int health3;
            ReadProcessMemory(phandle, (LPVOID)entity3health, &health3, sizeof(health3), 0);
            std::cout << "Current Health Entity [3] " << health3 << std::endl;
            float eastwest3;
            ReadProcessMemory(phandle, (LPVOID)entity3eastwest, &eastwest3, sizeof(eastwest3), 0);
            std::cout << "Current east/west Entity [3] " << eastwest3 << std::endl;
            float hight3;
            ReadProcessMemory(phandle, (LPVOID)entity3hight, &hight3, sizeof(hight3), 0);
            std::cout << "Current hight Entity [3] " << hight3 << std::endl;
            float northsouth3;
            ReadProcessMemory(phandle, (LPVOID)entity3northsouth, &northsouth3, sizeof(northsouth3), 0);
            std::cout << "Current norht/south Entity [3] " << northsouth3 << std::endl;


            std::cout << "------------------ " << std::endl;


          
            float myEW;
            ReadProcessMemory(phandle, (LPVOID)myEastWestPtrAddr, &myEW, sizeof(myEW), 0);
            std::cout << "My east/west " << myEW << std::endl;
            float myH;
            ReadProcessMemory(phandle, (LPVOID)hightPtrAddr, &myH, sizeof(myH), 0);
            std::cout << "My hight Entity  " << myH << std::endl;
            float myNS;
            ReadProcessMemory(phandle, (LPVOID)myNorthSouthPtrAddr, &myNS, sizeof(myNS), 0);
            std::cout << "My norht/south  " << myNS << std::endl;
            float myviewmtrx; // [16] ;
            ReadProcessMemory(phandle, (LPVOID)viewMatrixPtrAddr, &myviewmtrx, sizeof(myviewmtrx), 0);
            std::cout << "My view matrix" << myviewmtrx << std::endl;

            std::cout << "------------------ " << std::endl;

            //W2S
            uintptr_t xAddress = reinterpret_cast<uintptr_t>(&eastwest1); //  x coordinate address
            uintptr_t yAddress = reinterpret_cast<uintptr_t>(&northsouth1); // y coordinate address
            uintptr_t zAddress = reinterpret_cast<uintptr_t>(&hight1); // Z coordinate address


            Vector3D playerCoords = ReadPlayerCoordinates(phandle, xAddress, yAddress, zAddress);

          
            float viewMatrix[16];
           
            
            ReadProcessMemory(phandle, (LPVOID)viewMatrixPtrAddr, &viewMatrix, sizeof(viewMatrix), 0);



            float screenWidth = 2560.0f; 
            float screenHeight = 1440.0f; 

            Vector3D screenPosition = WorldToScreen(playerCoords, viewMatrix, screenWidth, screenHeight);

            std::cout << " [ ! ]  Player's 2D Screen Position: (" << screenPosition.x << ", " << screenPosition.y << ")" << std::endl;

            HDC hdc = GetDC(NULL);

            HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));

            SelectObject(hdc, redBrush);

            Ellipse(hdc, screenPosition.x - 5, screenPosition.y - 5, screenPosition.x + 5, screenPosition.y + 5);
            ReleaseDC(NULL, hdc);
            DeleteObject(redBrush);

         //   std::cout << " [ ! ] view matrix value: " << viewMatrix << std::endl;

            //Sleep(1000);
            


            
            // ## ----- Teleport on player

            if (GetAsyncKeyState(VK_F1)) // if u press f1, write ur coords to enemy 1 in entity list coords
            {

                float newCoordsHight = hight1; //write to enemy 1 hight
                WriteProcessMemory(phandle, (LPVOID*)(hightPtrAddr), &newCoordsHight, sizeof(newCoordsHight), 0);
                float newCoordsEastWest = eastwest1; //write to enemy 1 east / west
                WriteProcessMemory(phandle, (LPVOID*)(myEastWestPtrAddr), &newCoordsEastWest, sizeof(newCoordsEastWest), 0);
                float newCoordsNorthSouth = northsouth1; //write to enemy 1 east / west
                WriteProcessMemory(phandle, (LPVOID*)(myNorthSouthPtrAddr), &newCoordsNorthSouth, sizeof(newCoordsNorthSouth), 0);
                
            }

            if (GetAsyncKeyState(VK_F2)) // if u press f2, write ur coords to enemy 2 in entity list coords
            {

                float newCoordsHight = hight2; //write to enemy 1 hight
                WriteProcessMemory(phandle, (LPVOID*)(hightPtrAddr), &newCoordsHight, sizeof(newCoordsHight), 0);
                float newCoordsEastWest = eastwest2; //write to enemy 1 east / west
                WriteProcessMemory(phandle, (LPVOID*)(myEastWestPtrAddr), &newCoordsEastWest, sizeof(newCoordsEastWest), 0);
                float newCoordsNorthSouth = northsouth2; //write to enemy 1 east / west
                WriteProcessMemory(phandle, (LPVOID*)(myNorthSouthPtrAddr), &newCoordsNorthSouth, sizeof(newCoordsNorthSouth), 0);

            }
            if (GetAsyncKeyState(VK_F3)) // if u press f3, write ur coords to enemy 3 in entity list coords
            {

                float newCoordsHight = hight3; //write to enemy 1 hight
                WriteProcessMemory(phandle, (LPVOID*)(hightPtrAddr), &newCoordsHight, sizeof(newCoordsHight), 0);
                float newCoordsEastWest = eastwest3; //write to enemy 1 east / west
                WriteProcessMemory(phandle, (LPVOID*)(myEastWestPtrAddr), &newCoordsEastWest, sizeof(newCoordsEastWest), 0);
                float newCoordsNorthSouth = northsouth3; //write to enemy 1 east / west
                WriteProcessMemory(phandle, (LPVOID*)(myNorthSouthPtrAddr), &newCoordsNorthSouth, sizeof(newCoordsNorthSouth), 0);

            }

            // ## ----- Players teleport to you

            if (GetAsyncKeyState(VK_F4))
            {
                float newCoordsHight = myH;
                WriteProcessMemory(phandle, (LPVOID*)(entity1hight), &newCoordsHight, sizeof(newCoordsHight), 0);
                float newCoordsEastWest = myEW;
                WriteProcessMemory(phandle, (LPVOID*)(entity1eastwest), &newCoordsEastWest, sizeof(newCoordsEastWest), 0);
                float newCoordsNorthSouth = myNS;
                WriteProcessMemory(phandle, (LPVOID*)(entity1northsouth), &newCoordsNorthSouth, sizeof(newCoordsNorthSouth), 0);
            }
            if (GetAsyncKeyState(VK_F5))
            {
                float newCoordsHight = myH;
                WriteProcessMemory(phandle, (LPVOID*)(entity2hight), &newCoordsHight, sizeof(newCoordsHight), 0);
                float newCoordsEastWest = myEW;
                WriteProcessMemory(phandle, (LPVOID*)(entity2eastwest), &newCoordsEastWest, sizeof(newCoordsEastWest), 0);
                float newCoordsNorthSouth = myNS;
                WriteProcessMemory(phandle, (LPVOID*)(entity2northsouth), &newCoordsNorthSouth, sizeof(newCoordsNorthSouth), 0);
            }
            if (GetAsyncKeyState(VK_F6))
            {
                float newCoordsHight = myH; 
                WriteProcessMemory(phandle, (LPVOID*)(entity3hight), &newCoordsHight, sizeof(newCoordsHight), 0);
                float newCoordsEastWest = myEW;
                WriteProcessMemory(phandle, (LPVOID*)(entity3eastwest), &newCoordsEastWest, sizeof(newCoordsEastWest), 0);
                float newCoordsNorthSouth = myNS;
                WriteProcessMemory(phandle, (LPVOID*)(entity3northsouth), &newCoordsNorthSouth, sizeof(newCoordsNorthSouth), 0);
            }

            // magic walk (walk through anything)

            if (GetAsyncKeyState(VK_NUMPAD5)) //backwords is numpad 5
            {
                float MyCurrentNorthSouth;
                ReadProcessMemory(phandle, (LPVOID)myNorthSouthPtrAddr, &MyCurrentNorthSouth, sizeof(MyCurrentNorthSouth), 0);

                float newNorthSouth = MyCurrentNorthSouth + 3.0;
                WriteProcessMemory(phandle, (LPVOID*)(myNorthSouthPtrAddr), &newNorthSouth, sizeof(newNorthSouth), 0);

            }
            if (GetAsyncKeyState(VK_NUMPAD8))//forward is numpad 8
            {
                float MyCurrentNorthSouth;
                ReadProcessMemory(phandle, (LPVOID)myNorthSouthPtrAddr, &MyCurrentNorthSouth, sizeof(MyCurrentNorthSouth), 0);

                float newNorthSouth = MyCurrentNorthSouth - 3.0;
                WriteProcessMemory(phandle, (LPVOID*)(myNorthSouthPtrAddr), &newNorthSouth, sizeof(newNorthSouth), 0);

            }
            if (GetAsyncKeyState(VK_NUMPAD4))//side left is numpad 4
            {
                float MyCurrentEastWest;
                ReadProcessMemory(phandle, (LPVOID)myEastWestAddr, &MyCurrentEastWest, sizeof(MyCurrentEastWest), 0);

                float newEastWest = MyCurrentEastWest - 3.0;
                WriteProcessMemory(phandle, (LPVOID*)(myEastWestAddr), &newEastWest, sizeof(newEastWest), 0);

            }


            if (GetAsyncKeyState(VK_SPACE))
            {
             

                float jumpHight = 8.0;
                WriteProcessMemory(phandle, (LPVOID*)(hightPtrAddr), &jumpHight, sizeof(jumpHight), 0);

            }
           // Sleep(1000);

        }

    }
}