// 飞机大战
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
Mode currentMode = Mode::AUTO; // 默认自动模式

void ShowRecords()
{
    BeginBatchDraw();
    cleardevice();

    settextstyle(40, 0, _T("黑体"));
    settextcolor(BLUE);
    const TCHAR* title = _T("历史击杀记录");
    int textWidth = textwidth(title);
    int xTitle = (getwidth() - textWidth) / 2;
    outtextxy(xTitle, 80, title);

    std::ifstream fin("kill_records.txt");
    if (!fin.is_open())
    {
        settextstyle(40, 0, _T("黑体"));
        settextcolor(RED);
        const TCHAR* noRecordMsg = _T("暂无击杀记录");
        int noRecordTextWidth = textwidth(noRecordMsg);
        int xNoRecord = (getwidth() - noRecordTextWidth) / 2;

        outtextxy(xNoRecord, 180, noRecordMsg);
    }
    else
    {
        std::vector<unsigned long long> records;
        unsigned long long kill;
        int count = 0;

        // 读取所有击杀记录
        while (fin >> kill)
        {
            records.push_back(kill);
        }
        fin.close();

        // 对击杀记录进行从高到低排序
        std::sort(records.begin(), records.end(), [](unsigned long long a, unsigned long long b) {
            return a > b; // 按降序排列
            });

        settextstyle(24, 0, _T("黑体"));
        settextcolor(BLACK);

        // 显示排序后的击杀记录
        for (size_t i = 0; i < records.size() && i < 10; ++i)
        {
            TCHAR line[64];
            _stprintf_s(line, _T("第 %d 名：击杀数 %llu"), i + 1, records[i]);
            outtextxy((swidth - textwidth(line)) / 2, 180 + i * 30, line);
        }

        if (records.empty())
        {
            settextstyle(40, 0, _T("黑体"));
            settextcolor(RED);
            const TCHAR* noRecordMsg = _T("暂无击杀记录");
            int noRecordTextWidth = textwidth(noRecordMsg);
            int xNoRecord = (getwidth() - noRecordTextWidth) / 2;

            outtextxy(xNoRecord, 180, noRecordMsg);
        }
    }



    // 键盘事件 （按Enter返回）
    LPCTSTR info = _T("按Enter返回");
    settextstyle(30, 0, _T("黑体")); // 字小一些
    settextcolor(BLACK);
    // 绘制提示文字（下方一点）
    outtextxy(swidth / 2 - textwidth(info) / 2, sheight / 5 + 500, info);

    EndBatchDraw();

    // 等待用户按 Enter 返回
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


    LPCTSTR title = _T("飞机大战");
    LPCTSTR tplay = _T("开始游戏");
    LPCTSTR trecord = _T("击杀记录");
    LPCTSTR texit = _T("退出游戏");


    RECT tplay_r, trecordr, texitr;
    BeginBatchDraw();
    setbkcolor(RGB(195, 200, 201));
    cleardevice();
    putimage(0, 0, &bkimg);
    settextstyle(60, 0, _T("黑体"));
    settextcolor(BLACK);
    outtextxy(swidth / 2 - textwidth(title) / 2, sheight / 5, title);

    settextstyle(40, 0, _T("黑体"));
    // 开始游戏
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

    // 退出
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
                currentMode = Mode::AUTO; // 设置为自动模式
                return; // 直接返回
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
    // 准备提示文本
    LPCTSTR info1 = _T("GAME OVER");

    // 准备击杀数文本
    TCHAR* str = new TCHAR[128];
    _stprintf_s(str, 128, _T("击杀数：%llu"), kill);

    // 准备提示文本
    LPCTSTR info2 = _T("按Enter返回");

    BeginBatchDraw();
    setbkcolor(RGB(195, 200, 201));
    cleardevice();
    putimage(0, 0, &bkimg);

    // 更改字体大小用于提示文字
    settextstyle(60, 0, _T("黑体")); // 字小一些
    settextcolor(RED);
    // 绘制提示文字（下方一点）
    outtextxy(swidth / 2 - textwidth(info1) / 2, sheight / 5, info1);

    // 设置字体样式
    settextstyle(40, 0, _T("黑体"));
    settextcolor(RED);
    // 绘制击杀数（顶部）
    outtextxy(swidth / 2 - textwidth(str) / 2, sheight / 5 + textheight(str) + 60, str);

    // 更改字体大小用于提示文字
    settextstyle(30, 0, _T("黑体")); // 字小一些
    settextcolor(BLACK);
    // 绘制提示文字（下方一点）
    outtextxy(swidth / 2 - textwidth(info2) / 2, sheight / 5 + textheight(str) + 240, info2);

    EndBatchDraw();

    // 将击杀数写入文件
    ofstream fout("kill_records.txt", ios_base::app); // 追加模式
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
    int damage1=1,damage2=1;//1是子弹伤害，2是碰撞伤害
public:
    virtual ~Enemy() = default; // 必须有虚析构
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
            Isdie(); // 标记为死亡
            return false; // 返回false表示敌人死亡
        }

        return true; // 还活着
    }
};

class NormalEnemy : public Enemy {
public:
    NormalEnemy(IMAGE& img, int x, IMAGE* boom)
        : Enemy(img, x, boom) {
        // 派生类中修改属性
        SetHP(1);
        SetDamage1(1);
        SetDamage2(1);
    }
};
class TankEnemy : public Enemy {
public:
    TankEnemy(IMAGE& img, int x, IMAGE* boom)
        : Enemy(img, x, boom) {
        SetHP(3);  // 高血量
        SetDamage1(1);
        SetDamage2(2);  // 碰撞伤害更高
    }
};

// 4. 敏捷敌人类（快速移动）
class AgileEnemy : public Enemy {
public:
    AgileEnemy(IMAGE& img, int x, IMAGE* boom)
        : Enemy(img, x, boom) {
        SetHP(2);
        SetDamage1(2);  // 子弹伤害更高
        SetDamage2(1);
    }

   /* bool Show() override {
        if (isdie) {
            if (boomsum == 5) {  // 使用动态帧数量
                return false;
            }
            putimage(rect.left, rect.top, boom + boomsum);  // 直接使用传入的爆炸动画
            boomsum++;
            return true;
        }
        // 移动速度是普通敌人的1.5倍
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
    int damage; // 子弹伤害值
};

// 支持不同类型敌机及对应图片的AddEnemy函数
bool AddEnemy(vector<Enemy*>& es, int enemyType,
    IMAGE& normalImg, IMAGE& tankImg, IMAGE& agileImg,
    IMAGE* normalBoom, IMAGE* tankBoom, IMAGE* agileBoom) {
    Enemy* e = nullptr;

    // 根据敌机类型创建对应的派生类并加载对应图片
    switch (enemyType) {
    case 0: // 普通敌机
        e = new NormalEnemy(normalImg,
            abs(rand()) % (swidth - normalImg.getwidth()), normalBoom);
        break;
    case 2: // 坦克敌机
        e = new TankEnemy(tankImg,
            abs(rand()) % (swidth - tankImg.getwidth()), tankBoom);
        break;
    case 1: // 敏捷敌机
        e = new AgileEnemy(agileImg,
            abs(rand()) % (swidth - agileImg.getwidth()), agileBoom);
        break;
    default:
        return false;
    }

    // 碰撞检测
    for (auto& existingEnemy : es) {
        if (RectDuangRect(existingEnemy->GetRect(), e->GetRect())) {
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

    IMAGE heroimg, bkimg, bulletimg, Ebulletimg;
    IMAGE normalEnemyImg, tankEnemyImg, agileEnemyImg;  // 三种敌机图片
    IMAGE eboom1[3], meboom[3], eboom2[5], eboom3[5];  // 爆炸动画

    // 加载三种敌机图片
    loadimage(&normalEnemyImg, _T("images/enemy1.png"));   // 普通敌机
    loadimage(&tankEnemyImg, _T("images/enemy3.png"));    // 坦克敌机（肉盾）
    loadimage(&agileEnemyImg, _T("images/enemy2.png"));   // 敏捷敌机

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

    bool is_paused = false;  // 暂停状态标志

    int hero_bullet_timer = 0;     // 英雄自动子弹用
    int enemy_bullet_timer = 0;    // 敌机发射子弹计数器

    for (int i = 0; i < 5; i++) {
        AddEnemy(es, 0, normalEnemyImg, tankEnemyImg, agileEnemyImg,
            eboom1, eboom3, eboom2);
    }

    Mode currentMode = mode; // 新增，游戏内当前模式

    while (is_play)
    {
        // 合并所有键盘事件处理
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
            // 新暂停界面
            BeginBatchDraw();


            settextstyle(40, 0, _T("黑体"));
            settextcolor(RGB(0, 0, 0));
            RECT bg = { 0, 0, swidth, sheight * 2 };
            setfillcolor(RGB(195, 200, 201));
            solidrectangle(bg.left, bg.top, bg.right, bg.bottom);

            LPCTSTR pauseText = _T("游戏已暂停");
            LPCTSTR contText = _T("按空格键继续游戏");
            LPCTSTR escText = _T("按ESC键退出到主菜单");

            // 显示当前模式
            LPCTSTR modeText = (currentMode == Mode::AUTO) ? _T("当前模式：自动") : _T("当前模式：手动");
            LPCTSTR tabText = _T("按Tab键切换模式");

            settextcolor(RGB(200, 50, 50));
            setbkcolor(RGB(195, 200, 201));

            outtextxy(swidth / 2 - textwidth(pauseText) / 2, sheight / 2 - 100, pauseText);
            settextcolor(BLACK);
            setbkcolor(RGB(195, 200, 201));
            settextstyle(30, 0, _T("黑体"));
            outtextxy(swidth / 2 - textwidth(contText) / 2, sheight / 2 - 20, contText);
            outtextxy(swidth / 2 - textwidth(escText) / 2, sheight / 2 + 30, escText);

            settextstyle(25, 0, _T("黑体"));
            settextcolor(RGB(50, 100, 200));
            setbkcolor(RGB(195, 200, 201));
            outtextxy(swidth / 2 - textwidth(modeText) / 2, sheight / 2 + 75, modeText);
            settextcolor(RGB(60, 60, 60));
            setbkcolor(RGB(195, 200, 201));
            outtextxy(swidth / 2 - textwidth(tabText) / 2, sheight / 2 + 115, tabText);

            EndBatchDraw();

            // 只响应空格或ESC或Tab
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
                    if (pause_mess.message == WM_KEYDOWN && pause_mess.vkcode == VK_TAB)
                    {
                        // 切换模式
                        if (currentMode == Mode::AUTO) currentMode = Mode::MANUAL;
                        else currentMode = Mode::AUTO;
                        // 刷新暂停界面
                        BeginBatchDraw();
                        RECT bg = { 0, 0, swidth, sheight * 2 };

                        setbkcolor(RGB(195, 200, 201));
                        settextstyle(40, 0, _T("黑体"));
                        settextcolor(RGB(0, 0, 0));
                        setfillcolor(RGB(195, 200, 201));
                        solidrectangle(bg.left, bg.top, bg.right, bg.bottom);

                        settextcolor(RGB(200, 50, 50));
                        outtextxy(swidth / 2 - textwidth(pauseText) / 2, sheight / 2 - 100, pauseText);
                        settextcolor(BLACK);
                        settextstyle(30, 0, _T("黑体"));
                        outtextxy(swidth / 2 - textwidth(contText) / 2, sheight / 2 - 20, contText);
                        outtextxy(swidth / 2 - textwidth(escText) / 2, sheight / 2 + 30, escText);

                        // 显示切换后的模式
                        LPCTSTR newModeText = (currentMode == Mode::AUTO) ? _T("当前模式：自动") : _T("当前模式：手动");

                        settextstyle(25, 0, _T("黑体"));
                        settextcolor(RGB(50, 100, 200));
                        outtextxy(swidth / 2 - textwidth(newModeText) / 2, sheight / 2 + 75, newModeText);
                        settextcolor(RGB(60, 60, 60));
                        outtextxy(swidth / 2 - textwidth(tabText) / 2, sheight / 2 + 115, tabText);
                        EndBatchDraw();
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
        if (currentMode == Mode::MANUAL) {
            if (hp.isalive()&&fireKeyTriggered)
                bs.push_back(new Bullet(bulletimg, hp.GetRect()));
        }
        else if (currentMode == Mode::AUTO) {
            hero_bullet_timer++;
            if (hp.isalive()&&hero_bullet_timer >= 20) { // 约每10帧发一发
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

        // 显示当前击杀数
        TCHAR killStr[64];
        _stprintf_s(killStr, _T("击杀数：%llu"), kill);
        setbkcolor(RGB(195, 200, 201));
        settextstyle(24, 0, _T("黑体"));
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
                    int da = (*it)->GetDamage2(); // 获取敌人的攻击力
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
                enemyType = 0;  // 普通敌机
            }
            else if (kill < 20) {
                enemyType = rand() % 2;  // 普通或坦克
            }
            else {
                enemyType = rand() % 3;  // 普通、坦克、敏捷
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
    // easyx初始化
    initgraph(swidth, sheight, EW_NOMINIMIZE | EW_SHOWCONSOLE);
    bool is_live = true;
    while (is_live)
    {
        Welcome();
        is_live = Play(currentMode);
    }
    return 0;
}