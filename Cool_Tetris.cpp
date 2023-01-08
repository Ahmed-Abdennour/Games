#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>

#ifndef UNICODE
#define UNICODE
#endif

#ifdef UNICODE_WAS_UNDEFINED
#undef UNICODE_WAS_UNDEFINED
#endif

#include <Windows.h>
#include <stdio.h>

using namespace std;

wstring tetro[7];
int FieldWidth = 14;         
int FieldHeight = 20;        
unsigned char *pField = nullptr;

int ScreenWidth = 120;
int ScreenHeight = 30;

// GAME MODULES -----------------------------------

int Rotate(int tx, int ty, int ang);
bool DoesPieceFit(int curr_tetro, int rotation, int posX, int posY);

//-----------------------------------------------------------------------------------
 
int main()
{
    tetro[0].append(L"..X.");
    tetro[0].append(L"..X.");
    tetro[0].append(L"..X.");
    tetro[0].append(L"..X.");

    tetro[1].append(L"..X.");
    tetro[1].append(L".XX.");
    tetro[1].append(L".X..");
    tetro[1].append(L"....");

    tetro[2].append(L".X..");
    tetro[2].append(L".XX.");
    tetro[2].append(L"..X.");
    tetro[2].append(L"....");

    tetro[3].append(L"....");
    tetro[3].append(L".XX.");
    tetro[3].append(L".XX.");
    tetro[3].append(L"....");

    tetro[4].append(L"..X.");
    tetro[4].append(L".XX.");
    tetro[4].append(L"..X.");
    tetro[4].append(L"....");

    tetro[5].append(L"....");
    tetro[5].append(L".XX.");
    tetro[5].append(L"..X.");
    tetro[5].append(L"..X.");

    tetro[6].append(L"....");
    tetro[6].append(L".XX.");
    tetro[6].append(L".X..");
    tetro[6].append(L".X..");


    pField = new unsigned char[FieldWidth * FieldHeight];
    for (int x = 0; x < FieldWidth; x++)
        for (int y = 0; y < FieldHeight; y++)
            pField[y * FieldWidth + x] = (x == 0 || x == FieldWidth - 1 || y == FieldHeight - 1) ? 9 : 0;
    
    wchar_t *screen = new wchar_t[ScreenWidth * ScreenHeight];
    for (int i = 0; i < ScreenWidth * ScreenHeight; i++)
        screen[i] = L' ';
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0; 


    // Game Logic environnement
    bool GameOver = false;

    int currentPiece = 0;
    int currentRotation = 0;
    int currentX = FieldWidth / 2;
    int currentY = 0; 

    bool bKey[4];
    bool bRotateHold = false;

    int speed = 20;
    int speedCounter = 0;
    bool forceDown = false;
    int pieceCount = 0;
    int Score = 0;

    vector<int> vLines;

    while(!GameOver)
    {
        // GAME TIME ================================ ' FOR CONSISTENCY '
            this_thread::sleep_for(50ms);  // The ticking of the game
            speedCounter++;
            forceDown = (speed == speedCounter);

        // USER'S INPUT ===================================== 
            for (int k =0; k < 4; k++)         // hexadecimal for :   R    L  D  'Z to rotate'
                bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28R"[k]))) != 0;   


        // GAME LOGIC =========================================
            currentX += bKey[0] && DoesPieceFit(currentPiece, currentRotation, currentX + 1, currentY) ? 1 : 0;
            currentX -= bKey[1] && DoesPieceFit(currentPiece, currentRotation, currentX - 1, currentY) ? 1 : 0;
            currentY += bKey[2] && DoesPieceFit(currentPiece, currentRotation, currentX, currentY + 1) ? 1 : 0;
            if (bKey[3])
            {
                currentRotation += !bRotateHold && DoesPieceFit(currentPiece, currentRotation + 1, currentX, currentY) ? 1 : 0;
                bRotateHold = true;
            }
            else   
                bRotateHold = false;

            if(forceDown)
            {
                if(DoesPieceFit(currentPiece, currentRotation, currentX, currentY + 1))
                    currentY++;

                else   // the tetris cant go down more, which means the current tetris must get locked
                {
                    // 1 - Locking the current piece
                    for (int px = 0; px < 4; px++)
                        for(int py = 0; py < 4; py++)
                            if(tetro[currentPiece][Rotate(px, py, currentRotation)] == L'X')   // We can safely draw the tetro
                                pField[(currentY + py) * FieldWidth + (currentX + px)] = currentPiece + 1;

                    pieceCount++;
                    speed += speed >= 10 && pieceCount % 10 == 0 ? 1 : 0;

                    // 2 - Have we got any lines ?
                    for (int py = 0; py < 4; py++)
                        if (currentY + py < FieldHeight - 1)
                        {
                            bool Line = true;
                            for (int px = 1; px < FieldWidth - 1; px++)
                                Line &= (pField[(currentY + py) * FieldWidth + px]) != 0;

                            if (Line) // If we have a line, we must set it to '=' then remove it 
                            {
                                for (int px = 1; px < FieldWidth - 1; px++)
                                   pField[(currentY + py) * FieldWidth + px] = 8;
                                vLines.push_back(currentY + py); 
                            }
                        }
                    Score += 25;
                    Score += !vLines.empty() ? (1 << vLines.size()) * 100 : 0;

                    // 3 - Getting the next piece
                    currentX = FieldWidth / 2;
                    currentY = 0;
                    currentPiece = rand() % 7; // taking the module of 7 to make sure we're in the tetro range [1..6]
                    currentRotation = 0;
                     

                    // 4 - Does the next piece fit ? or is it game over ?
                    GameOver = !DoesPieceFit(currentPiece, currentRotation, currentX, currentY);

                    }  
                speedCounter = 0; 
            }
    
        // RENDERING THE OUTPUT ====================================




        // Drawing the Field
        for (int x = 0; x < FieldWidth; x++)
            for (int y = 0; y < FieldHeight; y++)
                screen[(y + 2) * ScreenWidth + (x + 2)] = L" ØØØØØØØ=#"[pField[y * FieldWidth + x]];


        // Drawing the current piece !!
        for (int px = 0; px < 4; px++)
            for(int py = 0; py < 4; py++)
                if(tetro[currentPiece][Rotate(px, py, currentRotation)] == L'X')   // We can safely draw the tetro
                    screen[(currentY + py + 2) * ScreenWidth + (currentX + px + 2)] = 216 ;

        // Drawing the current score
        swprintf_s(&screen[ScreenWidth + FieldWidth + 10], 20, L"SCORE : %8d", Score);     // swprintf_s means print a s tring while on console

        if (!vLines.empty())
        {
            WriteConsoleOutputCharacter(hConsole, screen, ScreenWidth * ScreenHeight, { 0,0 }, &dwBytesWritten);
            this_thread::sleep_for(350ms);

            for (auto &v : vLines)
                for (int px = 1; px < FieldWidth - 1; px++)
                {
                    for (int py = v; py > 0; py--)
                        pField[(py * FieldWidth) + px] = pField[(py - 1) * FieldWidth + px];
                    pField[px] = 0;
                }
                vLines.clear();
        }

        // Displaying the current Frame
         WriteConsoleOutputCharacter(hConsole, screen, ScreenWidth * ScreenHeight, { 0,0 }, &dwBytesWritten);
    }

       CloseHandle(hConsole); // We must close the console otherwise we wont be able to use the screen of the command prompt
       cout << endl << endl << endl << "    GAME OVER!! SCORE : " << Score << endl << endl << endl << endl;
       system("pause");

return 0;
}

int Rotate(int tx, int ty, int ang)
{
    switch(ang % 4)
    {
        case 0 : return ty * 4 + tx;            // angle is 0 degree    ' no rotation '
        case 1 : return 12 + ty - (tx * 4);     // angle is 90 degrees  ' rotation to the right '
        case 2 : return 15 - (ty * 4) - tx;     // angle is 180 degrees ' reflexion '
        case 3 : return 3 - ty + (tx * 4);      // angle is 270 degrees ' rotation to the left  '
    }

    return 0;
}
bool DoesPieceFit(int curr_tetro, int rotation, int posX, int posY)
{
    for (int px = 0; px < 4; px++)
        for (int py = 0; py < 4; py++)
        {
            // This index is to select our current tetro into its place
            int pi = Rotate(px, py, rotation);

            // This index is to select our currentt tetro into the field
            int fi = (posY + py) * FieldWidth + (posX + px);

            if (posX + px >= 0 && posX + px < FieldWidth)
            {
                if (posY + py >= 0 && posY + py < FieldHeight)
                {
                    if (tetro[curr_tetro][pi] == L'X' && pField[fi] != 0) // pField equals 0 means that place in the field is not occupied
                        return false;    // the first hit means directly a fail  
                }
            }
        }

    return true;
}
