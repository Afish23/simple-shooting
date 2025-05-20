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

// 一个开始界面
void Welcome()
{
	LPCTSTR title = _T("飞机大战");
	LPCTSTR tplay = _T("开始游戏");
	LPCTSTR texit = _T("退出游戏");

	RECT tplayr, texitr;
	BeginBatchDraw();
	setbkcolor(WHITE);
	cleardevice();
	settextstyle(60, 0, _T("黑体"));
	settextcolor(BLACK);
	outtextxy(swidth / 2 - textwidth(title) / 2, sheight / 5, title);

	settextstyle(40, 0, _T("黑体"));
	tplayr.left = swidth / 2 - textwidth(tplay) / 2;
	tplayr.right = tplayr.left + textwidth(tplay);
	tplayr.top = sheight / 5 * 2.5;
	tplayr.bottom = tplayr.top + textheight(tplay);

	texitr.left = swidth / 2 - textwidth(texit) / 2;
	texitr.right = texitr.left + textwidth(texit);
	texitr.top = sheight / 5 * 3;
	texitr.bottom = texitr.top + textheight(texit);

	outtextxy(tplayr.left, tplayr.top, tplay);
	outtextxy(texitr.left, texitr.top, texit);

	EndBatchDraw();

	while (true)
	{
		ExMessage mess;
		getmessage(&mess, EM_MOUSE);
		if (mess.lbutton)
		{
			if (PointInRect(mess.x, mess.y, tplayr))
			{
				return;
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

bool Play()
{
	setbkcolor(WHITE);        // 设置背景颜色为白色
	cleardevice();            // 清除屏幕

	bool is_play = true;      // 游戏运行标志

	// 加载各种图像资源
	IMAGE heroimg, enemyimg, bkimg, bulletimg;
	IMAGE eboom[3];           // 爆炸动画图片数组

	loadimage(&heroimg, _T("images/me1.png"));              // 加载英雄飞机图片
	loadimage(&enemyimg, _T("images/enemy1.png"));          // 加载敌机图片
	loadimage(&bkimg, _T("images/bk2.png"), swidth, sheight * 2);  // 加载背景图片
	loadimage(&bulletimg, _T("images/bullet1.png"));        // 加载子弹图片

	loadimage(&eboom[0], _T("images/enemy1_down2.png"));    // 加载爆炸动画第1帧
	loadimage(&eboom[1], _T("images/enemy1_down3.png"));    // 加载爆炸动画第2帧
	loadimage(&eboom[2], _T("images/enemy1_down4.png"));    // 加载爆炸动画第3帧

	BK bk = BK(bkimg);         // 创建背景对象
	Hero hp = Hero(heroimg);  // 创建英雄飞机对象

	vector<Enemy*> es;         // 敌机集合
	vector<Bullet*> bs;        // 子弹集合
	vector<EBullet*> ebs;      // 敌方子弹集合
	int bsing = 0;             // 子弹计时器

	clock_t hurtlast = clock();  // 上次受伤时间

	unsigned long long kill = 0; // 击杀计数

	// 初始生成5架敌机
	for (int i = 0; i < 5; i++)
	{
		AddEnemy(es, enemyimg, eboom);
	}

	while (is_play)
	{
		bsing++;

		// 每隔一定时间发射一颗子弹
		if (bsing % 10 == 0)
		{
			bs.push_back(new Bullet(bulletimg, hp.GetRect()));
		}

		// 每秒让所有敌机发射一次子弹
		if (bsing == 60)
		{
			bsing = 0;
			for (auto& i : es)
			{
				ebs.push_back(new EBullet(bulletimg, i->GetRect()));
			}
		}

		BeginBatchDraw();       // 开始批量绘图

		bk.Show();              // 绘制背景
		Sleep(2);               // 短暂延迟
		flushmessage();         // 清空消息队列
		Sleep(2);               // 短暂延迟
		hp.Control();           // 处理英雄飞机控制

		// 处理空格键暂停功能
		if (_kbhit())
		{
			char v = _getch();
			if (v == 0x20)
			{
				Sleep(500);
				while (true)
				{
					if (_kbhit())
					{
						v = _getch();
						if (v == 0x20)
						{
							break;
						}
					}
					Sleep(16);
				}
			}
		}

		hp.Show();              // 绘制英雄飞机

		// 更新和绘制所有子弹
		auto bsit = bs.begin();
		while (bsit != bs.end())
		{
			if (!(*bsit)->Show())
			{
				bsit = bs.erase(bsit);
			}
			else
			{
				bsit++;
			}
		}

		// 更新和绘制所有敌方子弹，并检测与英雄飞机的碰撞
		auto ebsit = ebs.begin();
		while (ebsit != ebs.end())
		{
			if (!(*ebsit)->Show())
			{
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

		// 更新和绘制所有敌机，检测碰撞和击毁
		auto it = es.begin();
		while (it != es.end())
		{
			// 检测敌机与英雄飞机的碰撞
			if (RectDuangRect((*it)->GetRect(), hp.GetRect()))
			{
				if (clock() - hurtlast >= hurttime)
				{
					is_play = hp.hurt();
					hurtlast = clock();
				}
			}

			// 检测敌机与子弹的碰撞
			auto bit = bs.begin();
			while (bit != bs.end())
			{
				if (RectDuangRect((*bit)->GetRect(), (*it)->GetRect()))
				{
					(*it)->Isdie();          // 标记敌机为死亡
					delete (*bit);           // 删除子弹
					bs.erase(bit);           // 从列表中移除

					kill++;                  // 击杀数增加

					break;
				}
				bit++;
			}

			// 更新敌机状态，如果敌机超出屏幕则删除
			if (!(*it)->Show())
			{
				delete (*it);
				es.erase(it);
				it = es.begin();
			}
			it++;
		}

		// 补充敌机数量至5架
		for (int i = 0; i < 5 - es.size(); i++)
		{
			AddEnemy(es, enemyimg, eboom);
		}

		EndBatchDraw();         // 结束批量绘图
	}

	printf_s("e");             // 调试输出
	Over(kill);                // 显示游戏结束画面

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
		// Play
		is_live = Play();
	}

	return 0;
}