#include "framework.h"
#include "Player.h"
#include "Map.h"
#include "Sound.h"

#define HEIGHT 600
#define WIDTH 800
<<<<<<< HEAD
#define TEXT_GAP 200
=======

// for TCP
//#define Multi
#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000

<<<<<<< HEAD
=======
>>>>>>> e42000de1409db1cb2bf1ee0bea795e85c46f5a8

>>>>>>> a1ac0573eca3e0fe2327b50804531780ad1d6920
>>>>>>> 8382e9450833c8af6992653b2daef1882ba3adbb
using namespace std;

const float length = 0.5;

char* arr;

<<<<<<< HEAD
int GameState = 3;
=======
int GameState = 2;
#ifdef Multi
GameState = 0;
#endif // Multi



>>>>>>> a1ac0573eca3e0fe2327b50804531780ad1d6920

GLuint vertexShader;
GLuint fragmentShader;

GLuint ShaderProgram;

float ambient = 0.6f;

Player player1;
#ifdef Multi
vector<Player> p;
#endif

Map m;

Data* dat;
Player_data* pd;
vector<Cube_data> cd;
Game_Communication_Data gcd;

float Rotate = 0;

int cnt = 0;

int ip_number_len = 0;
vector<string> words;

SOCKET sock;


void PD_print(Player_data* pd)
{
	cout << "x " << pd->PosVec.x << "y " << pd->PosVec.y << "z " << pd->PosVec.z << endl << "speed " << pd->speed << endl;
}

// Data print function for check
/*void D_print(Data* d)
{
	Player_data* pd = d->PlayerData;
	PD_print(pd);
}*/

Player_data* PD_pack_data(Player p)
{
	Player_data* pd = new Player_data;
	pd->KeyDownlist = p.getKey();
	pd->PosVec = p.getPosition();
	pd->speed = p.getSpeed();
	return pd;
}



//Cube_data* CD_pack_data(Cube c)
//{
//	
//}

Data* pack_data(Player_data* pd, Cube_data* cd)
{
	Data* d = new Data;
	d->PlayerData = *pd;
	return d;
}

void glutPrint(float x, float y, LPVOID font, string text)
{
	glWindowPos2f(x, y);

	for (int i = 0; i < text.size(); i++)
	{
		glutBitmapCharacter(font, text[i]);
	}
}

int CalculateFrameRate()
{
	static float framesPerSecond = 0.0f;
	static int fps;
	static float lastTime = 0.0f;
	float currentTime = GetTickCount() * 0.001f;
	++framesPerSecond;
	if (currentTime - lastTime > 1.0f)
	{
		lastTime = currentTime;
		fps = (int)framesPerSecond;
		framesPerSecond = 0;
	}

	return fps;
}

void convertXY(int w, int h, int x, int y, float& ox, float& oy)
{
	ox = (float)(x - (float)w / 2.0) * (float)(1.0 / (float)(w / 2.0));
	oy = -(float)(y - (float)h / 2.0) * (float)(1.0 / (float)(h / 2.0));
}

GLchar* filetobuf(const char* filename)
{
	string tmp;
	string Source;
	ifstream readFile;

	readFile.open(filename);

	readFile.seekg(0, ios::end);
	Source.reserve(readFile.tellg());
	readFile.seekg(0, ios::beg);

	Source.assign((istreambuf_iterator<char>(readFile)), istreambuf_iterator<char>());

	arr = new char[Source.length() + 2];

	strcpy_s(arr, Source.length() + 1, Source.c_str());

	arr[Source.length() + 1] = '\0';

	return arr;
}

void make_vertexShaders()
{
	GLchar* vertexShaderSource = filetobuf("Vertex.glsl");

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	delete[] arr;

	GLint result;
	GLchar errorLog[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		cerr << "ERROR: vertex shader error!\n" << errorLog << endl;
		return;
	}
}

void make_fragmentShaders()
{
	GLchar* fragmentShaderSource = filetobuf("Fragment.glsl");;

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	delete[] arr;

	GLint result;
	GLchar errorLog[512];

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		cerr << "ERROR: fragment shader error!\n" << errorLog << endl;
		return;
	}
}

void InitShader()
{
	make_vertexShaders();
	make_fragmentShaders();

	//-- shader Program
	ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, vertexShader);
	glAttachShader(ShaderProgram, fragmentShader);
	glLinkProgram(ShaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glUseProgram(ShaderProgram);
}

GLvoid drawScene()
{
	if (GameState == 0) //게임 시작
	{
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(ShaderProgram);

		glm::vec3 lc = glm::vec3(0.5f, 0.5f, 0.5f);

		unsigned int LightColorLocation = glGetUniformLocation(ShaderProgram, "lightColor");
		glUniform3fv(LightColorLocation, 1, glm::value_ptr(lc));

		glm::vec3 cp = player1.getCamera().getPosition();

		unsigned int viewPosLocation = glGetUniformLocation(ShaderProgram, "viewPos");
		glUniform3fv(viewPosLocation, 1, glm::value_ptr(cp));

		m.Render(ShaderProgram);
		player1.Render(ShaderProgram);
#ifdef Multi
		for (auto i = p.begin(); i != p.end(); ++i)
		{
			i->Render(ShaderProgram);
		}
#endif
		string score = "Score : ";
		score += std::to_string((int)player1.getPosition().z);
		glutPrint(700.0f, 580.0f, GLUT_BITMAP_HELVETICA_18, score);

		string speed = "Speed : ";
		speed += std::to_string((int)(player1.getSpeed() * 500)) + "km/h";
		glutPrint(0.0f, 0.0f, GLUT_BITMAP_HELVETICA_18, speed);

		string life = "Life : ";
		life += std::to_string(player1.getLife());
		glutPrint(0.0f, 580.0f, GLUT_BITMAP_HELVETICA_18, life);

		glutSwapBuffers();
	}
	else if (GameState == 1) //게임종료
	{
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glutPrint(420.0f, 550.0f, GLUT_BITMAP_TIMES_ROMAN_24, "GAME OVER");
		glutPrint(WIDTH / 3.0f, HEIGHT / 2.0f, GLUT_BITMAP_TIMES_ROMAN_24, "GAME OVER");
		glutPrint(WIDTH / 3.0f, HEIGHT / 3.0f, GLUT_BITMAP_TIMES_ROMAN_24, "Press R to CONTINUE");

		glutSwapBuffers();
	}
	else if (GameState == 2) // 타이틀 씬
	{

		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glutPrint(420.0f, 550.0f, GLUT_BITMAP_TIMES_ROMAN_24, "GAME OVER");
		glutPrint(WIDTH / 2.5f, HEIGHT / 1.5f, GLUT_BITMAP_TIMES_ROMAN_24, "Tube Racing");

		glutPrint(WIDTH / 3.0f, HEIGHT / 3.0f, GLUT_BITMAP_HELVETICA_18, "ENTER IP : ");

		for (int i = 0; i < ip_number_len; i++) {
			glutPrint(WIDTH / 3.0f + 100 + 10 * i, HEIGHT / 3.0f, GLUT_BITMAP_HELVETICA_18, words[i]);
		}

		glutSwapBuffers();

	}
	else if (GameState == 3) // 로비 씬
	{
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glutPrint(WIDTH / 2.5f-25, HEIGHT / 1.5f +100, GLUT_BITMAP_TIMES_ROMAN_24, "Waiting for players...");
		//플레이어 1
		glutPrint(WIDTH / 5.5f, HEIGHT / 2.0f, GLUT_BITMAP_HELVETICA_18, "----------");//상단
		glutPrint(WIDTH / 5.5f, HEIGHT / 1.5f, GLUT_BITMAP_HELVETICA_18, "----------");
		for (int i = 0; i < 7; i++) {
			glutPrint(WIDTH / 5.5f, HEIGHT / 1.5f -14*i-8 , GLUT_BITMAP_HELVETICA_18, "I");
		}
		for (int i = 0; i < 7; i++) {
			glutPrint(WIDTH / 3.2f, HEIGHT / 1.5f - 14 * i - 8, GLUT_BITMAP_HELVETICA_18, "I");
		}
		if (gcd.Players_Pt[0]) {
			glutPrint(WIDTH / 5.5f+20, HEIGHT / 1.7f, GLUT_BITMAP_HELVETICA_18, "Player 1 ");
		}

		//플레이어 2
		glutPrint(WIDTH / 5.5f+ TEXT_GAP, HEIGHT / 2.0f, GLUT_BITMAP_HELVETICA_18, "----------");//상단
		glutPrint(WIDTH / 5.5f + TEXT_GAP, HEIGHT / 1.5f, GLUT_BITMAP_HELVETICA_18, "----------");
		for (int i = 0; i < 7; i++) {
			glutPrint(WIDTH / 5.5f + TEXT_GAP, HEIGHT / 1.5f - 14 * i - 8, GLUT_BITMAP_HELVETICA_18, "I");
		}
		for (int i = 0; i < 7; i++) {
			glutPrint(WIDTH / 3.2f + TEXT_GAP, HEIGHT / 1.5f - 14 * i - 8, GLUT_BITMAP_HELVETICA_18, "I");
		}
		if (gcd.Players_Pt[1]) {
			glutPrint(WIDTH / 5.5f + 20 + TEXT_GAP , HEIGHT / 1.7f, GLUT_BITMAP_HELVETICA_18, "Player 2 ");
		}

		//플레이어 3
		glutPrint(WIDTH / 5.5f + TEXT_GAP*2, HEIGHT / 2.0f, GLUT_BITMAP_HELVETICA_18, "----------");//상단
		glutPrint(WIDTH / 5.5f + TEXT_GAP*2, HEIGHT / 1.5f, GLUT_BITMAP_HELVETICA_18, "----------");
		for (int i = 0; i < 7; i++) {
			glutPrint(WIDTH / 5.5f + TEXT_GAP*2, HEIGHT / 1.5f - 14 * i - 8, GLUT_BITMAP_HELVETICA_18, "I");
		}
		for (int i = 0; i < 7; i++) {
			glutPrint(WIDTH / 3.2f + TEXT_GAP * 2, HEIGHT / 1.5f - 14 * i - 8, GLUT_BITMAP_HELVETICA_18, "I");
		}
		if (gcd.Players_Pt[2]) {
			glutPrint(WIDTH / 5.5f + 20+ TEXT_GAP * 2, HEIGHT / 1.7f, GLUT_BITMAP_HELVETICA_18, "Player 3 ");
		}
<<<<<<< HEAD

		//ready
		for (int i = 0; i < 3; i++) {
			if (gcd.Players_Ready[i]) {
				glutPrint(WIDTH / 5.5f+ TEXT_GAP*i, HEIGHT / 2.2f, GLUT_BITMAP_HELVETICA_18, "Ready");
			}
		}
		
=======
		glutPrint(320.0f, 350.0f, GLUT_BITMAP_TIMES_ROMAN_24, "GAME OVER");
		glutPrint(270.0f, 200.0f, GLUT_BITMAP_TIMES_ROMAN_24, "Press R to CONTINUE");
>>>>>>> a1ac0573eca3e0fe2327b50804531780ad1d6920

		glutSwapBuffers();
	}
}

GLvoid Timer(int Value)
{
	if (GameState == 1)
	{
		SoundManager::sharedManager()->play(OVER_SOUND);
		SoundManager::sharedManager()->stop(BACKGROUND_SOUND);
		SoundManager::sharedManager()->stop(DRIVE_SOUND);
		return;
	}

	if (GameState == 0)
	{
		float pz = player1.getPosition().z;
		float fpz = 0.0f;
		float spz = 10000000.0f;

		vector<Player>::iterator fastest_player_iter = p.begin();
		vector<Player>::iterator slowest_player_iter = p.begin();
		for (auto i = p.begin(); i != p.end(); ++i)
		{
			fpz = max(fpz, i->getPosition().z);
			spz = min(spz, i->getPosition().z);
		}

		m.Fastest_Update(pz);
		m.Slowest_Update(pz);

		//m.Fastest_Update(fpz);
		//m.Slowest_Update(spz);

		player1.Update();



		if (m.PlayerCollisionCheck(pz, player1.getRotate()))
		{
			SoundManager::sharedManager()->play(CRUSH_SOUND);

			if (player1.collision())
			{
				GameState = 1;
				glutPostRedisplay();
			}
		}

		std::vector<Bullet> tmpList = player1.getBulletList();
		m.BulletCollisionCheck(tmpList);

		player1.setBulletList(tmpList);
		pd = PD_pack_data(player1);
		//D_print(dat);		
	}

	string str = "Turbo_Racing   fps:";

	glutSetWindowTitle((str + std::to_string(CalculateFrameRate())).c_str());
	glutPostRedisplay();
	glutTimerFunc(1, Timer, 0);
}

void BGM()
{
	SoundManager::sharedManager()->init();
	SoundManager::sharedManager()->loading();
	if (GameState == 0)
	{
		SoundManager::sharedManager()->play(BACKGROUND_SOUND);
		SoundManager::sharedManager()->play(DRIVE_SOUND);
	}
}

void Reset()
{
	GameState = 0;
	for (auto i = p.begin(); i != p.end(); ++i)
	{
		i->Reset();
	}
	player1.Reset();
	m.Reset();
	BGM();

	glutTimerFunc(1, Timer, 0);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	player1.Key_Input(key, TRUE);
	switch (key)
	{
	case 'Q':
		exit(0);
		break;
	}

	if (GameState == 1)
	{
		switch (key)
		{
		case 'R':
			Reset();
			break;
		case 'r':
			Reset();
			break;
		}
	}

	if (GameState == 2) //타이틀
	{
		switch (key)
		{
		case '1':

			words.push_back("1");
			ip_number_len++;
			break;
		case '2':
			words.push_back("2");
			ip_number_len++;

			break;
		case '3':
			words.push_back("3");
			ip_number_len++;

			break;
		case '4':
			words.push_back("4");
			ip_number_len++;

			break;
		case '5':
			words.push_back("5");
			ip_number_len++;

			break;
		case '6':
			words.push_back("6");
			ip_number_len++;

			break;
		case '7':
			words.push_back("7");
			ip_number_len++;

			break;
		case '8':
			words.push_back("8");
			ip_number_len++;

			break;
		case '9':
			words.push_back("9");
			ip_number_len++;

			break;
		case '.':
			words.push_back(".");
			ip_number_len++;

			break;
		case '0':

			words.push_back("0");
			ip_number_len++;
			break;
		case 'r':
			Reset();
			break;
		case 8: //backspace
			words.pop_back();
			ip_number_len--;
			break;
		case 13://enter
			// 올바른 ip주소라면
			string ip_add = "";
			for (string word : words)
				ip_add += word;

			cout << ip_add << endl;
			break;
		}
	}

	if (GameState == 3) //로비
	{
		switch (key)
		{
		case 32:
			if (gcd.Im_Ready) {
				gcd.Im_Ready = false;
			}
			else if (!gcd.Im_Ready) {
				gcd.Im_Ready = true;
			}
			
			break;
		}
	}

}

GLvoid KeyboardUp(unsigned char key, int x, int y)
{
	player1.Key_Input(key, FALSE);
}

GLvoid sKeyboard(int key, int x, int y)
{
	player1.sKey_Input(key, TRUE);
}

GLvoid sKeyboardUp(int key, int x, int y)
{
	player1.sKey_Input(key, FALSE);
}

//소켓함수 오류 출력 후 종료
void err_quit(const char* msg)
{
<<<<<<< HEAD
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

//소켓함수 오류 출력
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

DWORD WINAPI JoinThread(LPVOID arg)
{
	int retval;
=======
	p.reserve(3);
	srand((unsigned int)time(NULL));
>>>>>>> cfe202dac8b36961a72bb666429ee9a10d33ccb5

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit("socket()");

	//connect
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("connect()");

	printf("연결 성공");
}


int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL));

	HANDLE hThread_Join;
	hThread_Join = CreateThread(NULL, 0, JoinThread, NULL, 0, 0);

	GLint width = WIDTH;
	GLint height = HEIGHT;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Turbo_Racing||      fps:");

	glewExperimental = GL_TRUE;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glFrontFace(GL_CCW);
	glewInit();

	InitShader();

	player1.Init();
	
	
	m.Init();

	glutTimerFunc(1, Timer, 0);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);
	glutSpecialFunc(sKeyboard);
	glutSpecialUpFunc(sKeyboardUp);
	glutDisplayFunc(drawScene);

	BGM();

	glutMainLoop();
}