#pragma once
// Connect Mode No
#define ConnectRQ            0
#define WaitConnect          1
#define Disconnect           2
#define Succeed              3
#define ConnectModeNum       4
// User Status No
#define Host                 0
#define Guest                1
#define Offline              2
#define UserStatusNum        3

#define SENum                8
#define MinSoundVol          0
#define MaxSoundVol        200
// Control No
#define KeyNum              67
#define MaxKeyNum          212
// TwinKey No
#define SINGLE               0
#define SHIFT                1  
#define CTRL                 2  
#define ALT                  3
#include <iostream>
using namespace std;
struct SceneID {
    static constexpr int Home        = 0;
    static constexpr int BoardSelect = 1;
    static constexpr int CharSelect  = 2;
    static constexpr int GameStart   = 3;
    static constexpr int Game        = 4;
    static constexpr int Pause       = 5;
    static constexpr int Num         = 6;
};
struct ConfigID {
    static constexpr int Home        = -6;
    static constexpr int BoardSelect = -5;
    static constexpr int CharSelect  = -4;
    static constexpr int GameStart   = -3;
    static constexpr int Game        = -2;
    static constexpr int Pause       = -1;
    static constexpr int Num         =  6;
};
struct CtrlID {
    static constexpr int Guide   =  0;
    static constexpr int Reverse =  1;
    static constexpr int Home    =  2; 
    static constexpr int Restart =  3;
    static constexpr int Init    =  4; 
    static constexpr int Skip    =  5;
    static constexpr int Exit    =  6; 
    static constexpr int Config  =  7;
    static constexpr int ChMusic =  8; 
    static constexpr int Window  =  9;
    static constexpr int Sound   = 10; 
    static constexpr int Pause   = 11;
    static constexpr int Start   = 12; 
    static constexpr int Yes     = 13;
    static constexpr int No      = 14; 
    static constexpr int Back    = 15;
    static constexpr int Forward = 16; 
    static constexpr int Down    = 17;
    static constexpr int Up      = 18; 
    static constexpr int Num     = 19;
};

class Othello {
public:
    Othello();
    ~Othello();
    void BackSystem();
    int DrawScene();
    int HitRoleKey(int KeyRole);
    int ClickBox(int x1, int y1, int x2, int y2);
private:
    int Simulation();
    void AIput(); 
    int Reverse(int x, int y);
    int UpperReverse(int x, int y);
    int LowerReverse(int x, int y);
    int LeftReverse(int x, int y);
    int RightReverse(int x, int y);
    int UpperLeftReverse(int x, int y);
    int UpperRightReverse(int x, int y);
    int LowerLeftReverse(int x, int y);
    int LowerRightReverse(int x, int y);
    int ClockwiseReverse(int x, int y);
    int CounterClockwiseReverse(int x, int y);
    void ChangeWindow(int WindowModeFlag);
    void ChangeKey(int x1, int y1, int x2, int y2, int KeyRole);
    int HitKey(int keyCode);
    void ChangeSound(int SoundModeFlag);
    void ChangeSoundVol(int SoundNo, int Vol);
    void ChangeBGM(int TargetBGM);
    // Communication
    char Signal[256];
    IPDATA ip = { 192, 168, 13, 98 };
    int NetHandle, LostHandle, DataLength, ConnectMode, UserStatus;
    const string UserStatusName[UserStatusNum] = { "Host", "Guest", "Offline" };
    // Image
    struct IconID {
        static constexpr int Guide          =  0;
        static constexpr int Reverse        =  1;
        static constexpr int Home           =  2; 
        static constexpr int Restart        =  3;
        static constexpr int Init           =  4; 
        static constexpr int Skip           =  5;
        static constexpr int Exit           =  6; 
        static constexpr int Config         =  7;
        static constexpr int ChMusic        =  8; 
        static constexpr int AnotherWindow  =  9;
        static constexpr int MaximizeWindow = 10;
        static constexpr int Mute           = 11;
        static constexpr int Unmute         = 12;
        static constexpr int Pause          = 13;
        static constexpr int Start          = 14; 
        static constexpr int Back           = 15;
        static constexpr int Forward        = 16; 
        static constexpr int Down           = 17;
        static constexpr int Up             = 18; 
        static constexpr int Selected       = 19;
        static constexpr int Cursor         = 20;
        static constexpr int Num            = 21;
    };
    int Icon[IconID::Num];
    // Control No
    const string CtrlName[10] = { "Guide", "Reverse", "Title", "Restart", "Initialize", "           Skip",
                            "Quit Game", "Config", "Change BGM", "Change Window" };
    // Scene
    int Scene; const string SceneName[3] = {"Title", "Board Select", "Char Select"};
    // BGM
    struct BGMID {
        static constexpr int Chocolate  = 0;
        static constexpr int DeadAlive  = 1;
        static constexpr int Detect     = 2;
        static constexpr int Hometown   = 3;
        static constexpr int Legend     = 4;
        static constexpr int NothingDie = 5;
        static constexpr int Sorrow     = 6;
        static constexpr int Victory    = 7;
        static constexpr int Num        = 8;
    };
    int BGM[BGMID::Num], PlayingBGM, BGMVol;
    const string BGMName[BGMID::Num] = { "Chocolate", "Dead or Alive", "Detect", "Hometown", "Memory of Legend",
                               "Nothing dies in my mind", "Sorrow", "Victory belongs to You" };
    struct BGMPlayModeID {
        static constexpr int Loop    = 0;
        static constexpr int Ascend  = 1;
        static constexpr int Descend = 2;
        static constexpr int Random  = 3;
        static constexpr int Num     = 4;
    };
    const string BGMPlayModeName[BGMPlayModeID::Num] = { "Loop", "Ascending order", "Descending order", "Random" };
    // SE
    int SE[SENum], SEVol;
    struct MuteModeID {
        static constexpr int Mute   = 0;
        static constexpr int Unmute = 1;
        static constexpr int Num    = 2;
    };
    int MuteMode; const string MuteModeName[MuteModeID::Num] = { "Mute", "Unmute" };
    struct SoundID {
        static constexpr int Total = 0;
        static constexpr int BGM   = 1;
        static constexpr int SE    = 2;
        static constexpr int Num   = 3;
    };
    const string SoundName[SoundID::Num] = { "Volume", " BGM", " SE" };
    // Window
    int WindowMode;
    // Character
    struct CharID {
        static constexpr int John    = 0;
        static constexpr int Komachi = 1;
        static constexpr int Misuzu  = 2;
        static constexpr int Michel  = 3;
        static constexpr int Yoichi  = 4;
        static constexpr int Num     = 5;
    };
    int CharImage[CharID::Num]; 
    const string CharName[CharID::Num] = { "  John", "Komachi", " Misuzu", " Michel", " Yoichi" };
    struct OrderID {
        static constexpr int First1P = 0;
        static constexpr int First2P = 1;
        static constexpr int Random  = 2;
        static constexpr int Num     = 3;
    };
    const string OrderName[OrderID::Num] = { "1P First", "2P First", "Random" };
    // Board
    struct BoardID {
        static constexpr int Default = -7;
        static constexpr int Skip    = -4;
        static constexpr int Othello =  0;
        static constexpr int Nip     =  1;
        static constexpr int Sphere  =  2;
        static constexpr int Num     =  3;
    };
    int Board; const string BoardName[BoardID::Num] = { "Othello", "Nip", "Sphere" };
    // Progress of Game
    int PauseMode; const string PauseName[2] = { "Pause", "Continue" };
    const string Alphabet[8] = { "A", "B", "C", "D", "E", "F", "G", "H" };
    // Player ID Black:0 White:1 Space:2 Attacker:Player
    struct PlayerID {
        static constexpr int Black = 0;
        static constexpr int White = 1;
        static constexpr int Num   = 2;
    };
    int Player; 
    struct PieceID {
        static constexpr int Black = 0;
        static constexpr int White = 1;
        static constexpr int Space = 2;
        static constexpr int Num   = 3;
    };
    // the number of turn
    int Turn, MaxTurn; 
    // Speed of AI
    int AITimeCount;
    // Time in game
    time_t StartTime, LapseTime; long int LapseHour, LapseMin, LapseSec;
    // Key
    const int KeyNo[KeyNum] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 28, 29, 30, 31, 32,
                                33, 34, 35, 36, 37, 38, 42, 44, 45, 46, 47, 48, 49, 50, 54, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66,
                                67, 68, 87, 88, 157, 184, 197, 200, 203, 205, 208, 210, 211 };
    int KeyImage[MaxKeyNum];
    // X-Y coordinate into which coordinate name is converted in order of X:Å®  Y:Å´ 
    int CursorX, CursorY;
    // xyr: distance between center & (x,y), xyd, xys: distance between diagonal line & (x,y) 
    int xyr, xyd, xys; 
    // Show Reverse
    struct ReverseModeID {
        static constexpr int Off = 0;
        static constexpr int On  = 1;
        static constexpr int Num = 2;
    };
    const string ReverseModeName[ReverseModeID::Num] = { "Off", "On" };
    // Guide
    struct GuideLvID {
        static constexpr int Off    = 0;
        static constexpr int Simple = 1;
        static constexpr int Kind   = 2;
        static constexpr int Debag  = 3;
        static constexpr int Num    = 4;
    };
    const string GuideLvName[GuideLvID::Num] = { "Off", "Simple", "Kind"};
    // the number of time where Players can't successively put their pieces
    int FailureSuccession; bool GameEnd; double ReverseSuccess, AISuccess[8][8];
    // Piece[x][y] = (Player ID of the piece on the Place)
    int Piece[8][8], PutPlayer[8][8];
    // PieceRecord[Turn][x][y] = (piece[x][y] in the turn)
    int PieceRecord[200][8][8];
    // JudgePiece[x][y] = (the number of pieces the piece on the Place can reverse)
    int JudgePiece[8][8];
    int PieceNum[PieceID::Num]; float Propotion[PlayerID::Num];
    // Color
    unsigned int TitleColor, FrameColor, PieceColor[PieceID::Num], GuideColor[PieceID::Num];
    // Font
    int NormalFont, TitleFont, CharNameFont;
    struct CharSelectData {
        int CharSelectNo; 
        int MaxCharSelectNo; 
        int CharNo[PlayerID::Num]; 
        int CharLv[PlayerID::Num];
        int Order;
        string Name[PlayerID::Num];  // PlayerName[PlayerNo]
        int Lv[PlayerID::Num];       // PlayerLv[PlayerNo]
        int Image[PlayerID::Num];    // PlayerImage[PlayerNo]
    }; CharSelectData csd;
    struct ConfigData {
        int SoundVol[SoundID::Num];      // SoundVol[SoundNo] SoundNo Total:0 BGM:1 SE:2
        int BGMPlayMode;             // BGMStyle Loop:0 Ascending order:1 Decending order:2 Random:3 
        int Key[CtrlID::Num];        // Key[KeyRole]
        int TwinKey[CtrlID::Num];    // TwinKey[KeyRole]
        int SceneBGM[3];             // SceneBGM[SceneNo]
        int BoardBGM[BoardID::Num];  // SceneBGM[SceneNo]
        int MaxCharLv[BoardID::Num]; // MaxCharLv[Board]
        int GuideLv;
        int ReverseMode;             // Show Reverse
        int AITime;                  // Speed of AI
        double PieceWorth[BoardID::Num][8][8];
    }; ConfigData cd;
    const char* ConfigDataFileName = "ConfigData.dat";
    // Function of each part
    void UpdateScene(int TargetScene);
    void Title();
    void BoardSelect();
    void CharSelect();
    void GameStart();
    void Game();
    void PutPiece(int x, int y);
    void ReversePiece(int x, int y);
    void Config();
    void InitScene();
    void CallBoard();
    int DrawTime(int x, int y, unsigned int Color, int FontHandle);
    int DrawCursor();
};
