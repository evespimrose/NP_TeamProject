#include "framework.h"
#include "Player.h"
#include "Map.h"
#include "Sound.h"
#include "MPlayer.h"
#define HEIGHT 600
#define WIDTH 800
#define TEXT_GAP 200
#define BUFSIZE 1024
// for TCP
//#define MULTI

using namespace std;

int Scene = TITLE_SCENE;

const float length = 0.5;

char* arr;

int user_id = -1; //0~2
#ifdef Multi
GameState = 0;
#endif // Multi

GLuint vertexShader;
GLuint fragmentShader;

GLuint ShaderProgram;

float ambient = 0.6f;

Player player1;
MPlayer mplayer[2];
Bullet bullet[MAX_BULLET];

Map m;

Data* dat;
Game_data game_data;

vector<Cube_data> cd;
all_ready_info ari;
ready_info ri;

float Rotate = 0;
int cnt = 0;
int ip_number_len = 0;
vector<string> words;

SOCKET sock;
char Buffer[BUFSIZE];
Cube cube[MAX_CUBE];


int get_ClientID(SOCKET sock);
int recvn(SOCKET s, char* buf, int len, int flags);


Player_data PD_pack_data(Player p)
{
	Player_data* pd = new Player_data;
	//pd->KeyDownlist[3] = p.getKey();
	/*pd->PosVec_z = p.getPosition().z;
	pd->speed = p.getSpeed();*/
	//pd->rotate = p.getRotate();
	return *pd;
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


DWORD WINAPI JoinThread(LPVOID arg)
{
	sock = init_sock();
	
	
	while (1) {
		Recv_Packet(sock);
			//game_data = recv_Player(sock);
			//cout << game_data.player_data->BulletList[0] << endl;


	}
	return 0;

}

void ProcessPacket(char* packet_buffer)
{
	char* ptr = packet_buffer;

	switch (ptr[2])
	{
	case SC_LOGIN_OK:
	{
		std::cout << "login ok" << std::endl;
		sc_packet_login_ok packet;
		memcpy(&packet, ptr, sizeof(packet));
		user_id = packet.id;
		cout << user_id << endl;
	//	ari.Pt_Players[user_id] = true;
	//	ari.is_ready[user_id] = false;
		break;

	}
	case SC_READY:
	{
		std::cout << "ready ok" << std::endl;
		sc_packet_ready packet;
		memcpy(&packet, ptr, sizeof(packet));
		ari.is_ready[packet.id] = packet.ready;
		ari.Pt_Players[packet.id] = packet.pt;

		break;

	}
	case SC_GAMESTART:
	{
		std::cout << "game start" << std::endl;
		sc_packet_game_start packet;
		memcpy(&packet, ptr, sizeof(packet));
		ari.pt_clients_num = packet.clients_num;

		Scene = GAME_SCENE;
		break;
	}
	case SC_PLAYER_POS:
	{
		
		sc_packet_player_pos packet;
		memcpy(&packet, ptr, sizeof(packet));
		for (int i = 0; i < 3; i++) {
			if (packet.players[i].id != -1)
			{
				//cout << i << "---------" << packet.players[i].PosX << endl;
					if ( packet.players[i].PosY != NULL && packet.players[i].PosZ != NULL)
					{
					/*	game_data.player_data[user_id].Posvec.x = packet.players[user_id].PosX;
						game_data.player_data[user_id].Posvec.y = packet.players[user_id].PosY;
						game_data.player_data[user_id].Posvec.z = packet.players[user_id].PosZ;
						game_data.player_data[user_id].TR = packet.players[user_id].TR;*/
						
						for (int i = 0; i < 3; i++) {
							game_data.player_data[i].Posvec.x = packet.players[i].PosX;
							game_data.player_data[i].Posvec.y = packet.players[i].PosY;
							game_data.player_data[i].Posvec.z = packet.players[i].PosZ;
							game_data.player_data[i].PosMat = packet.players[i].PosMat;
							game_data.player_data[i].RotMat = packet.players[i].RotMat;
							game_data.player_data[i].SclMat = packet.players[i].SclMat;
						}
						
					}
				
			}
		}
		break;
	}
	case SC_BULLET_POS:
	{

		sc_packet_bullet_pos packet;
		memcpy(&packet, ptr, sizeof(packet));
		for (int i = 0; i < MAX_BULLET; i++) {
			bullet[i].setposMat(packet.bullets[i].PosMat);
		}

		break;
	}
	case SC_MAP_CUBE:
	{
		sc_packet_cube_pos packet;
		memcpy(&packet, ptr, sizeof(packet));
		for (int i = 0; i < MAX_CUBE; i++) {
			cube[i].set(packet.cubes[i].life, packet.cubes[i].PosMat, packet.cubes[i].RotMat);

		}

		break;
	}
	default:
		std::cout << "None Receive Packet" << std::endl;
		break;
	}
	return;
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
	if (Scene == GAME_SCENE) //게임 시작
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
#ifdef MULTI
		mplayer2.Render(ShaderProgram);
		mplayer3.Render(ShaderProgram);
#endif
		//2명일때
		
		if (ari.pt_clients_num == 2) {
			mplayer[0].Render(ShaderProgram);
		}
		//3명일때 
		else if(ari.pt_clients_num == 3){
			mplayer[0].Render(ShaderProgram);
			mplayer[1].Render(ShaderProgram);
		}

		//bullet.Render(ShaderProgram);

		for (int i = 0; i < MAX_BULLET; i++) {
			bullet[i].Render(ShaderProgram);
		}

		for (int i = 0; i < MAX_CUBE; i++) {
			cube[i].Render(ShaderProgram);
		}
		string score = "Score : ";
		score += std::to_string((int)player1.getPosition().z);
		glutPrint(700.0f, 580.0f, GLUT_BITMAP_HELVETICA_18, score);

		string speed = "Speed : ";
		speed += std::to_string((int)(player1.getSpeed() * 500)) + "km/h";
		glutPrint(0.0f, 0.0f, GLUT_BITMAP_HELVETICA_18, speed);


		glutSwapBuffers();
	}
	else if (Scene == OVER_SCENE) //게임종료
	{
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glutPrint(420.0f, 550.0f, GLUT_BITMAP_TIMES_ROMAN_24, "GAME OVER");
		glutPrint(WIDTH / 3.0f, HEIGHT / 2.0f, GLUT_BITMAP_TIMES_ROMAN_24, "GAME OVER");
		glutPrint(WIDTH / 3.0f, HEIGHT / 3.0f, GLUT_BITMAP_TIMES_ROMAN_24, "Press R to CONTINUE");

		glutSwapBuffers();
	}
	else if (Scene == TITLE_SCENE) // 타이틀 씬
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
	else if (Scene == LOBBY_SCENE) // 로비 씬
	{
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glutPrint(WIDTH / 2.5f - 25, HEIGHT / 1.5f + 100, GLUT_BITMAP_TIMES_ROMAN_24, "Waiting for players...");
		//플레이어 1
		glutPrint(WIDTH / 5.5f, HEIGHT / 2.0f, GLUT_BITMAP_HELVETICA_18, "----------");//상단
		glutPrint(WIDTH / 5.5f, HEIGHT / 1.5f, GLUT_BITMAP_HELVETICA_18, "----------");
		for (int i = 0; i < 7; i++) {
			glutPrint(WIDTH / 5.5f, HEIGHT / 1.5f - 14 * i - 8, GLUT_BITMAP_HELVETICA_18, "I");
		}
		for (int i = 0; i < 7; i++) {
			glutPrint(WIDTH / 3.2f, HEIGHT / 1.5f - 14 * i - 8, GLUT_BITMAP_HELVETICA_18, "I");
		}
		if (ari.Pt_Players[0]) {
			glutPrint(WIDTH / 5.5f + 20, HEIGHT / 1.7f, GLUT_BITMAP_HELVETICA_18, "Player 1 ");
		}

		//플레이어 2
		glutPrint(WIDTH / 5.5f + TEXT_GAP, HEIGHT / 2.0f, GLUT_BITMAP_HELVETICA_18, "----------");//상단
		glutPrint(WIDTH / 5.5f + TEXT_GAP, HEIGHT / 1.5f, GLUT_BITMAP_HELVETICA_18, "----------");
		for (int i = 0; i < 7; i++) {
			glutPrint(WIDTH / 5.5f + TEXT_GAP, HEIGHT / 1.5f - 14 * i - 8, GLUT_BITMAP_HELVETICA_18, "I");
		}
		for (int i = 0; i < 7; i++) {
			glutPrint(WIDTH / 3.2f + TEXT_GAP, HEIGHT / 1.5f - 14 * i - 8, GLUT_BITMAP_HELVETICA_18, "I");
		}
		if (ari.Pt_Players[1]) {
			glutPrint(WIDTH / 5.5f + 20 + TEXT_GAP, HEIGHT / 1.7f, GLUT_BITMAP_HELVETICA_18, "Player 2 ");
		}

		//플레이어 3
		glutPrint(WIDTH / 5.5f + TEXT_GAP * 2, HEIGHT / 2.0f, GLUT_BITMAP_HELVETICA_18, "----------");//상단
		glutPrint(WIDTH / 5.5f + TEXT_GAP * 2, HEIGHT / 1.5f, GLUT_BITMAP_HELVETICA_18, "----------");
		for (int i = 0; i < 7; i++) {
			glutPrint(WIDTH / 5.5f + TEXT_GAP * 2, HEIGHT / 1.5f - 14 * i - 8, GLUT_BITMAP_HELVETICA_18, "I");
		}
		for (int i = 0; i < 7; i++) {
			glutPrint(WIDTH / 3.2f + TEXT_GAP * 2, HEIGHT / 1.5f - 14 * i - 8, GLUT_BITMAP_HELVETICA_18, "I");
		}
		if (ari.Pt_Players[2]) {
			glutPrint(WIDTH / 5.5f + 20 + TEXT_GAP * 2, HEIGHT / 1.7f, GLUT_BITMAP_HELVETICA_18, "Player 3 ");
		}

		//ready
		for (int i = 0; i < 3; i++) {
			if (ari.is_ready[i]) {
				glutPrint(WIDTH / 5.0f + TEXT_GAP * i, HEIGHT / 2.2f, GLUT_BITMAP_HELVETICA_18, "Ready");
			}
		}

		glutSwapBuffers();
	}
}

GLvoid Timer(int Value)
{
	if (Scene == OVER_SCENE)
	{
		SoundManager::sharedManager()->play(OVER_SOUND);
		SoundManager::sharedManager()->stop(BACKGROUND_SOUND);
		SoundManager::sharedManager()->stop(DRIVE_SOUND);
		return;
	}

	if (Scene == GAME_SCENE)
	{
		float pz = player1.getPosition().z;
		//float pz = player2.getPosition().z;
		float fpz = 0.0f;
		float spz = 10000000.0f;

		/*vector<Player>::iterator fastest_player_iter = p.begin();
		vector<Player>::iterator slowest_player_iter = p.begin();*/
		/*for (auto i = p.begin(); i != p.end(); ++i)
		{
			fpz = max(fpz, i->getPosition().z);
			spz = min(spz, i->getPosition().z);
		}*/

		m.Fastest_Update(pz, cube);
		m.Slowest_Update(pz);


#ifdef MULTI
		mplayer2.Update(player_data[1]);
		mplayer3.Update(player_data[2]);
#endif


		player1.Update(game_data.player_data[user_id]);

		if (ari.pt_clients_num == 2) {

			mplayer[0].Update(game_data.player_data[user_id == 1 ? 0 : 1]);
		}

		if (ari.pt_clients_num == 3) {
			for (int i = 0; i < 3; i++) {
				if (i != user_id && cnt == 0) {
					mplayer[0].Update(game_data.player_data[i]);
					cnt++;
				}
				else if (i != user_id) {
					mplayer[1].Update(game_data.player_data[i]);
				}
			}
		}

		cnt = 0;

		std::vector<Bullet> tmpList = player1.getBulletList();
		m.BulletCollisionCheck(tmpList);

		player1.setBulletList(tmpList);
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
	if (Scene == GAME_SCENE)
	{
		SoundManager::sharedManager()->play(BACKGROUND_SOUND);
		SoundManager::sharedManager()->play(DRIVE_SOUND);
	}
}

void Reset()
{
	Scene = GAME_SCENE;
	/*for (auto i = p.begin(); i != p.end(); ++i)
	{
		i->Reset();
	}*/
	player1.Reset();
	m.Reset();
	BGM();

	glutTimerFunc(1, Timer, 0);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	player1.Key_Input(sock, key, TRUE);

	switch (key)
	{
	case 'Q':
		exit(0);
		break;
	}
	int len;
	int retval;
	if (Scene == GAME_SCENE) //게임 씬
	{
		switch (key)
		{
		case 'a'://왼쪽 방향키
			Send_event(sock, CS_PLAYER_LEFT_DOWN);
			break;

		}
	}

	if (Scene == OVER_SCENE)
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

	if (Scene == TITLE_SCENE) //타이틀
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

			string ip_add = "";
			for (string word : words)
				ip_add += word;

			
			//Send_event(sock, CS_READY);
			printf("전송완료");
			Scene = LOBBY_SCENE;
			break;
		}
	}

	if (Scene == LOBBY_SCENE) //로비
	{
		switch (key)
		{
		case 32:

			printf("전송완료");
			Send_event(sock, CS_READY);

			break;
		}
	}

}

GLvoid KeyboardUp(unsigned char key, int x, int y)
{
	player1.Key_Input(sock,key, FALSE);
}

GLvoid sKeyboard(int key, int x, int y)
{
	player1.sKey_Input(sock, key, TRUE);
}

GLvoid sKeyboardUp( int key, int x, int y)
{
	player1.sKey_Input(sock, key, FALSE);
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
	player1.Init(game_data.player_data[0], bullet);
#ifdef MULTI
	mplayer2.Init();
	mplayer3.Init();
#endif
	
	mplayer[0].Init();
	mplayer[1].Init();
	

	m.Init();

	glutTimerFunc(1, Timer, 0);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);
	glutSpecialFunc(sKeyboard);
	glutSpecialUpFunc(sKeyboardUp);
	glutDisplayFunc(drawScene);

	//BGM();

	glutMainLoop();
}
