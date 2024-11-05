// ConsoleTetris.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <Windows.h>
#include <thread>
#include <stdio.h>
#include <chrono>
#include <string>

using namespace std::chrono_literals;

std::wstring tetromino[7];
int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char* pField = nullptr;

int nScreenWidth = 36;
int nScreenHeight = 30;

int Rotate(int px, int py, int r)
{
	int pi = 0;
	switch (r % 4)
	{
	case 0: // 0 degrees			// 0  1  2  3
		pi = py * 4 + px;			// 4  5  6  7
		break;						// 8  9 10 11
									//12 13 14 15

	case 1: // 90 degrees			//12  8  4  0
		pi = 12 + py - (px * 4);	//13  9  5  1
		break;						//14 10  6  2
									//15 11  7  3

	case 2: // 180 degrees			//15 14 13 12
		pi = 15 - (py * 4) - px;	//11 10  9  8
		break;						// 7  6  5  4
									// 3  2  1  0

	case 3: // 270 degrees			// 3  7 11 15
		pi = 3 - py + (px * 4);		// 2  6 10 14
		break;						// 1  5  9 13
	}								// 0  4  8 12

	return pi;
}

bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{
	for (int px = 0; px < 4; px++)
		for (int py = 0; py < 4; py++)
		{
			// Get index into piece
			int pi = Rotate(px, py, nRotation);

			// Get index into field
			int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

			// Check that test is in bounds. Note out of bounds does
			// not necessarily mean a fail, as the long vertical piece
			// can have cells that lie outside the boundary, so we'll
			// just ignore them
			if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
			{
				if (nPosY + py >= 0 && nPosY + py < nFieldHeight)
				{
					// In Bounds so do collision check
					if (tetromino[nTetromino][pi] != L'.' && pField[fi] != 0)
						return false; // fail on first hit
				}
			}
		}
	return true;
}

int DisplayColor(HANDLE hConsole, wchar_t *screen)
{
	DWORD dwAttrsWritten = 0;
	WORD wAttr;

	for (int x = 0; x < nScreenWidth; x++)
		for (int y = 0; y < nScreenHeight; y++)
			// Check if in play field
			if ((x < nFieldWidth && y < nFieldHeight) || (x > 15 && x < 25 && y > 4 && y < 9))
			{
				for (int j = 0; j < 17; j++)
					/*if (screen[(y + 2) * nScreenWidth + (x + 2)] == LR"( #F.....-GEADBC=)"[j])*/	//Bl B G Aq R P Y W Gr LB LG LA LR LP LY BrW
					if (screen[(y + 2) * nScreenWidth + (x + 2)] == LR"( #F.....-ABCDEG=)"[j])
					{
						wAttr = (16 * (j)) + 1 * j;	// Calculate hex value corresponding to j value
						WriteConsoleOutputAttribute(hConsole, &wAttr, 1, { (short)(x + 2), (short)(y + 2) }, &dwAttrsWritten);
					}
			}		
	return dwAttrsWritten;
}

int main()
{	

#pragma region create console buffer and settings

	SMALL_RECT rect = { 0, 0, nScreenWidth - 1, nScreenHeight - 1 }; 
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO ci(false);

	SetConsoleCursorInfo(h, &ci);
	SetConsoleWindowInfo(h, TRUE, &rect); 

	SetConsoleTitle(TEXT("Console TETRIS")); 
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	SetConsoleCursorInfo(hConsole, &ci);

	// Font size
	static CONSOLE_FONT_INFOEX fontex(sizeof(CONSOLE_FONT_INFOEX));
	GetCurrentConsoleFontEx(hConsole, 0, &fontex);
	fontex.FontWeight = 700; 
	fontex.dwFontSize.X = 36;
	fontex.dwFontSize.Y = 24;
	SetCurrentConsoleFontEx(hConsole, NULL, &fontex);
	SetCurrentConsoleFontEx(h, NULL, &fontex);

	// No resize
	HWND consoleWindow = GetConsoleWindow(); 
	SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX); 

	DWORD dwBytesWritten = 0;
	DWORD dwAttrsWritten = 0;

	FillConsoleOutputAttribute(hConsole, 0x1F, nScreenWidth * nScreenHeight, { 0, 0 }, &dwAttrsWritten);

# pragma endregion

#pragma region initialize game variables

	tetromino[0].append(L"..X...X...X...X."); // Tetronimos 4x4 
	tetromino[1].append(L"..X..XX...X....."); // ..x.
	tetromino[2].append(L".....XX..XX....."); // .xx.
	tetromino[3].append(L"..X..XX..X......"); // .x..
	tetromino[4].append(L".X...XX...X.....");
	tetromino[5].append(L".X...X...XX.....");
	tetromino[6].append(L"..X...X..XX.....");

	pField = new unsigned char[nFieldWidth * nFieldHeight]; // Create play field buffer
	for (int x = 0; x < nFieldWidth; x++) // Board Boundary
		for (int y = 0; y < nFieldHeight; y++)
			pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;  

	wchar_t* Screen = new wchar_t[nScreenHeight * nScreenWidth];
	for (int i = 0; i < nScreenHeight * nScreenWidth; i++) Screen[i] = L' ';

	// Next box
	swprintf_s(&Screen[6 * nScreenWidth + nFieldWidth + 6], 5, L"NEXT");

	// Control text
	swprintf_s(&Screen[13 * nScreenWidth + nFieldWidth + 6], 15, L"USE ARROW KEYS");

	bool bGameOver = 0;

	srand(time(0));

	int nCurrentPiece = rand() % 7;
	int nCurrentRotation = 0;
	int nCurrentX = nFieldWidth / 2 - 2;
	int nCurrentY = 0;

	int nNextPiece = rand() % 7;

	bool bKey[5];
	bool bRotateHold = false;

	int nSpeed = 20;
	int nSpeedCounter = 0;
	bool bForceDown = false;
	int nPieceCount = 0;
	int nScore = 0;

	std::vector<int> vLines;

#pragma endregion

	while (!bGameOver)
	{
		// GAME TIMING
		std::this_thread::sleep_for(50ms); // Game tick
		nSpeedCounter++;
		bForceDown = (nSpeedCounter == nSpeed);
		
		// INPUT
		for (int k = 0; k < 5; k++)
		{														// R   L   D  U   ESC
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28\x26\x1B"[k]))) != 0;
		}

		if (bKey[4]) { break; }

		// GAME LOGIC
		nCurrentX += (bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
		nCurrentX += (bKey[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? -1 : 0;

		if (bKey[3])
		{
			nCurrentRotation += (!bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
			bRotateHold = true;
		}
		else { bRotateHold = false; }

		if (bForceDown || bKey[2])
		{
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
				nCurrentY++;
			else
			{
				// Lock piece
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;

				// Count piece and speed the game up
				nPieceCount++;
				if (nPieceCount % 10 == 0)
					if (nSpeed >= 10) { nSpeed--; }

				// Check have we got full lines
				for (int py = 0; py < 4; py++)
					if (nCurrentY + py < nFieldHeight - 1)
					{
						bool bLine = true;
						for (int px = 1; px < nFieldWidth - 1; px++)
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;

						if (bLine)
						{
							for (int px = 1; px < nFieldWidth - 1; px++)
								pField[(nCurrentY + py) * nFieldWidth + px] = 8;

							vLines.push_back((nCurrentY + py));
						}
					}

				// Scoring
				nScore += 25;
				if (!vLines.empty()) { nScore += (1 << vLines.size()) * 100; }	// << shift bits to the right basically raising it 2^n times (n = number of bits shifted)
														
				// Choose next piece
				nCurrentX = nFieldWidth / 2 - 2;
				nCurrentY = 0;
				nCurrentRotation = 0;
				nCurrentPiece = nNextPiece;
				nNextPiece = rand() % 7;

				// If piece does not fit
				bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentPiece, nCurrentX, nCurrentY);
			}
			nSpeedCounter = 0;
		}


		// RENDER OUTPUT

		// Draw field
		for (int x = 0; x < nFieldWidth; x++)
			for (int y = 0; y < nFieldHeight; y++)
				Screen[(y + 2) * nScreenWidth + (x + 2)] = LR"( ABCDEFG=#)"[pField[y * nFieldWidth + x]];

		// Draw next box
		for (int x = nFieldWidth + 6; x < nFieldWidth + 15; x++)
			for (int y = 7; y < 11; y++)
				Screen[(y) * nScreenWidth + (x)] = L' ';

		// Draw current tetromino
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
					Screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;

		// Draw next tetromino
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetromino[nNextPiece][Rotate(px, py, 1)] != L'.')
					Screen[(7 + py) * nScreenWidth + (20 + px)] = nNextPiece + 65;
					
		// Display score
		swprintf_s(&Screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);

		if (!vLines.empty())
		{
			// Display frame (to draw the lines)
			DisplayColor(hConsole, Screen);
			std::this_thread::sleep_for(400ms);

			for (auto &v : vLines)
				for (int px = 1; px < nFieldWidth - 1; px++)
				{
					for (int py = v; py > 0; py--)
					{
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
					}
					pField[px] = 0;
				}
			vLines.clear();
		}

		// Display 
		WriteConsoleOutputCharacterW(hConsole, Screen, nScreenHeight * nScreenWidth, { 0, 0 }, &dwBytesWritten);

		// Color 
		DisplayColor(hConsole, Screen);
	}

	// GAME OVER
	//std::wstring gameovertext = std::format(L"GAMEOVER SCORE: {}", nScore);
	std::wstring gameovertext = L"GAMEOVER";
	std::wstring gameovertextsecond = L"PRESS ENTER TO QUIT";

	WriteConsoleOutputCharacterW(hConsole, gameovertext.c_str(), gameovertext.length(), { (short)((nScreenWidth / 2) - (gameovertext.length() / 2)), 20 }, & dwBytesWritten);
	WriteConsoleOutputCharacterW(hConsole, gameovertextsecond.c_str(), gameovertextsecond.length(), { (short)((nScreenWidth / 2) - (gameovertextsecond.length() / 2)), 21 }, & dwBytesWritten);

	while (!GetAsyncKeyState(VK_RETURN)) {}

	CloseHandle(hConsole);

	return 0;
}
