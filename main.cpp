// �ɻ���ս
// EasyX
#include <iostream>
#include <graphics.h>//easyx
#include <vector>
#include <conio.h>
#include <fstream>
#include <algorithm>
using namespace std;

constexpr auto swidth = 600;
constexpr auto sheight = 1100;

constexpr int SHP = 8;
constexpr auto hurttime = 1000;//ms

enum class Mode { AUTO, MANUAL };

void Welcome();
Mode currentMode = Mode::AUTO; // Ĭ���Զ�ģʽ

void ShowRecords()
{
    BeginBatchDraw();
    cleardevice();

    settextstyle(40, 0, _T("����"));
    settextcolor(BLUE);
    const TCHAR* title = _T("��ʷ��ɱ��¼");
    int textWidth = textwidth(title);
    int xTitle = (getwidth() - textWidth) / 2;
    outtextxy(xTitle, 80, title);

    std::ifstream fin("kill_records.txt");
    if (!fin.is_open())
    {
        settextstyle(40, 0, _T("����"));
        settextcolor(RED);
        const TCHAR* noRecordMsg = _T("���޻�ɱ��¼");
        int noRecordTextWidth = textwidth(noRecordMsg);
        int xNoRecord = (getwidth() - noRecordTextWidth) / 2;

        outtextxy(xNoRecord, 180, noRecordMsg);
    }
    else
    {
        std::vector<unsigned long long> records;
        unsigned long long kill;
        int count = 0;

        // ��ȡ���л�ɱ��¼
        while (fin >> kill)
        {
            records.push_back(kill);
        }
        fin.close();

        // �Ի�ɱ��¼���дӸߵ�������
        std::sort(records.begin(), records.end(), [](unsigned long long a, unsigned long long b) {
            return a > b; // ����������
            });

        settextstyle(24, 0, _T("����"));
        settextcolor(BLACK);

        // ��ʾ�����Ļ�ɱ��¼
        for (size_t i = 0; i < records.size() && i < 10; ++i)
        {
            TCHAR line[64];
            _stprintf_s(line, _T("�� %d ������ɱ�� %llu"), i + 1, records[i]);
            outtextxy((swidth - textwidth(line)) / 2, 180 + i * 30, line);
        }

        if (records.empty())
        {
            settextstyle(40, 0, _T("����"));
            settextcolor(RED);
            const TCHAR* noRecordMsg = _T("���޻�ɱ��¼");
            int noRecordTextWidth = textwidth(noRecordMsg);
            int xNoRecord = (getwidth() - noRecordTextWidth) / 2;

            outtextxy(xNoRecord, 180, noRecordMsg);
        }
    }



    // �����¼� ����Enter���أ�
    LPCTSTR info = _T("��Enter����");
    settextstyle(30, 0, _T("����")); // ��СһЩ
    settextcolor(BLACK);
    // ������ʾ���֣��·�һ�㣩
    outtextxy(swidth / 2 - textwidth(info) / 2, sheight / 5 + 500, info);

    EndBatchDraw();

    // �ȴ��û��� Enter ����
    while (true)
    {
        ExMessage mess;
        getmessage(&mess, EM_KEY);
        if (mess.vkcode == 0x0D)
        {
            return Welcome();
        }
    }
}


bool PointInRect(int x, int y, RECT& r)
{
    return (r.left <= x && x <= r.right && r.top <= y && y <= r.bottom);
}

bool RectDuangRect(RECT& r1, RECT& r2)
{
    RECT r;
    r.left = r1.left - (r2.right - r2.left);
    r.right = r1.right;
    r.top = r1.top - (r2.bottom - r2.top);
    r.bottom = r1.bottom;
    return (r.left < r2.left && r2.left <= r.right && r.top <= r2.top && r2.top <= r.bottom);
}


void Welcome()
{
    IMAGE bkimg;
    loadimage(&bkimg, _T("images/bk2.png"), swidth, sheight * 2);


    LPCTSTR title = _T("�ɻ���ս");
    LPCTSTR tplay = _T("��ʼ��Ϸ");
    LPCTSTR trecord = _T("��ɱ��¼");
    LPCTSTR texit = _T("�˳���Ϸ");


    RECT tplay_r, trecordr, texitr;
    BeginBatchDraw();
    setbkcolor(RGB(195, 200, 201));
    cleardevice();
    putimage(0, 0, &bkimg);
    settextstyle(60, 0, _T("����"));
    settextcolor(BLACK);
    outtextxy(swidth / 2 - textwidth(title) / 2, sheight / 5, title);

    settextstyle(40, 0, _T("����"));
    // ��ʼ��Ϸ
    tplay_r.left = swidth / 2 - textwidth(tplay) / 2;
    tplay_r.right = tplay_r.left + textwidth(tplay);
    tplay_r.top = sheight / 5 * 2.5;
    tplay_r.bottom = tplay_r.top + textheight(tplay);
    outtextxy(tplay_r.left, tplay_r.top, tplay);
    trecordr.left = swidth / 2 - textwidth(texit) / 2;
    trecordr.right = trecordr.left + textwidth(texit);
    trecordr.top = sheight / 5 * 3;
    trecordr.bottom = trecordr.top + textheight(texit);
    outtextxy(trecordr.left, trecordr.top, trecord);

    // �˳�
    texitr.left = swidth / 2 - textwidth(texit) / 2;
    texitr.right = texitr.left + textwidth(texit);
    texitr.top = sheight / 5 * 3.5;
    texitr.bottom = texitr.top + textheight(texit);
    outtextxy(texitr.left, texitr.top, texit);

    EndBatchDraw();

    while (true)
    {
        ExMessage mess;
        getmessage(&mess, EM_MOUSE);
        if (mess.lbutton)
        {
            if (PointInRect(mess.x, mess.y, tplay_r))
            {
                currentMode = Mode::AUTO; // ����Ϊ�Զ�ģʽ
                return; // ֱ�ӷ���
            }
            else if (PointInRect(mess.x, mess.y, trecordr))
            {
                return ShowRecords();
            }
            else if (PointInRect(mess.x, mess.y, texitr))
            {
                exit(0);
            }
        }
    }
}

void Over(unsigned long long& kill)
{
    IMAGE bkimg;
    loadimage(&bkimg, _T("images/bk2.png"), swidth, sheight * 2);
    // ׼����ʾ�ı�
    LPCTSTR info1 = _T("GAME OVER");

    // ׼����ɱ���ı�
    TCHAR* str = new TCHAR[128];
    _stprintf_s(str, 128, _T("��ɱ����%llu"), kill);

    // ׼����ʾ�ı�
    LPCTSTR info2 = _T("��Enter����");

    BeginBatchDraw();
    setbkcolor(RGB(195, 200, 201));
    cleardevice();
    putimage(0, 0, &bkimg);

    // ���������С������ʾ����
    settextstyle(60, 0, _T("����")); // ��СһЩ
    settextcolor(RED);
    // ������ʾ���֣��·�һ�㣩
    outtextxy(swidth / 2 - textwidth(info1) / 2, sheight / 5, info1);

    // ����������ʽ
    settextstyle(40, 0, _T("����"));
    settextcolor(RED);
    // ���ƻ�ɱ����������
    outtextxy(swidth / 2 - textwidth(str) / 2, sheight / 5 + textheight(str) + 60, str);

    // ���������С������ʾ����
    settextstyle(30, 0, _T("����")); // ��СһЩ
    settextcolor(BLACK);
    // ������ʾ���֣��·�һ�㣩
    outtextxy(swidth / 2 - textwidth(info2) / 2, sheight / 5 + textheight(str) + 240, info2);

    EndBatchDraw();

    // ����ɱ��д���ļ�
    ofstream fout("kill_records.txt", ios_base::app); // ׷��ģʽ
    if (fout.is_open())
    {
        fout << kill << "\n";
        fout.close();
    }

    EndBatchDraw();
    while (true)
    {
        ExMessage mess;
        getmessage(&mess, EM_KEY);
        if (mess.vkcode == 0x0D)
        {
            return;
        }
    }
}
class BK
{
public:
    BK(IMAGE& img)
        :img(img), y(-sheight)
    {
    }
    void Show()
    {
        if (y == 0) { y = -sheight; }
        y += 4;
        putimage(0, y, &img);
    }
private:
    IMAGE& img;
    int y;
};

class Hero
{
public:
    Hero(IMAGE& img, IMAGE* boom)
        :img(img), HP(SHP),boomsum(0)
    {
        selfboom[0] = boom[0];
        selfboom[1] = boom[1];
        selfboom[2] = boom[2];
        rect.left = swidth / 2 - img.getwidth() / 2;
        rect.top = sheight - img.getheight();
        rect.right = rect.left + img.getwidth();
        rect.bottom = sheight;
    }
    bool isalive() {
        if (HP>0)return true;
        else return false;
    }
    bool Show()
    {
        if (HP <= 0)
        {
            if (boomsum == 3)
            {
                return false;
            }
            putimage(rect.left, rect.top, &selfboom[boomsum]);
            Sleep(50);
            boomsum++;

            return true;
        }
        setlinecolor(RED);
        setlinestyle(PS_SOLID, 4);
        putimage(rect.left, rect.top, &img);
        float barWidth = (img.getwidth()*1.0*HP/SHP);
        //cout << HP <<"\n";
        if(HP>=0)line(rect.left, rect.top - 5,rect.left+ barWidth, rect.top - 5);
        return true;
    }
    void Control()
    {
        ExMessage mess;
        if (peekmessage(&mess, EM_MOUSE))
        {
            rect.left = mess.x - img.getwidth() / 2;
            rect.top = mess.y - img.getheight() / 2;
            rect.right = rect.left + img.getwidth();
            rect.bottom = rect.top + img.getheight();
        }
    }

    bool hurt(int damage)
    {
        if (HP > 0) {
            HP -= damage;
            //cout << HP << " " << damage << "\n";
        }
        return (HP <= 0 && boomsum == 3) ? false : true;
    }

    RECT& GetRect() { return rect; }

private:
    IMAGE& img;
    RECT rect;
    int HP;
    bool isdie;
    int boomsum;
    IMAGE selfboom[3];
};

class Enemy
{
protected:
    void SetHP(int hp) { HP = hp; }//cout << HP << "\n";
    void SetDamage1(int damage1) { this->damage1 = damage1;  }
    void SetDamage2(int damage2) { this->damage2 = damage2; }
    IMAGE& img;
    RECT rect;
    IMAGE selfboom[3];
    bool isdie;
    int boomsum;
    int HP;
    int damage1=1,damage2=1;//1���ӵ��˺���2����ײ�˺�
public:
    virtual ~Enemy() = default; // ������������
    Enemy(IMAGE& img, int x, IMAGE*& boom)
        :img(img), isdie(false), boomsum(0),HP(1)
    {
        selfboom[0] = boom[0];
        selfboom[1] = boom[1];
        selfboom[2] = boom[2];
        rect.left = x;
        rect.right = rect.left + img.getwidth();
        rect.top = -img.getheight();
        rect.bottom = 0;
    }
    int GetDamage1() const { return damage1; }
    int GetDamage2() const { return damage2; }
    virtual bool Show()
    {
        if (isdie)
        {
            if (boomsum == 3)
            {
                return false;
            }
            putimage(rect.left, rect.top, selfboom + boomsum);
            boomsum++;
            return true;
        }
        if (rect.top >= sheight)
        {
            return false;
        }
        rect.top += 4;
        rect.bottom += 4;
        putimage(rect.left, rect.top, &img);
        return true;
    }
    virtual void Isdie()
    {
        isdie = true;
    }
    virtual RECT& GetRect() { return rect; }
    virtual bool TakeDamage()
    {
        HP --;

        if (HP <= 0)
        {
            Isdie(); // ���Ϊ����
            return false; // ����false��ʾ��������
        }

        return true; // ������
    }
};

class NormalEnemy : public Enemy {
public:
    NormalEnemy(IMAGE& img, int x, IMAGE* boom)
        : Enemy(img, x, boom) {
        // ���������޸�����
        SetHP(1);
        SetDamage1(1);
        SetDamage2(1);
    }
};
class TankEnemy : public Enemy {
public:
    TankEnemy(IMAGE& img, int x, IMAGE* boom)
        : Enemy(img, x, boom) {
        SetHP(3);  // ��Ѫ��
        SetDamage1(1);
        SetDamage2(2);  // ��ײ�˺�����
    }
};

// 4. ���ݵ����ࣨ�����ƶ���
class AgileEnemy : public Enemy {
public:
    AgileEnemy(IMAGE& img, int x, IMAGE* boom)
        : Enemy(img, x, boom) {
        SetHP(2);
        SetDamage1(2);  // �ӵ��˺�����
        SetDamage2(1);
    }

   /* bool Show() override {
        if (isdie) {
            if (boomsum == 5) {  // ʹ�ö�̬֡����
                return false;
            }
            putimage(rect.left, rect.top, boom + boomsum);  // ֱ��ʹ�ô���ı�ը����
            boomsum++;
            return true;
        }
        // �ƶ��ٶ�����ͨ���˵�1.5��
        rect.top += 6;
        rect.bottom += 6;
        putimage(rect.left, rect.top, &img);
        return rect.top < sheight;
    }
    */
};
class Bullet
{
public:
    Bullet(IMAGE& img, RECT pr)
        :img(img)
    {
        rect.left = pr.left + (pr.right - pr.left) / 2 - img.getwidth() / 2;
        rect.right = rect.left + img.getwidth();
        rect.top = pr.top - img.getheight();
        rect.bottom = rect.top + img.getheight();
    }
    bool Show()
    {
        if (rect.bottom <= 0)
        {
            return false;
        }
        rect.top -= 3;
        rect.bottom -= 3;
        putimage(rect.left, rect.top, &img);
        return true;
    }
    RECT& GetRect() { return rect; }
protected:
    IMAGE& img;
    RECT rect;
};

class EBullet : public Bullet
{
public:
    EBullet(IMAGE& img, RECT pr,int damage)
        :Bullet(img, pr),damage(damage)
    {
        rect.left = pr.left + (pr.right - pr.left) / 2 - img.getwidth() / 2;
        rect.right = rect.left + img.getwidth();
        rect.top = pr.bottom;
        rect.bottom = rect.top + img.getheight();
    }
    bool Show()
    {
        if (rect.top >= sheight)
        {
            return false;
        }
        rect.top += 5;
        rect.bottom += 5;
        putimage(rect.left, rect.top, &img);
        return true;
    }
    int GetDamage() const {
        return damage;
    }
private:
    int damage; // �ӵ��˺�ֵ
};

// ֧�ֲ�ͬ���͵л�����ӦͼƬ��AddEnemy����
bool AddEnemy(vector<Enemy*>& es, int enemyType,
    IMAGE& normalImg, IMAGE& tankImg, IMAGE& agileImg,
    IMAGE* normalBoom, IMAGE* tankBoom, IMAGE* agileBoom) {
    Enemy* e = nullptr;

    // ���ݵл����ʹ�����Ӧ�������ಢ���ض�ӦͼƬ
    switch (enemyType) {
    case 0: // ��ͨ�л�
        e = new NormalEnemy(normalImg,
            abs(rand()) % (swidth - normalImg.getwidth()), normalBoom);
        break;
    case 2: // ̹�˵л�
        e = new TankEnemy(tankImg,
            abs(rand()) % (swidth - tankImg.getwidth()), tankBoom);
        break;
    case 1: // ���ݵл�
        e = new AgileEnemy(agileImg,
            abs(rand()) % (swidth - agileImg.getwidth()), agileBoom);
        break;
    default:
        return false;
    }

    // ��ײ���
    for (auto& existingEnemy : es) {
        if (RectDuangRect(existingEnemy->GetRect(), e->GetRect())) {
            delete e;
            return false;
        }
    }

    es.push_back(e);
    return true;
}

// mode �����Զ�/�ֶ�
bool Play(Mode mode)
{
    setbkcolor(WHITE);
    cleardevice();
    bool is_play = true;

    IMAGE heroimg, bkimg, bulletimg, Ebulletimg;
    IMAGE normalEnemyImg, tankEnemyImg, agileEnemyImg;  // ���ֵл�ͼƬ
    IMAGE eboom1[3], meboom[3], eboom2[5], eboom3[5];  // ��ը����

    // �������ֵл�ͼƬ
    loadimage(&normalEnemyImg, _T("images/enemy1.png"));   // ��ͨ�л�
    loadimage(&tankEnemyImg, _T("images/enemy3.png"));    // ̹�˵л�����ܣ�
    loadimage(&agileEnemyImg, _T("images/enemy2.png"));   // ���ݵл�

    loadimage(&Ebulletimg, _T("images/bullet2.png"));
    loadimage(&heroimg, _T("images/me1.png"));
    loadimage(&bkimg, _T("images/bk2.png"), swidth, sheight * 2);
    loadimage(&bulletimg, _T("images/bullet1.png"));

    loadimage(&meboom[0], _T("images/me_destroy_1.png"));
    loadimage(&meboom[1], _T("images/me_destroy_2.png"));
    loadimage(&meboom[2], _T("images/me_destroy_3.png"));

    loadimage(&eboom1[0], _T("images/enemy1_down2.png"));
    loadimage(&eboom1[1], _T("images/enemy1_down3.png"));
    loadimage(&eboom1[2], _T("images/enemy1_down4.png"));

    loadimage(&eboom2[0], _T("images/enemy2_down1.png"));
    loadimage(&eboom2[1], _T("images/enemy2_down2.png"));
    loadimage(&eboom2[2], _T("images/enemy2_down3.png"));
    loadimage(&eboom2[3], _T("images/enemy2_down4.png"));
    loadimage(&eboom2[4], _T("images/enemy2_down5.png"));

    loadimage(&eboom3[0], _T("images/enemy3_down1.png"));
    loadimage(&eboom3[1], _T("images/enemy3_down2.png"));
    loadimage(&eboom3[2], _T("images/enemy3_down4.png"));
    loadimage(&eboom3[3], _T("images/enemy3_down5.png"));
    loadimage(&eboom3[4], _T("images/enemy3_down6.png"));

    BK bk = BK(bkimg);
    Hero hp = Hero(heroimg,meboom);

    vector<Enemy*> es;
    vector<Bullet*> bs;
    vector<EBullet*> ebs;

    clock_t hurtlast = clock();

    unsigned long long kill = 0;

    bool is_paused = false;  // ��ͣ״̬��־

    int hero_bullet_timer = 0;     // Ӣ���Զ��ӵ���
    int enemy_bullet_timer = 0;    // �л������ӵ�������

    for (int i = 0; i < 5; i++) {
        AddEnemy(es, 0, normalEnemyImg, tankEnemyImg, agileEnemyImg,
            eboom1, eboom3, eboom2);
    }

    Mode currentMode = mode; // ��������Ϸ�ڵ�ǰģʽ

    while (is_play)
    {
        // �ϲ����м����¼�����
        ExMessage mess;
        bool pauseKeyTriggered = false;
        bool fireKeyTriggered = false;
        bool escKeyTriggered = false;
        bool tabKeyTriggered = false;
        while (peekmessage(&mess, EM_KEY))
        {
            if (mess.message == WM_KEYDOWN && mess.vkcode == VK_SPACE)
                pauseKeyTriggered = true;
            if (mess.message == WM_KEYDOWN && (mess.vkcode == 'Q' || mess.vkcode == 'q'))
                fireKeyTriggered = true;
            if (mess.message == WM_KEYDOWN && mess.vkcode == VK_ESCAPE)
                escKeyTriggered = true;
            if (mess.message == WM_KEYDOWN && mess.vkcode == VK_TAB)
                tabKeyTriggered = true;
        }
        if (is_paused)
        {
            // ����ͣ����
            BeginBatchDraw();


            settextstyle(40, 0, _T("����"));
            settextcolor(RGB(0, 0, 0));
            RECT bg = { 0, 0, swidth, sheight * 2 };
            setfillcolor(RGB(195, 200, 201));
            solidrectangle(bg.left, bg.top, bg.right, bg.bottom);

            LPCTSTR pauseText = _T("��Ϸ����ͣ");
            LPCTSTR contText = _T("���ո��������Ϸ");
            LPCTSTR escText = _T("��ESC���˳������˵�");

            // ��ʾ��ǰģʽ
            LPCTSTR modeText = (currentMode == Mode::AUTO) ? _T("��ǰģʽ���Զ�") : _T("��ǰģʽ���ֶ�");
            LPCTSTR tabText = _T("��Tab���л�ģʽ");

            settextcolor(RGB(200, 50, 50));
            setbkcolor(RGB(195, 200, 201));

            outtextxy(swidth / 2 - textwidth(pauseText) / 2, sheight / 2 - 100, pauseText);
            settextcolor(BLACK);
            setbkcolor(RGB(195, 200, 201));
            settextstyle(30, 0, _T("����"));
            outtextxy(swidth / 2 - textwidth(contText) / 2, sheight / 2 - 20, contText);
            outtextxy(swidth / 2 - textwidth(escText) / 2, sheight / 2 + 30, escText);

            settextstyle(25, 0, _T("����"));
            settextcolor(RGB(50, 100, 200));
            setbkcolor(RGB(195, 200, 201));
            outtextxy(swidth / 2 - textwidth(modeText) / 2, sheight / 2 + 75, modeText);
            settextcolor(RGB(60, 60, 60));
            setbkcolor(RGB(195, 200, 201));
            outtextxy(swidth / 2 - textwidth(tabText) / 2, sheight / 2 + 115, tabText);

            EndBatchDraw();

            // ֻ��Ӧ�ո��ESC��Tab
            bool wait = true;
            while (wait)
            {
                ExMessage pause_mess;
                if (peekmessage(&pause_mess, EM_KEY))
                {
                    if (pause_mess.message == WM_KEYDOWN && pause_mess.vkcode == VK_SPACE)
                    {
                        is_paused = false;
                        wait = false;
                    }
                    if (pause_mess.message == WM_KEYDOWN && pause_mess.vkcode == VK_ESCAPE)
                    {
                        // �˳������˵�
                        return true;
                    }
                    if (pause_mess.message == WM_KEYDOWN && pause_mess.vkcode == VK_TAB)
                    {
                        // �л�ģʽ
                        if (currentMode == Mode::AUTO) currentMode = Mode::MANUAL;
                        else currentMode = Mode::AUTO;
                        // ˢ����ͣ����
                        BeginBatchDraw();
                        RECT bg = { 0, 0, swidth, sheight * 2 };

                        setbkcolor(RGB(195, 200, 201));
                        settextstyle(40, 0, _T("����"));
                        settextcolor(RGB(0, 0, 0));
                        setfillcolor(RGB(195, 200, 201));
                        solidrectangle(bg.left, bg.top, bg.right, bg.bottom);

                        settextcolor(RGB(200, 50, 50));
                        outtextxy(swidth / 2 - textwidth(pauseText) / 2, sheight / 2 - 100, pauseText);
                        settextcolor(BLACK);
                        settextstyle(30, 0, _T("����"));
                        outtextxy(swidth / 2 - textwidth(contText) / 2, sheight / 2 - 20, contText);
                        outtextxy(swidth / 2 - textwidth(escText) / 2, sheight / 2 + 30, escText);

                        // ��ʾ�л����ģʽ
                        LPCTSTR newModeText = (currentMode == Mode::AUTO) ? _T("��ǰģʽ���Զ�") : _T("��ǰģʽ���ֶ�");

                        settextstyle(25, 0, _T("����"));
                        settextcolor(RGB(50, 100, 200));
                        outtextxy(swidth / 2 - textwidth(newModeText) / 2, sheight / 2 + 75, newModeText);
                        settextcolor(RGB(60, 60, 60));
                        outtextxy(swidth / 2 - textwidth(tabText) / 2, sheight / 2 + 115, tabText);
                        EndBatchDraw();
                    }
                }
                Sleep(8);
            }
            continue; // ������ѭ��
        }
        if (pauseKeyTriggered)
        {
            is_paused = !is_paused;
        }
        if (escKeyTriggered) // ֱ���˳������˵�
        {
            return true;
        }

        // --- �Զ�/�ֶ�ģʽ�����ӵ����� ---
        if (currentMode == Mode::MANUAL) {
            if (hp.isalive()&&fireKeyTriggered)
                bs.push_back(new Bullet(bulletimg, hp.GetRect()));
        }
        else if (currentMode == Mode::AUTO) {
            hero_bullet_timer++;
            if (hp.isalive()&&hero_bullet_timer >= 20) { // Լÿ10֡��һ��
                bs.push_back(new Bullet(bulletimg, hp.GetRect()));
                hero_bullet_timer = 0;
            }
        }

        // �л���ʱ�����ӵ�������ʲôģʽ�����Զ���
        enemy_bullet_timer++;
        if (enemy_bullet_timer >= 60)
        {
            for (auto& i : es)
            {
                int damage = i->GetDamage1();
                //cout << damage << "\n";
                ebs.push_back(new EBullet(Ebulletimg, i->GetRect(), damage));
            }
            enemy_bullet_timer = 0;
        }

        BeginBatchDraw();

        bk.Show();
        Sleep(2);
        flushmessage();
        Sleep(2);
        hp.Control();

        hp.Show();

        // ��ʾ��ǰ��ɱ��
        TCHAR killStr[64];
        _stprintf_s(killStr, _T("��ɱ����%llu"), kill);
        setbkcolor(RGB(195, 200, 201));
        settextstyle(24, 0, _T("����"));
        settextcolor(RED);
        outtextxy(10, 10, killStr);

        auto bsit = bs.begin();
        while (bsit != bs.end())
        {
            if (!(*bsit)->Show())
            {
                delete* bsit;
                bsit = bs.erase(bsit);
            }
            else
            {
                bsit++;
            }
        }

        auto ebsit = ebs.begin();
        while (ebsit != ebs.end())
        {
            if (!(*ebsit)->Show())
            {
                delete* ebsit;
                ebsit = ebs.erase(ebsit);
            }
            else
            {
                if (RectDuangRect((*ebsit)->GetRect(), hp.GetRect()))
                {
                    if (clock() - hurtlast >= hurttime)
                    {
                        int da = (*ebsit)->GetDamage();
                        is_play = hp.hurt(da);
                        hurtlast = clock();
                    }
                }
                ebsit++;
            }
        }

        auto it = es.begin();
        while (it != es.end())
        {
            if (RectDuangRect((*it)->GetRect(), hp.GetRect()))
            {
                if (clock() - hurtlast >= hurttime)
                {
                    int da = (*it)->GetDamage2(); // ��ȡ���˵Ĺ�����
                    is_play = hp.hurt(da);
                    hurtlast = clock();
                }
            }
            auto bit = bs.begin();
            while (bit != bs.end())
            {
                if (RectDuangRect((*bit)->GetRect(), (*it)->GetRect()))
                {
                    (*it)->Isdie();
                    delete (*bit);
                    bit = bs.erase(bit);

                    kill++;

                    break;
                }
                else
                {
                    bit++;
                }
            }
            if (!(*it)->Show())
            {
                delete (*it);
                it = es.erase(it);
            }
            else
            {
                it++;
            }
        }
        for (int i = 0; i < 5 - es.size(); i++) {
            int enemyType;
            if (kill < 10) {
                enemyType = 0;  // ��ͨ�л�
            }
            else if (kill < 20) {
                enemyType = rand() % 2;  // ��ͨ��̹��
            }
            else {
                enemyType = rand() % 3;  // ��ͨ��̹�ˡ�����
            }
            AddEnemy(es, enemyType, normalEnemyImg, tankEnemyImg, agileEnemyImg,
                eboom1, eboom3, eboom2);
        }

        EndBatchDraw();
    }
    printf_s("e");
    Over(kill);

    return true;
}

int main()
{
    // easyx��ʼ��
    initgraph(swidth, sheight, EW_NOMINIMIZE | EW_SHOWCONSOLE);
    bool is_live = true;
    while (is_live)
    {
        Welcome();
        is_live = Play(currentMode);
    }
    return 0;
}