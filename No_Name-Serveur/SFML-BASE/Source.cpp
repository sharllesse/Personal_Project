#include "Game.h"

#include <Windows.h>

#ifndef _DEBUG
int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd )
{
#else
int main(int argc, char* argv[])
{
#endif // !_DEBUG
	Game main_game(960, 540, "Server Console", false);
	main_game.runGame();

    return EXIT_SUCCESS;
}