#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "winmm.lib")   // MSVC —p
#include "DxLib.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Othello.h"
#include "resource.h"
#include <string>
using namespace std;
// Initialize various Value
Othello::Othello() : ConnectMode(Disconnect), UserStatus(Offline), Scene(ConfigID::Home), PlayingBGM(BGMID::Chocolate),
Piece(), PieceRecord(), JudgePiece(), PieceNum(), Propotion(), CursorX(0), CursorY(0), xyr(0), xyd(0), xys(0),
Board(BoardID::Default), WindowMode(FALSE), PauseMode(0), MuteMode(1), Player(0), Turn(1), MaxTurn(1),
AITimeCount(0), StartTime(0), LapseTime(0), LapseHour(0), LapseMin(0), LapseSec(0), FailureSuccession(0), GameEnd(false) {
    SetWindowIconID(IDI_ICON1); SetWindowText("Othello World"); SetLogDrawOutFlag(FALSE); if (DxLib_Init()) exit(-1);
    // initialize random number by the lapse time in Windows
    SRand(GetNowCount());
    for (int i = 0; i < 2; i++) { csd.CharLv[i] = 0; csd.Lv[i] = 0; csd.CharNo[i] = i - CharID::Num; }
    csd.CharSelectNo = 0; csd.MaxCharSelectNo = 0; csd.Order = OrderID::Random;
    // Load Music
    for (int i = 0; i < BGMID::Num; i++) BGM[i] = LoadSoundMemByResource(MAKEINTRESOURCE(IDR_WAVE1 + i), "WAVE");
    for (int i = 0; i < SENum; i++) SE[i] = LoadSoundMemByResource(MAKEINTRESOURCE(IDR_WAVE9 + i), "WAVE");
    // Load Color
    TitleColor = GetColor(0, 98, 160); FrameColor = GetColor(112, 75, 56);
    PieceColor[PieceID::Black] = GetColor(0, 0, 0); PieceColor[PieceID::White] = GetColor(255, 255, 255);
    PieceColor[PieceID::Space] = GetColor(0, 78, 45); GuideColor[0] = GetColor(255, 164, 0);
    GuideColor[1] = GetColor(187, 0, 75); GuideColor[2] = GetColor(0, 78, 135);
    FILE* icdf = fopen(ConfigDataFileName, "rb"); 
    if (icdf == NULL) InitScene(); else { fread(&cd, sizeof(cd), 1, icdf); fclose(icdf); }
    cd.Key[CtrlID::Start] = KEY_INPUT_SPACE; cd.Key[CtrlID::Yes] = KEY_INPUT_RETURN; cd.Key[CtrlID::No] = KEY_INPUT_BACK;
    ChangeSoundVol(0, cd.SoundVol[0]); ChangeWindow(WindowMode); UpdateScene(SceneID::Home);
}
void Othello::ChangeWindow(int WindowModeFlag) {
    ChangeWindowMode(WindowModeFlag); SetDrawScreen(DX_SCREEN_BACK);
    // Load Image
    for (int i = 0; i < KeyNum; i++) KeyImage[KeyNo[i]] = LoadGraphToResource(MAKEINTRESOURCE(KeyNo[i]), "PNG");
    for (int i = 0; i < CharID::Num; i++) CharImage[i] = LoadGraphToResource(MAKEINTRESOURCE(IDB_PNG68 + i), "PNG");
    for (int i = 0; i < IconID::Num - WindowModeFlag; i++) Icon[i] = LoadGraphToResource(MAKEINTRESOURCE(IDB_PNG73 + i), "PNG");
    // Load Font
    TitleFont = CreateFontToHandle("Arial Rounded MT Bold", 60, 5, DX_FONTTYPE_ANTIALIASING_EDGE);
    NormalFont = CreateFontToHandle("Calisto MT Bold", 18, 5, DX_FONTTYPE_NORMAL);
    CharNameFont = CreateFontToHandle("Arial Rounded MT Bold", 30, 2, DX_FONTTYPE_ANTIALIASING_EDGE);
    WindowMode = GetWindowModeFlag(); return;
}
void Othello::Config() {
    for (int i = 0; i < 2; i++) {
        DrawBox(415 * i, 25, 415 + 225 * i, 455, GuideColor[i], TRUE); 
        ChangeKey(354 + 31 * i, 53, 385 + 31 * i, 83, CtrlID::Down + i);
        DrawGraph(357 + 31 * i, 55, KeyImage[cd.Key[CtrlID::Down + i]], TRUE); 
        DrawGraph(360 + 25 * i, 175, Icon[CtrlID::Down + i], TRUE);
    }
    DrawStringToHandle(5, 30, "Music", PieceColor[PieceID::Black], TitleFont);
    for (int i = 0; i < SoundID::Num; i++) {
        DrawStringToHandle(0, 90 + 30 * i, SoundName[i].c_str(), PieceColor[PieceID::Black], NormalFont);
        DrawBox(100, 95 + 30 * i, 100 + cd.SoundVol[i], 105 + 30 * i, PieceColor[PieceID::White], TRUE);
        DrawBox(100 + cd.SoundVol[i], 95 + 30 * i, 300, 105 + 30 * i, PieceColor[PieceID::Black], TRUE);
        DrawBox(95 + cd.SoundVol[i], 90 + 30 * i, 105 + cd.SoundVol[i], 110 + 30 * i, GuideColor[1], TRUE);
        DrawFormatStringToHandle(315, 90 + 30 * i, PieceColor[PieceID::Black], NormalFont, "%3d", cd.SoundVol[i]);
        DrawStringToHandle(5, 240 + 30 * i, SceneName[i].c_str(), PieceColor[PieceID::Black], NormalFont);
        DrawStringToHandle(5, 360 + 30 * i, BoardName[i].c_str(), PieceColor[PieceID::Black], NormalFont);
        DrawStringToHandle(120, 240 + 30 * i, (": " + BGMName[cd.SceneBGM[i]]).c_str(), PieceColor[PieceID::Black], NormalFont);
        DrawStringToHandle(120, 360 + 30 * i, (": " + BGMName[cd.BoardBGM[i]]).c_str(), PieceColor[PieceID::Black], NormalFont);
        for (int j = 0; j < 2; j++) {
            DrawGraph(360 + 25 * j, 85 + 30 * i, Icon[IconID::Down + j], TRUE);
            for (int k = 0; k < 2; k++) {
                DrawGraph(360 + 25 * j, 235 + 30 * i + 120 * k, Icon[IconID::Down + j], TRUE);
            }
        }
        if (ClickBox(360, 85 + 30 * i, 385, 110 + 30 * i) > 10) ChangeSoundVol(i, cd.SoundVol[i] - 1);
        else if (ClickBox(385, 85 + 30 * i, 410, 110 + 30 * i) > 10) ChangeSoundVol(i, cd.SoundVol[i] + 1);
        else if (ClickBox(360, 85 + 30 * i, 385, 110 + 30 * i) == 1) ChangeSoundVol(i, cd.SoundVol[i] - 4);
        else if (ClickBox(385, 85 + 30 * i, 410, 110 + 30 * i) == 1) ChangeSoundVol(i, cd.SoundVol[i] + 4);
        else if (ClickBox(99, 85 + 30 * i, 301, 115 + 30 * i) > 0) ChangeSoundVol(i, CursorX - 100);
        else if (ClickBox(360, 235 + 30 * i, 410, 260 + 30 * i) == 1) {
            PlaySoundMem(SE[1], DX_PLAYTYPE_BACK);
            if (CursorX < 385) { cd.SceneBGM[i] += BGMID::Num - 1; }
            else { cd.SceneBGM[i]++; } cd.SceneBGM[i] %= BGMID::Num;
        }
        else if (ClickBox(360, 355 + 30 * i, 410, 380 + 30 * i) == 1) {
            PlaySoundMem(SE[1], DX_PLAYTYPE_BACK);
            if (CursorX < 385) { cd.BoardBGM[i] += BGMID::Num - 1; }
            else { cd.BoardBGM[i]++; } cd.BoardBGM[i] %= BGMID::Num;
        }
    }
    DrawStringToHandle(5, 180, "Play Style", PieceColor[PieceID::Black], NormalFont);
    DrawStringToHandle(120, 180, (": " + BGMPlayModeName[cd.BGMPlayMode]).c_str(), PieceColor[PieceID::Black], NormalFont);
    DrawStringToHandle(0, 210, "Scene Theme", PieceColor[PieceID::Black], NormalFont);
    DrawStringToHandle(0, 330, "Board Theme", PieceColor[PieceID::Black], NormalFont);
    DrawStringToHandle(415, 30, "System", PieceColor[PieceID::White], TitleFont);
    for (int i = 0; i < 9; i++) {
        DrawGraph(420, 95 + 30 * i, Icon[i], TRUE);
        DrawStringToHandle(450, 100 + 30 * i, CtrlName[i].c_str(), PieceColor[PieceID::White], NormalFont);
    } DrawStringToHandle(530, 100, GuideLvName[cd.GuideLv].c_str(), PieceColor[PieceID::White], NormalFont);
    DrawStringToHandle(530, 130, ReverseModeName[cd.ReverseMode].c_str(), PieceColor[PieceID::White], NormalFont);
    DrawGraph(420, 365, Icon[IconID::AnotherWindow + WindowMode], TRUE);
    DrawStringToHandle(450, 370, CtrlName[CtrlID::Window].c_str(), PieceColor[PieceID::White], NormalFont);
    DrawGraph(420, 395, Icon[IconID::Mute + MuteMode], TRUE);
    DrawStringToHandle(450, 400, MuteModeName[MuteMode].c_str(), PieceColor[PieceID::White], NormalFont);
    DrawGraph(420, 425, Icon[IconID::Pause + PauseMode], TRUE);
    DrawStringToHandle(450, 430, PauseName[PauseMode].c_str(), PieceColor[PieceID::White], NormalFont);
    if (ClickBox(30, 0, 55, 25) == 1 || HitRoleKey(CtrlID::Back) == 1) { UpdateScene(Scene + SceneID::Num); return; }
    if (ClickBox(119, 140, 377, 160) == 1) { 
        BGMVol = CursorX - 120; ChangeVolumeSoundMem(BGMVol, BGM[PlayingBGM]); return; 
    }
    if (ClickBox(360, 175, 410, 200) == 1) {
        PlaySoundMem(SE[1], DX_PLAYTYPE_BACK);
        if (CursorX < 385) { cd.BGMPlayMode++; }
        else { cd.BGMPlayMode += BGMPlayModeID::Num - 1; } cd.BGMPlayMode %= BGMPlayModeID::Num;
        if (!cd.BGMPlayMode) PlaySoundMem(BGM[PlayingBGM], DX_PLAYTYPE_BACK | DX_PLAYTYPE_LOOP);
        else PlaySoundMem(BGM[PlayingBGM], DX_PLAYTYPE_BACK);
    }
    for (int i = 0; i < 12; i++) {
        ChangeKey(607, 93 + 30 * i, 638, 123 + 30 * i, i); DrawGraph(610, 95 + 30 * i, KeyImage[cd.Key[i]], TRUE);
    }
    if (HitKey(KEY_INPUT_F5) == 1) { ConnectMode = ConnectRQ; }
    else if (HitKey(KEY_INPUT_INSERT) == 1) { ConnectMode = WaitConnect; PreparationListenNetWork(9850); }
    else if (HitKey(KEY_INPUT_BACK) == 1) { ConnectMode = Disconnect; UserStatus = Offline; CloseNetWork(NetHandle); }
    DrawFormatStringToHandle(240, 400, PieceColor[PieceID::White], NormalFont, "ConnectMode:%d", ConnectMode);
    switch (ConnectMode) {
    case ConnectRQ: // Send connection request to the other PC
        NetHandle = ConnectNetWork(ip, 9850);
        DrawStringToHandle(240, 200, "Now connecting...", PieceColor[PieceID::White], NormalFont);
        if (NetHandle != -1) {
            NetWorkSend(NetHandle, "Request to connect...", 22);
            DataLength = GetNetWorkDataLength(NetHandle);
            if (DataLength != -1) {
                NetWorkRecv(NetHandle, Signal, DataLength);
                DrawStringToHandle(240, 200, Signal, PieceColor[PieceID::White], NormalFont); WaitTimer(1000);
                ConnectMode = Succeed; UserStatus = Host;
            }
        } break;
    case WaitConnect: // Wait connection request from the other PC
        DrawStringToHandle(240, 200, "Now connecting...", PieceColor[PieceID::White], NormalFont);
        NetHandle = GetNewAcceptNetWork();
        if (NetHandle != -1) {
            StopListenNetWork(); GetNetWorkIP(NetHandle, &ip);
            DataLength = GetNetWorkDataLength(NetHandle);
            if (DataLength != -1) {
                NetWorkRecv(NetHandle, Signal, DataLength);
                DrawStringToHandle(240, 200, Signal, PieceColor[PieceID::White], NormalFont); WaitTimer(1000);
                NetWorkSend(NetHandle, "Connection succeeded!!", 23);
                ConnectMode = Succeed; UserStatus = Guest;
            }
        } break;
    default:
        break;
    }
    return;
}
int Othello::HitKey(int KeyCode) { return Keyboard::GetInstance()->GetPressingCount(KeyCode); }
int Othello::HitRoleKey(int KeyRole) {
    switch (cd.TwinKey[KeyRole]) {
    case SINGLE: return HitKey(cd.Key[KeyRole]);
    case SHIFT:  return HitKey(KEY_INPUT_LSHIFT) + HitKey(KEY_INPUT_RSHIFT);
    case CTRL:   return HitKey(KEY_INPUT_LCONTROL) + HitKey(KEY_INPUT_RCONTROL);
    case ALT:    return HitKey(KEY_INPUT_LALT) + HitKey(KEY_INPUT_RALT);
    default: exit(-1);
    }
}
void Othello::ChangeKey(int x1, int y1, int x2, int y2, int KeyRole) {
    if (CursorX > x1 && CursorY > y1 && CursorX < x2 && CursorY < y2) {
        DrawBox(x1, y1, x2, y2, PieceColor[PieceID::White], TRUE);
        for (int i = 0; i < KeyNum; i++) {
            if (HitKey(KeyNo[i]) > 0) {
                for (int j = 0; j < CtrlID::Num; j++) { 
                    if (KeyNo[i] == cd.Key[j]) { DrawBox(x1, y1, x2, y2, GuideColor[2], TRUE); return; } 
                }
                cd.Key[KeyRole] = KeyNo[i]; PlaySoundMem(SE[7], DX_PLAYTYPE_BACK);
                if (cd.Key[KeyRole] == KEY_INPUT_LSHIFT || cd.Key[KeyRole] == KEY_INPUT_RSHIFT) { cd.TwinKey[KeyRole] = SHIFT; }
                else if (cd.Key[KeyRole] == KEY_INPUT_LCONTROL || cd.Key[KeyRole] == KEY_INPUT_RCONTROL) { cd.TwinKey[KeyRole] = CTRL; }
                else if (cd.Key[KeyRole] == KEY_INPUT_LALT || cd.Key[KeyRole] == KEY_INPUT_RALT) { cd.TwinKey[KeyRole] = ALT; }
                else { cd.TwinKey[KeyRole] = 0; } return;
            }
        }
    } return;
}
void Othello::ChangeSound(int SoundModeFlag) {
    if (!SoundModeFlag) StopSoundMem(BGM[PlayingBGM]);
    else {
        ChangeVolumeSoundMem(BGMVol, BGM[PlayingBGM]);
        if (!cd.BGMPlayMode) { PlaySoundMem(BGM[PlayingBGM], DX_PLAYTYPE_BACK | DX_PLAYTYPE_LOOP, 0); }
        else { PlaySoundMem(BGM[PlayingBGM], DX_PLAYTYPE_BACK, 0); }
    } MuteMode = CheckSoundMem(BGM[PlayingBGM]); return;
}
void Othello::ChangeSoundVol(int SoundNo, int Vol) {
    if (Vol > MaxSoundVol) { if (cd.SoundVol[SoundNo] == MaxSoundVol) { return; } cd.SoundVol[SoundNo] = MaxSoundVol; }
    else if (Vol < MinSoundVol) { if (cd.SoundVol[SoundNo] == MinSoundVol) { return; } cd.SoundVol[SoundNo] = MinSoundVol; }
    else cd.SoundVol[SoundNo] = Vol;
    switch (SoundNo) {
    case SoundID::Total: case SoundID::BGM:
        BGMVol = (int)(0.0064 * cd.SoundVol[SoundID::Total] * cd.SoundVol[SoundID::BGM]); 
        ChangeVolumeSoundMem(BGMVol, BGM[PlayingBGM]);
        if (SoundNo == SoundID::BGM) return;
    case SoundID::SE: SEVol = (int)(0.0064 * cd.SoundVol[SoundID::Total] * cd.SoundVol[SoundID::SE]);
        for (int i = 0; i < SENum; i++) { ChangeVolumeSoundMem(SEVol, SE[i]); } return;
    default: exit(-1);
    }
}
void Othello::ChangeBGM(int TargetBGM) {
    StopSoundMem(BGM[PlayingBGM]); PlayingBGM = TargetBGM; ChangeVolumeSoundMem(BGMVol, BGM[PlayingBGM]);
    if (MuteMode) {
        if (!cd.BGMPlayMode) { PlaySoundMem(BGM[PlayingBGM], DX_PLAYTYPE_BACK | DX_PLAYTYPE_LOOP); }
        else { PlaySoundMem(BGM[PlayingBGM], DX_PLAYTYPE_BACK); }
    } return;
}
int Othello::ClickBox(int x1, int y1, int x2, int y2) {
    if (CursorX > x1 && CursorY > y1 && CursorX < x2 && CursorY < y2) 
        return Mouse::GetInstance()->GetPressingCount(Mouse::LEFT_CLICK);
    return -1;
}
void Othello::Title() {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) { DrawBox(320 * i, 25 + 215 * j, 320 + 320 * i, 240 + 215 * j, PieceColor[(i + j + 1) % 2], TRUE); }
    }
    for (int i = 0; i < 4; i++) { DrawGraph(160 * i - 50, 30, CharImage[i], TRUE); }
    DrawStringToHandle(90, 210, "Othello World", TitleColor, TitleFont); 
    DrawGraph(265, 387, KeyImage[cd.Key[CtrlID::Start]], TRUE);
    DrawStringToHandle(205, 390, "Press        space", PieceColor[PieceID::White], NormalFont);
    DrawStringToHandle(385, 390, "to start...", PieceColor[PieceID::Black], NormalFont);
    if (HitRoleKey(CtrlID::Start) == 1) {
        if (ConnectMode == Succeed) { NetWorkSend(NetHandle, "S", 2); } UpdateScene(SceneID::BoardSelect); return;
    }
    if (UserStatus == Guest) {
        DataLength = GetNetWorkDataLength(NetHandle);
        if (DataLength == 2) { NetWorkRecv(NetHandle, Signal, DataLength); return; }
        if (Signal[0] == 'S') { UpdateScene(SceneID::BoardSelect); return; }
    }
    return;
}
void Othello::BoardSelect() {
    if (Board >= -BoardID::Num) DrawGraph(55, 0, Icon[IconID::Forward], TRUE);
    if (HitRoleKey(CtrlID::Back) == 1 || ClickBox(30, 0, 55, 25) == 1) { UpdateScene(SceneID::Home); return; }
    if (Board >= -BoardID::Num && (HitRoleKey(CtrlID::Forward) == 1 || ClickBox(55, 0, 80, 25) == 1)) 
        Board += BoardID::Num;
    if (ClickBox(565, 430, 640, 455) == 1 || HitRoleKey(CtrlID::Skip) == 1) {
        Board += BoardID::Num; if (Board == BoardID::Skip) Board = BoardID::Othello;
    }
    if (Board >= BoardID::Othello && Board <= BoardID::Sphere) { UpdateScene(SceneID::CharSelect); return; }
    for (int i = 0; i < BoardID::Num; i++) {
        DrawBox(213 * i, 25, 213 * (i + 1), 455, GuideColor[i], TRUE);
        if (ClickBox(213 * i, 25, 213 * (i + 1), 430) == 1) { Board = i;  UpdateScene(SceneID::CharSelect); return; }
    }
    for (int i = 0; i < 2; i++) {
        DrawBox(5 + 213 * i, 45, 205 + 213 * i, 245, FrameColor, TRUE);
        DrawBox(25 + 213 * i, 65, 185 + 213 * i, 225, PieceColor[PieceID::Space], TRUE);
    } // Draw Nip Board
    for (int i = 0; i < 6; i++) { // Draw vertical & horizontal line
        if (i == 2 || i == 3) {
            DrawLine(268 + 20 * i, 75, 268 + 20 * i, 215, PieceColor[PieceID::Black]);
            DrawLine(248, 95 + 20 * i, 388, 95 + 20 * i, PieceColor[PieceID::Black]);
        }
        else {
            DrawLine(268 + 20 * i, 95, 268 + 20 * i, 195, PieceColor[PieceID::Black]);
            DrawLine(268, 95 + 20 * i, 368, 95 + 20 * i, PieceColor[PieceID::Black]);
        }
        for (int j = 0; i + j < 5 && j < 2; j++) { // Draw diagonal line 
            if (2 * i + j == 4) {
                DrawLine(228 + 20 * i, 155 + 20 * (i + j), 328 + 20 * (i + j), 55 + 20 * i, PieceColor[PieceID::Black]);
                DrawLine(228 + 20 * i, 135 - 20 * (i + j), 328 + 20 * (i + j), 235 - 20 * i, PieceColor[PieceID::Black]);
            }
            else {
                DrawLine(248 + 20 * i, 135 + 20 * (i + j), 308 + 20 * (i + j), 75 + 20 * i, PieceColor[PieceID::Black]);
                DrawLine(248 + 20 * i, 155 - 20 * (i + j), 308 + 20 * (i + j), 215 - 20 * i, PieceColor[PieceID::Black]);
            }
            for (int k = 0; i < 2 && k < 2; k++) { // Draw special diagonal line
                DrawLine(255 + 126 * k, 115 + 60 * i, 268 + 100 * k, 115 + 20 * (2 * i + j), PieceColor[PieceID::Black]);
                DrawLine(288 + 60 * i, 82 + 126 * k, 288 + 20 * (2 * i + j), 95 + 100 * k, PieceColor[PieceID::Black]);
            }
        }
    } DrawCircle(318, 145, 70, PieceColor[PieceID::Black], FALSE, 2);
    // Draw Sphere Board
    DrawCircle(531, 145, 100, PieceColor[PieceID::Space], TRUE);
    // Draw diagonal line
    DrawLine(461, 75, 601, 215, PieceColor[PieceID::Black], 2); DrawLine(461, 215, 601, 75, PieceColor[PieceID::Black], 2);
    for (int i = 0; i < 8; i++) {
        if (i > 0 && i < 7) {
            // Draw vertical & horizontal line
            DrawLine(461 + 20 * i, 65, 461 + 20 * i, 225, PieceColor[PieceID::Black], abs(i % 3 - (i + 1) % 3));
            DrawLine(451, 75 + 20 * i, 611, 75 + 20 * i, PieceColor[PieceID::Black], abs(i % 3 - (i + 1) % 3));
            // Draw diagonal line
            DrawLine(591 - 20 * i, 65, 611, 85 + 20 * i, PieceColor[PieceID::Black], abs((i + 1) % 3 - (i + 2) % 3));
            DrawLine(591 - 20 * i, 225, 611, 205 - 20 * i, PieceColor[PieceID::Black], abs((i + 1) % 3 - (i + 2) % 3));
            DrawLine(451, 85 + 20 * i, 471 + 20 * i, 65, PieceColor[PieceID::Black], abs((i + 1) % 3 - (i + 2) % 3));
            DrawLine(451, 205 - 20 * i, 471 + 20 * i, 225, PieceColor[PieceID::Black], abs((i + 1) % 3 - (i + 2) % 3));
        }
        else {
            DrawLine(461 + 20 * i, 75, 461 + 20 * i, 215, PieceColor[PieceID::Black]);
            DrawLine(461, 75 + 20 * i, 601, 75 + 20 * i, PieceColor[PieceID::Black]);
        }
        for (int j = 0; j < 3; j++) {
            DrawStringToHandle(27 + 20 * i + 213 * j, 46, Alphabet[i].c_str(), PieceColor[PieceID::White], NormalFont);
            DrawFormatStringToHandle(10 + 213 * j, 68 + 20 * i, PieceColor[PieceID::White], NormalFont, "%d", i + 1);
        }
    } // Draw Othello Board
    for (int i = 0; i <= 8; i++) { // Draw vertical & horizontal line
        DrawLine(25 + 20 * i, 65, 25 + 20 * i, 225, PieceColor[PieceID::Black]);
        DrawLine(25, 65 + 20 * i, 185, 65 + 20 * i, PieceColor[PieceID::Black]);
        for (int j = 0; j < 2; j++) {
            for (int k = 0; i < 2 && k < 3; k++) { // Draw initialized pieces
                DrawCircle(95 + 20 * i + 213 * k, 135 + 20 * j, 8, PieceColor[(i + j + 1) % 2], TRUE);
            }
        }
    }
    DrawStringToHandle(-5, 250, "Othello", PieceColor[PieceID::Black], TitleFont);
    DrawStringToHandle(20, 325, "was born in Japan\nEnjoy playing!!", PieceColor[PieceID::Black], NormalFont);
    DrawStringToHandle(265, 250, "Nip", PieceColor[PieceID::White], TitleFont);
    DrawStringToHandle(240, 325, "is round Othello,\nso has no corner!!", PieceColor[PieceID::White], NormalFont);
    DrawStringToHandle(425, 250, "Sphere", GuideColor[PieceID::Black], TitleFont);
    DrawStringToHandle(440, 325, "The line is connected\n   behind the board!!", GuideColor[0], NormalFont);
    DrawStringToHandle(180, 400, "Which would you like to play?", PieceColor[PieceID::White], NormalFont);
}
void Othello::CharSelect() {
    DataLength = GetNetWorkDataLength(NetHandle);
    if (DataLength == sizeof(csd)) { NetWorkRecv(NetHandle, &csd, DataLength); return; }
    if (csd.CharSelectNo < 0) { UpdateScene(SceneID::BoardSelect); return; }
    else if (csd.CharSelectNo > 2) { UpdateScene(SceneID::GameStart); return; }
    if (csd.MaxCharSelectNo > csd.CharSelectNo) DrawGraph(55, 0, Icon[IconID::Forward], TRUE);
    for (int i = 0; i < 4; i++) {
        DrawBox(160 * i, 25, 160 * (i + 1), 235, PieceColor[(i + 1) % 2], TRUE);
        DrawGraph(160 * i - 50, 30, CharImage[i], TRUE);
        DrawStringToHandle(160 * i + 15, 200, CharName[i].c_str(), PieceColor[PieceID::White], CharNameFont);
        if (i == csd.CharNo[0] || i == csd.CharNo[1]) DrawGraph(160 * i, 60, Icon[IconID::Selected], TRUE);
    } DrawBox(0, 235, 160, 455, PieceColor[PieceID::Black], TRUE); DrawGraph(-50, 240, CharImage[CharID::Yoichi], TRUE);
    DrawStringToHandle(15, 410, CharName[CharID::Yoichi].c_str(), PieceColor[PieceID::White], CharNameFont);
    if (CharID::Yoichi == csd.CharNo[0] || CharID::Yoichi == csd.CharNo[1]) 
        DrawGraph(0, 270, Icon[IconID::Selected], TRUE);
    DrawBox(480, 235, 640, 300, PieceColor[PieceID::White], TRUE);
    DrawStringToHandle(490, 240, "Order", PieceColor[PieceID::Black], CharNameFont);
    DrawStringToHandle(520, 275, OrderName[csd.Order].c_str(), PieceColor[PieceID::Black], NormalFont);
    DrawBox(480, 300, 640, 455, GuideColor[2], TRUE);
    DrawFormatStringToHandle(485, 310, PieceColor[PieceID::White], NormalFont, "AI Max Lv.%d", cd.MaxCharLv[Board]);
    // DrawStringToHandle(485, 335, ("Your Status:\n" + UserStatusName[UserStatus]).c_str(), PieceColor[PieceID::White], NormalFont);
    // DrawFormatStringToHandle(485, 380, PieceColor[PieceID::White], NormalFont, "Opponent IP:\n%d.%d.%d.%d", ip.d1, ip.d2, ip.d3, ip.d4);
    for (int i = 0; i < 2; i++) {
        DrawBox(160 + 160 * i, 235, 320 + 160 * i, 455, GuideColor[(i + csd.Order) % 2], TRUE);
        if (i < csd.CharSelectNo) {
            DrawGraph(110 + 160 * i, 240, CharImage[csd.CharNo[i]], TRUE);
            DrawStringToHandle(175 + 160 * i, 410, CharName[csd.CharNo[i]].c_str(), PieceColor[PieceID::White], CharNameFont);
        }
        if (ClickBox(160 + 160 * i, 235, 320 + 160 * i, 455) == 1) csd.CharLv[i] = (csd.CharLv[i] + 1) % (cd.MaxCharLv[Board] + 1);
        if (csd.CharLv[i] > 0) {
            DrawStringToHandle(290 + 160 * i, 245, "CP", PieceColor[(i + csd.Order) % 2], NormalFont);
            DrawFormatStringToHandle(160 + 160 * i, 245, PieceColor[(i + csd.Order) % 2], NormalFont, "Lv.%d", csd.CharLv[i]);
        }
        else DrawFormatStringToHandle(290 + 160 * i, 245, PieceColor[(i + csd.Order) % 2], NormalFont, "%dP", i + 1);
    }
    if (csd.CharSelectNo < 2) {
        if (UserStatus == Offline || UserStatus == csd.CharSelectNo) {
            DrawFormatStringToHandle(170 + 160 * csd.CharSelectNo, 300, PieceColor[csd.CharSelectNo], NormalFont,
                "%dP, select\nyour character...", csd.CharSelectNo + 1);
            for (int i = 0; i < 4; i++) {
                if (i == csd.CharNo[0]) { continue; }
                if (ClickBox(160 * i, 25, 160 * (i + 1), 235) == 1) {
                    csd.CharNo[csd.CharSelectNo] = i; csd.CharSelectNo++; csd.MaxCharSelectNo = csd.CharSelectNo;
                    if (UserStatus == csd.CharSelectNo) { NetWorkSend(NetHandle, &csd, sizeof(csd)); }
                }
            }
            if (ClickBox(0, 235, 160, 455) == 1) {
                csd.CharNo[csd.CharSelectNo] = CharID::Yoichi; csd.CharSelectNo++; 
                csd.MaxCharSelectNo = csd.CharSelectNo;
                if (UserStatus == csd.CharSelectNo) { NetWorkSend(NetHandle, &csd, sizeof(csd)); }
            }
        }
        else {
            DrawFormatStringToHandle(170 + 160 * csd.CharSelectNo, 300, PieceColor[csd.CharSelectNo], NormalFont,
                "%dP is selecting\na charcter...", csd.CharSelectNo + 1);
        }
    }
    else {
        if (UserStatus == Offline || UserStatus == Host) {
            for (int i = 0; i < 2; i++) { DrawGraph(605, 340 + 55 * i, KeyImage[cd.Key[CtrlID::Yes + i]], TRUE); }
            DrawStringToHandle(485, 360, "Game Start!!", PieceColor[PieceID::White], NormalFont);
            DrawStringToHandle(485, 400, "Cancel...", PieceColor[PieceID::White], NormalFont); DrawGraph(55, 0, Icon[IconID::Forward], TRUE);
            if (HitRoleKey(CtrlID::Yes) == 1 || ClickBox(55, 0, 80, 25) == 1 || HitRoleKey(CtrlID::Forward) == 1) {
                int Order = csd.Order; if (Order == OrderID::Random) { Order = GetRand(1); }
                for (int i = 0; i < 2; i++) {
                    int CharNo = csd.CharNo[(i + Order) % 2]; csd.Lv[i] = csd.CharLv[(i + Order) % 2];
                    csd.Name[i] = CharName[CharNo]; csd.Image[i] = CharImage[CharNo];
                } UpdateScene(SceneID::GameStart); csd.CharSelectNo = 3;
                if (UserStatus == Host) { NetWorkSend(NetHandle, &csd, sizeof(csd)); } return;
            }
            else if (HitRoleKey(CtrlID::No) == 1) {
                csd.CharSelectNo--; csd.CharNo[csd.CharSelectNo] -= CharID::Num;
                if (UserStatus == Host) { NetWorkSend(NetHandle, &csd, sizeof(csd)); } return;
            }
        }
    }
    if (ClickBox(30, 0, 55, 25) == 1 || HitRoleKey(CtrlID::Back) == 1) {
        if (csd.CharSelectNo < 1) { csd.CharSelectNo = -1; UpdateScene(SceneID::BoardSelect); return; }
        csd.CharSelectNo--; csd.CharNo[csd.CharSelectNo] -= CharID::Num; return;
    }
    if (csd.MaxCharSelectNo > csd.CharSelectNo && (ClickBox(55, 0, 80, 25) == 1 || HitRoleKey(CtrlID::Forward) == 1)) {
        csd.CharNo[csd.CharSelectNo] += CharID::Num; csd.CharSelectNo++; return;
    }
    if (csd.CharSelectNo > 2 || ClickBox(565, 430, 640, 455) == 1 || HitRoleKey(CtrlID::Skip) == 1) {
        for (int i = 0; i < 2; i++) {
            if (csd.CharNo[i] < 0) { csd.CharNo[i] += CharID::Num; }
            csd.Lv[i] = csd.CharLv[i]; csd.Name[i] = CharName[csd.CharNo[i]]; csd.Image[i] = CharImage[csd.CharNo[i]];
        } csd.CharSelectNo = 3; csd.MaxCharSelectNo = 3; UpdateScene(SceneID::GameStart); return;
    }
    if (ClickBox(480, 235, 640, 300) == 1) { csd.Order = (csd.Order + 1) % OrderID::Num; } return;
}
void Othello::GameStart() {
    for (int i = 0; i < 2; i++) {
        DrawBox(320 * i, 25, 320 * (i + 1), 455, GuideColor[i], TRUE);
        DrawGraph(380 * i, 40, csd.Image[i], TRUE);
        DrawCircle(285 + 160 * i, 308, 6, PieceColor[i], TRUE);
        DrawStringToHandle(190 + 160 * i, 300, csd.Name[i].c_str(), PieceColor[i], NormalFont);
    } DrawGraph(55, 0, Icon[IconID::Forward], TRUE);
    DrawStringToHandle(300, 300, "V.S.", PieceColor[PieceID::White], NormalFont);
    DrawStringToHandle(210, 340, "Order has been decided!!", PieceColor[PieceID::White], NormalFont);
    DrawGraph(565, 430, Icon[IconID::Skip], TRUE);
    if (ClickBox(30, 0, 55, 25) == 1 || HitRoleKey(CtrlID::Back) == 1) UpdateScene(SceneID::CharSelect);
    if (ClickBox(55, 0, 80, 25) == 1 || HitRoleKey(CtrlID::Forward) == 1 ||
        ClickBox(565, 430, 640, 455) == 1 || HitRoleKey(CtrlID::Skip) == 1 || time(NULL) >= StartTime) {
        InitScene(); UpdateScene(SceneID::Game);
    }
    return;
}
void Othello::InitScene() {
    switch (Scene) {
    case SceneID::Home: case SceneID::BoardSelect: Board = BoardID::Default;
    case SceneID::CharSelect: csd.MaxCharSelectNo = 0; csd.CharSelectNo = 0; csd.Order = OrderID::Random;
        for (int i = 0; i < 2; i++) { csd.CharLv[i] = 0; csd.CharNo[i] = i - CharID::Num; }
    case SceneID::GameStart: case SceneID::Game:
        if (Turn > 1) { ChangeBGM(cd.BoardBGM[Board]); } StartTime = time(NULL); MaxTurn = 1; Turn = 1;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) { Piece[i][j] = PieceID::Space; }
        }
        //put PieceID::Black & PieceID::White pieces on the center of board
        for (int i = 0; i < 2; i++) {
            PieceNum[i] = 2;
            for (int j = 0; j < 2; j++) { Piece[3 + i][3 + j] = (i + j + 1) % 2; }
        }
        if (Board == BoardID::Nip) PieceNum[PieceID::Space] = 48; else PieceNum[PieceID::Space] = 60;
    FailureSuccession = 0; GameEnd = false; case SceneID::Pause: if (Scene != SceneID::Home) break;
    default: for (int i = 0; i < CtrlID::Num; i++) { cd.TwinKey[i] = SINGLE; }
           cd.Key[CtrlID::Guide] = KEY_INPUT_G; cd.Key[CtrlID::Reverse] = KEY_INPUT_Z;
           cd.Key[CtrlID::Home] = KEY_INPUT_T; cd.Key[CtrlID::Restart] = KEY_INPUT_R;
           cd.Key[CtrlID::Init] = KEY_INPUT_I; cd.Key[CtrlID::Skip] = KEY_INPUT_X;
           cd.Key[CtrlID::Exit] = KEY_INPUT_Q; cd.Key[CtrlID::Config] = KEY_INPUT_C;
           cd.Key[CtrlID::ChMusic] = KEY_INPUT_M; cd.Key[CtrlID::Window] = KEY_INPUT_F11;
           cd.Key[CtrlID::Sound] = KEY_INPUT_ESCAPE; cd.Key[CtrlID::Pause] = KEY_INPUT_E;
           cd.Key[CtrlID::Back] = KEY_INPUT_LEFT; cd.Key[CtrlID::Forward] = KEY_INPUT_RIGHT;
           cd.Key[CtrlID::Down] = KEY_INPUT_3; cd.Key[CtrlID::Up] = KEY_INPUT_4;
           cd.GuideLv = 0; cd.ReverseMode = 0; cd.AITime = 40; cd.BGMPlayMode = 0;
           for (int i = 0; i < BoardID::Num; i++) { cd.MaxCharLv[i] = 1; } 
           cd.SceneBGM[SceneID::Home]        = BGMID::Chocolate; 
           cd.SceneBGM[SceneID::BoardSelect] = BGMID::Hometown; 
           cd.SceneBGM[SceneID::CharSelect]  = BGMID::Detect;
           cd.BoardBGM[BoardID::Othello]     = BGMID::Legend;
           cd.BoardBGM[BoardID::Nip]         = BGMID::DeadAlive;
           cd.BoardBGM[BoardID::Sphere]      = BGMID::NothingDie;
           for (int i = 0; i < SoundID::Num; i++) { cd.SoundVol[i] = MaxSoundVol; } 
           ChangeSoundVol(SoundID::Total, cd.SoundVol[SoundID::Total]); break;
    } UpdateScene(Scene); return;
}
void Othello::Game() {
    if (Turn < MaxTurn) DrawGraph(55, 0, Icon[IconID::Forward], TRUE);
    for (int i = 0; i < 2; i++) {
        DrawBox(320 * i, 25, 320 * (i + 1), 455, GuideColor[i], TRUE); DrawGraph(500 * i - 50, 80, csd.Image[i], TRUE);
        DrawFormatStringToHandle(490 * i + 30, 350, PieceColor[PieceID::White], CharNameFont, "%3.1f%%", Propotion[i] * 100);
    }
    if (Turn < 498 && FailureSuccession < 3) DrawGraph(565, 430, Icon[IconID::Skip], TRUE);
    switch (Board) {
    case BoardID::Othello:
        DrawBox(140, 45, 500, 405, FrameColor, TRUE); DrawBox(160, 65, 480, 385, PieceColor[PieceID::Space], TRUE);
        for (int i = 0; i <= 8; i++) { // Draw vertical & horizontal line
            DrawLine(160 + 40 * i, 65, 160 + 40 * i, 385, PieceColor[PieceID::Black], 2);
            DrawLine(160, 65 + 40 * i, 480, 65 + 40 * i, PieceColor[PieceID::Black], 2);
            for (int j = 0; i < 2 && j < 2; j++) {
                DrawCircle(240 + 160 * i, 145 + 160 * j, 4, PieceColor[PieceID::Black], TRUE);
            }
        }
        break;
    case BoardID::Nip:
        DrawBox(140, 45, 500, 405, FrameColor, TRUE); DrawBox(160, 65, 480, 385, PieceColor[PieceID::Space], TRUE);
        for (int i = 0; i < 6; i++) { // Draw vertical & horizontal line
            if (i == 2 || i == 3) {
                DrawLine(220 + 40 * i, 85, 220 + 40 * i, 365, PieceColor[PieceID::Black]);
                DrawLine(180, 125 + 40 * i, 460, 125 + 40 * i, PieceColor[PieceID::Black]);
            }
            else {
                DrawLine(220 + 40 * i, 125, 220 + 40 * i, 325, PieceColor[PieceID::Black]);
                DrawLine(220, 125 + 40 * i, 420, 125 + 40 * i, PieceColor[PieceID::Black]);
            }
            for (int j = 0; i + j < 5 && j < 2; j++) { // Draw diagonal line
                if (2 * i + j == 4) {
                    DrawLine(140 + 40 * i, 245 + 40 * (i + j), 340 + 40 * (i + j), 45 + 40 * i, PieceColor[PieceID::Black]);
                    DrawLine(140 + 40 * i, 205 - 40 * (i + j), 340 + 40 * (i + j), 405 - 40 * i, PieceColor[PieceID::Black]);
                }
                else {
                    DrawLine(180 + 40 * i, 205 + 40 * (i + j), 300 + 40 * (i + j), 85 + 40 * i, PieceColor[PieceID::Black]);
                    DrawLine(180 + 40 * i, 245 - 40 * (i + j), 300 + 40 * (i + j), 365 - 40 * i, PieceColor[PieceID::Black]);
                }
                for (int k = 0; i < 2 && k < 2; k++) { // Draw special diagonal line
                    DrawLine(194 + 252 * k, 165 + 120 * i, 220 + 200 * k, 165 + 40 * (2 * i + j), PieceColor[PieceID::Black]);
                    DrawLine(260 + 120 * i, 99 + 252 * k, 260 + 40 * (2 * i + j), 125 + 200 * k, PieceColor[PieceID::Black]);
                }
            }
        }
        DrawCircle(320, 225, 140, PieceColor[PieceID::Black], FALSE, 2);
        break;
    case BoardID::Sphere:
        DrawCircle(320, 225, 200, PieceColor[PieceID::Space], TRUE);
        // Draw diagonal line
        DrawLine(180, 85, 460, 365, PieceColor[PieceID::Black], 2); DrawLine(180, 365, 460, 85, PieceColor[PieceID::Black], 2);
        for (int i = 0; i < 8; i++) {
            if (i > 0 && i < 7) {
                // Draw vertical & horizontal line
                DrawLine(180 + 40 * i, 65, 180 + 40 * i, 385, PieceColor[PieceID::Black], abs(i % 3 - (i + 1) % 3));
                DrawLine(160, 85 + 40 * i, 480, 85 + 40 * i, PieceColor[PieceID::Black], abs(i % 3 - (i + 1) % 3));
                // Draw diagonal line
                DrawLine(440 - 40 * i, 65, 480, 105 + 40 * i, PieceColor[PieceID::Black], abs((i + 1) % 3 - (i + 2) % 3));
                DrawLine(440 - 40 * i, 385, 480, 345 - 40 * i, PieceColor[PieceID::Black], abs((i + 1) % 3 - (i + 2) % 3));
                DrawLine(160, 105 + 40 * i, 200 + 40 * i, 65, PieceColor[PieceID::Black], abs((i + 1) % 3 - (i + 2) % 3));
                DrawLine(160, 345 - 40 * i, 200 + 40 * i, 385, PieceColor[PieceID::Black], abs((i + 1) % 3 - (i + 2) % 3));
            }
            else {
                DrawLine(180 + 40 * i, 85, 180 + 40 * i, 365, PieceColor[PieceID::Black]);
                DrawLine(180, 85 + 40 * i, 460, 85 + 40 * i, PieceColor[PieceID::Black]);
            }
        }
        break;
    default: exit(-1);
    } DrawGraph(490, 0, Icon[IconID::Guide], TRUE); DrawGraph(515, 0, Icon[IconID::Reverse], TRUE);
    DrawBox(0, 30, (int)(Propotion[PieceID::Black] * 640), 45, PieceColor[PieceID::Black], TRUE);
    DrawBox((int)(Propotion[PieceID::Black] * 640), 30, 640, 45, PieceColor[PieceID::White], TRUE); DrawTime(530, 60, PieceColor[PieceID::White], NormalFont);
    for (int i = 0; i < 8; i++) {
        DrawStringToHandle(172 + 40 * i, 46, Alphabet[i].c_str(), PieceColor[PieceID::White], NormalFont);
        DrawFormatStringToHandle(145, 78 + 40 * i, PieceColor[PieceID::White], NormalFont, "%d", i + 1);
        for (int j = 0; j < 8; j++) {
            int k = 180 + 40 * i; int s = 85 + 40 * j; int t = 0;
            if (Board == BoardID::Nip) {
                t = (i - 3) * (i - 4) + (j - 3) * (j - 4);
                if (t == 14) {
                    if (!i) { k += 12; }
                    else if (!j) { s += 12; }
                    else if (i == 7) { k -= 12; }
                    else if (j == 7) { s -= 12; }
                }
            }
            if (t <= 14 && Board > 0) {
                DrawCircle(k, s, 4, PieceColor[PieceID::Black], TRUE);
                DrawCircle(k, s, 2, PieceColor[PieceID::Space], TRUE);
            }
            if (JudgePiece[i][j]) {
                if (cd.GuideLv > GuideLvID::Off) {
                    DrawBox(k - 19, s - 19, k + 20, s + 20, GuideColor[Player], TRUE);
                }
                if (cd.GuideLv == GuideLvID::Kind) {
                    DrawFormatStringToHandle(k - 5, s - 7, PieceColor[Player], NormalFont, "%d", JudgePiece[i][j]);
                }
            }
            if (cd.GuideLv == GuideLvID::Debag) {
                DrawFormatStringToHandle(k - 5, s - 7, PieceColor[PieceID::White], NormalFont, "%1.0f", cd.PieceWorth[Board][i][j]);
            }
            if (Piece[i][j] < PieceID::Space) {
                if (Board == BoardID::Nip) { DrawCircle(k, s, 12, PieceColor[Piece[i][j]], TRUE); }
                else { DrawCircle(k, s, 16, PieceColor[Piece[i][j]], TRUE); }
            }
        }
    }
    switch (FailureSuccession) {
    case 3:
        DrawFormatStringToHandle(25, 60, PieceColor[PieceID::Black], NormalFont, "Turn%4d", MaxTurn);
        if (PieceNum[PieceID::Black] == PieceNum[PieceID::White]) {
            DrawStringToHandle(290, 410, "Draw!!", GuideColor[2], NormalFont);
        }
        for (int i = 0; i < PlayerID::Num; i++) {
            if (PieceNum[i % PlayerID::Num] > PieceNum[(i + 1) % PlayerID::Num]) {
                DrawCircle(320, 418, 6, PieceColor[i % 2], TRUE);
                DrawFormatStringToHandle(220, 410, PieceColor[i % 2], NormalFont, "%7s", csd.Name[i % 2].c_str());
                DrawStringToHandle(340, 410, "Win!!", PieceColor[PieceID::White], NormalFont);
            }
        } break;
    case 2:
    case 1:
        DrawStringToHandle(160, 430, "You can't put your piece!? Skip", PieceColor[Player], NormalFont);
        DrawGraph(445, 427, Icon[IconID::Skip], TRUE);
    default:
        DrawFormatStringToHandle(25, 60, PieceColor[PieceID::Black], NormalFont, "Turn%4d", Turn);
        DrawFormatStringToHandle(480, 408, PieceColor[PieceID::White], NormalFont, "%2d remain", PieceNum[PieceID::Space]);
        DrawStringToHandle(220, 410, (csd.Name[Player] + ", it's your turn!").c_str(), PieceColor[Player], NormalFont);
        break;
    }
    for (int i = 0; i < 2; i++) {
        DrawCircle(280 + 160 * i, 398, 6, PieceColor[i], TRUE);
        DrawStringToHandle(180 + 160 * i, 388, csd.Name[i].c_str(), PieceColor[PieceID::White], NormalFont);
        DrawFormatStringToHandle(290 + 160 * i, 388, PieceColor[PieceID::White], NormalFont, "%2d", PieceNum[i]);
    }
    return;
}
void Othello::CallBoard() {
    for (int i = 0; i < 3; i++) { PieceNum[i] = 0; }
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece[i][j] = PieceRecord[Turn][i][j];
            PieceNum[Piece[i][j]]++;
        }
    } if (Board == BoardID::Nip) PieceNum[PieceID::Space] -= 12; return;
}
int Othello::DrawScene() {
    DrawBox(0, 0, 640, 25, FrameColor, TRUE);
    switch (Scene) {
    case SceneID::Home:        Title();       break;
    case SceneID::BoardSelect: BoardSelect(); break;
    case SceneID::CharSelect:  CharSelect();  break;
    case SceneID::GameStart:   GameStart();   break;
    case SceneID::Game: case SceneID::Pause: Game(); break;
    default:                   Config();      break;
    } DrawBox(0, 455, 640, 480, FrameColor, TRUE);
    if (Scene > SceneID::Home && Scene < SceneID::Game) { DrawGraph(565, 430, Icon[IconID::Skip], TRUE); }
    if (Scene < 0 || Scene > SceneID::CharSelect) {
        DrawFormatStringToHandle(280, 5, PieceColor[PieceID::White], NormalFont, "AI speed:%3d/100", 100 - cd.AITime);
        if (cd.AITime < 100) { DrawGraph(440, 0, Icon[IconID::Down], TRUE); if (ClickBox(440, 0, 465, 25) == 1) { cd.AITime++; } }
        if (cd.AITime > 0) { DrawGraph(465, 0, Icon[IconID::Up], TRUE); if (ClickBox(465, 0, 490, 25) == 1) { cd.AITime--; } }
    }
    switch (Scene) {

    case ConfigID::Game: case ConfigID::Pause: case SceneID::Game: case SceneID::Pause:
        DrawGraph(490, 0, Icon[IconID::Guide], TRUE); DrawGraph(515, 0, Icon[IconID::Reverse], TRUE);
        DrawGraph(540, 0, Icon[IconID::Pause + PauseMode], TRUE);
    case ConfigID::BoardSelect: case ConfigID::CharSelect: case ConfigID::GameStart: case SceneID::CharSelect: case SceneID::GameStart:
        DrawGraph(0, 455, Icon[IconID::Restart], TRUE);
    default:
        DrawGraph(0, 0, Icon[IconID::Home], TRUE); DrawGraph(25, 455, Icon[IconID::Init], TRUE);
        DrawGraph(590, 0, Icon[IconID::AnotherWindow + WindowMode], TRUE); 
        DrawGraph(85, 0, Icon[IconID::Mute + MuteMode], TRUE);
        DrawGraph(615, 0, Icon[IconID::Exit], TRUE); DrawGraph(30, 0, Icon[IconID::Back], TRUE);
        DrawGraph(565, 0, Icon[IconID::Config], TRUE);
        DrawGraph(50, 455, Icon[IconID::ChMusic], TRUE); DrawCircle(533, 468, 10, PieceColor[PieceID::White], FALSE);
        DrawStringToHandle(75, 460, BGMName[PlayingBGM].c_str(), PieceColor[PieceID::White], NormalFont);
        DrawStringToHandle(380, 460, "Othello World  C 2022 mini", PieceColor[PieceID::White], NormalFont); break;
    }
    switch (Scene) {
    case SceneID::Home:        DrawStringToHandle(115, 5, "Title", PieceColor[PieceID::White], NormalFont);                                            break;
    case SceneID::BoardSelect: DrawStringToHandle(115, 5, "Board Select", PieceColor[PieceID::White], NormalFont);                                     break;
    case SceneID::CharSelect:  DrawStringToHandle(115, 5, (BoardName[Board] + " >> Charcter Select").c_str(), PieceColor[PieceID::White], NormalFont); break;
    case SceneID::GameStart:   DrawStringToHandle(115, 5, "Game Start", PieceColor[PieceID::White], NormalFont);                                       break;
    case SceneID::Game:        DrawStringToHandle(115, 5, BoardName[Board].c_str(), PieceColor[PieceID::White], NormalFont);                           break;
    default:                   DrawStringToHandle(115, 5, "Config", PieceColor[PieceID::White], NormalFont);                                           break;                                                                 break;
    }
    if (Scene == SceneID::Game && ClickBox(140, 45, 500, 405) >= 0) {
        if (Board == BoardID::Nip) { DrawCircle(CursorX, CursorY, 12, PieceColor[Player], TRUE); }
        else { DrawCircle(CursorX, CursorY, 16, PieceColor[Player], TRUE); }
    }
    DrawCursor(); return Scene;
}
void Othello::UpdateScene(int TargetScene) {
    switch (TargetScene) {
    case SceneID::Home: if (Scene) ChangeBGM(cd.SceneBGM[SceneID::Home]); break;
    case SceneID::BoardSelect: if (Scene != SceneID::BoardSelect) ChangeBGM(cd.SceneBGM[SceneID::BoardSelect]);
        if (Board >= 0) Board -= BoardID::Num; break;
    case SceneID::CharSelect: if (Scene != SceneID::CharSelect) ChangeBGM(cd.SceneBGM[SceneID::CharSelect]);
        if (Scene == SceneID::GameStart) csd.CharSelectNo = 2;
        else if (Scene >= 0) {
            csd.CharSelectNo = 0;
            for (int j = 0; j < PlayerID::Num && csd.CharNo[j] >= 0; j++) { csd.CharNo[j] -= CharID::Num; }
        }
        break;
    case SceneID::GameStart: if (Scene < SceneID::GameStart) ChangeBGM(cd.BoardBGM[Board]); StartTime = time(NULL) + 3; break;
    case SceneID::Pause: PauseMode = 1; if (Scene < 0) ChangeBGM(cd.BoardBGM[Board]); break;
    case SceneID::Game: 
        if (Scene == ConfigID::Game) { StartTime = time(NULL) - LapseTime; break; }
        Player = (Turn - 1) % 2; if (Scene < SceneID::GameStart) ChangeBGM(cd.BoardBGM[Board]);
        if (!Simulation()) FailureSuccession++; else FailureSuccession = 0;
        if (FailureSuccession > 2 || !PieceNum[PieceID::Space]) FailureSuccession = 3;
        Propotion[PieceID::Black] = (float)PieceNum[PieceID::Black] / (float)(PieceNum[PieceID::Black] + PieceNum[PieceID::White]);
        Propotion[1] = (float)PieceNum[PieceID::White] / (float)(PieceNum[PieceID::Black] + PieceNum[PieceID::White]);
        if (!GameEnd && FailureSuccession == 3) {
            for (int i = 0; i < PlayerID::Num; i++) {
                if (PieceNum[i] > PieceNum[1 - i]) {
                    for (int x = 0; x < 8; x++) {
                        for (int y = 0; y < 8; y++) {
                            if (PutPlayer[x][y] == i) { cd.PieceWorth[Board][x][y] += (double)(PieceNum[i] - PieceNum[1 - i]); }
                            else { cd.PieceWorth[Board][x][y] += (double)(PieceNum[1 - i] - PieceNum[i]); }
                        }
                    } if (!csd.Lv[i]) {
                        if (csd.Lv[1 - i] < 6 && csd.Lv[1 - i] >= cd.MaxCharLv[Board]) {
                            cd.MaxCharLv[Board] = csd.Lv[1 - i] + 1;
                        }
                    }
                    if (csd.Lv[i] <= csd.Lv[1 - i]) { ChangeBGM(BGMID::Victory); }
                    else { ChangeBGM(BGMID::Sorrow); }
                }
            } double PieceCount[10] = {};
            for (int x = 0; x < 8; x++) {
                for (int y = 0; y < 8; y++) {
                    int t = (x - 3) * (x - 4) + (y - 3) * (y - 4);
                    switch (t) {
                    case  0: PieceCount[0] += cd.PieceWorth[Board][x][y] / 4.0; break;
                    case  2: PieceCount[1] += cd.PieceWorth[Board][x][y] / 8.0; break;
                    case  4: PieceCount[2] += cd.PieceWorth[Board][x][y] / 4.0; break;
                    case  6: PieceCount[3] += cd.PieceWorth[Board][x][y] / 8.0; break;
                    case  8: PieceCount[4] += cd.PieceWorth[Board][x][y] / 8.0; break;
                    case 12: if (x == 1 || x == 6) { PieceCount[5] += cd.PieceWorth[Board][x][y] / 4.0; break; }
                            PieceCount[6] += cd.PieceWorth[Board][x][y] / 8.0; break;
                    case 14: PieceCount[7] += cd.PieceWorth[Board][x][y] / 8.0; break;
                    case 18: PieceCount[8] += cd.PieceWorth[Board][x][y] / 8.0; break;
                    case 24: PieceCount[9] += cd.PieceWorth[Board][x][y] / 4.0; break;
                    default: break;
                    }
                }
            }
            for (int x = 0; x < 8; x++) {
                for (int y = 0; y < 8; y++) {
                    int t = (x - 3) * (x - 4) + (y - 3) * (y - 4);
                    switch (t) {
                    case  0: cd.PieceWorth[Board][x][y] = PieceCount[0]; break;
                    case  2: cd.PieceWorth[Board][x][y] = PieceCount[1]; break;
                    case  4: cd.PieceWorth[Board][x][y] = PieceCount[2]; break;
                    case  6: cd.PieceWorth[Board][x][y] = PieceCount[3]; break;
                    case  8: cd.PieceWorth[Board][x][y] = PieceCount[4]; break;
                    case 12: if (x == 1 || x == 6) { cd.PieceWorth[Board][x][y] = PieceCount[5]; break; }
                            cd.PieceWorth[Board][x][y] = PieceCount[6]; break;
                    case 14: cd.PieceWorth[Board][x][y] = PieceCount[7]; break;
                    case 18: cd.PieceWorth[Board][x][y] = PieceCount[8]; break;
                    case 24: cd.PieceWorth[Board][x][y] = PieceCount[9]; break;
                    default: break;
                    }
                }
            } GameEnd = true;
        } break;
    default: if (Scene >= 0) ChangeBGM(BGMID::Sorrow); break;
    } Scene = TargetScene; return;
}
void Othello::BackSystem() {
    Keyboard::GetInstance()->Update();
    switch (Scene) {
    case SceneID::Game:
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                if (ClickBox(160 + 40 * x, 60 + 40 * y, 200 + 40 * x, 100 + 40 * y) == 1 && Reverse(x, y)) {
                    Turn++; if (Turn < 200) { MaxTurn = Turn; } PutPlayer[x][y] = Player; UpdateScene(SceneID::Game);
                    return;
                }
            }
        }
        if (Turn < 199 && FailureSuccession < 3 && (ClickBox(565, 430, 640, 455) == 1 || HitRoleKey(CtrlID::Skip) == 1)) {
            Turn++; MaxTurn = Turn; UpdateScene(SceneID::Game); return;
        }
        if (Turn < MaxTurn && (ClickBox(55, 0, 80, 25) == 1 || HitRoleKey(CtrlID::Forward) == 1)) {
            Turn++; CallBoard(); UpdateScene(SceneID::Game); return;
        }
        if (ClickBox(30, 0, 55, 25) == 1 || HitRoleKey(CtrlID::Back) == 1) {
            if (Turn == 1) { UpdateScene(SceneID::GameStart); return; }
            Turn--; if (FailureSuccession > 2) { FailureSuccession -= 2; }
            else { FailureSuccession--; }
            CallBoard(); UpdateScene(SceneID::Game); return;
        }
        if (Turn == MaxTurn && FailureSuccession < 3 && csd.Lv[Player] > 0) {
            AITimeCount++;
            if (AITimeCount >= cd.AITime) {
                AITimeCount = 0; AIput(); Turn++; if (Turn < 500) { MaxTurn = Turn; } UpdateScene(SceneID::Game); return;
            }
        }
    case SceneID::Pause:
        if (ClickBox(540, 0, 565, 25) == 1 || HitRoleKey(CtrlID::Pause) == 1) {
            if (!PauseMode) { UpdateScene(SceneID::Pause); return; }
            StartTime = time(NULL) - LapseTime; PauseMode = 0; Scene = SceneID::Game; return;
        }
    case ConfigID::BoardSelect: case ConfigID::CharSelect: case ConfigID::GameStart:
    case ConfigID::Game: case ConfigID::Pause:
    case SceneID::CharSelect: case SceneID::GameStart:
        if (ClickBox(0, 455, 25, 480) == 1 || HitRoleKey(CtrlID::Restart) == 1) { UpdateScene(SceneID::BoardSelect); return; }
    default:
        // Check Connection
        if (UserStatus != Offline) {
            LostHandle = GetLostNetWork();
            if (LostHandle == NetHandle) {
                DrawStringToHandle(240, 200, "Opponent was disconnected!!", PieceColor[PieceID::White], NormalFont); WaitTimer(1000);
                ConnectMode = Disconnect; UserStatus = Offline;
            }
        }
        if (ClickBox(490, 0, 515, 25) == 1 || HitRoleKey(CtrlID::Guide) == 1) 
            cd.GuideLv = (cd.GuideLv + 1) % GuideLvID::Num;
        else if (ClickBox(515, 0, 540, 25) == 1 || HitRoleKey(CtrlID::Reverse) == 1) 
            cd.ReverseMode = (cd.ReverseMode + 1) % ReverseModeID::Num;
        else if (ClickBox(25, 455, 50, 480) == 1 || HitRoleKey(CtrlID::Init) == 1) InitScene();
        else if (ClickBox(0, 0, 28, 25) == 1 || HitRoleKey(CtrlID::Home) == 1) UpdateScene(SceneID::Home);
        else if (ClickBox(50, 455, 75, 480) == 1 || HitRoleKey(CtrlID::ChMusic) == 1) 
            ChangeBGM((PlayingBGM + 1) % BGMID::Num);
        else if (ClickBox(565, 0, 590, 25) == 1 || HitRoleKey(CtrlID::Config) == 1) {
            if (Scene == SceneID::Game) { PauseMode = 1; Scene = SceneID::Pause; }
            if (Scene >= 0) { UpdateScene(Scene - SceneID::Num); }
            else { UpdateScene(Scene + SceneID::Num); }
        }
        else if (ClickBox(590, 0, 615, 25) == 1 || HitRoleKey(CtrlID::Window) == 1) ChangeWindow((WindowMode + 1) % 2);
        else if (ClickBox(85, 0, 110, 25) == 1 || HitRoleKey(CtrlID::Sound) == 1) ChangeSound((MuteMode + 1) % 2);
        else if (HitRoleKey(CtrlID::Down) > 10) ChangeSoundVol(SoundID::Total, cd.SoundVol[SoundID::Total] - 1);
        else if (HitRoleKey(CtrlID::Up) > 10) ChangeSoundVol(SoundID::Total, cd.SoundVol[SoundID::Total] + 1);
        else if (HitRoleKey(CtrlID::Down) == 1) ChangeSoundVol(SoundID::Total, cd.SoundVol[SoundID::Total] - 4);
        else if (HitRoleKey(CtrlID::Up) == 1) ChangeSoundVol(SoundID::Total, cd.SoundVol[SoundID::Total] + 4);
        else if (MuteMode && !CheckSoundMem(BGM[PlayingBGM])) {
            switch (cd.BGMPlayMode) {
            case BGMPlayModeID::Ascend: ChangeBGM((PlayingBGM + 1) % BGMID::Num); return;
            case BGMPlayModeID::Descend: ChangeBGM((PlayingBGM + BGMID::Num - 1) % BGMID::Num); return;
            case BGMPlayModeID::Random: ChangeBGM(GetRand(BGMID::Num - 1));
            default: return;
            }
        }
    } return;
}
int Othello::DrawTime(int x, int y, unsigned int Color, int FontHandle) {
    if (Scene == SceneID::Game) {
        LapseTime = time(NULL) - StartTime; LapseHour = (long int)(LapseTime - (long int)LapseTime % 3600) / 3600;
        LapseMin = (long int)(LapseTime - LapseSec) / 60 - LapseHour * 60; LapseSec = (long int)LapseTime % 60;
    } return DrawFormatStringToHandle(x, y, Color, FontHandle, "%2ld:%2ld:%2ld", LapseHour, LapseMin, LapseSec);
}
int Othello::DrawCursor() {
    Mouse::GetInstance()->Update(); CursorX = Mouse::GetInstance()->getX(); CursorY = Mouse::GetInstance()->getY();
    return DrawGraph(CursorX - 15, CursorY - 15, Icon[IconID::Cursor], TRUE);
}
void Othello::AIput() {
    int MAXSuccess = INT_MIN; cd.ReverseMode -= ReverseModeID::Num; 
    int AIX = 0, AIY = 0, AIPlayer = Player, AIOpponent = (Player + 1) % PlayerID::Num;;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            PieceRecord[Turn][i][j] = Piece[i][j];
        }
    }
    for (int x0 = 0; x0 < 8; x0++) {
        for (int y0 = 0; y0 < 8; y0++) {
            AISuccess[x0][y0] = 0; Player = AIPlayer;
            if (Reverse(x0, y0) > 0) {
                AISuccess[x0][y0] += ReverseSuccess;
                switch (Board) {
                case BoardID::Nip: if (Turn > 39) { break; }
                default: if (Turn > 53) { break; } AISuccess[x0][y0] += cd.PieceWorth[Board][x0][y0]; break;
                }
                if (csd.Lv[AIPlayer] == 1) {
                    for (int i = 0; i < 8; i++) {
                        for (int j = 0; j < 8; j++) {
                            Piece[i][j] = PieceRecord[Turn][i][j];
                        }
                    } 
                    if (AISuccess[x0][y0] > MAXSuccess) { MAXSuccess = AISuccess[x0][y0]; AIX = x0; AIY = y0; }
                    continue;
                }
                for (int i = 0; i < 8; i++) {
                    for (int j = 0; j < 8; j++) {
                        PieceRecord[Turn + 1][i][j] = Piece[i][j];
                    }
                }
                for (int x1 = 0; x1 < 8; x1++) {
                    for (int y1 = 0; y1 < 8; y1++) {
                        Player = AIOpponent;
                        if (Reverse(x1, y1) > 0) {
                            AISuccess[x0][y0] -= 3 * ReverseSuccess;
                            switch (Board) {
                            case BoardID::Nip: if (Turn > 39) { break; }
                            default: if (Turn > 53) { break; } AISuccess[x0][y0] -= 3*cd.PieceWorth[Board][x1][y1]; break;
                            }
                            if (csd.Lv[AIPlayer] == 2) {
                                for (int i = 0; i < 8; i++) {
                                    for (int j = 0; j < 8; j++) {
                                        Piece[i][j] = PieceRecord[Turn + 1][i][j];
                                    }
                                } continue;
                            }
                            for (int i = 0; i < 8; i++) {
                                for (int j = 0; j < 8; j++) {
                                    PieceRecord[Turn + 2][i][j] = Piece[i][j];
                                }
                            }
                            for (int x2 = 0; x2 < 8; x2++) {
                                for (int y2 = 0; y2 < 8; y2++) {
                                    Player = AIPlayer;
                                    if (Reverse(x2, y2) > 0) {
                                        AISuccess[x0][y0] += 0.01 * ReverseSuccess;
                                        switch (Board) {
                                        case BoardID::Nip: if (Turn > 39) { break; }
                                        default: if (Turn > 53) { break; } AISuccess[x0][y0] += 0.01 * cd.PieceWorth[Board][x2][y2]; break;
                                        }
                                        if (csd.Lv[AIPlayer] == 3) {
                                            for (int i = 0; i < 8; i++) {
                                                for (int j = 0; j < 8; j++) {
                                                    Piece[i][j] = PieceRecord[Turn + 2][i][j];
                                                }
                                            } continue;
                                        }
                                        for (int i = 0; i < 8; i++) {
                                            for (int j = 0; j < 8; j++) {
                                                PieceRecord[Turn + 3][i][j] = Piece[i][j];
                                            }
                                        }
                                        for (int x3 = 0; x3 < 8; x3++) {
                                            for (int y3 = 0; y3 < 8; y3++) {
                                                Player = AIOpponent;
                                                if (Reverse(x3, y3) > 0) {
                                                    AISuccess[x0][y0] -= 0.03 * ReverseSuccess;
                                                    switch (Board) {
                                                    case BoardID::Nip: if (Turn > 39) { break; }
                                                    default: if (Turn > 53) { break; }
                                                           AISuccess[x0][y0] -= 0.03 * cd.PieceWorth[Board][x3][y3]; break;
                                                    }
                                                    if (csd.Lv[AIPlayer] == 4) {
                                                        for (int i = 0; i < 8; i++) {
                                                            for (int j = 0; j < 8; j++) {
                                                                Piece[i][j] = PieceRecord[Turn + 3][i][j];
                                                            }
                                                        } continue;
                                                    }
                                                    for (int i = 0; i < 8; i++) {
                                                        for (int j = 0; j < 8; j++) {
                                                            PieceRecord[Turn + 4][i][j] = Piece[i][j];
                                                        }
                                                    }
                                                    for (int x4 = 0; x4 < 8; x4++) {
                                                        for (int y4 = 0; y4 < 8; y4++) {
                                                            Player = AIPlayer;
                                                            if (Reverse(x4, y4) > 0) {
                                                                AISuccess[x0][y0] += 0.001 * ReverseSuccess;
                                                                switch (Board) {
                                                                case BoardID::Nip: if (Turn > 39) { break; }
                                                                default: if (Turn > 53) { break; }
                                                                       AISuccess[x0][y0] += 0.001 * cd.PieceWorth[Board][x4][y4]; break;
                                                                }
                                                                if (csd.Lv[AIPlayer] == 5) {
                                                                    for (int i = 0; i < 8; i++) {
                                                                        for (int j = 0; j < 8; j++) {
                                                                            Piece[i][j] = PieceRecord[Turn + 4][i][j];
                                                                        }
                                                                    } continue;
                                                                }
                                                                for (int i = 0; i < 8; i++) {
                                                                    for (int j = 0; j < 8; j++) {
                                                                        PieceRecord[Turn + 5][i][j] = Piece[i][j];
                                                                    }
                                                                }
                                                                for (int x5 = 0; x5 < 8; x5++) {
                                                                    for (int y5 = 0; y5 < 8; y5++) {
                                                                        Player = AIOpponent;
                                                                        if (Reverse(x5, y5) > 0) {
                                                                            AISuccess[x0][y0] -= 0.003 * ReverseSuccess;
                                                                            switch (Board) {
                                                                            case BoardID::Nip: if (Turn > 39) { break; }
                                                                            default: if (Turn > 53) { break; }
                                                                                   AISuccess[x0][y0] -= 0.003 * cd.PieceWorth[Board][x5][y5]; break;
                                                                            }
                                                                            for (int i = 0; i < 8; i++) {
                                                                                for (int j = 0; j < 8; j++) {
                                                                                    Piece[i][j] = PieceRecord[Turn + 5][i][j];
                                                                                }
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                                for (int i = 0; i < 8; i++) {
                                                                    for (int j = 0; j < 8; j++) {
                                                                        Piece[i][j] = PieceRecord[Turn + 4][i][j];
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                    for (int i = 0; i < 8; i++) {
                                                        for (int j = 0; j < 8; j++) {
                                                            Piece[i][j] = PieceRecord[Turn + 3][i][j];
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        for (int i = 0; i < 8; i++) {
                                            for (int j = 0; j < 8; j++) {
                                                Piece[i][j] = PieceRecord[Turn + 2][i][j];
                                            }
                                        }
                                    }
                                }
                            }
                            for (int i = 0; i < 8; i++) {
                                for (int j = 0; j < 8; j++) {
                                    Piece[i][j] = PieceRecord[Turn + 1][i][j];
                                }
                            }
                        }
                    }
                }
                if (AISuccess[x0][y0] > MAXSuccess) { MAXSuccess = AISuccess[x0][y0]; AIX = x0; AIY = y0; }
                for (int i = 0; i < 8; i++) {
                    for (int j = 0; j < 8; j++) {
                        Piece[i][j] = PieceRecord[Turn][i][j];
                    }
                }
            }
        }
    } Player = AIPlayer; cd.ReverseMode += ReverseModeID::Num; 
    if (Reverse(AIX, AIY)) PutPlayer[AIX][AIY] = Player; return;
}
int Othello::Simulation() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            PieceRecord[Turn][i][j] = Piece[i][j];
        }
    } int Success = 0; cd.ReverseMode -= 2;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            JudgePiece[x][y] = Reverse(x, y);
            if (JudgePiece[x][y] > 0) {
                Success++;
                for (int i = 0; i < 8; i++) {
                    for (int j = 0; j < 8; j++) {
                        Piece[i][j] = PieceRecord[Turn][i][j];
                    }
                }
            }
        }
    } cd.ReverseMode += 2; return Success;
}
int Othello::Reverse(int x, int y) {
    ReverseSuccess = 0.0; xyr = (x - 3) * (x - 4) + (y - 3) * (y - 4);
    xyd = abs(x - y); xys = abs(x + y - 7);
    if (Board == BoardID::Nip && xyr >= 18) return 0;
    if (Piece[x][y] == PieceID::Space) {
        UpperReverse(x, y); LowerReverse(x, y); LeftReverse(x, y); RightReverse(x, y); UpperLeftReverse(x, y);
        UpperRightReverse(x, y); LowerLeftReverse(x, y); LowerRightReverse(x, y);
        if (Board == BoardID::Nip && xyr >= 12) { ClockwiseReverse(x, y); CounterClockwiseReverse(x, y); }
    } return (int)ReverseSuccess;
}
void Othello::ReversePiece(int x, int y) {
    Piece[x][y] = Player; if (cd.ReverseMode < 0) return; PieceNum[Player]++; PieceNum[1 - Player]--;
    if (cd.ReverseMode == ReverseModeID::Off) return; Game(); ScreenFlip(); WaitTimer(200); return;
}
void Othello::PutPiece(int x, int y) {
    if (ReverseSuccess > 0.0) return; Piece[x][y] = Player; if (cd.ReverseMode < 0) return;
    PieceNum[Player]++; PieceNum[PieceID::Space]--;
    if (cd.ReverseMode == ReverseModeID::Off) return; Game(); ScreenFlip(); WaitTimer(200); return;
}
int Othello::UpperReverse(int x, int y) {
    for (int Success = 0, TargetY = y - 1; Success < 7; Success++, TargetY--) {
        if (TargetY < 0) { if (Board != BoardID::Sphere) { return 0; } TargetY += 8; }
        if (Piece[x][TargetY] == PieceID::Space) return 0;
        if (Piece[x][TargetY] == Player) {
            if (!Success) { return 0; } PutPiece(x, y);
            for (int j = 0, TargetY = y - 1; j < Success; j++, TargetY--) {
                if (TargetY < 0) { TargetY += 8; } ReversePiece(x, TargetY);
            } ReverseSuccess += (double)Success; return Success;
        }
    } return 0;
}
int Othello::LowerReverse(int x, int y) {
    for (int Success = 0, TargetY = y + 1; Success < 7; Success++, TargetY++) {
        if (TargetY > 7) { if (Board != BoardID::Sphere) { return 0; } TargetY -= 8; }
        if (Piece[x][TargetY] == PieceID::Space) return 0;
        if (Piece[x][TargetY] == Player) {
            if (!Success) { return 0; } PutPiece(x, y);
            for (int j = 0, TargetY = y + 1; j < Success; j++, TargetY++) {
                if (TargetY > 7) { TargetY -= 8; } ReversePiece(x, TargetY);
            } ReverseSuccess += (double)Success; return Success;
        }
    } return 0;
}
int Othello::LeftReverse(int x, int y) {
    for (int Success = 0, TargetX = x - 1; Success < 7; Success++, TargetX--) {
        if (TargetX < 0) { if (Board != BoardID::Sphere) { return 0; } TargetX += 8; }
        if (Piece[TargetX][y] == PieceID::Space) return 0;
        if (Piece[TargetX][y] == Player) {
            if (!Success) { return 0; } PutPiece(x, y);
            for (int j = 0, TargetX = x - 1; j < Success; j++, TargetX--) {
                if (TargetX < 0) { TargetX += 8; } ReversePiece(TargetX, y);
            } ReverseSuccess += (double)Success; return Success;
        }
    } return 0;
}
int Othello::RightReverse(int x, int y) {
    for (int Success = 0, TargetX = x + 1; Success < 7; Success++, TargetX++) {
        if (TargetX > 7) { if (Board != BoardID::Sphere) { return 0; } TargetX -= 8; }
        if (Piece[TargetX][y] == PieceID::Space) return 0;
        if (Piece[TargetX][y] == Player) {
            if (!Success) { return 0; } PutPiece(x, y);
            for (int j = 0, TargetX = x + 1; j < Success; j++, TargetX++) {
                if (TargetX > 7) { TargetX -= 8; } ReversePiece(TargetX, y);
            } ReverseSuccess += (double)Success; return Success;
        }
    } return 0;
}
int Othello::UpperLeftReverse(int x, int y) {
    for (int Success = 0, TargetX = x - 1, TargetY = y - 1; Success < 7 - xyd; Success++, TargetX--, TargetY--) {
        if (TargetX < 0 || TargetY < 0) {
            if (Board != BoardID::Sphere) { return 0; }
            TargetX = TargetX + 8 - xyd; TargetY = TargetY + 8 - xyd;
        }
        if (Piece[TargetX][TargetY] == PieceID::Space) return 0;
        if (Piece[TargetX][TargetY] == Player) {
            if (!Success) { return 0; } PutPiece(x, y);
            for (int j = 0, TargetX = x - 1, TargetY = y - 1; j < Success; j++, TargetX--, TargetY--) {
                if (TargetX < 0 || TargetY < 0) { TargetX = TargetX + 8 - xyd; TargetY = TargetY + 8 - xyd; }
                ReversePiece(TargetX, TargetY);
            } ReverseSuccess += (double)Success; return Success;
        }
    } return 0;
}
int Othello::UpperRightReverse(int x, int y) {
    for (int Success = 0, TargetX = x + 1, TargetY = y - 1; Success < 7 - xys; Success++, TargetX++, TargetY--) {
        if (TargetX > 7 || TargetY < 0) {
            if (Board != BoardID::Sphere) { return 0; }
            TargetX = TargetX - 8 + xys; TargetY = TargetY + 8 - xys;
        }
        if (Piece[TargetX][TargetY] == PieceID::Space) return 0;
        if (Piece[TargetX][TargetY] == Player) {
            if (!Success) { return 0; } PutPiece(x, y);
            for (int j = 0, TargetX = x + 1, TargetY = y - 1; j < Success; j++, TargetX++, TargetY--) {
                if (TargetX > 7 || TargetY < 0) { TargetX = TargetX - 8 + xys; TargetY = TargetY + 8 - xys; }
                ReversePiece(TargetX, TargetY);
            } ReverseSuccess += (double)Success; return Success;
        }
    } return 0;
}
int Othello::LowerLeftReverse(int x, int y) {
    for (int Success = 0, TargetX = x - 1, TargetY = y + 1; Success < 7 - xys; Success++, TargetX--, TargetY++) {
        if (TargetX < 0 || TargetY > 7) {
            if (Board != BoardID::Sphere) { return 0; }
            TargetX = TargetX + 8 - xys; TargetY = TargetY - 8 + xys;
        }
        if (Piece[TargetX][TargetY] == PieceID::Space) return 0;
        if (Piece[TargetX][TargetY] == Player) {
            if (!Success) { return 0; } PutPiece(x, y);
            for (int j = 0, TargetX = x - 1, TargetY = y + 1; j < Success; j++, TargetX--, TargetY++) {
                if (TargetX < 0 || TargetY > 7) { TargetX = TargetX + 8 - xys; TargetY = TargetY - 8 + xys; }
                ReversePiece(TargetX, TargetY);
            } ReverseSuccess += (double)Success; return Success;
        }
    } return 0;
}
int Othello::LowerRightReverse(int x, int y) {
    for (int Success = 0, TargetX = x + 1, TargetY = y + 1; Success < 7 - xyd; Success++, TargetX++, TargetY++) {
        if (TargetX > 7 || TargetY > 7) {
            if (Board != BoardID::Sphere) { return 0; }
            TargetX = TargetX - 8 + xyd; TargetY = TargetY - 8 + xyd;
        }
        if (Piece[TargetX][TargetY] == PieceID::Space) return 0;
        if (Piece[TargetX][TargetY] == Player) {
            if (!Success) { return 0; } PutPiece(x, y);
            for (int j = 0, TargetX = x + 1, TargetY = y + 1; j < Success; j++, TargetX++, TargetY++) {
                if (TargetX > 7 || TargetY > 7) { TargetX = TargetX - 8 + xyd; TargetY = TargetY - 8 + xyd; }
                ReversePiece(TargetX, TargetY);
            } ReverseSuccess += (double)Success; return Success;
        }
    } return 0;
}
int Othello::ClockwiseReverse(int x, int y) {
    for (int Success = 0, TargetX = x, TargetY = y; Success < 19; Success++) {
        if (!TargetX && TargetY > 2) TargetY--;
        else if (TargetX < 2 && TargetY < 3) { TargetX++; TargetY--; }
        else if (TargetX < 5 && !TargetY) TargetX++;
        else if (TargetX > 4 && TargetY < 2) { TargetX++; TargetY++; }
        else if (TargetX == 7 && TargetY < 5) TargetY++;
        else if (TargetX > 5 && TargetY > 4) { TargetX--; TargetY++; }
        else if (TargetX > 2 && TargetY == 7) TargetX--;
        else if (TargetX < 3 && TargetY > 5) { TargetX--; TargetY--; }
        if (Piece[TargetX][TargetY] == PieceID::Space) return 0;
        if (Piece[TargetX][TargetY] == Player) {
            if (!Success) { return 0; } PutPiece(x, y);
            for (int j = 0, TargetX = x, TargetY = y; j < Success; j++) {
                if (!TargetX && TargetY > 2) { TargetY--; }
                else if (TargetX < 2 && TargetY < 3) { TargetX++; TargetY--; }
                else if (TargetX < 5 && !TargetY) { TargetX++; }
                else if (TargetX > 4 && TargetY < 2) { TargetX++; TargetY++; }
                else if (TargetX == 7 && TargetY < 5) { TargetY++; }
                else if (TargetX > 5 && TargetY > 4) { TargetX--; TargetY++; }
                else if (TargetX > 2 && TargetY == 7) { TargetX--; }
                else if (TargetX < 3 && TargetY > 5) { TargetX--; TargetY--; }
                ReversePiece(TargetX, TargetY);
            } ReverseSuccess += (double)Success; return Success;
        }
    } return 0;
}
int Othello::CounterClockwiseReverse(int x, int y) {
    for (int Success = 0, TargetX = x, TargetY = y; Success < 19; Success++) {
        if (!TargetX && TargetY < 5) TargetY++;
        else if (TargetX < 2 && TargetY > 4) { TargetX++; TargetY++; }
        else if (TargetX < 5 && TargetY == 7) TargetX++;
        else if (TargetX > 4 && TargetY > 5) { TargetX++; TargetY--; }
        else if (TargetX == 7 && TargetY > 2) TargetY--;
        else if (TargetX > 5 && TargetY < 3) { TargetX--; TargetY--; }
        else if (TargetX > 2 && !TargetY) TargetX--;
        else if (TargetX < 3 && TargetY < 2) { TargetX--; TargetY++; }
        if (Piece[TargetX][TargetY] == PieceID::Space) return 0;
        if (Piece[TargetX][TargetY] == Player) {
            if (!Success) { return 0; } PutPiece(x, y);
            for (int j = 0, TargetX = x, TargetY = y; j < Success; j++) {
                if (!TargetX && TargetY < 5) { TargetY++; }
                else if (TargetX < 2 && TargetY > 4) { TargetX++; TargetY++; }
                else if (TargetX < 5 && TargetY == 7) { TargetX++; }
                else if (TargetX > 4 && TargetY > 5) { TargetX++; TargetY--; }
                else if (TargetX == 7 && TargetY > 2) { TargetY--; }
                else if (TargetX > 5 && TargetY < 3) { TargetX--; TargetY--; }
                else if (TargetX > 2 && !TargetY) { TargetX--; }
                else if (TargetX < 3 && TargetY < 2) { TargetX--; TargetY++; }
                ReversePiece(TargetX, TargetY);
            } ReverseSuccess += (double)Success; return Success;
        }
    } return 0;
}
Othello::~Othello() {
    CloseNetWork(NetHandle); 
    FILE* ocdf = fopen(ConfigDataFileName, "wb+"); 
    fwrite(&cd, sizeof(cd), 1, ocdf); 
    fclose(ocdf);
    PlaySound(NULL, 0, 0); 
    InitSoundMem(); 
    DxLib_End();
}

