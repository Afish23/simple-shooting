// 飞机大战
// EasyX
#include <iostream>
#include <graphics.h>//easyx
#include <vector>
#include <conio.h>
using namespace std;

constexpr auto swidth = 600;
constexpr auto sheight = 1100;

constexpr unsigned int SHP = 4;

constexpr auto hurttime = 1000;//ms

enum class Mode { AUTO, MANUAL };

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

// 一个开始界面，返回选择模式
Mode Welcome()
{
    LPCTSTR title = _T("飞机大战");
    LPCTSTR tplay_auto = _T("开始游戏（自动）");
    LPCTSTR tplay_manual = _T("开始游戏（手动）");
    LPCTSTR texit = _T("退出游戏");

    RECT tplay_auto_r, tplay_manual_r, texitr;
    BeginBatchDraw();
    setbkcolor(WHITE);
    cleardevice();
    settextstyle(60, 0, _T("黑体"));
    settextcolor(BLACK);
    outtextxy(swidth / 2 - textwidth(title) / 2, sheight / 5, title);

    settextstyle(40, 0, _T("黑体"));
    // 自动
    tplay_auto_r.left = swidth / 2 - textwidth(tplay_auto) / 2;
    tplay_auto_r.right = tplay_auto_r.left + textwidth(tplay_auto);
    tplay_auto_r.top = sheight / 5 * 2.2;
    tplay_auto_r.bottom = tplay_auto_r.top + textheight(tplay_auto);
    outtextxy(tplay_auto_r.left, tplay_auto_r.top, tplay_auto);

    // 手动
    tplay_manual_r.left = swidth / 2 - textwidth(tplay_manual) / 2;
    tplay_manual_r.right = tplay_manual_r.left + textwidth(tplay_manual);
    tplay_manual_r.top = sheight / 5 * 2.7;
    tplay_manual_r.bottom = tplay_manual_r.top + textheight(tplay_manual);
    outtextxy(tplay_manual_r.left, tplay_manual_r.top, tplay_manual);

    // 退出
    texitr.left = swidth / 2 - textwidth(texit) / 2;
    texitr.right = texitr.left + textwidth(texit);
    texitr.top = sheight / 5 * 3.2;
    texitr.bottom = texitr.top + textheight(texit);
    outtextxy(texitr.left, texitr.top, texit);

    EndBatchDraw();

    while (true)
    {
        ExMessage mess;
        getmessage(&mess, EM_MOUSE);
        if (mess.lbutton)
        {
            if (PointInRect(mess.x, mess.y, tplay_auto_r))
            {
                return Mode::AUTO;
            }
            else if (PointInRect(mess.x, mess.y, tplay_manual_r))
            {
                return Mode::MANUAL;
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
    printf_s("o");
    TCHAR* str = new TCHAR[128];
    _stprintf_s(str, 128, _T("击杀数：%llu"), kill);

    settextcolor(RED);
    outtextxy(swidth / 2 - textwidth(str) / 2, sheight / 5, str);

    // 键盘事件 （按Enter返回）
    LPCTSTR info = _T("按Enter返回");
    settextstyle(20, 0, _T("黑体"));
    outtextxy(swidth - textwidth(info), sheight - textheight(info), info);

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

// 背景、敌机、英雄、子弹

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
    Hero(IMAGE& img)
        :img(img), HP(SHP)
    {
        rect.left = swidth / 2 - img.getwidth() / 2;
        rect.top = sheight - img.getheight();
        rect.right = rect.left + img.getwidth();
        rect.bottom = sheight;
    }
    void Show()
    {
        setlinecolor(RED);
        setlinestyle(PS_SOLID, 4);
        putimage(rect.left, rect.top, &img);
        line(rect.left, rect.top - 5, rect.left + (img.getwidth() / SHP * HP), rect.top - 5);
    }
    void Control()
    {
        ExMessage mess;
        if (peekmessage(&mess, EM_MOUSE))
        {
            rect.left = mess.x - img.getwidth() / 2;
            rect.top = mess.y - img.getheight() / 2;
            rect.right = rect.right = rect.left + img.getwidth();
            rect.bottom = rect.top + img.getheight();
        }
    }

    bool hurt()
    {
        HP--;
        return (HP == 0) ? false : true;
    }

    RECT& GetRect() { return rect; }

private:
    IMAGE& img;
    RECT rect;

    unsigned int HP;

};

class Enemy
{
public:
    Enemy(IMAGE& img, int x, IMAGE*& boom)
        :img(img), isdie(false), boomsum(0)
    {
        selfboom[0] = boom[0];
        selfboom[1] = boom[1];
        selfboom[2] = boom[2];
        rect.left = x;
        rect.right = rect.left + img.getwidth();
        rect.top = -img.getheight();
        rect.bottom = 0;
    }
    bool Show()
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

    void Isdie()
    {
        isdie = true;
    }

    RECT& GetRect() { return rect; }

private:
    IMAGE& img;
    RECT rect;
    IMAGE selfboom[3];

    bool isdie;
    int boomsum;
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
    EBullet(IMAGE& img, RECT pr)
        :Bullet(img, pr)
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
};


bool AddEnemy(vector<Enemy*>& es, IMAGE& enemyimg, IMAGE* boom)
{
    Enemy* e = new Enemy(enemyimg, abs(rand()) % (swidth - enemyimg.getwidth()), boom);
    for (auto& i : es)
    {
        if (RectDuangRect(i->GetRect(), e->GetRect()))
        {
            delete e;
            return false;
        }
    }
    es.push_back(e);
    return true;

}

// mode 控制自动/手动
bool Play(Mode mode)
{
    setbkcolor(WHITE);
    cleardevice();
    bool is_play = true;

    IMAGE heroimg, enemyimg, bkimg, bulletimg;
    IMAGE eboom[3];

    loadimage(&heroimg, _T("images/me1.png"));
    loadimage(&enemyimg, _T("images/enemy1.png"));
    loadimage(&bkimg, _T("images/bk2.png"), swidth, sheight * 2);
    loadimage(&bulletimg, _T("images/bullet1.png"));

    loadimage(&eboom[0], _T("images/enemy1_down2.png"));
    loadimage(&eboom[1], _T("images/enemy1_down3.png"));
    loadimage(&eboom[2], _T("images/enemy1_down4.png"));

    BK bk = BK(bkimg);
    Hero hp = Hero(heroimg);

    vector<Enemy*> es;
    vector<Bullet*> bs;
    vector<EBullet*> ebs;

    clock_t hurtlast = clock();

    unsigned long long kill = 0;

    bool is_paused = false;  // 暂停状态标志

    int hero_bullet_timer = 0;     // 英雄自动子弹用
    int enemy_bullet_timer = 0;    // 敌机发射子弹计数器

    for (int i = 0; i < 5; i++)
    {
        AddEnemy(es, enemyimg, eboom);
    }

    while (is_play)
    {
        // 合并所有键盘事件处理
        ExMessage mess;
        bool pauseKeyTriggered = false;
        bool fireKeyTriggered = false;
        bool escKeyTriggered = false;
        while (peekmessage(&mess, EM_KEY))
        {
            if (mess.message == WM_KEYDOWN && mess.vkcode == VK_SPACE)
                pauseKeyTriggered = true;
            if (mess.message == WM_KEYDOWN && (mess.vkcode == 'Q' || mess.vkcode == 'q'))
                fireKeyTriggered = true;
            if (mess.message == WM_KEYDOWN && mess.vkcode == VK_ESCAPE)
                escKeyTriggered = true;
        }
        if (is_paused)
        {
            // 新暂停界面
            BeginBatchDraw();
            settextstyle(40, 0, _T("黑体"));
            settextcolor(RGB(0, 0, 0));
            RECT bg = { 0, sheight / 3, swidth, sheight / 3 * 2 };
            setfillcolor(RGB(240, 240, 240));
            solidrectangle(bg.left, bg.top, bg.right, bg.bottom);

            LPCTSTR pauseText = _T("游戏已暂停");
            LPCTSTR contText = _T("按空格键继续游戏");
            LPCTSTR escText = _T("按ESC键退出到主菜单");

            settextcolor(RGB(200, 50, 50));
            outtextxy(swidth / 2 - textwidth(pauseText) / 2, sheight / 2 - 60, pauseText);
            settextcolor(BLACK);
            settextstyle(30, 0, _T("黑体"));
            outtextxy(swidth / 2 - textwidth(contText) / 2, sheight / 2, contText);
            outtextxy(swidth / 2 - textwidth(escText) / 2, sheight / 2 + 40, escText);
            EndBatchDraw();

            // 只响应空格或ESC
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
                        // 退出到主菜单
                        return true;
                    }
                }
                Sleep(8);
            }
            continue; // 继续主循环
        }
        if (pauseKeyTriggered)
        {
            is_paused = !is_paused;
        }
        if (escKeyTriggered) // 直接退出到主菜单
        {
            return true;
        }

        // --- 自动/手动模式控制子弹发射 ---
        if (mode == Mode::MANUAL) {
            if (fireKeyTriggered)
                bs.push_back(new Bullet(bulletimg, hp.GetRect()));
        }
        else if (mode == Mode::AUTO) {
            hero_bullet_timer++;
            if (hero_bullet_timer >= 10) { // 约每10帧发一发
                bs.push_back(new Bullet(bulletimg, hp.GetRect()));
                hero_bullet_timer = 0;
            }
        }

        // 敌机定时发射子弹（不管什么模式都是自动）
        enemy_bullet_timer++;
        if (enemy_bullet_timer >= 60)
        {
            for (auto& i : es)
            {
                ebs.push_back(new EBullet(bulletimg, i->GetRect()));
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
                        is_play = hp.hurt();
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
                    is_play = hp.hurt();
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
        for (int i = 0; i < 5 - es.size(); i++)
        {
            AddEnemy(es, enemyimg, eboom);
        }

        EndBatchDraw();
    }
    printf_s("e");
    Over(kill);

    return true;
}

int main()
{
    // easyx初始化
    initgraph(swidth, sheight, EW_NOMINIMIZE | EW_SHOWCONSOLE);
    bool is_live = true;
    while (is_live)
    {
        Mode mode = Welcome();
        is_live = Play(mode);
    }

    return 0;
}
