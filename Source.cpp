#include <SFML/Graphics.hpp>
#include <time.h>

using namespace sf;

const int H = 21;
const int W = 19;

const int ts = 25;

int q = 0, restartTime = 2000;
bool life = true;
bool beat = false; //цвет призрака
int betime = 10000; //10сек время синих призраков

String TileMap[H] = {
"AAAAAAAAAAAAAAAAAAA",
"A1       A       2A",
"A AA AAA A AAA AA A",
"A        A        A",
"A AA A AAAAA A AA A",
"A    A   A   A    A",
"AAAA AAA A AAA AAAA",
"   A A       A A   ",
"AAAA A AAAAA A AAAA",
"       A   A       ",
"AAAA A AAAAA A AAAA",
"   A A       A A   ",
"AAAA A AAAAA A AAAA",
"A        A        A",
"A AA AAA A AAA AA A",
"A  A     C     A  A",
"AA A A AAAAA A A AA",
"A    A   A   A    A",
"A AAAAAA A AAAAAA A",
"A3               4A",
"AAAAAAAAAAAAAAAAAAA",
};
//инф о текущем кадре, позиции игрока на карте, новой позиции игрока, направлении вращения, времени игрового цикла и флаге перемещения
class Player {
public:
	float frame = 0;
	int x = 9, y = 15;
	int newx = 0, newy = 0;
	int rotate = 1, ti = 0; //поворот и задержка движения
	bool tp = false; //телепортация

	void update() { //обновляет инфу о игроке каждый игровой цикл, перемещает игрока на новую позицию
		if (tp) {
			TileMap[y][x] = ' ';

			x = 9; //координаты для работы двмжения
			y = 15;

			TileMap[y][x] = 'C';

			newx = x;
			newy = y;

			tp = false;
		}
		else {
			frame += 0.01;
			if (frame > 5)
				frame -= 5; //всего 5 кадров движения игрока

			ti++; //задержка движения
			if (ti >= 300) {
				switch (rotate)
				{
				case 1: //стрелка вправо
					if (TileMap[y][newx + 1] != 'A') //проверка на стену
						newx += 1; //перемещение
					break;
				case 2: //лево
					if (TileMap[y][newx - 1] != 'A')
						newx -= 1;
					break;
				case 3:
					if (TileMap[newy - 1][x] != 'A')
						newy -= 1;
					break;
				case 4:
					if (TileMap[newy + 1][x] != 'A')
						newy += 1;
					break;
				}

				ti = 0;
			}
			//перемещение игрока на карте
			if ((TileMap[newy][newx] == ' ' || TileMap[newy][newx] == 'S' || TileMap[newy][newx] == 'B') ||
				(beat && (TileMap[newy][newx] == '1'
					|| TileMap[newy][newx] == '2' || TileMap[newy][newx] == '3' || TileMap[newy][newx] == '4'))) { //выбираются ходы
				if (TileMap[newy][newx] == ' ' || TileMap[newy][newx] == 'S') //игрок съедает точку
					q++; //счетчик очков увеличивается

				if (TileMap[newy][newx] == 'S') //если большая таблетка, тогда призраки синие 
					beat = true;

				if (TileMap[newy][newx] == '1'
					|| TileMap[newy][newx] == '2' || TileMap[newy][newx] == '3' || TileMap[newy][newx] == '4') {
					if (beat == false) //если ложно
						life = false; //призраки не нападут
				}
				//обновляется местоположение и координаты
				TileMap[y][x] = 'B';

				TileMap[newy][newx] = 'C';

				x = newx;
				y = newy;
			}
			//дорожки терепортации
			if (newy == 9 && (newx == 0 || newx == 18)) {
				if (newx == 0) //игрок - влевую дорожку до конца, то попадает в правую
					newx = 17;
				else
					newx = 1;
				
				TileMap[y][x] = 'B'; //рисуем пустоту
				TileMap[newy][newx] = 'C';

				x = newx;
				y = newy;
			}
		}
	}
};

class Enemy { // 4 врага (двигаются рандомно)
public:
	int x[4] = { 1, 17 , 1, 17 }, y[4] = { 1, 1, 19, 19 };
	int newx[4] = { 0 , 0 , 0, 0 }, newy[4] = { 0, 0, 0, 0 };
	int rotate[4] = { 1, 1, 1, 1 }, ti = 0;
	bool tp = false;
	bool go[4]; //движение и видимость призрака

	Enemy() {
		for (int i = 0; i < 4; i++)
			go[i] = true;
	}

	void update() {
		if (tp) {
			for (int i = 0; i < 4; i++)
				TileMap[y[i]][x[i]] = ' ';

			x[0] = 1; x[1] = 17; x[2] = 1; x[3] = 17; //начальные координаты врагов
			y[0] = 1; y[1] = 1; y[2] = 19; y[3] = 19;

			TileMap[y[0]][x[0]] = '1';
			TileMap[y[1]][x[1]] = '2';
			TileMap[y[2]][x[2]] = '3';
			TileMap[y[3]][x[3]] = '4';

			for (int i = 0; i < 4; i++) { //для телепортации призраков в начальные координты после рестара
				newx[i] = x[i];
				newy[i] = y[i];
			}

			tp = false;
		}
		else {
			ti++;

			if (ti >= 300) { //задержка движения врагов
				for (int i = 0; i < 4; i++) {
					rotate[i] = rand() % 4 + 1; //рандомное движение

					newx[i] = x[i];
					newy[i] = y[i];

					switch (rotate[i])
					{
					case 1:
						if (TileMap[y[i]][newx[i] + 1] != 'A')
							newx[i] += 1;
						break;
					case 2:
						if (TileMap[y[i]][newx[i] - 1] != 'A')
							newx[i] -= 1;
						break;
					case 3:
						if (TileMap[newy[i] - 1][x[i]] != 'A')
							newy[i] -= 1;
						break;
					case 4:
						if (TileMap[newy[i] + 1][x[i]] != 'A')
							newy[i] += 1;
						break;
					}
				}

				ti = 0;
			}

			for (int i = 0; i < 4; i++)
				if (go[i]) {
					if (TileMap[newy[i]][newx[i]] == ' ' || TileMap[newy[i]][newx[i]] == 'S' || TileMap[newy[i]][newx[i]] == 'B' ||
						TileMap[newy[i]][newx[i]] == 'C') {
						if (TileMap[newy[i]][newx[i]] == 'B')
							TileMap[y[i]][x[i]] = 'B';
						else if (TileMap[newy[i]][newx[i]] == ' ')
							TileMap[y[i]][x[i]] = ' ';
						else if (TileMap[newy[i]][newx[i]] == 'S')
							TileMap[y[i]][x[i]] = 'S';
						else if (TileMap[newy[i]][newx[i]] == 'C') //сталкиваются с игроком
							if (beat == false) //могут напасть
								life = false; //умер
							else {
								TileMap[newy[i]][newx[i]] == 'B'; //если пакман съел призрака
								go[i] = false; //враг умер
							}

						if (i == 0)
							TileMap[newy[i]][newx[i]] = '1';
						if (i == 1)
							TileMap[newy[i]][newx[i]] = '2';
						if (i == 2)
							TileMap[newy[i]][newx[i]] = '3';
						if (i == 3)
							TileMap[newy[i]][newx[i]] = '4';

						x[i] = newx[i];
						y[i] = newy[i];
					}

					if (newy[i] == 9 && (newx[i] == 0 || newx[i] == 18)) {
						if (newx[i] == 0)
							newx[i] = 17;
						else
							newx[i] = 1;

						TileMap[y[i]][x[i]] = 'B';

						if (i == 0)
							TileMap[newy[i]][newx[i]] = '1';
						if (i == 1)
							TileMap[newy[i]][newx[i]] = '2';
						if (i == 2)
							TileMap[newy[i]][newx[i]] = '3';
						if (i == 3)
							TileMap[newy[i]][newx[i]] = '4';

						x[i] = newx[i];
						y[i] = newy[i];
					}
				}
		}
	}
};

class Score {
public:
	Sprite sec[3];
	int s[3], c;
	bool vid[3];

	Score(Texture& image) {
		for (int i = 0; i < 3; i++) {
			sec[i].setTexture(image);

			sec[i].setPosition(10 + 22 * i, H * ts + 10);

			sec[i].setTextureRect(IntRect(0, 0, 22, 45));

			s[i] = 0;
			c = 0;
		}
	}

	void update() {
		if (c < 10) {
			s[0] = c;

			vid[0] = true;
			vid[1] = false;
			vid[2] = false;
		}
		else if (c >= 10 && c < 100) {
			s[0] = c / 10;
			s[1] = c % 10;

			vid[0] = true;
			vid[1] = true;
			vid[2] = false;
		}
		else if (c >= 100 && c < 1000) {
			s[0] = c / 100;
			s[1] = (c / 10) % 10;
			s[2] = (c % 100) % 10;

			vid[0] = true;
			vid[1] = true;
			vid[2] = true;
		}

		for (int i = 0; i < 3; i++)
			sec[i].setTextureRect(IntRect(22 * s[i], 0, 22, 45));
	}
};

void del() { //чистим карту, убираем точки на опр координатах
	for (int i = 0; i <= 2; i++)
		for (int j = 0; j <= 2; j++) {
			for (int x = 0; x < 2; x++) {
				TileMap[7 + j * 2][i + 16 * x] = 'B';

				TileMap[9][3 + 12 * x] = 'B';
			}

			TileMap[9][8 + j] = 'B';
		}

	for (int i = 0; i < 2; i++) {
		TileMap[2][1 + 16 * i] = 'S'; //большая таблетка
		TileMap[15][1 + 16 * i] = 'S';
	}
}

int main() {
	srand(time(0));
	RenderWindow window(VideoMode(W * ts, H * ts + 3 * ts), "Maze!");

	Texture t;
	t.loadFromFile("C:/Users/Ирина/OneDrive/Рабочий стол/Paint/title.png");
	Sprite plat(t);

	Texture yw;
	yw.loadFromFile("C:/Users/Ирина/OneDrive/Рабочий стол/Paint/youwin.png");
	Sprite youwin(yw);
	youwin.setPosition(100, 210);

	Texture yl;
	yl.loadFromFile("C:/Users/Ирина/OneDrive/Рабочий стол/Paint/youlose.png");
	Sprite youlose(yl);
	youlose.setPosition(100, 210);

	Player p;
	Enemy en;

	Texture sc;
	sc.loadFromFile("C:/Users/Ирина/OneDrive/Рабочий стол/Paint/score.png");
	Score score(sc);

	del();

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();

			if (q < 171 && life)
				if (event.type == Event::KeyPressed) {
					p.newx = p.x;
					p.newy = p.y;

					if (event.key.code == Keyboard::Right)
						p.rotate = 1;
					if (event.key.code == Keyboard::Left)
						p.rotate = 2;
					if (event.key.code == Keyboard::Up)
						p.rotate = 3;
					if (event.key.code == Keyboard::Down)
						p.rotate = 4;
				}
		}

		if ((q == 171 || !life) && restartTime != 0) { //создаем рестар для вин и лус
			restartTime--;

			if (restartTime == 0) {
				for (int i = 0; i < H; i++)
					for (int j = 0; j < W; j++)
						if (TileMap[i][j] == 'B')
							TileMap[i][j] = ' '; //таблетка

				p.tp = true;
				en.tp = true;

				del();

				q = 0;
				life = true;
				restartTime = 2000;

				betime = 10000;
				beat = false;
			}
		}

		score.c = q;
		//нападение на призраков
		if (beat) {
			betime--;

			if (betime == 0) {
				for (int i = 0; i < 4; i++) {
					en.go[i] = true; //разрешаем двигаться

					if (i == 0)
						TileMap[en.y[0]][en.x[0]] = '1';
					if (i == 1)
						TileMap[en.y[1]][en.x[1]] = '2';
					if (i == 2)
						TileMap[en.y[2]][en.x[2]] = '3';
					if (i == 3)
						TileMap[en.y[3]][en.x[3]] = '4';
				}

				betime = 10000;
				beat = false;
			}
		}

		if (!life) {
			for (int i = 0; i < H; i++)
				for (int j = 0; j < W; j++)
					if (TileMap[i][j] != 'A')
						TileMap[i][j] = 'B';
		}

		if (q < 171 && life) {
			p.update();
			en.update();
		}
		score.update();
		window.clear(Color::Black); //черная консоль 
		//прорисовка карты
		for (int i = 0; i < H; i++)
			for (int j = 0; j < W; j++) {
				if (TileMap[i][j] == 'A') //рисуем стену
					plat.setTextureRect(IntRect(0, 0, ts, ts));
				if (TileMap[i][j] == 'C') //рисуем игрока
					plat.setTextureRect(IntRect(ts * int(p.frame), ts * p.rotate, ts, ts));
				if (TileMap[i][j] == ' ') //рисуем точку баллов
					plat.setTextureRect(IntRect(ts, 0, ts, ts));
				if (TileMap[i][j] == 'S')
					plat.setTextureRect(IntRect(ts * 2, 0, ts, ts));
				if (!beat) { //красные призраки
					if (TileMap[i][j] == '1')
						plat.setTextureRect(IntRect(ts * 5, ts * en.rotate[0], ts, ts));
					if (TileMap[i][j] == '2')
						plat.setTextureRect(IntRect(ts * 5, ts * en.rotate[1], ts, ts));
					if (TileMap[i][j] == '3')
						plat.setTextureRect(IntRect(ts * 5, ts * en.rotate[2], ts, ts));
					if (TileMap[i][j] == '4')
						plat.setTextureRect(IntRect(ts * 5, ts * en.rotate[3], ts, ts));
				}
				else { //синие
					if (TileMap[i][j] == '1')
						plat.setTextureRect(IntRect(ts * 5, 0, ts, ts));
					if (TileMap[i][j] == '2')
						plat.setTextureRect(IntRect(ts * 5, 0, ts, ts));
					if (TileMap[i][j] == '3')
						plat.setTextureRect(IntRect(ts * 5, 0, ts, ts));
					if (TileMap[i][j] == '4')
						plat.setTextureRect(IntRect(ts * 5, 0, ts, ts));
				}
				if (TileMap[i][j] == 'B')
					continue;

				plat.setPosition(j * ts, i * ts); //для просмотра результата карты
				window.draw(plat);
			}

		if (q == 171) //максимум очков - победа
			window.draw(youwin);
		if (!life)
			window.draw(youlose);

		for (int i = 0; i < 3; i++)
			if (score.vid[i])
				window.draw(score.sec[i]);

		window.display();
	}

	return 0;
}