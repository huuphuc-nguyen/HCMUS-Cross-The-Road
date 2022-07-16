#pragma comment (lib, "winmm.lib")
#include <iostream>
#include <conio.h>
#include <thread>
#include <Windows.h>
#include <fstream>
#include <string>
#include <cstdlib>
#include <mmsystem.h>


using namespace std;

void Nocursortype()
{
    CONSOLE_CURSOR_INFO Info;
    Info.bVisible = FALSE;
    Info.dwSize = 20;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &Info);
}

void UnNocursortype()
{
    CONSOLE_CURSOR_INFO Info;
    Info.bVisible = TRUE;
    Info.dwSize = 20;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &Info);
}

void ResizeConsole(int width, int height)
{
    HWND console = GetConsoleWindow();
    RECT r;
    GetWindowRect(console, &r);
    MoveWindow(console, r.left, r.top, width, height, TRUE);
}

void ReposConsole()
{
    HWND consoleWindow = GetConsoleWindow();
    SetWindowPos(consoleWindow, 0, 200, 0, 1000, 750, SWP_NOSIZE | SWP_NOZORDER);
}

void FixConsoleWindow()
{
    HWND consoleWindow = GetConsoleWindow();
    LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
    style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
    SetWindowLong(consoleWindow, GWL_STYLE, style);
}

void GotoXY(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void SetColor(int SetColor)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), SetColor);
}

#define MAX_CAR 12
#define MAX_CAR_RIGHT_LENGTH 18
#define MAX_CAR_LEFT_LENGTH 15
#define MAX_AMBULANCE 16
#define MAX_SPEED 3
#define CAR_HEIGH 4

struct People
{
    POINT coordinate;
    char shape[4][4] = {
    " o ",
    "\\|/",
    " | ",
    "/ \\",
    };
};

struct CarLeft
{
    POINT coordinate;
    char shape[4][15] = {
    "  ________",
    " /|_ || _ \\__",
    "(____________\\",
    "  '(_)  (_)'",
    };
};

struct CarRight
{
    POINT coordinate;
    char shape[4][18] = {
    "     ____________",
    " ___/__)(__)(__)|",
    "(o  _| -|   _ o |",
    " '-(_)-----(_)--'",
    };
};

struct Ambulance
{
    POINT coordinate;
    char shape[4][16] = {
        " ________ ",
        "|__|__|__\\_\\_",
        "|    (+)     \\<",
        "'-(_)---(_)--'",
    };
};

int cnt = 0; //Biến hỗ trợ trong quá trình tăng tốc độ xe di chuyển
int MOVING;
int SPEED;
int HEIGH_CONSOLE = 40, WIDTH_CONSOLE = 91;
bool STATE;
POINT FinishPeople_Coordinate[MAX_SPEED];
int FinishPeople_Num;
bool flag0 = 0, flag2 = 0;
bool IsMoving_Car[6] = { 1,1,1,1,1,1 };

People Y; // lấy cái đầu hd[0][1] làm gốc tọa độ
CarLeft arr_cl[6];
CarRight arr_cr[6];


void ResetData();

void DrawBoard(int x, int y, int width, int height, int curPosX, int curPosY);

void DrawSubBoard();

void DrawTeamName();

void DrawGameTitle();

void Win();

void Lose();

void DrawAmbulance();

void LoadData();

void DrawFinishPeople();

void StartGame();

void ContinueGame();

void ExitGame(HANDLE t);

void PauseGame(HANDLE t);

void ProcessDead();

void ProcessFinish(People& Y);

void ProcessWin();

void DrawCars();

void DrawSticker(const People& Y);

void EraseSticker(const People& Y);

bool IsImpactvsP(const People& p);

bool IsImpactvsC(const People& p);

void MoveCars();

void EraseCars();

void MoveRight();

void MoveLeft();

void MoveUp();

void MoveDown();

void FileSave();

void DeadEffect();

void SubThread();

int Menu();

void ResetData()
{
    bool IsMoving_Car[6] = { 1,1,1,1,1,1 };
    for (int i = 0; i < MAX_SPEED; i++) FinishPeople_Coordinate[i] = { 0, 0 };
    FinishPeople_Num = 0;
    MOVING = 'D';
    SPEED = 1;
    Y.coordinate.x = 44; Y.coordinate.y = 36;

    for (int i = 0; i < 3; i++)
    {
        int temp_l = (rand() % (WIDTH_CONSOLE - MAX_CAR_LEFT_LENGTH - 1)) + 1;
        arr_cl[i].coordinate.x = temp_l;
        arr_cl[i].coordinate.y = 6 + i * 10;
        arr_cl[i + 3].coordinate.x = arr_cl[i].coordinate.x + 35;
        arr_cl[i + 3].coordinate.y = arr_cl[i].coordinate.y;
        if (arr_cl[i + 3].coordinate.x > 76)  arr_cl[i + 3].coordinate.x -= 76;
    }

    for (int i = 0; i < 3; i++)
    {
        int temp_r = (rand() % (WIDTH_CONSOLE - MAX_CAR_RIGHT_LENGTH)) + 1; //1 - > 74/ vì 1 vào hàm sẽ ra 0 rồi qua 74
        arr_cr[i].coordinate.x = temp_r;
        arr_cr[i].coordinate.y = 11 + i * 10;
        arr_cr[i + 3].coordinate.x = arr_cr[i].coordinate.x + 35;
        arr_cr[i + 3].coordinate.y = arr_cr[i].coordinate.y;
        if (arr_cr[i + 3].coordinate.x > 74)  arr_cr[i + 3].coordinate.x -= 74;
    }
}
// gán moving = 'D'  để vẽ 'Y' ra màn hình.

void DrawBoard(int x, int y, int width, int height, int curPosX = 0, int curPosY = 0)
{
    SetColor(11);
    GotoXY(x, y); cout << (char)219;
    for (int i = 1; i < width; i++) cout << (char)219;
    cout << (char)219;
    GotoXY(x, height + y); cout << (char)219;
    for (int i = 1; i < width; i++) cout << (char)219;
    cout << (char)219;
    for (int i = y + 1; i < height + y; i++)
    {
        GotoXY(x, i); cout << (char)219;
        GotoXY(x + width, i); cout << (char)219;
    }
    GotoXY(curPosX, curPosY);
    SetColor(7);
}

void DrawBoardIntro(int x, int y, int width, int height, int curPosX = 0, int curPosY = 0)
{
    SetColor(11);
    for (int i = 0; i <= width; i++)
    {
        GotoXY(i, y);
        cout << (char)219;
        GotoXY(width - i, height + y);
        cout << (char)219;
        Sleep(5);
    }
    for (int i = y + 1; i < height + y; i++)
    {
        GotoXY(x, height - i); cout << (char)219;
        GotoXY(x + width, i); cout << (char)219;
        Sleep(10);
    }
    GotoXY(curPosX, curPosY);
    SetColor(7);
}

void DrawSubBoard()
{
    int x = 5;
    while (x < HEIGH_CONSOLE)
    {
        GotoXY(1, x);
        for (int i = 1; i < WIDTH_CONSOLE; i++) cout << "-";
        x += 5;
    }
    DrawBoard(96, 15, 20, 14);
    GotoXY(98, 17); cout << "W : Up";
    GotoXY(98, 19); cout << "A : Left";
    GotoXY(98, 21); cout << "S : Down";
    GotoXY(98, 23); cout << "D : Right";
    GotoXY(98, 25); cout << "L : Save Game";
    SetColor(14);
    GotoXY(98, 27); cout << "Level: " << SPEED << " over " << 3;
    SetColor(7);
}

void DrawTeamName()
{
    PlaySound(TEXT("Intro.wav"), NULL, SND_FILENAME | SND_ASYNC);
    Nocursortype();
    Sleep(1000);
    SetColor(14);
    UnNocursortype();
    GotoXY(15, 5); cout << "____o__ __o____   o__ __o__/_          o           o          o          __o         __o" << endl; Sleep(200);
    GotoXY(15, 6); cout << " /   \\   /   \\   <|    v              <|>         <|\\        /|>       o/  v\\      o/  v\\" << endl; Sleep(200);
    GotoXY(15, 7); cout << "      \\o/        < >                  / \\         / \\\\o    o// \\      /|    <\\    /|    <\\" << endl; Sleep(200);
    GotoXY(15, 8); cout << "       |          |                 o/   \\o       \\o/ v\\  /v \\o/      //    o/    //    o/" << endl; Sleep(200);
    GotoXY(15, 9); cout << "      < >         o__/_            <|__ __|>       |   <\/>   |            /v          /v" << endl; Sleep(200);
    GotoXY(15, 10); cout << "       |          |                /       \\      / \\        / \\          />          />" << endl; Sleep(200);
    GotoXY(15, 11); cout << "       o         <o>             o/         \\o    \\o/        \\o/        o/          o/" << endl; Sleep(200);
    GotoXY(15, 12); cout << "      <|          |             /v           v\\    |          |        /v          /v" << endl; Sleep(200);
    GotoXY(15, 13); cout << "      / \\        / \\  _\\o__/_  />             <\\  / \\        / \\      /> __o__/_  /> __o__/_" << endl; Sleep(200);

    Sleep(500);
    SetColor(12);
    GotoXY(45, 20); cout << "Made by:" << endl; Sleep(500);
    SetColor(11);
    GotoXY(50, 22); cout << "NGUYEN HUU PHUC" << endl; Sleep(300);
    GotoXY(50, 24); cout << "MAI VINH HIEN" << endl; Sleep(300);
    GotoXY(50, 26); cout << "NGUYEN QUANG HIEN" << endl; Sleep(300);
    GotoXY(50, 28); cout << "LE HOANG HUY" << endl; Sleep(300);
    GotoXY(50, 30); cout << "DUONG DINH BAO HOANG" << endl; Sleep(300);
    SetColor(12);
    GotoXY(45, 32); cout << "Instructor:" << endl; Sleep(500);
    SetColor(11);
    GotoXY(50, 34); cout << "MR.TRUONG TOAN THINH" << endl; Sleep(300);
    Nocursortype();
    SetColor(7);
    Sleep(5500);
}

void DrawGameTitle()
{
    Nocursortype();
    SetColor(7);
    GotoXY(27, 2); cout << "  ______                       _   _             ____                 _" << endl;
    GotoXY(27, 3); cout << " /  ____|_ __ ___  ____ ____  | |_| |__   ____  |  _ \\ ___   __ _  __| |" << endl;
    GotoXY(27, 4); cout << "|  |    | '__/ _ \\/ ___/ ___| | __| '_ \\ / __ \\ | |_) / _ \\ / _' |/ _' |" << endl;
    GotoXY(27, 5); cout << "|  |____| | | (_) \\___ \\___ \\ | |_| | | |  ___/ |  _ < (_) | (_| | (_| |" << endl;
    GotoXY(27, 6); cout << " \\______|_|  \\___/|____/____/  \\__|_| |_|\\____| |_| \\_\\___/ \\__'_|\\__'_|";
}

void Win()
{
    DrawBoard(20, 10, 80, 4);
    GotoXY(40, 12); cout << "Win, type y to continue or ESC to exit\n";
}

void Lose()
{
    DrawBoard(20, 10, 80, 4);
    GotoXY(40, 12); cout << "Dead, type Y to continue, ESC to exit\n";
}

void DrawAmbulance()
{
    int color = 0;
    flag0 = 1; // cho xe cứu thương chạy
    STATE = 0;
    system("cls");
    DrawBoard(0, 0, WIDTH_CONSOLE, HEIGH_CONSOLE);
    GotoXY(Y.coordinate.x, Y.coordinate.y);
    SetColor(12);
    DrawSticker(Y);
    SetColor(7);
    // vẽ xe
    Ambulance abl;
    abl.coordinate.x = 1; abl.coordinate.y = Y.coordinate.y;
    abl.shape[0][10] = 219;

    while (abl.coordinate.x <= 75)
    {
        if (flag0 == 1)
        {
            for (int j = 0; j < 4; j++)
            {
                GotoXY(abl.coordinate.x, abl.coordinate.y + j);
                cout << " ";
            }
        }
        else {
            system("Cls");
            return;
        }
        if (flag0 == 1)
        {
            abl.coordinate.x++;
        }
        else {
            system("Cls");
            return;
        }
        if (flag0 == 1)
        {
            int x = abl.coordinate.x;
            int y = abl.coordinate.y + 1;
            color++;
            GotoXY(x, y - 1);
            for (int i = 0; i <= 10; i++) cout << abl.shape[0][i];

            if (color % 8 <= 3)
            {
                SetColor(12);
                GotoXY(x + 10, y - 1);
                cout << abl.shape[0][10];
            }
            SetColor(7);
            for (int j = 1; j < 4; j++)
            {
                GotoXY(x, y);
                cout << abl.shape[j];
                y++;
            }
        }
        else {
            system("Cls");
            return;
        }
        if (flag0 == 1)
        {
            if (abl.coordinate.x == 76)
            {
                for (int j = 0; j < 4; j++)
                {
                    GotoXY(abl.coordinate.x, abl.coordinate.y + j);
                    cout << "               ";
                }
                flag2 = 1;
                return;
            }
        }
        else {
            system("Cls");
            return;
        }
        Sleep(25);
    }
}

void LoadData()
{
    system("cls");
    ////////////////////////// mở file lưu tên ////////////////////////
    string name;
    int sofile = 0;
    ifstream fi("CacFileDaLuu.txt");
    while (!fi.eof())
    {
        getline(fi, name);

        if (name != "")
        {
            sofile++;
            GotoXY(55, 9 + sofile);
            cout << sofile << ". " << name << endl;
        }
    }
    DrawBoard(20, 7, 80, sofile + 4);
    GotoXY(52, 9); cout << "FILES HAS BEEN SAVED";
    fi.close();
    //////////////////////////////////////////////////////////////////

    ///////////////////////// mở file lưu game //////////////////////
    UnNocursortype();
    string tentracuu;
    DrawBoard(20, 1, 80, 4);
    GotoXY(25, 3); cout << "Enter your name: ";
    getline(cin, tentracuu);
    /////////////////////////////////////////////////////////////////
    system("cls");
    ifstream fin(tentracuu + ".txt");
    if (fin.fail() == true) ///////////// tồn tại file thì cho về menu
    {
        system("cls");
        DrawBoard(20, 10, 80, 4);
        GotoXY(50, 12); cout << "File doesn't exist ! ";
        GotoXY(0, 0);
        Sleep(500);
        Menu();
    }

    string bochu;
    getline(fin, bochu, ':');
    for (int i = 0; i < 6; i++)fin >> IsMoving_Car[i];

    getline(fin, bochu, ':');
    for (int i = 0; i < MAX_SPEED; i++) fin >> FinishPeople_Coordinate[i].x >> FinishPeople_Coordinate[i].y;

    getline(fin, bochu, ':');
    fin >> FinishPeople_Num;

    getline(fin, bochu, ':');
    fin >> SPEED;

    getline(fin, bochu, ':');
    fin >> Y.coordinate.x >> Y.coordinate.y;


    getline(fin, bochu, ':');
    for (int i = 0; i < 6; i++)
        fin >> arr_cl[i].coordinate.x >> arr_cl[i].coordinate.y;

    getline(fin, bochu, ':');
    for (int i = 0; i < 6; i++)
        fin >> arr_cr[i].coordinate.x >> arr_cr[i].coordinate.y;

    fin.close();

    MOVING = 'D';
    Nocursortype();
}

void DrawFinishPeople()
{
    for (int i = 0; i < MAX_SPEED; i++)
    {
        if (FinishPeople_Coordinate[i].x != 0 && FinishPeople_Coordinate[i].y != 0)
        {
            GotoXY(FinishPeople_Coordinate[i].x, FinishPeople_Coordinate[i].y);
            People temp;  // tạo temp để tránh nhầm lẫn với Y
            temp.coordinate.x = FinishPeople_Coordinate[i].x;
            temp.coordinate.y = FinishPeople_Coordinate[i].y;
            DrawSticker(temp);
        }
    }
}

void StartGame()
{
    system("cls");
    ResetData();
    DrawBoardIntro(0, 0, WIDTH_CONSOLE, HEIGH_CONSOLE);
    DrawSubBoard();
    STATE = true;
}
// Vẽ khung , speed = 1, tạo con Y ban đầu, di chuyển phải -> vẽ Y ra

void ContinueGame()
{
    system("cls");
    LoadData();
    DrawBoardIntro(0, 0, WIDTH_CONSOLE, HEIGH_CONSOLE);
    DrawSubBoard();
    DrawFinishPeople();
    STATE = true;//Bắt đầu cho Thread chạy
}

void ExitGame(HANDLE t)
{
    system("cls");
    TerminateThread(t, 0);
}

void PauseGame(HANDLE t)
{
    SuspendThread(t);
}

void ProcessDead()
{
    if (flag2 == 1) {
        STATE = 0;
        system("cls");
        Lose();
    }
}

void ProcessFinish(People& Y)
{
    SPEED == MAX_SPEED ? SPEED = 1 : SPEED++;
    Y.coordinate.x = 44; Y.coordinate.y = 36;
    MOVING = 'D'; //Ban đầu cho người di chuyển sang phải
    DrawSubBoard();
}

void ProcessWin()
{
    STATE = 0;
    system("cls");
    Win();
}

void DrawCarLeft(const CarLeft& car)
{
    int x = car.coordinate.x;
    int y = car.coordinate.y;
    for (int j = 0; j < 4; j++)
    {
        GotoXY(x, y);
        cout << car.shape[j];
        GotoXY(0, 0);
        y++;
    }
}

void DrawCarRight(const CarRight& car)
{
    int x = car.coordinate.x;
    int y = car.coordinate.y;
    for (int j = 0; j < 4; j++)
    {
        GotoXY(x, y);
        cout << car.shape[j];
        GotoXY(0, 0);
        y++;
    }
}

void DrawCars()
{
    srand(time(NULL));
    for (int i = 0; i < 3; i++)
    {
        int r = rand() % 100 + 1;
        if (r >= 100 - SPEED * 10)
            IsMoving_Car[i] = 0;

        DrawCarLeft(arr_cl[i]);
        DrawCarLeft(arr_cl[i + 3]);
    }

    for (int i = 0; i < 3; i++)
    {
        int r = rand() % 100 + 1;
        if (r >= 100 - SPEED * 10)
            IsMoving_Car[i + 3] = 0;

        DrawCarRight(arr_cr[i]);
        DrawCarRight(arr_cr[i + 3]);
    }
}

void DrawSticker(const People& Y)
{
    for (int kDong = 0; kDong <= 3; kDong++)
    {
        for (int kCot = -1; kCot <= 2; kCot++)
        {
            int x = kCot + Y.coordinate.x;
            int y = kDong + Y.coordinate.y;

            GotoXY(x, y);
            cout << Y.shape[kDong][kCot + 1] << endl;
            GotoXY(0, 0);
        }
    }
} // lấy đầu

void EraseSticker(const People& Y)
{
    for (int kDong = 0; kDong <= 3; kDong++)
    {
        for (int kCot = -1; kCot <= 1; kCot++)
        {
            int x = kCot + Y.coordinate.x;
            int y = kDong + Y.coordinate.y;

            GotoXY(x, y);
            cout << " " << endl;
        }
    }
}

bool IsImpactvsP(const People& Y)
{
    for (int i = 0; i < 3; i++)
    {
        if (Y.coordinate.x == FinishPeople_Coordinate[i].x && Y.coordinate.y == FinishPeople_Coordinate[i].y) return true;
    }
    return false;
}

bool IsImpactvsC(const People& Y) {
    if (Y.coordinate.y == 1 || Y.coordinate.y == 39) return false;
    for (int kDong = 0; kDong <= 3; kDong++)
    {
        for (int kCot = -1; kCot <= 2; kCot++)
        {
            int x = kCot + Y.coordinate.x;
            int y = kDong + Y.coordinate.y;

            // xe trai
            for (int i = 0; i < 6; i++)
            {
                for (int k = 0; k < 15; k++) // vcham hang 1 cua xe
                {
                    if (arr_cl[i].coordinate.x + k == x && arr_cl[i].coordinate.y == y) return true;
                }

                for (int k = 0; k < 15; k++) // vcham hang 4 cua xe
                {
                    if (arr_cl[i].coordinate.x + k == x && arr_cl[i].coordinate.y + 3 == y) return true;
                }
            }

            /// xe phai
            for (int i = 0; i < 6; i++)
            {
                for (int k = 0; k < 18; k++) // vcham hang 1 cua xe
                {
                    if (arr_cr[i].coordinate.x + k == x && arr_cr[i].coordinate.y == y) return true;
                }

                for (int k = 0; k < 18; k++) // vcham hang 4 cua xe
                {
                    if (arr_cr[i].coordinate.x + k == x && arr_cr[i].coordinate.y + 3 == y) return true;
                }
            }
        }
    }
    return false;
}

void MoveCarLeft(CarLeft& car)
{
    car.coordinate.x++;
    if (car.coordinate.x == 78)
    {
        car.coordinate.x = 1;
    }
}

void MoveCarRight(CarRight& car)
{
    car.coordinate.x--;
    if (car.coordinate.x == 0)
    {
        car.coordinate.x = 74;
    }
}

void MoveCars()
{
    for (int i = 0; i < 3; i++)
    {
        if (IsMoving_Car[i] != 0)
        {
            cnt = 0;
            do {
                cnt++;
                MoveCarLeft(arr_cl[i]);
                MoveCarLeft(arr_cl[i + 3]);
            } while (cnt < SPEED);
        }
    }

    for (int i = 0; i < 3; i++)
    {
        if (IsMoving_Car[i + 3] != 0)
        {
            cnt = 0;
            do {
                cnt++;
                MoveCarRight(arr_cr[i]);
                MoveCarRight(arr_cr[i + 3]);
            } while (cnt < SPEED);
        }
    }
}



void EraseCarLeft(const CarLeft& car)
{
    for (int j = 0; j < 4; j++)
    {
        GotoXY(car.coordinate.x, car.coordinate.y + j);

        for (int k = 0; k <= cnt; k++)
            cout << " ";

        if (cnt == 0)
        {
            if (car.coordinate.x == 1) // dáng lẽ 78 nhưng nếu là 77, qua hàm move thành 78 và tiếp luôn vê 1
            {
                GotoXY(78, car.coordinate.y + j);
                cout << "             ";
            }
        }
        else if (cnt == 1)
        {
            if (car.coordinate.x == 77 || car.coordinate.x == 1)
            {
                GotoXY(78, car.coordinate.y + j);
                cout << "             ";
            }
        }
        else if (cnt == 2)
        {
            if (car.coordinate.x == 77 || car.coordinate.x == 1 || car.coordinate.x == 76)
            {
                GotoXY(78, car.coordinate.y + j);
                cout << "             ";
            }
        }
    }
}

void EraseCarRight(CarRight& car)
{
    for (int j = 0; j < 4; j++)
    {
        GotoXY(car.coordinate.x + 16 - cnt, car.coordinate.y + j);
        cout << " ";

        if (cnt == 0)
        {
            if (car.coordinate.x == 1)
            {
                GotoXY(1, car.coordinate.y + j);
                cout << "                ";
            }
        }
        else if (cnt == 1)
        {
            if (car.coordinate.x == 2 || car.coordinate.x == 1)
            {
                GotoXY(1, car.coordinate.y + j);
                cout << "                ";
            }
        }
        else if (cnt == 2)
        {
            if (car.coordinate.x == 3 || car.coordinate.x == 2 || car.coordinate.x == 1)
            {
                GotoXY(1, car.coordinate.y + j);
                cout << "                ";
            }
        }
    }
}

void EraseCars()
{
    for (int i = 0; i < 3; i++)
    {
        if (IsMoving_Car[i] != 0)
        {
            cnt = 0;
            do {
                EraseCarLeft(arr_cl[i]);
                EraseCarLeft(arr_cl[i + 3]);
                cnt++;
            } while (cnt < SPEED);
        }
    }
    for (int i = 0; i < 3; i++)
    {
        if (IsMoving_Car[i + 3] != 0)
        {
            cnt = 0;
            do {
                EraseCarRight(arr_cr[i]);
                EraseCarRight(arr_cr[i + 3]);
                cnt++;
            } while (cnt < SPEED);
        }
    }
}

void MoveRight() {
    if (Y.coordinate.x + 1 < WIDTH_CONSOLE - 1) {
        EraseSticker(Y);
        Y.coordinate.x += 3;
        DrawSticker(Y);
    }
}
void MoveLeft() {
    if (Y.coordinate.x - 1 > 1) {
        EraseSticker(Y);
        Y.coordinate.x -= 3;
        DrawSticker(Y);
    }
}
void MoveDown() {
    if (Y.coordinate.y + 3 < HEIGH_CONSOLE - 1) {
        EraseSticker(Y);
        Y.coordinate.y += 5;
        DrawSticker(Y);
    }
}
void MoveUp() {
    if (Y.coordinate.y > 1) {
        EraseSticker(Y);
        Y.coordinate.y -= 5;
        DrawSticker(Y);
    }
}

void DeadEffect()
{
    EraseSticker(Y);
    SetColor(12);
    DrawSticker(Y);
    SetColor(7);
    Sleep(10);
    DrawAmbulance();
}

void FileSave()
{
    system("cls");
    UnNocursortype();
    string tenluu;
    DrawBoard(20, 10, 80, 4);
    GotoXY(25, 12); cout << "Enter your name: ";
    getline(cin, tenluu);

    int yes = 1;

    string search;
    ifstream fin("CacFileDaLuu.txt");
    while (!fin.eof())
    {
        getline(fin, search);
        if (search == tenluu)
        {
            system("cls");
            DrawBoard(20, 10, 80, 4);
            GotoXY(50, 12); cout << "File has already existed";
            GotoXY(0, 0);
            yes = 0;
            Sleep(500);
            break;
        }
    }

    fin.close();

    if (yes == 1)

    {
        ofstream fout(tenluu + ".txt");

        fout << "Car status: ";
        for (int i = 0; i < 6; i++) fout << IsMoving_Car[i] << " ";
        fout << endl;

        fout << "Coordinate of finish people: ";
        for (int i = 0; i < MAX_SPEED; i++) fout << FinishPeople_Coordinate[i].x << " " << FinishPeople_Coordinate[i].y << " ";
        fout << endl;

        fout << "Number of finish people: " << FinishPeople_Num << endl;
        fout << "Speed: " << SPEED << endl;
        fout << "Coordianate of player: " << Y.coordinate.x << " " << Y.coordinate.y << endl;

        fout << "Coordinate of left car: " << endl;

        for (int i = 0; i < 6; i++)
        {
            fout << arr_cl[i].coordinate.x << " " << arr_cl[i].coordinate.y << " ";
        }
        fout << endl;

        fout << "Coordinate of right car: " << endl;

        for (int i = 0; i < 6; i++)
        {
            fout << arr_cr[i].coordinate.x << " " << arr_cr[i].coordinate.y << " ";
        }
        fout << endl;

        fout.close();

        fstream fout2;
        fout2.open("CacFileDaLuu.txt", ios::app);
        fout2 << tenluu << endl;
        fout2.close();

        system("cls");
        DrawBoard(20, 10, 80, 4);
        GotoXY(50, 12); cout << "File Save Success !";
        GotoXY(0, 0);
        yes = 0;
        Sleep(500);
    }
    else FileSave();
    Nocursortype();
}

int tgs = 0;
void SubThread()
{
    PlaySound(TEXT("BGM.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    while (1) {
        if (STATE) //Nếu người vẫn còn sống
        {
            switch (MOVING) //Kiểm tra biến moving
            {
            case 'A':
                MoveLeft();
                break;
            case 'D':
                MoveRight();
                break;
            case 'W':
                MoveUp();
                break;
            case 'S':
                MoveDown();
                break;
            }
            MOVING = ' '; //Tạm khóa không cho di chuyển, chờ nhận phím từ hàm main
            EraseCars();
            MoveCars();
            DrawCars();

            tgs++; // thời gian xe dừng tính theo nhịp game
            if (tgs == 30)
            {
                for (int i = 0; i < 6; i++)
                {
                    if (IsMoving_Car[i] == 0)
                        IsMoving_Car[i] = 1;
                }
                tgs = 0;
            }

            if (IsImpactvsP(Y))
            {
                flag2 = 1;
                ProcessDead();
            }
            if (IsImpactvsC(Y))
            {
                DeadEffect();
                ProcessDead();
            }
            if (Y.coordinate.y == 1 && STATE == true) // kiểm tra xem sau khi va chạm với người thì còn sống k, nếu k có ktra này thi dù va chạm vs ng
                // state = 0 thì hàm win vẫn chạy
            {
                FinishPeople_Coordinate[FinishPeople_Num].x = Y.coordinate.x; FinishPeople_Coordinate[FinishPeople_Num].y = Y.coordinate.y;
                FinishPeople_Num++;
                if (FinishPeople_Num == MAX_SPEED) ProcessWin();
                ProcessFinish(Y);
            }
            Sleep(60);
        }
    }
}

int Menu()
{
    system("cls");
    PlaySound(NULL, NULL, SND_ASYNC); // dừng BGM trước đó
    int temp;

    int Set[] = { 7,7,7,7 };
    int optionNumber = 0;
    char key;
    GotoXY(50, 13);
    SetColor(Set[0]);
    cout << "1.New Game";

    GotoXY(50, 14);
    SetColor(Set[1]);
    cout << "2.Load Game";

    GotoXY(50, 15);
    SetColor(Set[2]);
    cout << "3.Help";

    GotoXY(50, 16);
    SetColor(Set[3]);
    cout << "4.Quit Game";

    for (int i = 0;;)
    {
        SetColor(7);
        DrawGameTitle();

        GotoXY(50, 13);
        SetColor(Set[0]);
        cout << "1.New Game";

        GotoXY(50, 14);
        SetColor(Set[1]);
        cout << "2.Load Game";

        GotoXY(50, 15);
        SetColor(Set[2]);
        cout << "3.Help";

        GotoXY(50, 16);
        SetColor(Set[3]);
        cout << "4.Quit Game";

        Set[0] = 7;
        Set[1] = 7;
        Set[2] = 7;
        Set[3] = 7;

        key = _getch();

        if (key == 72 || key == 'W' || key == 'w')
        {
            PlaySound(TEXT("MenuSelection.wav"), NULL, SND_FILENAME | SND_ASYNC);
            optionNumber--;
            if (optionNumber < 1) optionNumber = 4;
        }
        if (key == 80 || key == 'S' || key == 's')
        {
            PlaySound(TEXT("MenuSelection.wav"), NULL, SND_FILENAME | SND_ASYNC);
            optionNumber++;
            if (optionNumber > 4) optionNumber = optionNumber % 4;
        }
        if (key == '\r')
        {
            PlaySound(TEXT("MenuSelection.wav"), NULL, SND_FILENAME | SND_ASYNC);
            if (optionNumber == 1)
            {
                //////////////////////////////NEW GAME////////////////////////////
                system("cls");
                srand(time(NULL));
                StartGame();
                thread t1(SubThread);
                while (1)
                {
                    temp = toupper(_getch());
                    if (STATE == 1) {
                        if (temp == 27) {
                            ExitGame(t1.native_handle());
                            return 0;
                        }
                        else if (temp == 'P')
                            PauseGame(t1.native_handle());
                        else if (temp == 'L')
                        {
                            PauseGame(t1.native_handle());
                            system("cls");
                            FileSave();
                            PauseGame(t1.native_handle());
                            Menu();
                        }
                        else
                        {
                            ResumeThread((HANDLE)t1.native_handle());
                            if (temp == 'D' || temp == 'A' || temp == 'W' || temp == 'S')
                                MOVING = temp;
                        }
                    }
                    else {
                        if (temp == 'Y')
                        {
                            flag0 = 0; // ngưng xe cứu thương
                            flag2 = 0; // k cho dead ra
                            StartGame();
                        }
                        else if (temp == 27)
                        {
                            ExitGame(t1.native_handle());
                            return 0;
                        }
                    }
                }
            }
            ////////////////////////////////////////////////////////////////////////
            if (optionNumber == 2)
                //////////////////////////////CONTINUE GAME////////////////////////////
            {
                system("cls");
                srand(time(NULL));
                ContinueGame();

                thread t1(SubThread);
                while (1) {
                    temp = toupper(_getch());
                    if (STATE == 1) {
                        if (temp == 27) {
                            ExitGame(t1.native_handle());
                            return 0;
                        }
                        else if (temp == 'P')
                            PauseGame(t1.native_handle());
                        else if (temp == 'L')
                        {
                            PauseGame(t1.native_handle());
                            FileSave();
                            PauseGame(t1.native_handle());
                            Menu();
                        }
                        else
                        {
                            ResumeThread((HANDLE)t1.native_handle());
                            if (temp == 'D' || temp == 'A' || temp == 'W' || temp == 'S')
                                MOVING = temp;
                        }
                    }
                    else
                    {
                        if (temp == 'Y')
                        {
                            flag0 = 0; // ngưng xe cứu thương
                            flag2 = 0; // k cho dead ra
                            StartGame();
                        }
                        else if (temp == 27)
                        {
                            ExitGame(t1.native_handle());
                            return 0;
                        }
                    }
                }
                ///////////////////////////////////////////////////////////////////////
            }
            if (optionNumber == 3)
            {
                SetColor(7);
                system("cls");
                GotoXY(50, 1); cout << "This is instruction: " << endl;
                GotoXY(40, 2); cout << "1. Use W A S D to move" << endl;
                GotoXY(40, 3); cout << "2. Try to avoid cars. You lose if you collide with the cars" << endl;
                GotoXY(40, 4); cout << "3. There are 3 level to pass" << endl;
                GotoXY(40, 5); cout << "4. Press ESC to exit game. L to save game. P to pause game" << endl;
                system("pause");
                system("cls");
            }
            if (optionNumber == 4)
            {
                return 0;
            }

        }
        if (optionNumber == 1) Set[0] = 12;
        if (optionNumber == 2) Set[1] = 12;
        if (optionNumber == 3) Set[2] = 12;
        if (optionNumber == 4) Set[3] = 12;
    }
}


int main(int argc, char* argv[])
{
    ResizeConsole(1000, 750);
    ReposConsole();
    FixConsoleWindow();
    DrawTeamName();
    Menu();
    return 0;
}