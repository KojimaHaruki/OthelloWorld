// program for Othello World ©2022 mini
#include "DxLib.h"
#include "Othello.h"
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    Othello sys; int SceneNo = ConfigID::Home;
    while (!ScreenFlip() && !ProcessMessage() && !ClearDrawScreen()) {
        if (SceneNo == SceneID::Home && (sys.ClickBox(30, 0, 55, 25) == 1 || sys.HitRoleKey(CtrlID::Back) == 1)) return 0;
        if ((sys.ClickBox(615, 0, 640, 25) == 1) || (SceneNo >= 0 && sys.HitRoleKey(CtrlID::Exit) == 1)) return 0;
        SceneNo = sys.DrawScene();
        sys.BackSystem();
    } return 0;
}

